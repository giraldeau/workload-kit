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

#include "pincpu.h"
#include "spin.h"

static volatile int run;
static sem_t *sem_start;
static sem_t *sem_done;
static struct spin_args *args;
static int nr_thread;
static volatile int cont;
static pthread_t *threads;

static void signalhandler (int signr) {
    (void) signr;
    run = 0;
    __sync_synchronize();
}

void *worker(void *arg) {
	struct spin_args *args = arg;
	if (args->spin->init)
		args->spin->init(args);
	while(1) {
		sem_wait(&sem_start[args->id]);
		if(!cont) {
			break;
		}
		volatile unsigned long x;
		while(run) {
			x++;
		}
		sem_post(&sem_done[args->id]);
	}
	if (args->spin->done)
		args->spin->done(args);
	return NULL;
}

void spin_init(struct spin *spin)
{
	int i;

	if(cont) {
		return;
	}

	nr_thread = spin->n;
	threads = calloc(spin->n, sizeof(pthread_t));
	sem_start = calloc(spin->n, sizeof(sem_t));
	sem_done = calloc(spin->n, sizeof(sem_t));
	args = calloc(spin->n, sizeof(struct spin_args));
	if (threads == NULL || sem_start == NULL || sem_done == NULL || args == NULL)
		goto err;
	cont = 1;

	// define action
	signal(SIGALRM, signalhandler);

	for(i = 0; i < nr_thread; i++) {
		sem_init(&sem_start[i], 0, 0);
		sem_init(&sem_done[i], 0, 0);
	}

	for (i = 0; i < spin->n; i++) {
		args[i].id = i;
		args[i].spin = spin;
		pthread_create(&threads[i], NULL, worker, &args[i]);
	}
	return;

err:
	free(threads);
	free(sem_start);
	free(sem_done);
	free(args);
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
	free(args);
}

void spin_run(long usec) {
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
