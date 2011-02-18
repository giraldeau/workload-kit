/*
 * bossthread.c
 *
 *  Created on: 2011-02-18
 *      Author: francis
 */

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#ifdef WITH_UST
#include <ust/marker.h>
#endif
#include "calibrate.h"

#ifndef WITH_UST
#define trace_mark(...)
#endif

#define MAX_THREAD 4

struct worker_data {
	unsigned long int count;
	unsigned long int res;
};

void *worker(void *data) {
	struct worker_data *info = (struct worker_data *) data;
	int i;
	pthread_t self = pthread_self();
	trace_mark(ust, worker_entry, "id %lu", self);

	for(i=0; i<info->count; i++) {
		info->res++;
	}

	trace_mark(ust, worker_exit, "id %lu", self);
	return 0;
}

int main(int argc, char **argv) {

	trace_mark(ust, main_entry, "%d", 9);
	pthread_t threads[MAX_THREAD];
	void *status;
	int i = 0;
	struct worker_data *data[MAX_THREAD];
	unsigned long max;

	// count up to 10ms
	max = calibrate(10000);

	for(i=0;i<MAX_THREAD;i++) {
		data[i] = calloc(1, sizeof(struct worker_data));
		data[i]->count = max * (i + 1);
	}

	for(i=0; i<MAX_THREAD; i++) {
		pthread_create(&threads[i], NULL, worker, (void *) data[i]);
	}

	for(i=0; i<MAX_THREAD; i++) {
		pthread_join(threads[i], &status);
	}

	return 0;
}
