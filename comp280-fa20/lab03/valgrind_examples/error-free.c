#include <stdio.h>
#include <stdlib.h>

int *generate_random_ints(int n) {
	int *nums = calloc(n, sizeof(int));

	// generate random values for the array
	for (int i = 0; i < n; i++) {
		nums[i] = rand();
	}

	return nums;
}

int main() {
	int *random_ints = generate_random_ints(5);

	printf("Generating 5 random integers...\n");
	for (int i = 0; i < 5; i++) {
		printf("\t%d: %d\n", i, random_ints[i]);
	}

	free(random_ints);

	return 0;
}
