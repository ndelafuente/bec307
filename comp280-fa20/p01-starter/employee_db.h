#ifndef _EMPLOYEEDB_H_
#define _EMPLOYEEDB_H_
/*
 * Header file that defines our Employee DB's structs and declares functions to
 * be used by our application.
 *
 * Note: Having a separate module for the main function and everything else will
 * allow us to write automated tests for our functions.
 */

// Global constant definitions
#define MAXFILENAME 128
#define MAXNAME 64
#define MAXEMPLOYEE	1024

/**
 * A simple representation of an employee.
 */
struct Employee {
	char first_name[MAXNAME];
	char last_name[MAXNAME];
	int id;
	int salary;
};

// The following line allows us to use "Employee" rather than 
// "struct Employee" throughout this code.
typedef struct Employee Employee;


/**
 *  This function gets the filename passed in as a command line option
 *  and copies it into the filename parameter. It exits with an error 
 *  message if the command line is badly formed.
 *
 *  @param filename The string to fill with the passed filename.
 *  @param argc The number of command line arguments.
 *  @param argv The actual command line parameters.
 */
void parse_filename(char filename[], int argc, char *argv[]);

/**
 * Reads employee data from a file with the given filename, adding them into the
 * given list of employees.
 *
 * @note You may assume there will never be more than MAXEMPLOYEE employees in
 * the given file.
 *
 * @param filename The name of the file with the employee data.
 * @param employees An array where employee data will be stored. The function
 *   will overwrite the original contents of this array.
 *
 * @return The number of employees added to the list.
 */
int read_employees_from_file(char *filename, Employee employees[]);

/**
 * Prints menu of options and prompts user to enter a selection.
 * If the user enters an invalid selection, they should be reprompted.
 *
 * @return The user's validated selection.
 */
int get_menu_selection();

/**
 * Prints a nicely formatted version of the employee list.
 *
 * @param employees The list of employees.
 * @param num_employees The number of employees in the list.
 */
void print_db(Employee employees[], int num_employees);

/**
 * Prompts user to enter data for a new employee and then adds that user to the
 * list of employees.
 *
 * @note The user may decide they don't want to add an employee so this function
 * doesn't guarantee a new user will be added to the list. The return value will
 * indicate whether a new employee was added or not.
 *
 * @param employees The list of employees.
 * @param num_employees The number of employees in the list.
 *
 * @return True (i.e. 1) if a user was added, False (0) otherwise.
 */
int add_employee(Employee employees[], int num_employees);

// TODO: Add a function header comment, following the same style as shown in the
// given function comment
void lookup_by_id(Employee employees[], int num_employees);

// TODO: Add a function header comment, following the same style as shown in the
// given function comment
void lookup_by_last_name(Employee employees[], int num_employees);

#endif
