#include <iostream>

#include <fstream>
#include <sstream>
#include <filesystem>

#include <cstring>

#include <unistd.h>
#include <sys/epoll.h>
#include <sys/types.h>
#include <sys/socket.h>

#include "ChunkedDataSender.h"
#include "ConnectedClient.h"

namespace fs = std::filesystem;

using std::cout;
using std::cerr;
using std::string;
using fs::path;


ConnectedClient::ConnectedClient(int fd, ClientState initial_state) :
	client_fd(fd), sender(NULL), state(initial_state) {}

void ConnectedClient::send_txt_response(int epoll_fd, string str) {
	char *data_to_send = strcpy(new char[str.length() + 1], str.c_str());
	size_t data_size = strlen(data_to_send); 

	ArraySender *array_sender = new ArraySender(data_to_send, data_size);
	delete[] data_to_send;

	this->send_response(epoll_fd, array_sender);
}

void ConnectedClient::send_mp3_response(int epoll_fd, fs::path mp3_file) {
	FileSender *file_sender = new FileSender(mp3_file);

	this->send_response(epoll_fd, file_sender);
}

void ConnectedClient::send_response(int epoll_fd, ChunkedDataSender *sender) {
	ssize_t num_bytes_sent;
	ssize_t total_bytes_sent = 0;

	// keep sending the next chunk until it says we either didn't send
	// anything (0 return indicates nothing left to send) or until we can't
	// send anymore because of a full socket buffer (-1 return value)
	while((num_bytes_sent = sender->send_next_chunk(this->client_fd)) > 0) {
		total_bytes_sent += num_bytes_sent;
	}
	cout << "sent " << total_bytes_sent << " bytes to client\n";

	/*
	 * If the last call to send_next_chunk indicated we couldn't send
	 * anything because of a full socket buffer, we should do the following:
	 *
	 * 1. update our state field to be sending
	 * 2. set our sender field to be the ArraySender object we created
	 * 3. update epoll so that it also watches for EPOLLOUT for this client
	 *    socket (use epoll_ctl with EPOLL_CTL_MOD).
	 */

	if (num_bytes_sent < 0) {
		this->state = SENDING;
		this->sender = sender;

		struct epoll_event client_ev;
		client_ev.events = EPOLLOUT | EPOLLIN;
		client_ev.data.fd = this->client_fd;
		epoll_ctl(epoll_fd, EPOLL_CTL_MOD, this->client_fd, &client_ev);//epoll event struct in jukebox
	}
	else {
		// Sent everything with no problem so we are done with our ArraySender
		// object.
		shutdown(client_fd, SHUT_RDWR);
		delete sender;
	}
}

void ConnectedClient::handle_input(int epoll_fd, path_list mp3_paths) {
	char data[1024];
	ssize_t bytes_received = recv(this->client_fd, data, 1024, 0);
	if (bytes_received < 0) {
		perror("client_read recv");
		exit(EXIT_FAILURE);
	}

	// Convert the input into a lowercase string and print it out
	cout << "Received data: ";
	for (int i = 0; i < bytes_received; i++) {
		data[i] = std::tolower(data[i]);
		cout << data[i];
	}
	data[bytes_received] = '\0'; // ensuring that the data ends properly
	cout << " from client (" << this->client_fd << ")\n";

	std::stringstream ss(data);	
	string input;
	ss >> input;

	//This section of the code parses the input and sends it to the
	//appropriate command accordingly.
	if (input == "list") {
		this->list_songs(epoll_fd, mp3_paths);
	}
	else if (input == "info") {
		ss >> input; // extracting the song number
		fs::path song_file = this->select_song(epoll_fd, mp3_paths, input);

		if (song_file != fs::path())
			this->send_song_info(epoll_fd, song_file);
	}
	else if (input == "play") {
		ss >> input; // extracting the song number
		fs::path song_file = this->select_song(epoll_fd, mp3_paths, input);
		
		if (song_file != fs::path())
			this->send_mp3_response(epoll_fd, song_file);
	}
	else if (input == "stop") {
		this->stop_song(epoll_fd);
	}
	else if (input == "close") {
		this->handle_close(epoll_fd);
	}
	else {
		// Invalid command
		cout << "Invalid command (" << input << ")\n";
	}
}


int ConnectedClient::list_songs(int epoll_fd, path_list mp3_paths) {
	int song_num_int = 0;
	
	std::stringstream ss;
	ss << "No.\tFilename\n";
	
	// Iterate over all available mp3 files, printing their info
	for (fs::path entry : mp3_paths) {
		ss  << "(" << song_num_int++ << ")\t" << entry.filename().string() << "\n";
	}
	
	send_txt_response(epoll_fd, ss.str());

	return 0;
}


int ConnectedClient::send_song_info(int epoll_fd, fs::path song_file) {
	std::stringstream ss;

	// Test whether the mp3 file has an info file
	fs::path info_file_path = song_file.replace_extension(".mp3.info");
	if (fs::is_regular_file(info_file_path)) {
		std::ifstream t(info_file_path.string());
		std::stringstream buffer;
		buffer << t.rdbuf();
		ss << buffer.str() << "\n";
	}
	else {
		ss << song_file.filename().string() << "\n";
		ss << "(No additional info)\n";
	}

	send_txt_response(epoll_fd, ss.str());

	return 0;
}

fs::path ConnectedClient::select_song(int epoll_fd, path_list mp3_paths, std::string song_number) {
	long unsigned int song_num_int = std::stoi(song_number);

	std::stringstream ss;
	cout << "Song Number " << song_number << " info\n";

	if (song_num_int > mp3_paths.size()-1) {
		cout << "User tried to access song out of range.\n";
		
		send_txt_response(epoll_fd, "n");
		return fs::path();
	}

	return mp3_paths[song_num_int];
}

// This method stops a song from playing immediately
void ConnectedClient::stop_song(int epoll_fd) {
	if (this->state == SENDING) {
		this-> state = RECEIVING;
		struct epoll_event client_ev;
		client_ev.events = EPOLLIN | EPOLLRDHUP;
		client_ev.data.fd = this->client_fd;
		epoll_ctl(epoll_fd, EPOLL_CTL_MOD, this->client_fd, &client_ev);
	}
	else {
		cout << "Nothing is currently being sent\n";
	}
	shutdown(epoll_fd, SHUT_RDWR);
}


// You likely should not need to modify this function.
void ConnectedClient::handle_close(int epoll_fd) {
	cout << "Closing connection to client " << this->client_fd << "\n\n";

	if (epoll_ctl(epoll_fd, EPOLL_CTL_DEL, this->client_fd, NULL) == -1) {
		perror("handle_close epoll_ctl");
		exit(EXIT_FAILURE);
	}

	close(this->client_fd);
}


// Continue response continues the response with the client
void ConnectedClient::continue_response(int epoll_fd) {
	// Create a large array, just to make sure we can send a lot of data in
	// smaller chunks
	ssize_t num_bytes_sent;
	ssize_t total_bytes_sent = 0;

	// keep sending the next chunk until it says we either didn't send
	// anything (0 return indicates nothing left to send) or until we can't
	// send anymore because of a full socket buffer (-1 return value)
	while((num_bytes_sent = this->sender->send_next_chunk(this->client_fd)) > 0) {
		total_bytes_sent += num_bytes_sent;
	}
	cout << "sent " << total_bytes_sent << " bytes to client\n";
	
	
	
	if (num_bytes_sent >= 0) {
		this->state = SENDING;
		this->sender = NULL;
			
		struct epoll_event client_ev;
		client_ev.events = EPOLLIN | EPOLLRDHUP;
		client_ev.data.fd = this->client_fd;

		epoll_ctl(epoll_fd, EPOLL_CTL_MOD, this->client_fd, &client_ev);
		delete sender;
		shutdown(client_fd, SHUT_RDWR);
	}
}
