CC = gcc
CFLAGS = -Wall -g  -pthread

TARGETS = barriers

all: $(TARGETS) 

barriers: barriers.c
	$(CC) $(CFLAGS) -o $@ $^ 

clean:
	$(RM) $(TARGETS) 
