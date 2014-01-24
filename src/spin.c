/*
 * spin.c
 *
 *  Created on: Jan 23, 2014
 *      Author: francis
 */

#define _POSIX_PTHREAD_SEMANTICS
#include <time.h>
#include <stdio.h>
#include <errno.h>
#include <stdarg.h>
#include <string.h>
#include <limits.h>
#include <stdlib.h>
#include <signal.h>
#include <sys/time.h>
#include <semaphore.h>

static volatile int run;
static sem_t *sem_start;
static sem_t *sem_done;
static int *ids;
static int nr_thread;
static volatile int cont;
static pthread_t *threads;

static void signalhandler (int signr) {
    (void) signr;
    run = 0;
    __sync_synchronize();
}

void *worker(void *arg) {
	int i = *((int*)arg);
	while(1) {
		sem_wait(&sem_start[i]);
		if(!cont) {
			break;
		}
		volatile unsigned long x;
		while(run) {
			x++;
		}
		sem_post(&sem_done[i]);
	}
	return NULL;
}

void spin_init(int n)
{
	int i;

	if(cont) {
		return;
	}

	nr_thread = n;
	threads = calloc(n, sizeof(pthread_t));
	sem_start = calloc(n, sizeof(sem_t));
	sem_done = calloc(n, sizeof(sem_t));
	ids = calloc(n, sizeof(int));
	if (threads == NULL || sem_start == NULL || sem_done == NULL || ids == NULL)
		goto err;
	cont = 1;

	// define action
	signal(SIGALRM, signalhandler);

	for(i = 0; i < nr_thread; i++) {
		sem_init(&sem_start[i], 0, 0);
		sem_init(&sem_done[i], 0, 0);
	}

	for (i = 0; i < n; i++) {
		ids[i] = i;
		pthread_create(&threads[i], NULL, worker, (void *)&ids[i]);
	}
	return;

err:
	free(threads);
	free(sem_start);
	free(sem_done);
	free(ids);
	return;
}

void spin_exit()
{
	int i;

	if(!cont) {
		return;
	}
	cont = 0;
	__sync_synchronize();

	for(i = 0; i < nr_thread; i++) {
		sem_post(&sem_start[i]);
	}
	for (i = 0; i < nr_thread; i++) {
		pthread_join(threads[i], NULL);
	}

	for(i = 0; i < nr_thread; i++) {
		sem_destroy(&sem_start[i]);
	}
	free(threads);
	free(sem_start);
	free(sem_done);
	free(ids);
}

void spin(long usec) {
    struct itimerval timer;
    int i;

	if(!cont) {
		return;
	}

    run = 1;
    __sync_synchronize();

    timer.it_interval.tv_sec = 0;
    timer.it_interval.tv_usec = 0;
    timer.it_value.tv_sec = 0;
    timer.it_value.tv_usec = usec;

    // start timer
    if (setitimer (ITIMER_REAL, &timer, NULL)) {
        error(EXIT_FAILURE, "setitimer failed");
    }

    for(i = 0; i < nr_thread; i++) {
    	sem_post(&sem_start[i]);
    }
    for(i = 0; i < nr_thread; i++) {
    	sem_wait(&sem_done[i]);
    }
}
