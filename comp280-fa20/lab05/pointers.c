int pointers() {
	int x = 8;
	int y;
	int *ptr = &y;
	*ptr = 30;
	x = *ptr + 20;
	return x;
}
