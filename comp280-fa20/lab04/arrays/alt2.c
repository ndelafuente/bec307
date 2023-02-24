#include <stdio.h>
#include <stdlib.h>

void mystery_func(int *arr, int num_rows, int num_cols);
void print_array(int *arr, int num_rows, int num_cols);
int translate_to_1d(int row, int col, int num_rows, int num_cols);

int main(int argc, char *argv[]) {

	if (argc != 3) {
		printf("Need to give row then column when running.\n");
		exit(1);
	}

	// Get the number of rows and columns from the command line parameters
	// entered by the user (which are stored in the argv array).
	int num_rows = strtol(argv[1], NULL, 10); // strtol converts a string to a long int
	int num_cols = strtol(argv[2], NULL, 10);

	int *array = calloc(num_rows*num_cols, sizeof(int));

	mystery_func(array, num_rows, num_cols);
	print_array(array, num_rows, num_cols);

	return 0;
}

/**
 * Does something "mysterious"
 *
 * @param arr An array of integers.
 * @param num_rows The number of rows in the array
 * @param num_cols The number of columnss in the array
 */
void mystery_func(int *arr, int num_rows, int num_cols) {
	for (int i = 0; i < num_rows*num_cols; i++)
		arr[i] = i;
}

/**
 * Translates a 2D set of indexs (i.e. row/col), determine the index in the 1D
 * array where it would be located.
 *
 * @return The index in the 1D array where arr[row][col] would be located.
 */
int translate_to_1d(int row, int col, int num_rows, int num_cols) {
	// TODO: implement this function, returning the correct index
	// where (row,col) is located in the 1D version of the array.
	return 0;
}

/**
 * Prints the array with the given number of rows and columns.
 *
 * @param arr The array to print.
 * @param num_rows The number of rows in the array
 * @param num_cols The number of columnss in the array
 */
void print_array(int *arr, int num_rows, int num_cols) {
	for (int i = 0; i < num_rows; i++) {
		for (int j = 0; j < num_cols; j++) {
			int index = translate_to_1d(i, j, num_rows, num_cols);
			printf("arr[%d][%d] = %d\n", i, j, arr[index]);
		}
	}
}
