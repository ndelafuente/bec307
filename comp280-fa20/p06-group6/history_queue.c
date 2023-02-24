/*
 * The Tiny Torero Shell (TTSH)
 *
 * Add your top-level comments here.
 */
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include "history_queue.h"

// global variables: add only globals for history list state
//                   all other variables should be allocated on the stack
// static: means these variables are only in scope in this .c module
static HistoryEntry history[MAXHIST]; 

static int history_start = 0;
static int history_next = 0;
static int history_size = 0;
static int entry_count = 0;

void add_history_entry(char *cmdline) {
	// Add the command line text to the history
	history[history_next].cmd_num = entry_count;
	strncpy(history[history_next].cmdline, cmdline, MAXLINE);
	
	// Calculate the next index and update the size and starting index
	history_next = (history_next + 1) % MAXHIST;
	
	// Increment the size until it reaches the max
	if (history_size < MAXHIST) {
		history_size++;
	}
	// If the size is at the max, then start = next
	else {
		history_start = history_next;
	}
	
	// Update the entry count
	entry_count++;
}

char *get_history_entry(unsigned int entry_num) {
	for (int k = 0; k < history_size; k++) {
		if (history[k].cmd_num == entry_num) {
			return history[k].cmdline;
		}
	}
	return NULL;
}

void print_history() {
	// Print the history entries
	for (int k = 0; k < history_size; k++) {
		int index = (history_start + k) % history_size;

		HistoryEntry entry = history[index];

		printf("%d\t%s", entry.cmd_num, entry.cmdline);
	}
	printf("\n");
}
