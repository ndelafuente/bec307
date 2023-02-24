CC = gcc
CFLAGS = -Wall -Wextra -g -std=c11

TARGETS = command-line-args

all: $(TARGETS)

command-line-args: command-line-args.c
	$(CC) $(CFLAGS) -o $@ $<

clean:
	$(RM) $(TARGETS)
