/**
 * Implementation of the BoundedBuffer class.
 * See the associated header file (BoundedBuffer.hpp) for the declaration of
 * this class.
 */
#include <cstdio>

#include "BoundedBuffer.hpp"

/**
 * Constructor that sets capacity to the given value. The buffer itself is
 * initialized to en empty queue.
 *
 * @param max_size The desired capacity for the buffer.
 */
BoundedBuffer::BoundedBuffer(int max_size) {
	capacity = max_size;

	// buffer field implicitly has its default (no-arg) constructor called.
	// This means we have a new buffer with no items in it.
}

/**
 * Gets the first item from the buffer then removes it.
 */
int BoundedBuffer::getItem() {
	// Create and acquire the lock
	std::unique_lock<std::mutex> lk(this->mutex);
	
	// Wait for the producer to put something on the buffer
	while (this->buffer.size() == 0)
		this->dataAvailable.wait(lk);
	
	// Remove the data from the buffer
	int item = this->buffer.front(); // "this" refers to the calling object...
	this->buffer.pop(); // ... but like Java it is optional (no this in front of buffer on this line)

	// Send a signal to the waiting thread
	this->spaceAvailable.notify_one();

	// Release the ownership of the lock
	lk.unlock();

	return item;
}

/**
 * Adds a new item to the back of the buffer.
 *
 * @param new_item The item to put in the buffer.
 */
void BoundedBuffer::putItem(int new_item) {
	// Create and acquire the lock
	std::unique_lock<std::mutex> lk(this->mutex);

	// Wait for space to become available
	while (this->buffer.size() == this->capacity)
		this->spaceAvailable.wait(lk);
	
	// Push the new item onto the buffer
	buffer.push(new_item);
	
	// Let a consumer know that there is new data
	this->dataAvailable.notify_one();

	// Unlock the lock
	lk.unlock();
}
