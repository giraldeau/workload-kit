/*
 * mutex.c
 *
 * Exercice contention on mutex. The first worker thread locks the
 * mutex and the second worker() thread waits for it to be released.
 *
 *  Created on: 2012-12-04
 *      Author: francis
 */

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <signal.h>

pthread_mutex_t mutex;

#define DELAY 10000

void *worker(void *data) {
	pthread_mutex_lock(&mutex);
	usleep(DELAY);
	pthread_mutex_unlock(&mutex);
	return 0;
}

int main(int argc, char **argv) {

	pthread_t threads[2];
	void *status;

	pthread_mutex_init(&mutex, NULL);
	pthread_create(&threads[0], NULL, worker, NULL);
	usleep(DELAY / 10);
	pthread_create(&threads[1], NULL, worker, NULL);

	printf("pthread_join worker 1\n");
	pthread_join(threads[0], NULL);
	printf("pthread_join worker 1\n");
	pthread_join(threads[1], NULL);

	return 0;
}
