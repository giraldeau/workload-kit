/*
 * threadtree.c
 *
 *  Created on: 2012-05-27
 *      Author: francis
 */

#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <semaphore.h>
#include <errno.h>
#include <getopt.h>
#include <pthread.h>
#include <omp.h>

#define PROGNAME "wk-threadtree"
#define NB_THREAD 3
#define NB_STAGE 3

struct command_opts {
	int verbose;
};

typedef struct stage stage_t;

typedef struct stage {
	int nb_thread;
	pthread_t *threads;
	sem_t sem_join;
	sem_t mutex;
	int exit_count;
	stage_t *next;
} stage_t;

typedef struct task {
	int nb_stages;
	stage_t *stages;
} task_t;

__attribute__((noreturn))
static void usage(void)
{
	fprintf(stderr, "Usage: " PROGNAME " [OPTIONS] [COMMAND]\n");
	fprintf(stderr, "\nOptions:\n");
	fprintf(stderr, "  --help	this help\n");
	fprintf(stderr, "  --verbose   verbose mode\n");
	fprintf(stderr, "\n");
	exit(EXIT_FAILURE);
}

static void dump_opts(struct command_opts *opts)
{
	printf("%10s %s\n", "option", "value");
	printf("%10s %d\n", "verbose", opts->verbose);
}

static int parse_opts(int argc, char **argv, struct command_opts *opts)
{
	int idx;
	int opt;
	int ret = 0;

	struct option options[] = {
			{ "help",    0, 0, 'h' },
			{ "verbose", 0, 0, 'v' },
			{ 0, 0, 0, 0}
	};

	memset(opts, 0, sizeof(struct command_opts));

	while ((opt = getopt_long(argc, argv, "hv", options, &idx)) != -1) {
		switch(opt) {
		case 'h':
			usage();
			break;
		case 'v':
			opts->verbose = 1;
			break;
		default:
			printf("unknown option %c\n", opt);
			ret = -1;
			break;
		}
	}

	/* default values*/
	return ret;
}

void *thread_worker(void *arg);

void spawn_stage(stage_t *stage) {
	int i;
	if (stage == NULL)
		return;
	for (i = 0; i < stage->nb_thread; i++) {
		pthread_t *thd = stage->threads + i;
		printf("pthread_create\n");
		pthread_create(thd, NULL, thread_worker, stage);
	}
}

void do_something() {
	printf("%lu do_something\n", pthread_self());
	sleep(1);
}

void *thread_worker(void *arg) {
	int i;
	stage_t *stage = ((stage_t *)arg);

	printf("start thread_worker %lu\n", pthread_self());
	do_something();

	sem_wait(&stage->mutex);
	stage->exit_count--;
	if (stage->exit_count == 0) {
		sem_post(&stage->sem_join);
		spawn_stage(stage->next);
	}
	sem_post(&stage->mutex);
	return NULL;
}

task_t *make_task(int nb_stages, int nb_threads) {
	int i, j;
	stage_t **curr;
	task_t *task = calloc(1, sizeof(task_t));
	task->nb_stages = nb_stages;
	curr = &task->stages;
	for (i = 0; i < nb_stages; i++) {
		stage_t *stage = calloc(1, sizeof(stage_t));
		stage->nb_thread = nb_threads;
		stage->exit_count = nb_threads;
		sem_init(&stage->sem_join, 0, 0);
		sem_init(&stage->mutex, 0, 1);
		stage->threads = calloc(nb_threads, sizeof(pthread_t));
		*curr = stage;
		curr = &stage->next;
	}
	return task;
}

void free_task(task_t *task) {
	stage_t *curr, *next;
	if (task == NULL)
		return;
	if (task->stages == NULL)
		goto end;
	for (curr = task->stages; curr != NULL;) {
		if (curr->threads != NULL)
			free(curr->threads);
		next = curr->next;
		free(curr);
		curr = next;
	}
end:
	free(task);
}

void dump_task(task_t *task) {
	int i = 0;
	stage_t *curr;
	if (task == NULL)
		return;
	printf("task nb_stages=%d\n", task->nb_stages);
	for (curr = task->stages; curr != NULL;) {
		printf("\tstage %d: nb_thread=%d exit_count=%d\n", i++,
				curr->nb_thread, curr->exit_count);
		curr = curr->next;
	}
}

void wait_task(task_t *task) {
	stage_t *stage;
	int i, nb = 0;
	for (stage = task->stages; stage != NULL; stage = stage->next) {
		printf("main: wait stage %d\n", nb++);
		sem_wait(&stage->sem_join);
		for (i = 0; i < stage->nb_thread; i++) {
			pthread_join(stage->threads[i], NULL);
		}
	}
}

int main(int argc, char **argv) {
	int ret = 0;
	int i = 0;
	struct command_opts opts;

	if (parse_opts(argc, argv, &opts) < 0) {
		printf("Error while parsing arguments\n");
		usage();
	}
	dump_opts(&opts);
	task_t *task = make_task(2, 2);
	dump_task(task);
	spawn_stage(task->stages);
	wait_task(task);
	free_task(task);
done:
	printf("done\n");
	return ret;
err:
	ret = -1;
	goto done;
}
