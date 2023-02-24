/**
 * File: gol.c
 *
 * Implementation of the game of life simulator functions.
 */

#define _XOPEN_SOURCE 600

#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <curses.h>

#include "gol.h"

// Define the exceptions that may occur
#define FileNotFoundError 'F' // The file was unable to be opened
#define ConfigurationError 'C'
#define AllocationError 'A'
#define MVAddError 'M'

			// GAME OF LIFE FUNCTIONS //
//TODO don't change parameter list
int *initialize_world(char *config_filename, int *width, int *height) {
	// Open the configuration file to be read from
	FILE *config_file = fopen(config_filename, "r");
	if (config_file == NULL) { raise_exception(FileNotFoundError, config_filename); }

	// Read in the number of rows and columns
	int ret = fscanf(config_file, "%d", height);
	if (ret != 1 || *height < 0) { raise_exception(ConfigurationError, "height"); }
	ret = fscanf(config_file, "%d", width);
	if (ret != 1 || *width < 0) { raise_exception(ConfigurationError, "width"); }

	// Initialize a 1D emulation of a 2D array to represent the world
	int *world = calloc((*height) * (*width) + 1, sizeof(int));
	if (world == NULL) { raise_exception(AllocationError, "world"); }

	// Read in the number of coordinate pairs
	int num_coords = 0;
	ret = fscanf(config_file, "%d", &num_coords);
	if (ret != 1) { raise_exception(ConfigurationError, "number of coordinates"); }

	// Read in the coordinate pairs and set the value at that location to 1
	for (int i = 0, col, row; i < num_coords; i++) {
		ret = fscanf(config_file, "%d %d", &col, &row);
		if (ret != 2) { raise_exception(ConfigurationError, "coordinate"); }
		world[translate_to_1d(col, row, *width)] = 1;
	}
	
	return world;
}

void update_world(int *world, int width, int height) {
	// Create a copy of the world for reference
	int *old_world = copy(world, width, height);
	if (old_world == NULL) { raise_exception(AllocationError, "world copy"); }

	// Update each cell in the world
	for (int col = 0; col < width; col++) {
		for (int row = 0; row < height; row++) {
			int cell_pos = translate_to_1d(col, row, width);
			int num_near = count_neighbors(old_world, col, row, width, height);
			world[cell_pos] = update_cell(old_world[cell_pos], num_near);
		}
	}

	free(old_world);
}

int update_cell(int cell, int num_neighbors) {
	int is_alive = 1, is_dead = 0;

	if (cell) { // the cell is alive
		if (num_neighbors <= 1)
			return is_dead; // the cell dies from loneliness
		else if (num_neighbors >= 4)
			return is_dead; // the cell dies from overpopulation
	}
	else { // the cell is dead
		if (num_neighbors == 3) 
			return is_alive; // the birds and the bees take place
	}

	return cell; // the cell remains unchanged
}

int count_neighbors(int *world, int col, int row, int width, int height) {
	/**
	 * n n n    (col-1, row-1) (col, row-1) (col+1, row-1)
	 * n c n    (col-1,   row) [col,   row] (col+1,   row)
	 * n n n    (col-1, row+1) (col, row+1) (col+1, row+1)
	 */
	int count = 0;
	for (int c = col - 1; c <= col + 1; c++)
		for (int r = row - 1; r <= row + 1; r++) {
			if (!(c == col && r == row)) {
				int wrapped_c = wrap(c, width);
				int wrapped_r = wrap(r, height);
				count += world[translate_to_1d(wrapped_c, wrapped_r, width)];
			}
		}

	return count;
}

void print_world(int *world, int width, int height, int turn) {
	clear(); // clear the screen

	// Print each cell in the world
	for (int row = 0; row < height; row++) {
		for (int col = 0; col < width; col++) {
			int cell = world[translate_to_1d(col, row, width)];
			if (cell) // cell is alive
				mvaddstr(row, col * 2, "@ ");
			else // cell is dead
				mvaddstr(row, col * 2, ". ");
		}
	}
	// Print the current turn number
	mvadd_str_int(height + 1, 0, "Time Step:", turn);
	mvaddch(LINES-1, 0, '\n'); // move the cursor to the bottom of the screen
	
	refresh(); // display the text that has been added
}


			// ARRAY FUNCTIONS //
int translate_to_1d(int col, int row, int width) {
	// Translate to 1D
	return row * width + col;
}

int wrap(int n, int limit) {
	// Take care of cell wrapping
	n += limit;
	n %= limit;
	return n;
}

int *copy(int *arr, int width, int height) {
	// Copy each value from the array
	int *arr_copy = calloc(width * height + 1, sizeof(int));
	for (int i = 0; i < width * height; i++) {
		arr_copy[i] = arr[i];
	}
	return arr_copy;
}


			// OUTPUT FUNCTIONS //
int mvadd_str_int(int row, int col, char *s, int n) {
	int len = 0;
	// While the character at len is not null
	while(s[len]) { len++; }

	int ret = mvaddstr(row, col, s) 
			+ mvaddint(row, col + len, n);
	refresh();
	return ret;
}

int mvaddint(int row, int col, int n) {
	int ret = OK; // return flag

	// Print a negative sign if the number is negative
	int is_negative = n < 0; // keeps track of the sign
	if (is_negative) {
		ret = mvaddch(row, col, '-');
		if (ret == ERR) { return ret; }
		n *= -1;
	}
	
	for (int i = num_digits(n); i > 0; i--) {
		char digit = (n % 10) + 48; // convert the right-most digit into a char
		if (digit >= '0' && digit <= '9') {
			// Print the digit (moving over 1 when there is a negative sign)
			ret = mvaddch(row, col + is_negative + i, digit);
			if (ret == ERR) { return ret; }
		}
		else { return ERR; }
		n /= 10; // remove the right-most digit
	}

	return OK;
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


			// EXCEPTION FUNCTION //
void raise_exception(char ex, char *opt) {
    endwin(); // stop the ncurses window

	// Print the appropriate message for the "exception"
    if (ex == FileNotFoundError) {
	    fprintf(stderr, "FileNotFoundError: "
						"could not open file \"%s\".", opt);
	}
    else if (ex == ConfigurationError) {
        fprintf(stderr, "ConfigurationError: "
						"invalid %s in configuration file", opt);
	}
    else if (ex == AllocationError) {
        fprintf(stderr, "AllocationError: "
						"could not allocate enough memory for the %s", opt);
	}
    else if (ex == MVAddError) {
        fprintf(stderr, "MVAddError: "
						"could not print the %s to the screen.", opt);
	}
	else
		fprintf(stderr, "Unknown Error");
	fprintf(stderr, "\n\n");

	// Exit the program with failure status
    exit(EXIT_FAILURE);
}
