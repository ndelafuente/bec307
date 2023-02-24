#include <stdio.h>
#include <stdlib.h>

unsigned char get_max_unsigned_char() {
	// TODO: Implement this function
	return 0;
}

unsigned char zero_middle_bits(unsigned char x) {
	// TODO: Implement this function
	return x;
}

unsigned char give_me_92() {
	// TODO: Implement this function
	return 0;
}

unsigned char is_even(unsigned char x) {
	// TODO: Implement this function
	return x;
}

/**
 * Test all of the puzzles to make sure their operation is correct.
 * DO NOT MODIFY ANYTHING BELOW THIS POINT!!!
 */
int main() {
	int num_failed = 0;

	if (get_max_unsigned_char() != 255) {
		printf("get_max_unsigned_char failed.\n");
		printf("\tExpected: 255, Got: %hhu.\n", get_max_unsigned_char());
		num_failed++;
	}
	else
		printf("get_max_unsigned_char passed.\n");

	if (give_me_92() != 92) {
		printf("give_me_92 failed.\n");
		printf("\tExpected: 92, Got: %hhu.\n", give_me_92());
		num_failed++;
	}
	else
		printf("give_me_92 passed.\n");

	unsigned char zmb_inputs[6] = {0, 1, 155, 0xC3, 0xFF, 0x3C};
	unsigned char zmb_expected[6] = {0, 1, 131, 0xC3, 0xC3, 0};
	int zmb_failed = 0;

	for (int i = 0; i < 6; i++) {
		if (zero_middle_bits(zmb_inputs[i]) != zmb_expected[i]) {
			printf("zero_middle_bits(%hhu) failed.\n", zmb_inputs[i]);
			printf("\tExpected: %hhu, Got: %hhu.\n", zmb_expected[i], zero_middle_bits(zmb_inputs[i]));
			num_failed++;
			zmb_failed = 1;
			break;
		}
	}

	if (zmb_failed == 0)
			printf("zero_middle_bits passed.\n");

	unsigned char ie_inputs[6] = {0, 1, 6, 77, 254, 255};
	unsigned char ie_expected[6] = {1, 0, 1, 0, 1, 0};
	int ie_failed = 0;

	for (int i = 0; i < 6; i++) {
		if (is_even(ie_inputs[i]) != ie_expected[i]) {
			printf("is_even(%hhu) failed.\n", ie_inputs[i]);
			printf("\tExpected: %hhu, Got: %hhu.\n", ie_expected[i], is_even(ie_inputs[i]));
			num_failed++;
			ie_failed = 1;
			break;
		}
	}

	if (ie_failed == 0)
			printf("is_even passed.\n");

	printf("\nTest summary:\t");
	printf("%d tests failed.\n", num_failed);

	if (num_failed == 0)
		printf("Congratulations!\n");

	return 0;
}
