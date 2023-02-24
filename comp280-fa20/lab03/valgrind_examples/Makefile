CC=gcc
CFLAGS=-g -Wall -Wextra -std=c11

TARGETS=illegal_rw leaky uninitialized error-free

all: $(TARGETS)

#illegal_rw leaky uninitialized: %.c
%: %.c
	$(CC) $(CFLAGS) -o $@ $^

clean:
	rm -f $(TARGETS)
