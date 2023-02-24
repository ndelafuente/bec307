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
#include <pthread.h>

#include "gol.h"

struct ThreadData {
	int id;
	pthread_barrier_t *barrier;
	int *world;
	int *world_copy;
	int width;
	int height;
	int num_turns;
	int delay;
	int start;
	int end;
};

typedef struct ThreadData ThreadData;

// Function prototypes
void run_threads(int *world, int width, int height, int num_turns, int delay, int num_threads);
void *simulate_world(void *thread_args);
void update_world_copy(int *world_copy, int *world, int width, int height);

/**
 * Function that prints out how to use the program, in case the user forgets.
 *
 * @param prog_name The name of the executable.
 */
static void usage(char *prog_name) {
	fprintf(stderr, "usage: %s [-s] -c <config-file> -t <number of turns> "
					"-d <delay in ms> -p <number of threads>\n", prog_name);
	exit(1);
}


int main(int argc, char *argv[]) {

	// Step 1: Parse command line args 
	char *config_filename = NULL;

	int delay = 100; // default value for delay between turns is 100
	int num_turns = 20; // default to 20 turns per simulation
	int num_threads = 2;
	char ch;

	while ((ch = getopt(argc, argv, "c:t:d:p:")) != -1) {
		switch (ch) {
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
			case 'p':
				if (sscanf(optarg, "%d", &num_threads) != 1) {
					fprintf(stderr, "Invalid value for -p: %s\n", optarg);
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
	fprintf(stdout, "Delay between turns: %d ms\n", delay);
	
	// Step 2: Set up the text-based ncurses UI window.
	initscr(); 	// initialize screen
	cbreak(); 	// set mode that allows user input to be immediately available
	noecho(); 	// don't print the characters that the user types in
	clear();  	// clears the window
	

	// Step 3: Create and initialze the world.
	int width, height;
	int *world = initialize_world(config_filename, &height, &width);

	if (world == NULL) {
		endwin();
		fprintf(stderr, "Error initializing the world.\n");
		exit(1);
	}
	
	// Step 4: Simulate for the required number of steps, printing the world
	// after each step.
	run_threads(world, width, height, num_turns, delay, num_threads);
	
	print_world(world, width, height, num_turns); // print final world
	free(world);

	// Step 5: Wait for the user to type a character before ending the
	// program. Don't change anything below here.
	
	// print message to the bottom of the screen (i.e. on the last line)
	mvaddstr(LINES-1, 0, "Press any key to end the program.");

	getch(); // wait for user to enter a key
	endwin(); // close the ncurses UI window

	return 0;
}

/**
 * Run the threads
 *
 * @param world The matrix to keep track of the state of the game
 * @param width The width of the world
 * @param height The height of the world
 * @param num_turns The number of turns to run the game for
 * @param delay The delay in milliseconds between each turn
 */
void run_threads(int *world, int width, int height, int num_turns, int delay, int num_threads) {

	pthread_t *tids = malloc(num_threads * sizeof(pthread_t));
	ThreadData *thread_args = malloc(num_threads * sizeof(ThreadData));

	// Initialize the barrier, indicating how many threads need to reach it
	// before the barrier is opened.
	pthread_barrier_t shared_barrier; // declare the shared barrier
	if (pthread_barrier_init(&shared_barrier, 0, num_threads) != 0) {
		perror("pthread_barrier_init");
		exit(1);
	}

	// Create a shared copy of the world
	int *world_copy = malloc(width*height*sizeof(int));
	update_world_copy(world_copy, world, width, height);
	
	// Fill in the thread data for each thread
	int zp = num_threads - (height % num_threads); // ze price
	int pp = height / num_threads; 				   // per process
	int start = 0, end = 0, num_rows = 0;
	for (int i = 0; i < num_threads; i++) {
		// Boring maths to find the number of rows this thread is in charge of
		if (height % num_threads == 0)
			num_rows = pp;
		else
			num_rows = (i >= zp) ? (pp + 1) : pp;
		start = end;
		end = start + num_rows;

		// Massive one liner just because
		thread_args[i] = (ThreadData){i, &shared_barrier, world, world_copy, 
							width, height, num_turns, delay, start, end};
		
		// Create a thread, having it run the simulate_world function
		if (pthread_create(&tids[i], 0, simulate_world, &thread_args[i]) != 0) {
			perror("pthread_create\n");
			exit(1);
		}
	}
		
	// Have the main thread wait for all the worker threads to finish
	for (int i=0; i < num_threads; i++) {
		// Using NULL for the 2nd arg of join means that we will ignore the
		// result of the thread with which we're joining.
		pthread_join(tids[i], NULL);
	}
	
	// Clean up, clean up, everybody every where!
	pthread_barrier_destroy(&shared_barrier);
	free(world_copy);
	free(thread_args);
	free(tids);

}

/**
 * Simulate the game of life.
 *
 * @param thread_args A struct pointing to the game specifications.
 */
void *simulate_world(void *thread_args) {
	ThreadData *td = (ThreadData *)thread_args;
	
	// Print out the rows the thread is in charge of
	int num_rows = td->end - td->start;
	printf("\rtid %2d: rows:%2d:%2d (%2d)\n", td->id, td->start, td->end, num_rows);
		
	// Update the world
	update_world(td->world, td->world_copy, td->width, td->height, 0, td->height);

	// There is only one thread to rule them all
	int ret = pthread_barrier_wait(td->barrier);
	bool is_chosen_one = (ret == PTHREAD_BARRIER_SERIAL_THREAD);

	// Run the gol simulation
	for (int turn_number = 0; turn_number < td->num_turns; turn_number++) {
		// The chosen one unfortunately has to do more work
		if (is_chosen_one) {
			update_world_copy(td->world_copy, td->world, td->width, td->height);
			print_world(td->world, td->width, td->height, turn_number);
			usleep(1000 * td->delay);
		}
		
		// Update the world and synchronize the threads at this turn
		update_world(td->world, td->world_copy, td->width, td->height, 0, td->height);
		pthread_barrier_wait(td->barrier);
		
	}

	return NULL;
}


/**
 * Update the copy of the world.
 *
 * @param world_copy The copy of the world.
 * @param world The world.
 * @param width The width of the world.
 * @param height The height of the world.
 */
void update_world_copy(int *world_copy, int *world, int width, int height) {
	for (int i = 0; i < width*height; i++) {
		world_copy[i] = world[i];
	}
}
