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
#define MAX_FILENAME  128    // The maximum length of a filename
#define MAX_NAME      64     // The maximum length of an employee's name
#define MAX_INT_LEN   32     // The maximum length for a string
#define MAX_EMPLOYEE  1024   // The maximum number of employees that can be stored
#define DISPLAY_WIDTH 70     // The width of the display output in characters
#define MIN_ID        100000 // The minimum ID number
#define MAX_ID        999999 // The maximum ID number
#define MIN_SAL       30000  // The minimum salary amount
#define MAX_SAL       150000 // The maximum salary amount

/**
 * A simple representation of an employee.
 */
struct Employee {
	char first_name[MAX_NAME];
	char last_name[MAX_NAME];
	int id;
	int salary;
};

// The following line allows us to use "Employee" rather than 
// "struct Employee" throughout this code.
typedef struct Employee Employee;


/**
 * Reads employee data from a file with the given filename, adding them into the
 * given list of employees sorting by ID number.
 *
 * @note There should never be more than MAX_EMPLOYEES in the file.
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
 * If the user enters an invalid selection, they are be reprompted.
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
 * Lets the user search the employees database for an employee's id and prints
 * their information if an entry is found, and a message saying that it was not
 * found if not.
 * 
 * @note Uses binary search.
 * 
 * @param employees The list of employees.
 * @param num_employees The number of employees in the list. 
 */
void lookup_by_id(Employee employees[], int num_employees);

/**
 * Lets the user search the employees database for an employee's last name and
 * prints their information if an entry is found, and a message saying that it
 * was not found if not.
 * 
 * @note Uses linear search.
 * 
 * @param employees The list of employees.
 * @param num_employees The number of employees in the list. 
 */
void lookup_by_last_name(Employee employees[], int num_employees);

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

/**
 * Adds a new employee into the employee database while sorting by ID number.
 * 
 * @param employees The list of employees.
 * @param new_e The new employee.
 * @param num_employees The number of employees in the list.
 */
void sorted_insert(Employee employees[], Employee new_e, int num_employees);

/**
 * Prints the column name for the output.
 */
void print_output_header();

/**
 * Prints the horizontal line of length len.
 * 
 * @param len The length of the line.
 */
void print_horizontal_line(int len);

/**
 * Prints the employee's name, salary and ID in a formatted row.
 * 
 * @param e The employee object.
 */
void print_employee(Employee e);

/**
 * Gets an integer from the user between max and min.
 * 
 * @param message A message describing the requested input.
 * @param min The lower bound for an acceptable input.
 * @param max The upper bound for an acceptable input.
 * 
 * @return The user's valid integer.
 */
int get_valid_int(char message[], int min, int max);

/**
 * Converts a string to an integer.
 * 
 * @param str The string to be converted.
 * @param flag A variable to indicate the success of the conversion.
 * 
 * @return The converted integer value.
 */
int convert_str_to_int(char str[], char flag[]);

/**
 * Gets a string that is and less than max_size in length from the user. 
 * Exits the program if they enter ':q'
 * 
 * @param message A message describing the requested input.
 * @param str The variable that will store the user input.
 * @param max_size The maximum size for the string.
 */
void get_valid_str(char message[], char str[], int max_size); 

/**
 * Gets the next line of input.
 * 
 * @param s The variable that will be filled with the input.
 * @param max_size The maximum size allowed for the line.
 * 
 * @return The length of the user input (may be longer than s).
 */
int scan_line(char s[], int max_size);

/**
 * Implements a recursive binary search to find the specified id in the list of
 * employees.
 * 
 * @note The list must be sorted by id.
 * 
 * @param employees the employee database
 * @param start the index at which to start the search
 * @param stop the index at which to stop the search
 * @param id the id that is being searched for
 * 
 * @return the index of the employee with the matching id or -1 if not found
 */
int binary_lookup(Employee employees[], int start, int stop, int id);

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
 * The compiler kept producing an error saying that strnlen was
 * being implicitly declared
 */
size_t strnlen(const char *s, size_t maxlen);

#endif
