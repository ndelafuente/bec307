#ifndef CONNECTEDCLIENT_H
#define CONNECTEDCLIENT_H

#include <vector>
#include <filesystem>

typedef std::vector<std::filesystem::path> path_list;

/**
 * Represents the state of a connected client.
 */
enum ClientState { RECEIVING, SENDING };

/**
 * Class that models a connected client.
 * 
 * One object of this class will be created for every client that you accept a
 * connection from.
 */
class ConnectedClient {
  public:
	// Member Variablesa (i.e. fields)
	int client_fd;
	ChunkedDataSender *sender;
	ClientState state;

	// Constructors
	/**
	 * Constructor that takes the client's socket file descriptor and the
	 * initial state of the client.
	 */
	ConnectedClient(int fd, ClientState initial_state);

	/**
	 * No argument constructor.
	 */
	ConnectedClient() : client_fd(-1), sender(NULL), state(RECEIVING) {}


	// Member Functions (i.e. Methods)
	
	/**
	 * Sends a response to the client.
	 *
	 * @param epoll_fd File descriptor for epoll.
	 * @param data_to_send The text data to be sent to the client.
	 */
	void send_response(int epoll_fd, ChunkedDataSender *sender);

	/**
	 * Send text data to the client (using send_response)
	 *
	 * @param epoll_fd File descriptor for epoll.
	 * @param str The text data to be sent to the client.
	 */
	void send_txt_response(int epoll_fd, std::string str);

	/**
	 * Send mp3 data to the client (using send_response)
	 *
	 * @param epoll_fd File descriptor for epoll.
	 * @param mp3_file The mp3 data to be sent to the client.
	 */
	void send_mp3_response(int epoll_fd, std::filesystem::path mp3_file);

	/**
	 * Handles new input from the client.
	 *
	 * @param epoll_fd File descriptor for epoll.
	 * @param mp3_paths A vector of the file paths to each available mp3 file.
	 */
	void handle_input(int epoll_fd, path_list mp3_paths);

	/**
	 * Sends a list of songs to the client.
	 *
	 * @param epoll_fd File descriptor for epoll.
	 * @param mp3_paths The list of available mp3 files.
	 */
	int list_songs(int epoll_fd, path_list mp3_paths);

	/**
	 * Sends song information to the client.
	 *
	 * @param epoll_fd File descriptor for epoll.
	 * @param song_file The mp3 file to send information about.
	 */
	int send_song_info(int epoll_fd, std::filesystem::path song_file);
	
	/**
	 * Select a song from available songs using its number.
	 *
	 * @param epoll_fd File descriptor for epoll.
	 * @param mp3_paths The list of available mp3 files.
	 * @param song_number The song number.
	 */
	std::filesystem::path select_song(int epoll_fd, path_list mp3_paths, std::string song_number);

	/**
	 * If a song is playing, stop sending to the client.
	 *
	 * @param epoll_fd File descriptor for epoll.
	 */
	void stop_song(int epoll_fd);

	/**
	 * Handles a close request from the client.
	 *
	 * @param epoll_fd File descriptor for epoll.
	 */
	void handle_close(int epoll_fd);

	/**
	 * Continues a response from the client.
	 *
	 * @param epoll_fd File descriptor for epoll.
	 */
	void continue_response(int epoll_fd);
};

#endif
