CC = gcc
CFLAGS = -Wall -g -std=c11

TARGETS = file-io command-line-args calc-average-grades

all: $(TARGETS)

file-io: file-io.c
	$(CC) $(CFLAGS) -o $@ $<

command-line-args: command-line-args.c
	$(CC) $(CFLAGS) -o $@ $<

calc-average-grades: calc-average-grades.c
	$(CC) $(CFLAGS) -o $@ $<

clean:
	$(RM) $(TARGETS)
