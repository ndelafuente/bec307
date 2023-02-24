#include <stdio.h>

int main(int argc, char **argv) {
	if (argc != 3) {
		printf("usage: %s <grades_file> <average_file>\n", argv[0]);
	}

	// Step 1: open files
	// argv[1] should contain the input filename (with raw grades)
	// argv[2] should contain the output filename (with average grades)
	
	// Step 2: read and write data
	// You should use the fprintf and fscanf functions for file I/O
	
	// Note that when reading in a string with "%s" in fscanf, it will stop
	// the string when it reaches a space character (i.e. ' ').
	
	// Also note that fscanf will return EOF when it reaches the end of the
	// file. You can use this to determine when to stop reading.

	// Step 3: close files
	
	// Remember to free any memory that you have allocated from the heap.
	// It's ok if you didn't use any heap memory though.

	return 0;
}
