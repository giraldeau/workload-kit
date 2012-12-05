/*
 * pipeline.c
 *
 * This program simulates a queued pipeline processing.
 * The pipeline has 3 stages. The middle stage limits the processing.
 *
 *  Created on: 2012-12-04
 *      Author: francis
 */

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>
#include <string.h>

#define PIPELINE_DEPTH 3
#define QUEUE_LENGTH 4
#define MAX_ITEMS 10
#define DELAY 10000

typedef struct queue {
	char data[QUEUE_LENGTH];
	sem_t sem_free;
	sem_t sem_busy;
	int ip;
	int ic;
} queue_t;

void do_action(char *data, int udelay, char *name) {
	printf("do_action %d %s\n", *data, name);
	*data = *data + 1;
	usleep(udelay);
}

void *worker_1(void *arg) {
	queue_t *queues = (queue_t *) arg;
	char c = 0;
	int processed = 0;
	while (1) {
		sem_wait(&queues[0].sem_free);
		if (processed >= MAX_ITEMS) {
			queues[0].data[queues[0].ip] = -1;
			sem_post(&queues[0].sem_busy);
			break;
		}
		do_action(&c, 0, "worker_1");
		queues[0].data[queues[0].ip] = c;
		queues[0].ip = (queues[0].ip + 1) % QUEUE_LENGTH;
		sem_post(&queues[0].sem_busy);
		c = 0;
		processed++;
	}
	return NULL;
}

void *worker_2(void *arg) {
	queue_t *queues = (queue_t *) arg;
	char c;
	while (1) {
		sem_wait(&queues[0].sem_busy);
		sem_wait(&queues[1].sem_free);
		c = queues[0].data[queues[0].ic];
		if (c < 0) {
			queues[1].data[queues[1].ip] = c;
			sem_post(&queues[1].sem_busy);
			break;
		}
		do_action(&c, DELAY, "worker_2");
		queues[1].data[queues[1].ip] = c;
		queues[0].ic = (queues[0].ic + 1) % QUEUE_LENGTH;
		queues[1].ip = (queues[1].ip + 1) % QUEUE_LENGTH;
		sem_post(&queues[0].sem_free);
		sem_post(&queues[1].sem_busy);
	}
	return NULL;
}

void *worker_3(void *arg) {
	queue_t *queues = (queue_t *) arg;
	char c;
	while (1) {
		sem_wait(&queues[1].sem_busy);
		c = queues[1].data[queues[1].ic];
		if (c < 0)
			break;
		do_action(&c, 0, "worker_3");
		queues[1].ic = (queues[1].ic + 1) % QUEUE_LENGTH;
		sem_post(&queues[1].sem_free);
	}
	return NULL;
}

int main(int argc, char **argv) {
	int i;
	pthread_t thds[PIPELINE_DEPTH];
	queue_t queues[PIPELINE_DEPTH - 1];

	memset(&queues, 0, sizeof(queues));
	for (i = 0; i < PIPELINE_DEPTH - 1; i++) {
		sem_init(&queues[i].sem_free, 0, QUEUE_LENGTH);
		sem_init(&queues[i].sem_busy, 0, 0);
	}

	pthread_create(&thds[0], NULL, worker_1, &queues);
	pthread_create(&thds[1], NULL, worker_2, &queues);
	pthread_create(&thds[2], NULL, worker_3, &queues);

	for (i = 0; i < PIPELINE_DEPTH; i++) {
		pthread_join(thds[i], NULL);
	}
	printf("exit\n");
	return EXIT_SUCCESS;
}
