/**
 * file: rle_reader.c
 * 
 * A program that will take in a Game of Life configuration file in RLE format
 * and convert it into the proper format for this project
 */

#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>

// Function prototypes
int read_run_count(FILE *config_file);
char read_tag(FILE *config_file);
int tag_eval(char tag);
int num_digits(int n);

static void usage(char *prog_name) {
	fprintf(stderr, "usage: %s -c <config-file>\n", prog_name);
	exit(1);
}

static void config_error(char *err) {
    fprintf(stderr, "\n%s\n\n", err);
    fprintf(stderr, "Run Length Encoded format\n"
                    "Line 1: x = <width>, y = <height>\n"
                    "Lines 2+: <run_count><tag>\n"
                    "where <run_count> is the number of occurrences of <tag>\n"
                    "and <tag> is one of the following three characters:\n"
                    "<tag>    description\n"
                    "  b      dead cell\n"
                    "  o      alive cell\n"
                    "  $      end of line\n");
    exit(1);
}

int main(int argc, char *argv[]) {
    char *config_filename = NULL;
    char *output_filename = NULL;

    // Parse the external options
    char ch;
	while ((ch = getopt(argc, argv, "c:o:")) != -1) {
		switch (ch) {
			case 'c':
				config_filename = optarg;
				break;
            case 'o':
                output_filename = optarg;
                break;
			default:
				usage(argv[0]);
		}
	}

	// if config_filename is NULL, then the -c option was missing.
	if (config_filename == NULL) {
		fprintf(stderr, "Missing -c option\n");
		usage(argv[0]);
	}
    if (output_filename == NULL) {
		fprintf(stderr, "Missing -o option\n");
		usage(argv[0]);
    }

	// Print summary of simulation options
	fprintf(stdout, "Config Filename: %s\n", config_filename);
	fprintf(stdout, "Output Filename: %s\n", output_filename);

    // Open the configuration file to be read from
	FILE *config_file = fopen(config_filename, "r");
	if (config_file == NULL) {
        fprintf(stderr, "File unable to be opened\n");
        exit(1);
    }
    // Open the output file to be written to
	FILE *output_file = fopen(output_filename, "w");
	if (output_file == NULL) {
        fprintf(stderr, "File unable to be opened\n");
        exit(1);
    }

	// Read in the number of rows and columns
    int width = 0, height = 0;
	int ret = fscanf(config_file, "x = %d, y = %d", &width, &height);
	if (ret != 2 || width < 0 || height < 0) { 
        config_error("Invalid world dimensions");
    }
    fprintf(output_file, "%d\n%d\n", height, width);

    // Write the coordinates of each live cell to an array
    int live_count = 0;
    int *coords = calloc(width * height * 2, sizeof(int));
    for (int row = 0; row < height; row++) {
        int col = 0;
        while (col < width) {
            int run_count = read_run_count(config_file);
            char tag = read_tag(config_file);
            if (tag == '$') { break; }

            if (tag == 'b') // if cell is dead
                col += run_count;
            else if (tag == 'o') { // if cell is alive
                for (int i = 0; i < run_count; i++, col++, live_count++) {
                    coords[live_count * 2] = col;
                    coords[live_count * 2 + 1] = row / 2;
                }
            }
        }
    }
    // Print the count of the cells to the file
    fprintf(output_file, "%d\n", live_count);

    // Print the coordinates of each live cell to the file
    for (int i = 0; i < live_count; i++)
        fprintf(output_file, "%d %d\n", coords[i * 2], coords[i * 2 + 1]);
    free(coords);

    // Close the files
    fclose(config_file);
    fclose(output_file);
}

int read_run_count(FILE *config_file) {
    int run_count = 1;
    fscanf(config_file, "%d", &run_count);
    return run_count;
}

char read_tag(FILE *config_file) {
    char tag = 0;
    fscanf(config_file, "%c", &tag);
    int flag = tag_eval(tag);
    if (flag == 1)
        return tag;
    else if (flag == -1)
        fseek(config_file, -sizeof(char), SEEK_CUR);
    
    else
        config_error("Invalid RLE format");
    
    return 0;
}

int tag_eval(char tag) {
    if (tag == 'b' || tag == 'o' || tag == '$')
        return 1;
    else if (tag >= '0' && tag <= '9')
        return -1;
    else
        return 0;
}

int num_digits(int n) {
	if (n == 0) { return 1; }

	if (n < 0) { n *= -1; }

	int count = 0;
	while(n > 0) {
		n /= 10;
		count++;
	}
	return count;
}
