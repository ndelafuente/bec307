/*
 * This program shows a simple example of reading in values from a file.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(int argc, char *argv[]) {
	// Make sure someone specified the file we are to open/read from
	if (argc != 2) {
		printf("usage: %s <input.txt>\n", argv[0]);
		exit(1);
	}

	/*
	 * (1) Open the file for reading and check that open succeeded 
	 *     (the file name is passed as command line arg)
	 */
	FILE *infile = fopen(argv[1], "r");   
	if (infile == NULL) { 
		printf("ERROR: file open %s\n", argv[1]);
		exit(1);
	}

	/*
	 * (2) read in some values from the file using fscanf
	 *     fscanf is like scanf, except that it takes a FILE*
	 *     to the file from which to read as its first argument
	 *     it returns the number of items read (or fewer on EOF or error)
	 */
	int num_students, num1, num2, ret;
	ret = fscanf(infile, "%d", &num_students); 
	if (ret == 1) {
		char name[64];

		// read first student info
		ret = fscanf(infile, "%s %d %d", name, &num1, &num2); 

		int i = 0;
		while ((i < num_students) && (ret == 3)) {  
			printf("line %d:  %10s %4d %4d\n", i, name, num1, num2);

			// read next student's info
			ret = fscanf(infile, "%s %d %d", name, &num1, &num2); 

			i++;
		}

		// Check to see if we stopped reading in student info prematurely
		// because we couldn't successfully read in a student's info.
		if ((i < num_students) && (ret != 3)) {
			fprintf(stderr, "ERROR: Student info not in a valid format.\n");
			exit(1);
		}
	}
	else {
		// Had a problem reading in the first number
		fprintf(stderr, "ERROR: Could not read in number of students in file.\n");
		exit(1);
	}

	// (3) close the file when done with it
	fclose(infile);  
	return 0;
}
