#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <math.h>

#include "simplified_des.h"

#define KEY_LEN 9 // The length of the key
#define BLOCK_SIZE 12 // The size of the block

const uint8_t s1_box[16] = { 5, 2, 1, 6, 3, 4, 7, 0, 1, 4, 6, 2, 0, 7, 5, 3 };
const uint8_t s2_box[16] = { 4, 0, 6, 5, 7, 1, 3, 2, 5, 3, 0, 7, 6, 2, 1, 4 };

uint8_t expand(uint8_t input) {
	// Isolate the positions using masks
	uint8_t pos1_and_2 = input & 0x30; // 00xx 0000
	uint8_t pos3 = input & 0x8;        // 0000 x000
	uint8_t pos4 = input & 0x4;        // 0000 0x00
	uint8_t pos5_and_6 = input & 0x3;  // 0000 00xx

	// Expand the six-bit input into an eight-bit number
	uint8_t output_left= (pos1_and_2 << 2) | (pos4 << 3) | (pos3<<1);
	uint8_t output_right = (pos4 << 1) | (pos3 >>1) | (pos5_and_6);
	
	// Recombine the halves for output
	return (output_left | output_right);
}

uint8_t confuse(uint8_t input) {
	// Isolate the left and right halves of the input using masks
	uint8_t input_left = (input & 0xF0) >> 4;
	uint8_t input_right = input & 0xF;
	
	// Lookup the three-bit value associated with the four-bit half
	uint8_t output_left = s1_box[input_left];
	uint8_t output_right = s2_box[input_right];
	
	// Recombine the halves for output
	return (output_left << 3) | output_right;
}

uint8_t feistel(uint8_t input, uint8_t key) {
	return confuse(expand(input) ^ key);
}

uint16_t feistel_round(uint16_t input, uint8_t key) {
	// Separate the left and right halves of the input using masks
	uint16_t right_input = input & 0x003F; // 0000 0000 0011 1111
	uint16_t left_input  = (input & 0x0FC0) >> 6; // 0000 1111 1100 0000
	
	// Do the appropriate operations to those halves
	uint16_t left_output = right_input << 6;
	uint16_t right_output = feistel(right_input, key) ^ left_input;
	
	// Recombine the halves for output
	return left_output | right_output;
}

uint8_t *generate_round_keys(uint16_t original_key, unsigned int num_rounds) {
	// If the number of requested rounds is larger than the length of the key, return NULL
	if (num_rounds > KEY_LEN) { return NULL; }

	// Create the array to store the round keys and the initial masks
	uint8_t *round_keys = calloc(num_rounds, sizeof(uint8_t));
	uint32_t left_mask = (int)(pow(2, KEY_LEN - 1) - 1) << 1;  // 0 0000 0001 1111 1110
	uint32_t right_mask = left_mask << KEY_LEN; 	     	   // 1 1111 1110 0000 0000

	// Generate the number of keys that is requested
	for (unsigned int i = 0; i < num_rounds; i++) {
		// Apply the masks and align them on the left side of the number
		uint32_t left_key = (original_key & left_mask) << (KEY_LEN + i);
		uint32_t right_key = (original_key & right_mask) << (i);

		// Combine the halves, shift them into place, and add them to the array
		round_keys[i] = (left_key | right_key) >> (KEY_LEN + 1);
		
		// Move the masks over one position (to be ready for the next round)
		left_mask = left_mask >> 1;
		right_mask = right_mask >> 1;
	}
	return round_keys;
}

uint16_t encrypt(uint16_t unencrypted_data, uint8_t *round_keys, int num_rounds) {
	// Perform the feistel function for the number of specified rounds
	for (int n=0; n < num_rounds; n++)
		unencrypted_data = feistel_round(unencrypted_data, round_keys[n]);

	// Switch the left and right halves
	uint16_t right_encryptpted = (unencrypted_data & 0xFC0) >> 6;
	uint16_t left_encrypted = (unencrypted_data & 0x03F) << 6;
	
	return left_encrypted | right_encryptpted;
}

uint16_t decrypt(uint16_t encrypted_data, uint8_t *round_keys, int num_rounds) {
	// Perform the feistel function for the number of specified rounds
	// using the keys in the reverse order that they were used to encrypt
	for (int n=num_rounds-1; n >= 0; n--)
		encrypted_data = feistel_round(encrypted_data, round_keys[n]);
	
	// Switch the left and right halves
	uint16_t right_unencryptpted = (encrypted_data & 0xFC0) >> 6;
	uint16_t left_unencrypted = (encrypted_data & 0x03F) << 6;
	return left_unencrypted | right_unencryptpted;
}

// Note: This function was only used for testing
void print_bits(char *message, size_t n, uint8_t len) 
{
	// Print the specified message
	printf("%s: 0x%05zx --> ", message, n);
	
	// Create a single bit mask that starts at the left-most bit
	unsigned int i = 1 << (len - 1);

	// Pass over each bit
    for (unsigned int count = 0; count < len; i = i >> 1, count++) {
		// Compare the number to the mask and print the appropriate value
		if (n & i) { printf("1"); }
		else { printf("0"); }

		// Print spacing every four bits for ease of reading
		if ((count + 1) % 4 == 0)
			printf(" ");
	}
	printf("\n");
} 
