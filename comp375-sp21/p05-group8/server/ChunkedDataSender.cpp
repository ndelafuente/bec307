#include <algorithm>

#include <cstring>
#include <cerrno>
#include <cstdio>

#include <sys/types.h>
#include <sys/socket.h>

#include "ChunkedDataSender.h"

namespace fs = std::filesystem;

using fs::path;
using std::ifstream;


ArraySender::ArraySender(const char *array_to_send, size_t length) {
	this->array = new char[length];
	std::copy(array_to_send, array_to_send+length, this->array);
	this->array_length = length;
	this->curr_loc = 0;
}

ssize_t ArraySender::send_next_chunk(int sock_fd) {
	// Determine how many bytes we need to put in the next chunk.
	// This will be either the CHUNK_SIZE constant or the number of bytes left
	// to send in the array, whichever is smaller.
	size_t num_bytes_remaining = array_length - curr_loc;
	size_t bytes_in_chunk = std::min(num_bytes_remaining, CHUNK_SIZE);

	if (bytes_in_chunk > 0) {
		// Create the chunk and copy the data over from the appropriate
		// location in the array
		char chunk[CHUNK_SIZE];
		memcpy(chunk, array+curr_loc, bytes_in_chunk);

		ssize_t num_bytes_sent = send(sock_fd, chunk, bytes_in_chunk, 0);

		if (num_bytes_sent > 0) {
			// We successfully send some of the data so update our location in
			// the array so we know where to start sending the next time we
			// call this function.
			curr_loc += num_bytes_sent;
			return num_bytes_sent;
		}
		else if (num_bytes_sent < 0 && errno == EAGAIN) {
			// We couldn't send anything because the buffer was full
			return -1;
		}
		else {
			// Send had an error which we didn't expect, so exit the program.
			perror("send_next_chunk send");
			exit(EXIT_FAILURE);
		}
	}
	else {
		return 0;
	}
}



FileSender::FileSender(fs::path file_path) {
	// Open the file in binary mode and get its size
	this->file.open(file_path.string(), ifstream::binary); 
	this->file_size = fs::file_size(file_path);
	this->curr_pos = 0;
}

ssize_t FileSender::send_next_chunk(int sock_fd) {
	// Determine how many bytes we need to put in the next chunk.
	size_t num_bytes_remaining = this->file_size - this->curr_pos;
	ssize_t bytes_in_chunk = std::min(num_bytes_remaining, CHUNK_SIZE);

	if (bytes_in_chunk > 0) {
		// Create the chunk and read the data into it from the file
		char chunk[CHUNK_SIZE];
		this->file.read(chunk, bytes_in_chunk); // read up to bytes_in_chunk into file_data buffer
		
		if (this->file.gcount() != bytes_in_chunk) {
			// There was an error reading the file? TODO
			// Assign gcount value to bytes_read for later
		}

		ssize_t num_bytes_sent = send(sock_fd, chunk, bytes_in_chunk, 0);

		if (num_bytes_sent > 0) {
			// We successfully send some of the data so update our location in
			// the array so we know where to start sending the next time we
			// call this function.
			this->curr_pos += num_bytes_sent;
			this->file.seekg(curr_pos);

			return num_bytes_sent;
		}
		else if (num_bytes_sent < 0 && errno == EAGAIN) {
			// We couldn't send anything because the buffer was full
			this->file.seekg(curr_pos); // reset the position in the file

			return -1;
		}
		else {
			// Send had an error which we didn't expect, so exit the program.
			perror("send_next_chunk send");
			exit(EXIT_FAILURE);
		}
	}
	else {
		return 0;
	}
}
