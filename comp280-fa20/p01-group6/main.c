/**
 * Employee database program.
 */

#include <stdlib.h>
#include <stdio.h>

#include "readfile.h"
#include "employee_db.h"

// forward declaration of functions
void read_and_print(char *filename);

int main(int argc, char *argv[]) {
	// Hell is short for Heavenly Employees Large List
	printf("\nWelcome to Hell, the world's hottest database app!\n");
	printf("Enter ':q' at any time to quit\n\n");
	printf("Please feel free to try any input (e.g. spaces, letter, arrow keys)\n\n");

	// Initialize the filename string from the command line arguments
	char filename[MAX_FILENAME];
	parse_filename(filename, argc, argv);

	Employee employees[MAX_EMPLOYEE];
	int num_employees = read_employees_from_file(filename, employees);

	int done = 0;
	while (done == 0) {
		int selection = get_menu_selection();

		if (selection == 1) {
			// Print the employee DB
			print_db(employees, num_employees);
		}
		else if (selection == 2) {
			// Lookup an employee by ID
			lookup_by_id(employees, num_employees);
		}
		else if (selection == 3) {
			// Lookup an employee by Last Name
			lookup_by_last_name(employees, num_employees);
		}
		else if (selection == 4) {
			// Add an employee to the DB
			// Increments num_employees if the user confirmed the add
			num_employees += add_employee(employees, num_employees);
		}
		else {
			// Exit the program
			done = 1;
		}
	}

	printf("\n\nThank you for visiting Hell. Be sure to live a good life!\n\n");
	return 0;
}


/**
 * This is a function to test the readfile library on a DB input file: you will
 * not really call this in your program, but use it as an example of how to use
 * the readfile library functions.
 *
 * @note You will not use this function in your final program. It is here simply
 * as a reference for how to get started using the readfile library.
 *
 * @param filename the filename to open and read
 */
void read_and_print(char *filename) {
	printf("filename '%s'\n", filename);

	int ret = open_file(filename);  // try to open the DB file
	if (ret == -1) {
		printf("bad error: can't open %s\n", filename);
		exit(1);
	}

	int id = -1;
	int salary = -1;
	char fname[MAX_NAME], lname[MAX_NAME];

	while (ret != -1) {    
		ret = read_int(&id);         // read the first line of values from file
		if (ret) { break; }
		ret = read_string(fname);
		if (ret) { break; }
		ret = read_string(lname);
		if (ret) { break; }
		ret = read_int(&salary);
		if (ret == 0) { // stuff was read in okay
			printf("%d %s %s %d\n", id, fname, lname, salary);

		}
	}

	close_file();  // when all done close the file
}
