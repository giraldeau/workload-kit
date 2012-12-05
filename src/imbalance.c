/*
 * imbalance.c
 *
 * Launches 4 busy threads with imbalance workload.
 * The computation runs for 8 cycles with a barrier between each.
 * Each cycle, the slowest thread is changed in a round-robin fashion.
 *
 *  Created on: 2012-12-04
 *      Author: francis
 */

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <signal.h>
#include "calibrate.h"

pthread_barrier_t barrier;

#define THREADS 4
#define CYCLES 8
#define DELAY 10000

int max = 0;

void *worker(void *data) {
	int i;
	int self = (long) data;
	int pos = self;

	for (i = 0; i < CYCLES; i++) {
		//printf("self=%d pos=%d\n", self, pos);
		do_hog((pos + 1) * max);
		pthread_barrier_wait(&barrier);
		pos = (pos + 1) % THREADS;
	}
	return 0;
}

int main(int argc, char **argv) {

	long i;
	pthread_t threads[THREADS];
	void *status;

	pthread_barrier_init(&barrier, NULL, THREADS);
	max = calibrate(25000);
	for (i = 0; i < THREADS; i++) {
		pthread_create(&threads[i], NULL, worker, (void *) i);
	}
	for (i = 0; i < THREADS; i++) {
		pthread_join(threads[i], NULL);
	}
	return 0;
}
