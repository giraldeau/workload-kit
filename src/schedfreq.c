/*
 * semrelay.c
 *
 *  Created on: Feb 21, 2014
 *      Author: francis
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <getopt.h>
#include <semaphore.h>

#define DEFAULT_THREAD 2
#define DEFAULT_REPEAT 1000000
#define DEFAULT_LIB "sems"
#define progname "wk-schedfreq"

struct experiment;

typedef void *(*worker_t)(void *);
typedef void (*init_t)(struct experiment *);
typedef void (*done_t)(struct experiment *);

struct lib_def {
    const char *name;
    worker_t worker;
    init_t init;
    done_t done;
};

struct experiment {
	struct lib_def *lib;
	int n;
	long repeat;
	void *data;
};

struct args {
	pthread_t thread;
	struct experiment *exp;
	int rank;
};

/*
 * force scheduling with a round-robin semaphore
 */

void *sems_wrk(void *arg) {
	struct args *args = arg;
	long i;
	long repeat = args->exp->repeat;
	sem_t *sems = args->exp->data;
	sem_t *curr = &sems[args->rank];
	sem_t *next = &sems[(args->rank + 1) % args->exp->n];
	for (i = 0; i < repeat; i++) {
		sem_wait(curr);
		sem_post(next);
	}
	return NULL;
}

void sems_init(struct experiment *exp) {
	int i;
	sem_t *sems = calloc(exp->n, sizeof(sem_t));
	sem_init(&sems[0], 0, 1);
	for (i = 1; i < exp->n; i++) {
		sem_init(&sems[i], 0, 0);
	}
	exp->data = sems;
}

void sems_done(struct experiment *exp) {
	free(exp->data);
}

/*
 * nanosleep() for a short duration produces a scheduling cycle
 */

void *ns_wrk(void *arg) {
	struct args *args = arg;
	long i;
	long repeat = args->exp->repeat;
	struct timespec ts = { 0, 0 };
	for (i = 0; i < repeat; i++) {
		nanosleep(&ts, NULL);
	}
	return NULL;
}

static struct lib_def libs[] = {
	{ .name = "sems", .worker = sems_wrk, .init = sems_init, .done = sems_done, },
	{ .name = "nanosleep", .worker = ns_wrk, .init = NULL, .done = NULL, },
	{ .name = NULL, .worker = NULL, .init = NULL, .done = NULL, },
};

__attribute__((noreturn))
static void usage(void) {
    fprintf(stderr, "Usage: %s [OPTIONS] [COMMAND]\n", progname);
    fprintf(stderr, "\nOptions:\n\n");
    fprintf(stderr, "--thread N       number of threads to be spawned (default = 2)\n");
    fprintf(stderr, "--repeat         number of repetitions\n");
    fprintf(stderr, "--lib            library to use [ sems | nanosleep ]\n");
    fprintf(stderr, "--verbose        be verbose\n");
    fprintf(stderr, "--help           print this message and exit\n");
    fprintf(stderr, "\n");
    exit(EXIT_FAILURE);
}

struct lib_def *get_lib(char *name) {
	int i;
	for (i = 0; libs[i].name != NULL; i++) {
		if (strcmp(name, libs[i].name) == 0) {
			return &libs[i];
		}
	}
	return NULL;
}

static void parse_opts(int argc, char **argv, struct experiment *exp) {
    int opt;
    int idx;

    struct option options[] = {
            { "help",       0, 0, 'h' },
            { "repeat",     1, 0, 'r' },
            { "thread",     1, 0, 't' },
            { "lib",        1, 0, 'l' },
            { "verbose",    0, 0, 'v' },
            { 0, 0, 0, 0 }
    };

    exp->n = DEFAULT_THREAD;
    exp->repeat = DEFAULT_REPEAT;
    exp->lib = get_lib(DEFAULT_LIB);

    while ((opt = getopt_long(argc, argv, "hvr:t:l:", options, &idx)) != -1) {
        switch (opt) {
        case 'r':
			exp->repeat = atoi(optarg);
			break;
		case 't':
			exp->n = atoi(optarg);
			break;
		case 'l':
			exp->lib = get_lib(optarg);
			break;
        case 'h':
            usage();
            break;
        default:
            usage();
            break;
        }
    }

    if (exp->lib == NULL) {
    	usage();
    }
}

int main(int argc, char **argv) {
	int i;
	struct experiment exp;
	parse_opts(argc, argv, &exp);

	struct args *args = calloc(exp.n, sizeof(struct args));

	if (exp.lib->init)
		exp.lib->init(&exp);
	for (i = 0; i < exp.n; i++) {
		args[i].exp = &exp;
		args[i].rank = i;
		pthread_create(&args[i].thread, NULL, exp.lib->worker, &args[i]);
	}
	for (i = 0; i < exp.n; i++) {
		pthread_join(args[i].thread, NULL);
	}
	if (exp.lib->done)
		exp.lib->done(&exp);
	free(args);
	return 0;
}
