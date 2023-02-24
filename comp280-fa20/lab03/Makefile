CC = gcc
CFLAGS = -g -Wall -Wextra -std=c11

TARGETS = puzzle-tester

all: $(TARGETS)

puzzle-tester: bit-puzzles.c 
	$(CC) $(CFLAGS) -o $@ $<

clean:
	$(RM) $(TARGETS)
