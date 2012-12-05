/*
 * deadlock.c
 *
 *  Creates a deadlock between two threads and quit on timer event.
 *
 *  Created on: 2011-03-01
 *      Author: francis
 */

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <signal.h>

pthread_mutex_t mutex_a;
pthread_mutex_t mutex_b;

#define DELAY 10000

void *worker_a(void *data) {
	pthread_mutex_lock(&mutex_a);
	usleep(DELAY);
	pthread_mutex_lock(&mutex_b);
	// deadlock
	return 0;
}

void *worker_b(void *data) {
	pthread_mutex_lock(&mutex_b);
	usleep(DELAY);
	pthread_mutex_lock(&mutex_a);
	// deadlock
	return 0;
}


int main(int argc, char **argv) {

	pthread_t threads[2];
	void *status;

	pthread_mutex_init(&mutex_a, NULL);
	pthread_mutex_init(&mutex_b, NULL);
	pthread_create(&threads[0], NULL, worker_a, NULL);
	pthread_create(&threads[1], NULL, worker_b, NULL);
	usleep(DELAY * 10);

	pthread_kill(threads[0], SIGKILL);
	pthread_kill(threads[1], SIGKILL);

	return 0;
}
