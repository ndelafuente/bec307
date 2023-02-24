/*
 * simpleops.c
 *
 * An example C program that contains only basic arithmetic experessions.
 *
 * To compile to assembly:
 * 		gcc -S simpleops.c
 *
 * You can also just type: make
 */
int main() {
	int x, y;

	x = 1;
	x = x + 2;
	x = x - 14;
	y = x * 100;
	x = x + y * 6;

	return 0;
}
