/**
 * ToreroServe: A Lean Web Server
 * COMP 375 - Project 02
 *
 * This program should take two arguments:
 * 	1. The port number on which to bind and listen for connections
 * 	2. The directory out of which to serve files.
 *
 * Author 1: Nico de la Fuente (ndelafuente@sandiego.edu)
 * Author 2: Christian Gideon (christiangideon@sandiego.edu)
 *
 * This program will enable multiple clients to access a web server.
 */

// standard C libraries
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <cerrno>

// operating system specific libraries
#include <netinet/in.h>
#include <sys/select.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/uio.h>
#include <unistd.h>

// C++ standard libraries
#include <vector>
#include <thread>
#include <string>
#include <cstring>
#include <iostream>
#include <system_error>
#include <filesystem>
#include <fstream>
#include <regex>

// Custom headers
#include "BoundedBuffer.hpp"

#define BUFF_SIZE 256
#define NUM_CLIENTS 12
#define NUM_THREADS 4

// shorten the std::filesystem namespace down to just fs
namespace fs = std::filesystem;

// shorten other namespaces to use later
using std::cout;
using fs::path;
using std::string;
using std::vector;
using std::thread;
using std::regex;
using std::istringstream;

// This will limit how many clients can be waiting for a connection.
static const int BACKLOG = 10;

/* Forward declarations */
int createSocketAndListen(const int port_num);
void acceptConnections(const int server_sock);
void handleMultipleClients(BoundedBuffer &client_socks);
void handleClient(const int client_sock);
// General communication
void sendData(int socked_fd, const char *data, size_t data_length);
int receiveData(int socked_fd, char *dest, size_t buff_size);
// Server response functions
void send200Response(int fd, path file);
void send200Header(int fd, size_t fileSize, string dataType);
void send200Content(int fd, path file);
void send404Response(int fd);
void send400Response(int fd);
string generateIndex(string uri, string full_path);


int main(int argc, char** argv) {

	/* Make sure the user called our program correctly. */
	if (argc != 3) {
		// Print a proper error message informing user of proper usage
		cout << "usage: " << argv[0] << " <port number> WWW\n";
		exit(1);
	}

    /* Read the port number from the first command line argument. */
    int port = std::stoi(argv[1]);

	/* Create a socket and start listening for new connections on the
	 * specified port. */
	int server_sock = createSocketAndListen(port);

	/* Now let's start accepting connections. */
	acceptConnections(server_sock);

    close(server_sock);

	return 0;
}

/**
 * Creates a new socket and starts listening on that socket for new
 * connections.
 *
 * @param port_num The port number on which to listen for connections.
 * @returns The socket file descriptor
 */
int createSocketAndListen(const int port_num) {
    int sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock < 0) {
        perror("Creating socket failed");
        exit(1);
    }

    /* 
	 * A server socket is bound to a port, which it will listen on for incoming
     * connections.  By default, when a bound socket is closed, the OS waits a
     * couple of minutes before allowing the port to be re-used.  This is
     * inconvenient when you're developing an application, since it means that
     * you have to wait a minute or two after you run to try things again, so
     * we can disable the wait time by setting a socket option called
     * SO_REUSEADDR, which tells the OS that we want to be able to immediately
     * re-bind to that same port. See the socket(7) man page ("man 7 socket")
     * and setsockopt(2) pages for more details about socket options.
	 */
    int reuse_true = 1;

	int retval; // for checking return values

    retval = setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, &reuse_true,
                        sizeof(reuse_true));

    if (retval < 0) {
        perror("Setting socket option failed");
        exit(1);
    }

    /*
	 * Create an address structure.  This is very similar to what we saw on the
     * client side, only this time, we're not telling the OS where to connect,
     * we're telling it to bind to a particular address and port to receive
     * incoming connections.  Like the client side, we must use htons() to put
     * the port number in network byte order.  When specifying the IP address,
     * we use a special constant, INADDR_ANY, which tells the OS to bind to all
     * of the system's addresses.  If your machine has multiple network
     * interfaces, and you only wanted to accept connections from one of them,
     * you could supply the address of the interface you wanted to use here.
	 */
    struct sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_port = htons(port_num);
    addr.sin_addr.s_addr = INADDR_ANY;

    /* 
	 * As its name implies, this system call asks the OS to bind the socket to
     * address and port specified above.
	 */
    retval = bind(sock, (struct sockaddr*)&addr, sizeof(addr));
    if (retval < 0) {
        perror("Error binding to port");
        exit(1);
    }

    /* 
	 * Now that we've bound to an address and port, we tell the OS that we're
     * ready to start listening for client connections. This effectively
	 * activates the server socket. BACKLOG (a global constant defined above)
	 * tells the OS how much space to reserve for incoming connections that have
	 * not yet been accepted.
	 */
    retval = listen(sock, BACKLOG);
    if (retval < 0) {
        perror("Error listening for connections");
        exit(1);
    }

	return sock;
}

/**
 * Sit around forever accepting new connections from client.
 *
 * @param server_sock The socket used by the server.
 */
void acceptConnections(const int server_sock) {
    while (true) {
        // Declare a socket for the client connection.
        int sock;

        /* 
		 * Another address structure.  This time, the system will automatically
         * fill it in, when we accept a connection, to tell us where the
         * connection came from.
		 */
        struct sockaddr_in remote_addr;
        unsigned int socklen = sizeof(remote_addr); 

        /* 
		 * Accept the first waiting connection from the server socket and
         * populate the address information.  The result (sock) is a socket
         * descriptor for the conversation with the newly connected client.  If
         * there are no pending connections in the back log, this function will
         * block indefinitely while waiting for a client connection to be made.
         */
        sock = accept(server_sock, (struct sockaddr*) &remote_addr, &socklen);
        if (sock < 0) {
            perror("Error accepting connection");
            exit(1);
        }

        /* 
		 * At this point, you have a connected socket (named sock) that you can
         * use to send() and recv(). The handleClient function should handle all
		 * of the sending and receiving to/from the client.
		 */
	
		BoundedBuffer client_sockets(NUM_CLIENTS); // a shared buffer to store socks

		// Put the socket in the buffer
		client_sockets.putItem(sock);

		// Create independent threads to handle multiple client requests
		for (size_t i = 0; i < NUM_THREADS; ++i) {
			thread consumer(handleMultipleClients, std::ref(client_sockets));

			// Let the consumers run without waiting to be rejoined
			consumer.detach();
		}
    }
}

/**
 * A thread's sole purpose: to wait for someone to connect to the server.
 * 
 * @param client_socks The client's socket file descriptor.
 */
void handleMultipleClients(BoundedBuffer &client_socks) {
	while (true) {
		// Wait for a socket to be put on the buffer
		int sock = client_socks.getItem();
		
		// Handle the client's request
		handleClient(sock);
	}
}

/**
 * Receives a request from a connected HTTP client and sends back the
 * appropriate response.
 *
 * @note After this function returns, client_sock will have been closed (i.e.
 * may not be used again).
 *
 * @param client_sock The client's socket file descriptor.
 */
void handleClient(const int client_sock) {
	// Receive the request message from the client
	char received_data[2048];
	int bytes_received = receiveData(client_sock, received_data, 2048);

	// Turn the char array into a C++ string for easier processing.
	string request_string(received_data, bytes_received);

	// Parse the request string to determine what response to generate.
	istringstream f(request_string);

	string tmp, uri;
	getline(f, tmp, ' '); // stores "GET"
	getline(f, uri, ' '); // stores URI (e.g. /index.html)
	
	// Generate and send HTTP response message based on the request received.
	string full_path = "WWW/" + uri; // create the file path
	// If the URI is a file
	if (fs::is_regular_file(full_path)) {
		// Send the contents of the file
		send200Response(client_sock, full_path);
	}
	// Else if the URI is a directory
	else if (fs::is_directory(full_path)) {
		// If index.html exists within the directory
		string path_with_index = full_path + "index.html";

		if (fs::is_regular_file(path_with_index)) {
			// Send the existing index file
			send200Response(client_sock, path_with_index); 
		}
		else {
			// Create and send an index for the directory 
			string index = generateIndex(uri, full_path);
			send200Header(client_sock, index.length(), "text/html");
			sendData(client_sock, index.c_str(), index.length());
		}
	}
	// Else the requested URI does not exist in the server's disk
	else {
		send404Response(client_sock);
	}

	// Close connection with client.
	close(client_sock);
}

// General communication functions
/**
 * Sends message over given socket, raising an exception if there was a problem
 * sending.
 *
 * @param socket_fd The socket to send data over.
 * @param data The data to send.
 * @param data_length Number of bytes of data to send.
 */
void sendData(int socket_fd, const char *data, size_t data_length) {
	int num_bytes_sent = 0;
	long unsigned int total_data_sent = 0;

	// Keep sending until the data has been completely sent.
	while(total_data_sent < data_length) {
		// Send the data, keeping track of how much was actually sent
		num_bytes_sent = send(socket_fd, (data + total_data_sent), 
					(data_length - total_data_sent), 0);
		total_data_sent += num_bytes_sent;
		
		if (num_bytes_sent == -1) {
			std::error_code ec(errno, std::generic_category());
			throw std::system_error(ec, "send failed");
		}
	}
}

/**
 * Receives message over given socket, raising an exception if there was an
 * error in receiving.
 *
 * @param socket_fd The socket to send data over.
 * @param dest The buffer where we will store the received data.
 * @param buff_size Number of bytes in the buffer.
 * @return The number of bytes received and written to the destination buffer.
 */
int receiveData(int socket_fd, char *dest, size_t buff_size) {
	int num_bytes_received = recv(socket_fd, dest, buff_size, 0);
	if (num_bytes_received == -1) {
		std::error_code ec(errno, std::generic_category());
		throw std::system_error(ec, "recv failed");
	}

	return num_bytes_received;
}

// Server response functions
/**
 * Handle the response for a valid client request (200 OK).
 *   - Parse the file size and type.
 *   - Send the file data to the server.
 * 
 * @param fd The socket to send data over.
 * @param file The address of the file.
 */
void send200Response(int fd, path file) {
	// Get the size of the file
	size_t fileSize = fs::file_size(file);

	// Parse the file extension
	istringstream f(file);
	string tmp, extension;
	getline(f, tmp, '.');
	getline(f, extension);
	
	// Convert the extension into a header content-type 
	if (extension == "jpg") { extension = "jpeg"; }
	if (extension == "txt") { extension = "plain"; }
	
	string dataType; // only handles html, css, txt, jpeg, gif, png, and pdf
	if (extension == "html" || extension == "css" || extension == "plain")
		dataType = "text/" + extension;
	else if (extension == "jpeg" || extension == "gif" || extension == "png")
		dataType = "image/" + extension;
	else if(extension == "pdf")
		dataType = "application/pdf";
	else {
		cout << "File type not supported" << '\n';
		exit(1);
	}

	// Send the file data to the server
	send200Header(fd, fileSize, dataType);
	send200Content(fd, file);
}

/**
 * Send the header for a 200 OK response.
 * 
 * @param fd The socket to send data over.
 * @param fileSize The size of the file.
 * @param dataType The type of data in the file.
 */
void send200Header(int fd, size_t fileSize, string dataType) {
	// Create the response header
	string responseHeader = "HTTP/1.1 200 OK\r\n"
							"Content-Type: " + dataType + "\r\n"
					  		"Content-Length: " + std::to_string(fileSize) + "\r\n\r\n";

	// Send the header
	sendData(fd, responseHeader.c_str(), responseHeader.length());
}

/**
 * Send the contents of the requested file.
 * 
 * @param fd The socket to send data over.
 * @param file The address of the file.
 */
void send200Content(int fd, path file) {
	size_t buffer_size = 16;
	char file_data[buffer_size]; // array for file data

	// Open the file in binary mode
	std::ifstream fileReader(file, std::ios::binary);

	// Keep reading while we haven't reached the end of the file (EOF)
	while (!fileReader.eof()) {
		memset(file_data, 0, buffer_size); // clear the buffer

		fileReader.read(file_data, buffer_size); // read up to buffer_size bytes into file_data buffer
		int bytes_read = fileReader.gcount(); // find out how many bytes we actually read

		// Add the data to the response
		sendData(fd, file_data, bytes_read);
	}

	// Close the file
	fileReader.close();
}

/**
 * Handle the response for an invalid URI (404 Not Found).
 * 
 * @param fd The socket to send data over.
 */
void send404Response(int fd) {
	string html = // Little bit of HTML
			"<html>"
				"<head>"
					"<title>Page not found!</title>"
				"</head>"
			"<body>"
				"404 Page Not Found! *cries in HTML*<br>"
			"</body>"
			"</html>";
	
	// The response header
	string response = "HTTP/1.0 404 NOT FOUND\r\n"
					"Content-Type: text/html\r\n"
					"Content-Length: ";
	response += std::to_string(html.length()) + "\r\n\r\n" + html;

	// Send the data to the server
	sendData(fd, response.c_str(), response.length());
}

/** 
 * Handle the response for an invalid request (400 Bad Request).
 * 
 * @note Not used due to assumption of well-formatted request.
 * 
 * @param fd The socket to send data over.
 */
void send400Response(int fd) {
	string data = "HTTP/1.1 400 BAD REQUEST\r\n\r\n";
	sendData(fd, data.c_str(), data.length());
}

/**
 * Create a temporary index for a requested directory that does not already 
 * have an index.html file.
 * 
 * @param uri The uniform resource identifier for the file.
 * @param full_path The full address on the local machine ("WWW" + uri).
 *
 * @return The generated HTML for the index.
 */
string generateIndex(string uri, string full_path) {
	string index = // Little bit of HTML
		"<html>"
			"<head>"
				"<title>File directory</title>"
			"</head>"
			"<body>"
				"<ul>" // List opener
				"<h2>Index of ~" + uri + "</h2>";

	// Pass through each entry in the directory and add it to the index
	for(auto& entry: fs::directory_iterator(full_path)) {
		string filename = entry.path().filename();

		// Append a / to the end of a directory
		if (fs::is_directory(entry.path()))
			filename += '/';

		// Creating a link to the entry
		index += "<li><a href=\"" + filename + "\">" + filename + "</a></li>";
	}
	
	return index + "</ul></body></html>";
}
