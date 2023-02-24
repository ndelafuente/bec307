#ifndef __GOL_H__
#define __GOL_H__
/**
 * File: gol.h
 *
 * Header file of the game of life simulator functions.
 */

/**
 * Creates an initializes the world based on the given configuration file.
 *
 * @param config_filename The name of the file containing the simulation
 *    configuration data (e.g. world dimensions)
 * @param width Location where to store the width of the world.
 * @param height Location where to store the height of the world.
 *
 * @return A 1D array representing the created/initialized world, or NULL if
 *   if there was an problem with initialization.
 */
int *initialize_world(char *config_filename, int *width, int *height);

/**
 * Updates the world for one step of simulation, based on the rules of the
 * game of life.
 *
 * @param world The world to update.
 * @param width The width of the world.
 * @param height The height of the world.
 */
void update_world(int *world, int width, int height);

/**
 * Update a cell for one step of simulation, based on the rules of the
 * game of life.
 * 
 * @param cell The state of the cell to be updated.
 * @param num_neighbors The number of alive neighbors that the cell has.
 * 
 * @return The updated cell state.
 */
int update_cell(int cell, int num_neighbors);

/**
 * Counts the number of alive neighbors a cell has
 * 
 * @param world The world.
 * @param row The row of the cell.
 * @param col The column of the cell.
 * @param num_rows The number of rows in the 2D array.
 * @param num_cols The number of columnss in the 2D array.
 * 
 * @return The number of neighbors.
 */
int count_neighbors(int *world, int row, int col, int num_rows, int num_cols);

/**
 * Prints the given world using the ncurses UI library.
 *
 * @param world The world to print.
 * @param width The width of the world.
 * @param height The height of the world.
 * @param turn The current turn number.
 */
void print_world(int *world, int width, int height, int turn);


            // ARRAY FUNCTIONS //
/**
 * Translates a pair of coordinates into the appropriate index for a 1D array.
 * 
 * @param row The row of the coordinate.
 * @param col The column of the coordinate.
 * @param width The width of the 2D array.
 * 
 * @return The corresponding index for a one-dimensional array.
 */
int translate_to_1d(int row, int col, int width);

/**
 * Wrap an integer around an upper limit.
 * 
 * @param n The integer.
 * @param limit The upper limit.
 * 
 * @return The wrapped integer.
 */
int wrap(int n, int limit);

/**
 * Creates a copy of an array.
 * 
 * @param arr The array to copy.
 * @param width The width of the array.
 * @param height The height of the array.
 * 
 * @return A copy of the array.
 */
int *copy(int *arr, int width, int height);


            // OUTPUT FUNCTIONS //
/**
 * Adds a string followed by an integer to the screen.
 * 
 * @param row The row for the output.
 * @param col The column for the output.
 * @param s The string.
 * @param n The integer.
 * 
 * @return OK if operations went smoothly, ERR if not
 */
int mvadd_str_int(int row, int col, char *s, int n);

/**
 * Adds an integer to the screen.
 * 
 * @param row The row for the output.
 * @param col The column for the output.
 * @param n The integer.
 * 
 * @return OK if operations went smoothly, ERR if not
 */
int mvaddint(int row, int col, int n);

/**
 * Counts the number of digits in an integer.
 * 
 * @param n The integer.
 * @return The number of digits.
 */
int num_digits(int n);

            // EXCEPTION FUNCTION //
/**
 * "Raise" an "exception", print an error message, and exit the program.
 * 
 * @param ex The "exception" name.
 * @param opt A description of what caused the exception.
 */
void raise_exception(char ex, char *opt);

#endif
