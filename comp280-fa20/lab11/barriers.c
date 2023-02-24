/**
 * 
 * Example program that uses Pthread barriers.
 *
 * @author: Sat Garcia (sat@sandiego.edu)
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>

// forward declarations
void *worker_task(void *arg);
void do_work(int time);
static void *Malloc(size_t size);

struct WorkerArgs {
	int id;
	int work_time;
	pthread_barrier_t *barrier;
};

typedef struct WorkerArgs WorkerArgs;

int main(int argc, char **argv) {

	// check that we gave the correct number of command line args
	if (argc != 2) {
		printf("usage: %s num_threads\n", argv[0]);
		exit(1);
	}

	// get the number of threads from the command line args
	int num_threads = strtol(argv[1], NULL, 10);
	if ((num_threads < 1) || (num_threads > 100)) { 
		num_threads = 10;
	}

	pthread_t *tids = Malloc(sizeof(pthread_t)*num_threads);
	WorkerArgs *thread_args = Malloc(sizeof(WorkerArgs)*num_threads);

	pthread_barrier_t shared_barrier; // declare our barrier

	// initialize the barrier, indicating how many threads need to reach it
	// before the barrier is opened.
	if (pthread_barrier_init(&shared_barrier, 0, num_threads) != 0) {
		perror("pthread_barrier_init");
		exit(1);
	}

	// Fill in the arguments for the worker threads
	for (int i = 0; i < num_threads; i++) {
		thread_args[i].id = i;

		// set "work time" to a random length between 5 and 15
		thread_args[i].work_time = (rand() % 10) + 5;

		// every thread needs access to the barrier
		thread_args[i].barrier = &shared_barrier;

		// Create a thread, having it run the worker_task function
		if (pthread_create(&tids[i], 0, worker_task, &thread_args[i]) != 0) {
			perror("pthread_create\n");
			exit(1);
		}
	}

	// Have the main thread wait for all the worker threads to finish
	for (int i=0; i < num_threads; i++) {
		// Using NULL for the 2nd arg of join means that we will ignore the
		// result of the thread with which we're joining.
		pthread_join(tids[i], NULL);
	}
	printf("Main thread is done waiting for workers!\n");

	free(thread_args);
	free(tids);
	
	return 0;
}

/**
 * "Wrapper" for malloc to handle error cases.
 *
 * @param size How many bytes to allocate from the heap (with malloc)
 */
static void *Malloc(size_t size) {
	void *mem = malloc(size);
	if (mem == NULL) {
		perror("malloc array");
		exit(1);
	}
	return mem;
}

/**
 * Thread function for all worker threads.
 *
 * @param arg A WorkerArgs *, with id and work_time fields
 *
 * @return NULL (unused... need to appease pthreads function format)
 */
void *worker_task(void *arg) {

	WorkerArgs* worker_args = (WorkerArgs*)arg;

	printf("I'm worker %d and I will be \"working\" for %d seconds\n",
			worker_args->id, worker_args->work_time);

	// Wait until all workers have printed out their message before they can
	// start their work
	pthread_barrier_wait(worker_args->barrier);

	do_work(worker_args->work_time);
	printf("Worker %d just finished!\n", worker_args->id);

	// Wait until all workers have finished working before continuing on.
	pthread_barrier_wait(worker_args->barrier);

	return NULL;
}

/**
 * Do some highly advanced calculations for the given amount of time.
 *
 * @param time The amount of time required for the "calculations"
 */
void do_work(int time) {
	sleep(time);
}
