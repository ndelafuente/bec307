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
 * Prints the given world using the ncurses UI library.
 *
 * @param world The world to print.
 * @param width The width of the world.
 * @param height The height of the world.
 */
void print_world(int *world, int width, int height);

#endif
