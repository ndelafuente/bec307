/**
 * File: main.c
 *
 * Main function for the game of life simulator.
 */

#define _XOPEN_SOURCE 600

#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdbool.h>
#include <curses.h>

#include "gol.h"

static void usage(char *prog_name) {
	fprintf(stderr, "usage: %s [-s] -c <config-file> -t <number of turns> -d <delay in ms>\n", prog_name);
	exit(1);
}


int main(int argc, char *argv[]) {

	// Step 1: Parse command line args (Nothing to change for this step!)
	char *config_filename = NULL;

	int delay = 100; // default value for delay between turns is 100
	int num_turns = 20; // default to 20 turns per simulation
	bool step_mode = false; // unless specified, step_mode is off

	char ch;

	while ((ch = getopt(argc, argv, "c:t:d:s")) != -1) {
		switch (ch) {
			case 's':
				step_mode = true;
				break;
			case 'c':
				config_filename = optarg;
				break;
			case 't':
				if (sscanf(optarg, "%d", &num_turns) != 1) {
					fprintf(stderr, "Invalid value for -t: %s\n", optarg);
					usage(argv[0]);
				}
				break;
			case 'd':
				if (sscanf(optarg, "%d", &delay) != 1) {
					fprintf(stderr, "Invalid value for -d: %s\n", optarg);
					usage(argv[0]);
				}
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

	// Print summary of simulation options
	fprintf(stdout, "Config Filename: %s\n", config_filename);
	fprintf(stdout, "Number of turns: %d\n", num_turns);
	
	if (step_mode == true) {
		fprintf(stdout, "Step mode: Enabled\n");
	}
	else {
		fprintf(stdout, "Step mode: Disabled\n");
		fprintf(stdout, "Delay between turns: %d ms\n", delay);
	}


	// Step 2: Set up the text-based ncurses UI window. This is alrady done so
	// don't modify the following 4 function calls.
	//
	initscr(); 	// initialize screen
	cbreak(); 	// set mode that allows user input to be immediately available
	noecho(); 	// don't print the characters that the user types in
	clear();  	// clears the window
	
	// Step 3: Create and initialze the world.
	int width, height;
	int *world = initialize_world(config_filename, &width, &height);

	// Step 4: Simulate for the required number of steps, printing the world
	// after each step.
	for (int turn = 0; turn <= num_turns; turn++) {
		// Print and update the world
		print_world(world, width, height, turn);
		update_world(world, width, height);

		// Pause the simulation if step mode is enabled
		if (step_mode) {
			mvaddstr(LINES-1, 0, "Press any key to continue.");
			getch();
		}
		// If not, continue after the specified delay
		else
			usleep(delay * 1000);
	}
	
	free(world); // free the world

	// Step 5: Wait for the user to type a character before ending the
	// program. Don't change anything below here.
	
	// print message to the bottom of the screen (i.e. on the last line)
	mvaddstr(LINES-1, 0, "Press any key to end the program.");

	getch(); // wait for user to enter a key
	endwin(); // close the ncurses UI window

	return 0;
}
