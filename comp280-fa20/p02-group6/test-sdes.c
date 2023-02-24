#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

#include "simplified_des.h"
#include "mut.h"

TESTSUITE_BEGIN

TEST_BEGIN ("expand")
	uint8_t expanded = expand(0x35);
	//printf("%hhx\n", expanded);
	CHECK(expanded == 0xE9);
TEST_END

TEST_BEGIN ("confuse")
	// test case 1
	uint8_t jumbled = confuse(0x74);
	//printf("%hhx\n", jumbled);
	CHECK(jumbled == 0x07);

	// test case 2
	jumbled = confuse(0x0A);
	//printf("%hhx\n", jumbled);
	CHECK(jumbled == 0x28);
TEST_END

TEST_BEGIN ("feistel function")
	// test case 1
	uint8_t output = feistel(0x25, 0x3B);
	//printf("%hhx\n", output);
	CHECK(output == 0x26);

	// test case 2
	output = feistel(0x35, 0xE3);
	//printf("%hhx\n", output);
	CHECK(output == 0x28);
TEST_END

TEST_BEGIN ("one feistel round")
	// test case 1 function
	uint16_t round_output = feistel_round(0xD65, 0x3B);
	//printf("%hx\n", round_output);
	CHECK(round_output == 0x953)

	// test case 2 function
	round_output = feistel_round(0x8B5, 0xE3);
	//printf("%hx\n", round_output);
	CHECK(round_output == 0xD4A)
TEST_END

TEST_BEGIN ("generate_round_keys")
	// test case 1
	uint8_t *keys = generate_round_keys(0x13B, 4);
	/*
	for (int i = 0; i < 4; i++) {
		printf("round %d: %hhx\n", i, keys[i]);
	}
	*/
	CHECK(keys[0] == 0x9D);
	CHECK(keys[1] == 0x3B);
	CHECK(keys[2] == 0x77);
	CHECK(keys[3] == 0xEE);
	free(keys);

	// test case 2
	keys = generate_round_keys(0x1C7, 2);
	/*
	for (int i = 0; i < 2; i++) {
		printf("round %d: %hhx\n", (i+1), keys[i]);
	}
	*/
	CHECK(keys[0] == 0xE3);
	CHECK(keys[1] == 0xC7);
	free(keys);

	// test case 3 (too many rounds!)
	keys = generate_round_keys(0x1C7, 10);
	CHECK(keys == NULL);
TEST_END

TEST_BEGIN ("encrypt")
	// test case 1
	uint8_t keys[2] = { 0x9D, 0x3B };
	uint16_t encrypted = encrypt(0x8B5, keys, 2);
	//printf("%hx\n", encrypted);
	CHECK(encrypted == 0x4E5);
TEST_END

TEST_BEGIN ("decrypt")
	// test case 1
	uint8_t keys[2] = { 0x9D, 0x3B };
	uint16_t decrypted = decrypt(0x4E5, keys, 2);
	//printf("%hx\n", decrypted);
	CHECK(decrypted == 0x8B5);
TEST_END

TESTSUITE_END
