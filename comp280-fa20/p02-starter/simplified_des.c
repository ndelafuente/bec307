#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

#include "simplified_des.h"

const uint8_t s1_box[16] = { 5, 2, 1, 6, 3, 4, 7, 0, 1, 4, 6, 2, 0, 7, 5, 3 };

// TODO: Fill in s2_box with correct values
const uint8_t s2_box[16];

uint8_t expand(uint8_t input) {
	// TODO: Implement this function
	return input;
}

uint8_t confuse(uint8_t input) {
	// TODO: Implement this function
	return input;
}

uint8_t feistel(uint8_t input, uint8_t key) {
	// TODO: Implement this function
	return input ^ key;
}

uint16_t feistel_round(uint16_t input, uint8_t key) {
	// TODO: Implement this function
	return input ^ key;
}

uint8_t *generate_round_keys(uint16_t original_key, unsigned int num_rounds) {
	// TODO: Replace this "dummy" implementation with a correct implementation
	
	uint8_t *round_keys = calloc(2, sizeof(uint8_t));
	round_keys[0] = 0x55;
	round_keys[1] = 0xAA;
	return round_keys;
}

uint16_t encrypt(uint16_t unencrypted_data, uint8_t *round_keys, int num_rounds) {
	// TODO: Implement this function
	return unencrypted_data;
}

uint16_t decrypt(uint16_t encrypted_data, uint8_t *round_keys, int num_rounds) {
	// TODO: Implement this function
	return encrypted_data;
}
