CC = gcc
CFLAGS = -g -Wall -Wextra -std=c11 -lncurses

TARGETS = gol

GOL_LIB=gol.o

all: $(TARGETS)

gol: main.c $(GOL_LIB)
	$(CC) $(CFLAGS) -o $@ $^

$(GOL_LIB): gol.c gol.h
		$(CC) -c $(CFLAGS) $<

rle_reader: rle_reader.c
		$(CC) $(CFLAGS) -o $@ $^

clean:
	$(RM) $(TARGETS) $(GOL_LIB) rle_reader

