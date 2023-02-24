CC = gcc
CFLAGS = -g -Wall -Wextra -std=c11

TARGETS = fun-program segfaulter

all: $(TARGETS)

fun-program: fun-program.c 
		$(CC) $(CFLAGS) -o $@ $<

segfaulter: segfaulter.c 
		$(CC) $(CFLAGS) -o $@ $<

clean:
		$(RM) $(TARGETS)
