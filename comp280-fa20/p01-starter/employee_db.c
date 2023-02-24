/**
 * TODO: ADD YOUR TOP-LEVEL COMMENT HERE
 *
 * And, remove all my comments to you...I should not see any TODO's in your
 * submitted code.
 */

#include <stdio.h>  // the C standard I/O library
#include <stdlib.h> // the C standard library
#include <string.h> // the C string library

// Include headers that are unique to this program
#include "readfile.h"  // file reading routines
#include "employee_db.h" // DB program functions
 

// TODO: Add forward declaration(s) of any new functions you write here.



int read_employees_from_file(char *filename, Employee employees[]) {
	// Dummy implementation: creates two fake employees.
	// TODO: Replace this dummy implementation with a real implementation, using
	// the read_and_print function (from main.c) to help you with reading the
	// employees.
	
	// set first employee
	employees[0].id = 111111;
	employees[0].salary = 55000;
	strncpy(employees[0].first_name, "Joe", MAXNAME);
	strncpy(employees[0].last_name, "Exotic", MAXNAME);

	// set second employee
	employees[1].id = 222222;
	employees[1].salary = 33000;
	strncpy(employees[1].first_name, "Carole", MAXNAME);
	strncpy(employees[1].last_name, "Baskin", MAXNAME);

	return 2;
}

int get_menu_selection() {
	printf("\nEmployee DB Menu:\n");
	printf("-----------------------------------\n");
	printf("  (1) Print the Database.\n");
	printf("  (2) Lookup by ID.\n");
	printf("  (3) Lookup by Last Name.\n");
	printf("  (4) Add an Employee.\n");
	printf("  (5) Quit.\n");
	printf("-----------------------------------\n");

	// TODO: Implement this function
	
	return -1;
}


void print_db(Employee employees[], int num_employees) {
	printf("Printing is not yet supported!\n");

	// TODO: Implement this function
}


int add_employee(Employee employees[], int num_employees) {
	printf("Adding an employee is not yet supported!\n");

	// TODO: Implement this function
}


void lookup_by_id(Employee employees[], int num_employees) {
	printf("Looking up an employee by ID is not yet supported!\n");

	// TODO: Implement this function
}


void lookup_by_last_name(Employee employees[], int num_employees) {
	printf("Looking up an employee by last Name is not yet supported!\n");

	// TODO: Implement this function
}


// TODO: ADD ANY NEW FUNCTION _DEFINITONS_ BELOW THIS LINE



// DO NOT MODIFY ANY OF THE CODE BELOW THIS POINT!!!

void parse_filename(char filename[], int argc, char *argv[]) {

	if (argc != 2) {
		printf("Usage: %s database_file\n", argv[0]);
		// exit function: quits the program immediately...some errors are not 
		// recoverable by the program, so exiting with an error message is 
		// reasonable error handling option in this case
		exit(1);   
	}
	if (strlen(argv[1]) >= MAXFILENAME) { 
		printf("Filename, %s, is too long, cp to shorter name and try again\n",
				filename);
		exit(1);
	}
	strcpy(filename, argv[1]);
}
