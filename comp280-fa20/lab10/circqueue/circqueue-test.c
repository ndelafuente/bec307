#include <stdio.h>
#include <stdlib.h>

// To use the circqueue library, we need the forward declarations that were
// defined in the circqueue library header file.
#include "circqueue.h"

#define NUM_TO_ADD 3

/**
 * A tester for our circqueue library.
 * It fills the queue with 0 to QUEUE_CAPACITY-1 then asks the user to enter
 * NUM_TO_ADD more numbers.
 */
int main() {
	// some circular queue test code

	printf("Queue at beginning:\n");
	print_queue();

	for(int i = 0; i < QUEUE_CAPACITY-1; i++) {
		printf("Adding %d to the queue\n", i);
		add_queue(i);
		print_queue();
	}

	int i = 0;
	while (i < NUM_TO_ADD) {
		// Prompts the user for an integer then adds this value to the queue.
		printf("Enter an integer: ");
		char input[100];
		fgets(input, 100, stdin);

		int val;
		int num_scanned = sscanf(input, "%d", &val);
		if (num_scanned == 1) {
			printf("Adding %d to the queue\n", val);
			add_queue(val);
			print_queue();
			i++;
		}
		else {
			printf("I thought I told you to enter an integer (╯°□°）╯︵ ┻━┻\n");
		}
	}

	return 0;
}
