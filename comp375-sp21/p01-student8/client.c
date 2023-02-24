/**
 * client.c
 *
 * @author Nicolas de la Fuente 
 *
 * USD COMP 375: Computer Networks
 * Project 1
 *
 * A replacement for your favorite weather channel ;)
 * Accesses high tech weather balloon miles above San Diego
 * to provide only the most accurate of weather data formatted
 * for your viewing pleasure.
 */

#define _XOPEN_SOURCE 600
#define BUFF_SIZE 1024
#

#include <errno.h>
#include <limits.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <time.h>

long prompt();
int connectToHost(char *hostname, char *port);
void mainLoop(int server_fd);
void weatherBoy(int weather_fd, char *command);
int hopperAuth(int server_fd); 

int main() {
	int server_fd = connectToHost("hopper.sandiego.edu", "7030");	
		
	mainLoop(server_fd);
	close(server_fd);
	return 0;
}

/**
 * Loop to keep asking user what they want to do and calling the appropriate
 * function to handle the selection.
 *
 * @param server_fd Socket file descriptor for communicating with the server
 */
void mainLoop(int server_fd) {
	printf("WELCOME TO THE COMP375 SENSOR NETWORK\n\n\n");
	
	char command[BUFF_SIZE];
	while (1) {
		long selection = prompt();

		memset(command, 0, BUFF_SIZE);
		// Handle the user input
		switch (selection) {
			case 1:
				strcpy(command, "AIR TEMPERATURE\n");

				break;
			case 2:
				strcpy(command, "RELATIVE HUMIDITY\n");

				break;
			case 3:
				strcpy(command, "WIND SPEED\n");

				break;
			case 4:
				// Close the connection
				close(server_fd);

				// Exit the program with *style*
				printf("GOODBYE!\n");
				exit(0);

			default:
				fprintf(stderr, "ERROR: Invalid selection\n");
				break;
		}
		// Execute the appropriate command on the weather station
		weatherBoy(server_fd, command);
		
		// Close the connection and reconnect
		close(server_fd);
		server_fd = connectToHost("hopper.sandiego.edu", "7030");
	}
}

/**
 * Request information from the weather server and parse its response
 *
 * @param weather_fd The socket to send the request to
 * @param command The command to send.
 */
void weatherBoy(int server_fd, char *command) {
	// Access the weather station server
	int weather_fd = hopperAuth(server_fd);

	// Create the buffer
	char buff[BUFF_SIZE];
	memset(buff, 0, BUFF_SIZE);

	// Send the authentication to the weatherstation server
	send(weather_fd, "AUTH sensorpass321\n", 19, 0);
	recv(weather_fd, buff, BUFF_SIZE, 0);
	
	// Send the command
	send(weather_fd, command, sizeof(command), 0);

	// Get the server's response
	memset(buff, 0, BUFF_SIZE);
	recv(weather_fd, buff, BUFF_SIZE, 0);

	// Parse the information from the response
	time_t time = 0;
	int info = 0;
	char unit[16];
	memset(unit, 0, 16);
	sscanf(buff, "%ld %d %s", &time, &info, unit);
	
	command[strlen(command) - 1] = '\0';
	printf("\nThe last %s reading was %d %s, taken at %s\n", command, info, unit, ctime(&time));
		
	// Close the connection to the server
	send(weather_fd, "CLOSE\n", 6, 0);
}

/**
 * Authenticate the request to the hopper server to access to the weather station.
 * @param server_fd The socket for the hopper server
 * @return The socket for the weather station
 */
int hopperAuth(int server_fd) {
	char buff[BUFF_SIZE];
	memset(buff, 0, BUFF_SIZE);	

	// Provide authentication to the hopper server
	strcpy(buff, "AUTH password123\n");
	send(server_fd, buff, 17, 0);

	// Get the server's response
	memset(buff, 0, BUFF_SIZE);
	recv(server_fd, buff, BUFF_SIZE, 0);
	
	// Parse the port number from the hopper server's response
	char port[8];
	memset(port, 0, 8);
	sscanf(buff, "CONNECT weatherstation.sandiego.edu %s sensorpass321", port); 
	
	// Connect to the weatherstation server
	return connectToHost("weatherstation.sandiego.edu", port);
}

/** 
 * Print command prompt to user and obtain user input.
 *
 * @return The user's desired selection, or -1 if invalid selection.
 */
long prompt() {
	// Print out the options
	printf("Which sensor would you like to read:\n\n");
	printf("\t(1) Air temperature\n");
	printf("\t(2) Relative humidity\n");
	printf("\t(3) Wind speed\n");
	printf("\t(4) Quit program\n\n");
	printf("Selection: ");

	// Read in a value from standard input
	char input[10];
	memset(input, 0, 10); // set all characters in input to '\0' (i.e. nul)
	char *read_str = fgets(input, 10, stdin);

	// Check if EOF or an error, exiting the program in both cases.
	if (read_str == NULL) {
		if (feof(stdin)) {
			exit(0);
		}
		else if (ferror(stdin)) {
			perror("fgets");
			exit(1);
		}
	}

	// get rid of newline, if there is one
	char *new_line = strchr(input, '\n');
	if (new_line != NULL) new_line[0] = '\0';

	// convert string to a long int
	char *end;
	long selection = strtol(input, &end, 10);

	if (end == input || *end != '\0') {
		selection = -1;
	}

	return selection;
}

/**
 * Socket implementation of connecting to a host at a specific port.
 *
 * @param hostname The name of the host to connect to (e.g. "foo.sandiego.edu")
 * @param port The port number to connect to
 * @return File descriptor of new socket to use.
 */
int connectToHost(char *hostname, char *port) {
	// Step 1: fill in the address info in preparation for setting 
	//   up the socket
	int status;
	struct addrinfo hints;
	struct addrinfo *servinfo;  // will point to the results

	memset(&hints, 0, sizeof hints); // make sure the struct is empty
	hints.ai_family = AF_INET;       // Use IPv4
	hints.ai_socktype = SOCK_STREAM; // TCP stream sockets

	// get ready to connect
	if ((status = getaddrinfo(hostname, port, &hints, &servinfo)) != 0) {
		fprintf(stderr, "getaddrinfo error: %s\n", gai_strerror(status));
		exit(1);
	}

	// Step 2: Make a call to socket
	int fd = socket(servinfo->ai_family, servinfo->ai_socktype, servinfo->ai_protocol);
	if (fd == -1) {
		perror("socket");
		exit(1);
	}

	// Step 3: connect!
	if (connect(fd, servinfo->ai_addr, servinfo->ai_addrlen) != 0) {
		perror("connect");
		exit(1);
	}

	freeaddrinfo(servinfo); // free's the memory allocated by getaddrinfo

	return fd;
}
