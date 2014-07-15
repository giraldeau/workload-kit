/*
 * test-uevent.c
 *
 *  Created on: 2012-07-02
 *      Author: francis
 *
 *  Stress test for lttng-uevent module insertion and removal
 */

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <error.h>
#include <inttypes.h>
#include <semaphore.h>

#define LTTNG_FILE "/proc/lttng_uevent"
#define NB_THREAD 20
#define NB_EXP 4
#define NB_EV (1 << 16)
#define MSG "test-uevent"
#define MAX 1024

void *worker(void *arg) {
	uint64_t i, ret, miss = 0;
	long self = (long) arg;
	FILE *f = NULL;
	while(f == NULL) {
		f = fopen(LTTNG_FILE, "w");
	}
	for (i = 0; i < NB_EV; i++) {
		ret = fprintf(f, "%s %" PRId64 " %" PRId64, MSG, self, i);
		if (ret < 0)
			miss++;
		fflush(f);
	}
	fclose(f);
	printf("Done %" PRId64 " misses=%" PRId64 "\n", self, miss);
	return NULL;
}

void do_run() {
	long i;
	int x, y;
	pthread_t t[NB_THREAD];

	for (i = 0; i < NB_THREAD; i++) {
		pthread_create(&t[i], NULL, worker, (void *) i);
	}

	for (i = 0; i < NB_THREAD; i++) {
		pthread_join(t[i], NULL);
	}
}

int main(int argc, char **argv) {
	int i;
	printf("Start main\n");
	for (i = 0; i < 10; i++) {
		do_run();
	}
	printf("Done main\n");
	return 0;
}
