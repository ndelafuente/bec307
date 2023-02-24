/**
 * Program with a segmentation fault.
 * 
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>


/**
 * Initializes a given array.
 *
 * @param array The array to initialize.
 * @param len The number of elements in the array.
 */
void init_array(int *array, int len) {
	for (int i = 0; i < len; i++) {
		array[i] = i;
	}
}

/**
 * Function that returns the max value in the given array.
 * 
 * @param array The array to work with.
 * @param len The number of elements in the array.
 *
 * @return The maximum value in the array.
 */
int get_max_in_array(int *array, int len) {
	int max = array[0];
	for (int i = 1; i < len; i++) {
		if (max < array[i]) { 
			max = array[i];
		}
	}

	return max;
}

int main() {
	int *arr = NULL;

	init_array(arr, 100);
	int max = get_max_in_array(arr, 100);
	printf("max value in the array is %d\n", max);

	return 0;
}
