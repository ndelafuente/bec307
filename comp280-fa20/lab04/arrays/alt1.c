/**
 * This program shows examples of two ways of declaring "2D" arrays
 */

#include <stdio.h>
#include <stdlib.h>

#define NUM_ROWS  4 
#define NUM_COLS  2 

// Function to print addresses of all elements in 3 types of 2D arrays
void print_arr1_addresses(int array[][NUM_COLS], int rows) ;
void print_arr2_addresses(int **array, int rows, int cols) ;


int main() {

	// A statically allocated 2D array with NUM_ROWS rows and NUM_COLS
	// columns. By "static" we mean that the size is known even before we run
	// the program.
	int arr1[NUM_ROWS][NUM_COLS];  
	print_arr1_addresses(arr1, NUM_ROWS); 

	// a dynamically allocated "2D" array
	int **arr2 = NULL;

	// (1) statically declared: let's see how these are laid out in memory


	// Let's allocate memory for this 2D array.
	// First: malloc 1 array of NUM_ROWS int *'s, one for each row
	//  Next: malloc NUM_ROWS arrays of NUM_COLS ints, one for each row of values 
	arr2 = calloc(NUM_ROWS, sizeof(int *));
	if(!arr2) {
		printf("malloc arr2 failed\n");
		exit(1);
	}

	for(int i=0; i < NUM_ROWS; i++) {
		// next: malloc up space for each row's set of int values
		arr2[i] = calloc(NUM_COLS, sizeof(int));
		if(!arr2[i]) {
			printf("malloc arr2 failed\n");
			exit(1);
		}
	}
	// let's see how these are laid out in memory
	print_arr2_addresses(arr2, NUM_ROWS, NUM_COLS);


	// Can't forget to free our allocated heap memory.
	for(int i=0; i < NUM_ROWS; i++) {
		free(arr2[i]); 
	}
	free(arr2);

	return 0;
}


/**
 * Print out the addresses of each element in the passed
 * statically declared array
 *
 * @param array The array of ints
 * @param rows The number of rows in the passed array
 */
void print_arr1_addresses(int array[][NUM_COLS], int rows) {

	printf("Statically Declared 2D Array addresses:\n");
	printf("-------------------------------------------------\n");
	for(int i = 0; i < rows; i++) {
		for(int j = 0; j < NUM_COLS; j++) {
			printf("[%d][%d]:%p  ", i, j, &array[i][j]);
		}
		printf("\n");
	}
	printf("\n");
}



/**
 * Print out the addresses of each element in the passed
 * dynamically declared 2D array (the array of arrays version)
 *
 * @param array The array of ints
 * @param rows The number of rows in the passed array
 * @param cols The number of rows in the passed array
 */
void print_arr2_addresses(int **array, int rows, int cols) {

	printf("Dynamically allocateed 2D array (as array of arrays) addresses:\n");
	printf("---------------------------------------------------------------\n");
	for(int i = 0; i < rows; i++) {
		for(int j = 0; j < cols; j++) {
			// note: the difference in how we are accessing array elements 
			// in this versionf from the other dynmaically allocated
			// 2D array (as one big malloc) in print_dynamic_addrs
			printf("[%d][%d]: %p  ", i, j, &array[i][j]);
		}
		printf("\n");
	}
}
