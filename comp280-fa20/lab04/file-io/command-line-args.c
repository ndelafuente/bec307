/**
 * An example of command line arguments in C
 * 
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/**
 * @param argc A count of the number of command line arguments, the executable 
 *       name (command) is included so argc is at least 1 for every program
 * @param argv An array of strings, there is one string for each command line arg
 */
int main(int argc, char *argv[]) {

	// print out the argv strings:
	int i;
	for (i=0; i < argc; i++) {
		printf("Command line arg %2d: %s\n", i, argv[i]);
	}
	printf("\n");

	// often times we want to check the the user passed in the correct number
	// of command line arguments, if not, usually print an error message with
	// some information on how to call this execuatable (what command line
	// options it expects, and in which order), and then exit:
	if (argc != 5) {
		printf("usage: ./prog name age id_num gpa\n");
		exit(1);
	}

	// strtol, strtoll, and strtod convert a string of numeric characters to
	// the equivalent long, long long, or double value
	long int age = strtol(argv[2], NULL, 10);
	long long int id = strtoll(argv[3], NULL, 10);
	double gpa = strtod(argv[4], NULL);

	// Let's copy the name argument to a dynamically allocated 
	// string and modify it  (just to illustrate malloc, strlen, strcpy)

	char *name = calloc( (strlen(argv[1])+1), sizeof(char) );
	if (!name) { 
		printf("Error: calloc failed....\n");
		exit(1);
	} 

	if (strcpy(name, argv[1]) != NULL) { 
		printf("name: %s age: %ld id: %lld gpa: %3.2f\n", name, age, id, gpa);
	} else { 
		printf("Error: strcpy failed....\n");
	}

	free(name);
	name = NULL;

	return 0;
}
