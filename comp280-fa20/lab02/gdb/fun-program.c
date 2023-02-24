#include <stdio.h>
#include <stdlib.h>

void fun_facts(int n);

int main(int argc, char** argv) {
	printf("You entered %d command line arguments\n", argc);
	for (int i = 0; i < argc; i++) {
		printf("\t%d: %s\n", i, argv[i]);
	}

	char *first_name = calloc(20, sizeof(char));
	printf("Enter your first name: ");
	scanf("%s", first_name);

	printf("Nice to meet you, %s\n", first_name);

	int selected_int = 0;
	printf("Enter an integer between -10 and 10: ");
	scanf("%d", &selected_int);

	fun_facts(selected_int);

	char selected_char =  0;
	printf("Enter a single, lower-case letter (e.g. t): ");
	scanf(" %c", &selected_char);

	printf("How did you know I was thinking of the letter %c?\n", selected_char);

	free(first_name);

	return 0;
}

void fun_facts(int n) {
	if (n >= 0) {
		printf("You entered a non-negative number. Excellent choice!\n");
	}
	else {
		printf("You entered a negative number. Stupendous!\n");
	}

	int doubled_num = n * 2;
	printf("Did you know that if you double that number, you get %d? Isn't that interesting?!?!\n", doubled_num);
}
