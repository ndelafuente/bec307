CC = gcc
CFLAGS = -Wall -Wextra -std=c11
TARGETS = simpleops memparts pointers.s 

all: $(TARGETS)

memparts: memparts.c
	$(CC) -o $@ $<

pointers.s: pointers.c 
	$(CC) -S $<

# generate executable along with intermediate assembly and object code
simpleops: simpleops.c
	$(CC) -S $<
	$(CC) -c $@.s
	$(CC) -g -o $@ $<

# clean up (remove) all files that can be re-built from source files
clean: 
	$(RM) $(TARGETS) *.s *.o
