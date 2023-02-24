/**
 * File: circqueue.c
 *
 * Implementation of circular queue (circqueue) library.
 */

/*
 * Including a file literally copies its contents into this file.
 * This means we'll have the definition of QUEUE_CAPACITY as well as the
 * forward declarations for add_queue and print_queue in this file, without
 * having to type them out again.
 */
#include "circqueue.h"


/**
 * We'll use global variables for the queue and its state so that is persists
 * across function calls and the user doesn't have to know anything about the
 * internals.
 *
 * The downside of this setup is that means we can only have a single circular
 * queue in our application.
 */

static int queue[QUEUE_CAPACITY];

static int queue_start = 0; // the index where queue begins

static int queue_next = 0; 	// The index where we will insert next item.
							// This should change when we add something
							// to the queue.

static int queue_size = 0;  // The number of items in the queue.
							// Note that the capacity (i.e. max size) is
							// QUEUE_CAPACITY.

// TODO: implement this function then delete this comment
void add_queue(int val) {
}

// TODO: implement this function then delete this comment
void print_queue() {
}
