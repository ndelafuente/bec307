/*
 * The Tiny Torero Shell (TTSH)
 *
 * Add your top-level comments here.
 */

// NOTE: all new includes should go after the following #define
#define _XOPEN_SOURCE 600

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h> 
#include <sys/wait.h>

#include "parse_args.h"
#include "history_queue.h"

// Function prototypes
void execute(char *cmdline);
void sigchld_handler(__attribute__ ((unused)) int sig);

int main() { 
	// Set up the handler for SIGCHLD
	struct sigaction sa;
	sa.sa_handler = sigchld_handler;
	sa.sa_flags = SA_NOCLDSTOP; 
	sigaction(SIGCHLD, &sa, NULL);
	
	while(1) {
		// (1) print the shell prompt
		fprintf(stdout, "ttsh> ");  
		fflush(stdout);

		// (2) read in the next command entered by the user
		char cmdline[MAXLINE];
		if ((fgets(cmdline, MAXLINE, stdin) == NULL)
				&& ferror(stdin)) {
			// fgets could be interrupted by a signal.
			// This checks to see if that happened, in which case we simply
			// clear out the error and restart our loop so it re-prompts the
			// user for a command.
			clearerr(stdin);
			continue;
		}

		/*
		 * Check to see if we reached the end "file" for stdin, exiting the
		 * program if that is the case. Note that you can simulate EOF by
		 * typing CTRL-D when it prompts you for input.
		 */
		if (feof(stdin)) {
			fflush(stdout);
			exit(0);
		}


		// (4) Call a function that will determine how to execute the command
		// that the user entered, and then execute it
		execute(cmdline);
	}

	return 0;
}

/**
 * Execute a command
 *
 * @param cmdline The command that the user entered
 */
void execute(char *cmdline) {
	// Handle !<num> commands
	while (cmdline[0] == '!') {
		// Parse the requested entry number
		char *str = cmdline;
		for (int i = 1; i < (int) sizeof(str); i++) {
			str[i - 1] = str[i];
		}
		unsigned int entry_num = strtol(str, NULL, 10);

		// Access the entry with the requested entry number
		cmdline = get_history_entry(entry_num);
		if (cmdline == NULL) {
			printf("ERROR: no entry in history with number %d\n", entry_num);
			return;
		}

	}

	// Parse the arguments from the comand line
	char *argv[MAXARGS];
	int background = parseArguments(cmdline, argv);

	// Ignore blank input
	if (argv[0] == NULL) { return; }
	
	// Add the command to the history
	add_history_entry(cmdline);

	/* Internal commands */
	// exit
	if (strncmp(argv[0], "exit", 4) == 0) {
		if (argv[1] != NULL) {
			printf("Usage: exit\n");
			return;
		}

		// Exit the shell
		printf("\nThe Tiny Torero Shell is exiting now. Goodbye!\n");
		exit(0);
	}
	// history
	else if (strncmp(argv[0], "history", MAXLINE) == 0) {
		if (argv[1] != NULL) {
			printf("Usage: history\n");
			return;
		}
		
		// Print the history
		print_history();
	}
	
	/* External commands */
	else {
		// Create a child process
		pid_t pid = fork();

		if (pid == 0) { // Child
			execvp(argv[0], argv);
			
			// Only occurs if execvp returns
			printf("ERROR: (%s) command not found\n", argv[0]);
			exit(1);	
		}
		else if (pid > 0) { // Parent
			// Run the process in the foreground
			if (background == 0) {
				// Wait for the child process to finish
				int status;
				waitpid(pid, &status, 0);

				// The child exited normally
				if (WIFEXITED(status)) {
					return;
				}
				
				printf("Process exited abnormally\n");
			}
			// Run the process in the background
			else {
				// Do not wait for the child
				return;
			}
		}
		else {
			printf("There was an error creating the process\n");
		}
	}
}

/**
 * Handler for when a child finishes
 *
 * @param sig Unused
 */
void sigchld_handler(__attribute__ ((unused)) int sig) {
	int status;
	pid_t pid;
	while( (pid = waitpid(-1, &status, WNOHANG)) > 0) {
	/* uncomment debug print stmt to see what is being handled
	 *         printf("signal %d me:%d child: %d\n", signum, getpid(), pid);
	 *                  */
	}
}
