CC = gcc
CFLAGS = -Wall -Wextra -g -std=c11

all: ls_and_cat

ls_and_cat: ls_and_cat.c
	$(CC) $(CFLAGS) -o $@ $^

clean:
	$(RM) ls_and_cat

