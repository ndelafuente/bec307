/* *
 * employee_db.c
 *
 * This file contains the code to create an employee database from a file
 * which can be searched, printed, and edited through an interactive menu.
 * To call this program on a file, use: employee_db <Input_File.txt>
 *
 * This file is part of COMP 280, Project #01
 *
 * Authors:
 *  1. Phuong Mai (pmai@sandiego.edu)
 *  2. Nicolas de la Fuente (ndelafuente@sandiego.edu)
 *
 * Last updated: September 03, 2020
 * */

#include <stdio.h>      // the C standard I/O library
#include <stdlib.h>     // the C standard library
#include <string.h>     // the C string library

// Include headers that are unique to this program
#include "readfile.h"  // file reading routines
#include "employee_db.h" // DB program functions


int read_employees_from_file(char *filename, Employee employees[]) {
	printf("Initializing database from file: '%s'\n", filename);

	int ret = open_file(filename);  // try to open the DB file
	if (ret == -1) {
		printf("bad error: can't open %s\n", filename);
		exit(1);
	}
	
	Employee e = {"", "", -1, -1};
	int num_employees = 0;
	while (ret != -1) {    
		ret = read_int(&e.id);         // read the first line of values from file
		if (ret) { break; }
		ret = read_string(e.first_name);
		if (ret) { break; }
		ret = read_string(e.last_name);
		if (ret) { break; }
		ret = read_int(&e.salary);
		if (ret == 0) { // stuff was read in okay
			// Inserts the employee into the database
			sorted_insert(employees, e, num_employees);
			num_employees++;
		}
	}

	close_file();  // when all done close the file
	return num_employees;
}

int get_menu_selection() {
	printf("\nHell's Menu:\n");
	print_horizontal_line(35);
	printf("  (1) Print the Database.\n");
	printf("  (2) Lookup by ID.\n");
	printf("  (3) Lookup by Last Name.\n");
	printf("  (4) Add an Employee.\n");
	printf("  (5) Quit.\n");
	print_horizontal_line(35);

	return get_valid_int("Enter your choice", 1, 5);
}

void print_db(Employee employees[], int num_employees) {
	print_output_header();
	for (int i = 0; i < num_employees; i++)
		print_employee(employees[i]);
	print_horizontal_line(DISPLAY_WIDTH);
	printf("Number of employees: (%d)\n", num_employees);
}

void lookup_by_id(Employee employees[], int num_employees) {
	int id = get_valid_int("Enter a 6-digit employee id", MIN_ID, MAX_ID);
	int match_index = binary_lookup(employees, 0, num_employees - 1, id);
	if (match_index == -1)
		printf("Employee with id %d not found in DB\n", id);
	else {
		print_output_header();
		print_employee(employees[match_index]);
	}
}

void lookup_by_last_name(Employee employees[], int num_employees) {
	char last_name[MAX_NAME] = "";
	get_valid_str("Enter a last name", last_name, MAX_NAME);
	
	int i;
	for (i = 0; i < num_employees; i++) {
		if (strncmp(employees[i].last_name, last_name, MAX_NAME) == 0) {
			print_output_header();
			print_employee(employees[i]);
			break;
		}
	}
	if (i == num_employees)
		printf("Employee with last name '%s' not found in DB\n", last_name);
			 
}

int add_employee(Employee employees[], int num_employees) {
	Employee new_e = {"", "", 0, 0};

	// Get the employee information
	get_valid_str("Enter the first name of the employee", new_e.first_name, MAX_NAME);
	get_valid_str("Enter the last name of the employee", new_e.last_name, MAX_NAME);
	new_e.id = get_valid_int("Enter a 6-digit employee id num", 
					MIN_ID, MAX_ID);
	new_e.salary = get_valid_int("Enter the employee's salary (30000 to 150000)",
					MIN_SAL, MAX_SAL);
	
	// Confirm that the user wants to complete the add
	printf("The following employee will be added to the database:\n");
	printf("\tName: %s %s\n", new_e.first_name, new_e.last_name);
	printf("\tID: %d\n", new_e.id);
	printf("\tSalary: $%d\n", new_e.salary);
	int confirmation = get_valid_int("Enter '1' to confirm or '0' "
										"to cancel the add", 0, 1);
	if (confirmation == 1) {
		printf("New employee will be added to the database.\n");
		sorted_insert(employees, new_e, num_employees);
	}
	else {
		printf("Employee add cancelled!\n");
	}
	return confirmation;
}

void sorted_insert(Employee employees[], Employee new_e, int num_employees){
	// Places the new employee at the end of the list
	employees[num_employees] = new_e;
	num_employees++;

	// Uses an insertion sort algorithm to sort the list by id
    for (int i = 1; i < num_employees; i++) 
    {  
        Employee key = employees[i];
		int j;
        for (j = i - 1; j >= 0 && employees[j].id > key.id; j--) {
        	employees[j + 1] = employees[j];
        }
		employees[j + 1] = key;
    } 
}

void print_output_header() {
	printf("\nNAME                                               SALARY    ID\n");
	print_horizontal_line(DISPLAY_WIDTH);
}

void print_horizontal_line(int len) {
	for(int i = 0; i < len; i++)
		printf("-");
	printf("\n");
}

void print_employee(Employee e) {
	printf("%-25s%-26s%6d%10d\n", e.first_name, e.last_name, e.salary, e.id);
}

int get_valid_int(char message[], int min, int max) {
	int int_val = 0;
	int done = 0;
	while (done == 0) { // infinite loop
		char input_str[MAX_INT_LEN] = "";
		get_valid_str(message, input_str, MAX_INT_LEN);

		// Parses the string input to get an integer
		char flag[] = "SUCCESS"; 
		int_val = convert_str_to_int(input_str, flag);

		// Exit the while loop if the input is a valid integer
		if (strcmp(flag, "SUCCESS") == 0 && int_val >= min && int_val <= max)
			done = 1;
		// Print a message if the input is invalid and reprompt the user
		else {
			printf("Invalid input (%s", input_str);
			printf("). Valid values are %d through %d\n", min, max);
		}
	}
	return int_val;
}

int convert_str_to_int(char str[], char flag[]) {
	double val = 0; // use double to catch decimal point input
	int len = strlen(str);
	for (int i = 0; i < len; i++) {
		// If the character is a number
		if (str[i] >= '0' && str[i] <= '9') {
			val *= 10; // shift the value up one digit
			val += (int)str[i] - 48; // convert the ascii number to an int
		}
		else { 
			strcpy(flag, "FAILURE"); // indicate that the conversion failed
			break; // exit the loop
		}
	}
	return (int)val;
}

void get_valid_str(char message[], char str[], int max_size) {
	int done = 0;
	while (done == 0) {
		printf("%s: ", message);
		int len = scan_line(str, max_size); // get the user's input
		
		if (len > max_size)
			printf("Input is too large (%d). Max length is %d\n", len, max_size);
		else if (len > 0)
			done = 1;
	}
	// Exit the program if input was ':q' 
	if (strncmp(str, ":q", 2) == 0)
		exit(0);
}

int scan_line(char s[], int max_size) {
	strncpy(s, "", 1);
	// Ignore leading spaces
	char c = ' ';
	while (c == ' ') {
		scanf("%c", &c);
	}
	// Read all the characters up to the new line ('\n')
	int stream_len = 0;
	while (c != '\n') {
		if (stream_len < max_size - 1 && c >= ' ')
			strncat(s, &c, 1);
		scanf("%c", &c);
		stream_len++;
	}
	// Remove trailing spaces
	for (int i = strnlen(s, max_size) - 1; i >= 0 && s[i] == ' '; i--)
		s[i] = '\0';

	return stream_len;
}

int binary_lookup(Employee employees[], int start, int stop, int id) {
	// Recursively search the half of the list that the id might be in
	if (stop >= start) {
		int mid = start + (stop - start) / 2;
		if (employees[mid].id == id)
			return mid;
		else if (employees[mid].id > id) // check the first half of the array
			return binary_lookup(employees, start, mid - 1, id);
		else // check the latter half of the array
			return binary_lookup(employees, mid + 1, stop, id);
	}
	return -1;
}

void parse_filename(char filename[], int argc, char *argv[]) {

	if (argc != 2) {
		printf("Usage: %s database_file\n", argv[0]);
		// exit function: quits the program immediately...some errors are not 
		// recoverable by the program, so exiting with an error message is 
		// reasonable error handling option in this case
		exit(1);   
	}
	if (strlen(argv[1]) >= MAX_FILENAME) { 
		printf("Filename, %s, is too long, cp to shorter name and try again\n",
				filename);
		exit(1);
	}
	strcpy(filename, argv[1]);
}
