package edu.sandiego.comp375.jukebox;

import java.io.BufferedInputStream;
import java.io.PrintWriter;
import java.net.Socket;
import java.util.Scanner;
import java.io.IOException;

/**
 * Class representing a client to our Jukebox.
 */

public class AudioClient {
	private static final int BUFF_SIZE = 2048;

	private static String server = "";
	private static int port = -1;
	private static Socket socket = null;
	private static BufferedInputStream in = null;
	private static PrintWriter out = null;
	private static Thread player = null;

	public static void main(String[] args) throws Exception {
		try {
			server = args[0];
			port = Integer.parseInt(args[1]);

			System.out.println("Testing connection with " + server + " on port " + port + "...");

			socket = new Socket(server, port);
			if (socket.isConnected()) {
				System.out.print("Successfully connected!\n");
				socket.close();
			}

			event_loop();
		}
		catch (NumberFormatException nfe) {
			System.err.println("Port is not an integer.");
		}
		catch (Exception e) {
			System.err.println(e);
		}

		System.out.println("Client: Exited");
	}


	private static void event_loop() throws Exception {
		Scanner s = new Scanner(System.in);

		while (true) {
			System.out.print(">> ");
			String[] command = s.nextLine().split(" ");

			if (command[0].equals("exit")) {
				// Your final solution should make sure that the exit command
				// causes music to stop playing immediately.
				stop_player();	
				break;
			}

			if(socket == null || socket.isClosed()){
				socket = new Socket(server, port);
				in = new BufferedInputStream(socket.getInputStream(), BUFF_SIZE);
				out = new PrintWriter(socket.getOutputStream());
			}

			// Handle client input
			try {
				handle_input(command);
			}
			catch (IOException e) {
				System.err.println(e);
			}

			//Closing the socket once finished with whatever command we run
			System.out.println("\nEnd of message.\n");
			if(player == null) {
				socket.close();
				in.close();
				out.close();
			}

		} // end of while loop
	}

	private static void handle_input(String[] command) throws Exception {
			if (command[0].equals("list") && command.length == 1) {
				out.print("list");
				out.flush();
				
				System.out.print("Asked for list of songs. Received:\n\n");

				System.out.print((char)in.read());
				read_buff();
			}
			else if (command[0].equals("info") && command.length == 2) {
				out.print("info " + command[1]);
				out.flush();
				System.out.print("Asked for info on song number " + command[1] + ". Received:\n\n");

				char c = (char)in.read();
				if(c == 'n')
				{
					System.out.println("ERROR: Invalid song number. Please try again.");
				}
				else{
					System.out.print(c);
				}
				read_buff();
			}
			else if (command[0].equals("play") && command.length == 2) {	
				stop_player();

				if(socket.isClosed()) {
					socket = new Socket(server, port);
					in = new BufferedInputStream(socket.getInputStream(), BUFF_SIZE);
					out = new PrintWriter(socket.getOutputStream());
				}

				out.print("play " + command[1]);
				out.flush();
			
				System.out.println("Requested to play song number " +
				command[1] + ". . .\n");

				if((char)in.read() == 'n') {
					System.out.println("ERROR: Invalid song number. Please try agian.");
				}
				else {
					player = new Thread(new AudioPlayerThread(in));
					player.start();
				}
			}
			else if (command[0].equals("stop") && command.length == 1) {
				stop_player();
			}
			else {
				System.err.println("ERROR. This could be caused by an unknown command, unsuccessful " +
									"connection, or improper usage of a function. Please try again.");
			}
	}

	private static void read_buff() throws IOException {
		while (in.available() > 0) {
        	// Read the byte and convert the integer to character
			char c = (char)in.read();
			System.out.print(c);
		}
	}

	private static void stop_player() {
		if(player != null) {
			System.out.print("Song stopped. ");
			out.print("stop");
			out.flush();
			try {
				player.interrupt();
				player.join();
			} catch (Exception e) { /* no problem */ }

			player = null;
		}
	}
}
