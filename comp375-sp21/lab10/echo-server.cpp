/*
 * File: echo-server.cpp
 *
 * Authors: (TODO: Fill this in with author info)
 *
 * Implementation of an "echo server" that uses epoll to enable concurrent
 * client connections.
 *
 */

// C++ standard libraries
#include <string>
#include <iostream>

// C standard libraries
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <cerrno>

// POSIX / OS-specific libraries
#include <unistd.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/select.h>
#include <sys/epoll.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <sys/types.h>
#include <dirent.h>
#include <fcntl.h>

using std::cout;
using std::cerr;


const int BACKLOG = 10; // max number of pending connections

const int MAX_EVENTS = 10; // max number of epoll events
const int MAX_DATA_LENGTH = 4096; // max bytes read by a recv

/* 
 * Use fcntl (file control) to set the given socket to non-blocking mode.
 *
 * @info Setting your sockets to non-blocking mode is not required, but it
 * might help with your debugging. By setting each socket you get from
 * accept() to non-blocking, you can be sure that normally blocking calls like
 * send, recv, and accept will instead return an error condition and set errno
 * to EWOULDBLOCK/EAGAIN. I would recommend that you set your sockets for
 * non-blocking and then explicitly check each call to send, recv, and accept
 * for this errno. If you see it happening, you know that you're attempting
 * to call one of those functions when you shouldn't be.
 *
 * @param sock The file descriptor for the socket you want to make
 * 				non-blocking.
 */
void setNonBlocking(int sock) {
    // Get the current flags. We want to add O_NONBLOCK to this set.
    int socket_flags = fcntl(sock, F_GETFD);
    if (socket_flags < 0) {
        perror("fcntl");
        exit(1);
    }

    // TODO: Add in the O_NONBLOCK flag by bitwise ORing it to the old flags.
    socket_flags = socket_flags | /* FIXME */;

    /* Set the new flags, including O_NONBLOCK. */
    int result = fcntl(sock, F_SETFD, socket_flags);
    if (result < 0) {
        perror("fcntl");
        exit(1);
    }

    // YAY! The socket is now in non-blocking mode.
}

/**
 * Creates a socket, sets it to non-blocking, binds it to the given port, then
 * sets it to start listen for incoming connections.
 *
 * @param port_num The port number we will listen on.
 * @return The file descriptor of the newly created/setup server socket.
 */
int setup_server_socket(uint16_t port_num) {
    /* Create the socket that we'll listen on. */
    int sock_fd = socket(AF_INET, SOCK_STREAM, 0);

    /* Set SO_REUSEADDR so that we don't waste time in TIME_WAIT. */
    int val = setsockopt(sock_fd, SOL_SOCKET, SO_REUSEADDR, 
							&val, sizeof(val));
    if (val < 0) {
        perror("Setting socket option failed");
        exit(1);
    }

    /* 
	 * Set our server socket to non-blocking mode.  This way, if we
     * accidentally accept() when we shouldn't have, we won't block
     * indefinitely.
	 */
    setNonBlocking(sock_fd);

    struct sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_port = htons(port_num);
    addr.sin_addr.s_addr = INADDR_ANY;

    /* Bind our socket and start listening for connections. */
    if (bind(sock_fd, (struct sockaddr*)&addr, sizeof(addr)) < 0) {
        perror("Error binding to port");
        exit(1);
    }

    if (listen(sock_fd, BACKLOG) < 0) {
        perror("Error listening for connections");
        exit(1);
    }

	return sock_fd;
}


/*
 * The main function for the server.
 * You should refactor this to be smaller and add additional functionality as
 * needed.
 */
int main(int argc, char **argv) {

    if (argc < 2) {
        printf("Usage:\t%s <port>\n", argv[0]);
        exit(0);
    }

    /* Get the port number from the arguments. */
    uint16_t port = (uint16_t) std::stoul(argv[1]);

	int server_socket = setup_server_socket(port);

	// Create the epoll, which returns a file descriptor for us to use later.
	int epoll_fd = epoll_create1(0);
	if (epoll_fd < 0) {
		perror("epoll_create1");
		exit(1);
	}

	/*
	 * epoll uses a special struct named epoll_event to track I/O events.
	 * This struct has two fields: events and data. The events field is used
	 * to indicate what type of event it is (e.g. input/read or output/write).
	 * The data field will contain a file descriptor for our associated
	 * socket.
	 */

	 /*
	  * We'll start by creating an epoll_event for our server socket to
	  * indicate that we want epoll to watch for "input" events on this
	  * socket, i.e. incoming requests to connect to this server.
	  */
	struct epoll_event server_ev;

	server_ev.data.fd = server_socket; // set the file descriptor to watch

	// TODO: Change this to the "read" event type that you identified while
	// answering the lab questions.
	server_ev.events = /* FIXME */;

	// TODO: Fill in the first two parameters to the following function call
	// so that it correctly sets up the epoll file descriptor you just created
	// (epoll_fd) by ADDING the new epoll_event to the list of events.  Review
	// the epoll_ctl manual if you need a refresher on what each of these
	// parameters mean.
	if (epoll_ctl(/* FIXME */, /* FIXME */, server_socket, &server_ev) == -1) {
		perror("epoll_ctl");
		exit(1);
	}

    while (true) {
		// The following array will be filled in by epoll_wait with all of the
		// events that are ready to be handled.
		struct epoll_event events_to_handle[MAX_EVENTS];

        /* 
		 * epoll_wait will block until at least one of the sockets we
		 * registered (using epoll_ctl) has an actual I/O event for us to
		 * handle.
		 */
		// TODO: Update the last parameters so that we wait indefinitely for
		// an event to occur (just like you identified in your lab answers).
		int num_events = epoll_wait(epoll_fd, events_to_handle, MAX_EVENTS, /* FIXME */);
		if (num_events < 0) {
			perror("epoll_wait");
			exit(1);
		}

		// Loop through all the I/O events that just happened.
		for (int n = 0; n < num_events; n++) {

			// Check if this event is an input event. We can figure out if
			// this as an input event by anding together the events field of
			// the current event with EPOLLIN, and making sure it doesn't
			// equal 0.
			if ((events_to_handle[n].events & EPOLLIN) != 0) {

				// Check to see if this event was the server socket ready to
				// accept a new connection.
				if (events_to_handle[n].data.fd == server_socket) {
					// The only event that will trigger our server_socket is when
					// there is a new client wanting to connect to the server.
					// In this case, we'll need to call the accept function to
					// accept a connection, then add the new file descriptor
					// to our set of all file descriptors.

					struct sockaddr_storage client_addr;
					socklen_t addr_size = sizeof(client_addr);

					// TODO: Fill in the missing parameter so that the server
					// socket accepts the connection.
					int client_fd = accept(/* FIXME*/, 
											(struct sockaddr *)&client_addr, 
											&addr_size);
					if (client_fd == -1) {
						perror("accept");
						exit(EXIT_FAILURE);
					}

					// TODO: Make the new client socket non-blocking... we have a
					// function for that, right...?

					// Create a new epoll_event for the new client, setting
					// it so that we only check for "input" events (i.e. client
					// sent us data that we can now read).
					struct epoll_event new_client_ev;

					// TODO: fill in both the events and the data.fd fields of
					// new_client_ev so that we wait for read events from the
					// new client. This should look very similar to what we
					// did when setting up server_ev above.

					// TODO: use epoll_ctl to add the new epoll_event to the list
					// to watch for. Again, this should look very similar to
					// the code for adding the server event from earlier.
					if (epoll_ctl(/* FIXME */) == -1) {
						perror("epoll_ctl: client_fd");
						exit(1);
					}
				}
				else {
					// This is a normal client socket so we need to recv the data
					// it has sent to us.

					// For this example program we are just implementing an echo
					// server so once we receive data, we'll send the same data
					// back to the client (using the send function).

					char received_data[MAX_DATA_LENGTH];
					// TODO: Step 1: Receive data from the client, using the
					// venerable recv system call to store it into the
					// received_data array. Recall that the client's socket
					// file descriptor will be stored in
					// events_to_handle[n].data.fd

					// TODO: Step 2: Send the same data back to the client
					// using the send system call.
				}
			}

			// Check if this event is an output event, this time by anding
			// with EPOLLOUT.
			if (events_to_handle[n].events & EPOLLOUT) {
				// Note: For this example program, we don't have any sockets
				// set up for "output" events. For your next project, you will
				// have clients who will want to be written to so you will
				// need to do something interesting here.
				cerr << "ERROR: socket " << events_to_handle[n].data.fd 
					 << " should not have any output/write events.\n";
				exit(1);
			}
		}
    }
}
