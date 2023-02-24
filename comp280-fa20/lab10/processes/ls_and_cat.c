/**
 * A program that runs two commands on the specified file:
 *
 * 	1. ls -l
 * 	2. cat
 */
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main(int argc, char **argv) {

	if (argc != 2) {
		// TODO: Modify the printf to add a newline to the end. Note what is
		// displayed in the gutter to the left.
		printf("usage: %s <filename>", argv[0]);
		exit(1);
	}

	char *filename = argv[1];

	// TODO: Delete this comment and note what shows up in the gutter.

	// TODO: Complete the 5 steps listed below. As you add new lines, note how
	// the gutter changes.
	
	// Step 1: Create a new process with fork, then have parent wait for the
	// child to exit.
	
	// Step 2: Use execvp to have the child perform "ls -l filename"
	
	// Step 3: Check that child exited normally, printing out an error message
	// and exiting (with status 1) if it did not.
	
	// Step 4: Create another new process, and have the parent wait for the
	// child to exit.
	
	// Step 5: Make the child perform the "cat filename" command (using execvp
	// again)
	
	return 0;
}
