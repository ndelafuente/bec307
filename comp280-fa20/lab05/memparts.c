/*
 * File: memparts.c
 *
 * Program that prints out addresses in different parts of the memory.
 *
 */

#include <stdio.h>
#include <stdlib.h>

// This is a global variable.
// Remember: Do not use globals unless told to do so.
int a_global_variable;

int main() {
	// local variables
	int *a_pointer = NULL;
	int a_local_variable = 6;

	a_global_variable = 10;

	// let's see some memory addresses:
	printf("Address of a_global_variable: %p\n", 
			&a_global_variable);
	printf("Address of a_local_variable:  %p\n", 
			&a_local_variable);
	printf("Address of a_pointer (a local var on the stack):  %p\n",
			&a_pointer);
	printf("Address of the main function in memory %p\n\n",
			*main);

	a_pointer = &a_global_variable;
	printf("Address a_pointer points to (a_global_variable):  %p\n",
			a_pointer);

	a_pointer = malloc(sizeof(int)*3);
	if (a_pointer) { 
		printf("Address a_pointer points to (on the heap):  %p\n", a_pointer);
		int i;
		for(i=0; i< 3; i++) {
			a_pointer[i] = i+10;
		}
		printf("The value stored in what a_pointer points to:  %d\n", *a_pointer);
	}

	free(a_pointer);
	a_pointer = NULL;

	return 0;
}
