CC = gcc
CFLAGS = -Wall -Wextra -g -std=c11

all: circqueue-test

circqueue-test: circqueue-test.c circqueue.o
	$(CC) $(CFLAGS) -o $@ $^

circqueue.o: circqueue.c
	$(CC) $(CFLAGS) -c -o $@ $<

clean:
	$(RM) *.o circqueue-test

