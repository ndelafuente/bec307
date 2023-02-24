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

// TODO: Add your translate_to_1D function (from lab 04) here.

int *initialize_world(char *config_filename, int *num_rows, int *num_cols) {
	// TODO: Implement this function
	fprintf(stdout, "The initialize_world function is currently unimplemented.\n");

	return NULL;
}

void update_world(int *world, int width, int height) {
	// TODO: Implement this function
	fprintf(stdout, "The update_world function is currently unimplemented.\n");
}

void print_world(int *world, int width, int height) {
	fprintf(stdout, "The print_world function is currently unimplemented.\n");

	// TODO: The rest of the code in this function is a dummy implementation. 
	// Replace it with the real one.

	clear(); // clears the screen

	for (int row = 0; row < 5; row++) {
		for (int col = 0; col < 5; col++) {
			if (row == col) {
				mvaddch(row, col, '@');
			}
			else {
				mvaddch(row, col, '.');
			}

		}
	}

	refresh(); // displays the text we've added
}
