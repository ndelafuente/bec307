#ifndef CIRCQUEUE_H
#define CIRCQUEUE_H

/** 
 * Header file for circular queue (circqueue) library.
 * Users of the library will include this file.
 */

// The maximum number of elements that can be in the queue
#define QUEUE_CAPACITY 6

// declarations of public library functions

/**
 * Adds the given value to the circular queue.
 *
 * @param val The value to add to the queue.
 */
void add_queue(int val);


/*
 * TODO: Add a declaration for the print_queue function and delete this
 * comment.
 * Remember our mantra: "A function's always better with a header... comment"
 * (i.e. don't forget the header comment).
 */

#endif
