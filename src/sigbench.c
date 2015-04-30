/*
 * sigbench.c
 *
 *  Created on: Mar 3, 2015
 *      Author: francis
 *
 *  Test signal scalability
 */


/*
 * interference.c
 *
 *  Created on: Jan 23, 2014
 *      Authors: Francis Giraldeau <francis.giraldeau@gmail.com>
 *      		 Mohamad Gebai <mohamad.gebai@gmail.com>
 *
 * continuous: keep all CPUs busy
 * periodic: add load on specific CPUs each 100ms
 *
 * threads are pinned on each CPUs from 0 to n
 *
 * arguments: thread, delay
 */

#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <inttypes.h>
#include <pthread.h>
#include <getopt.h>
#include <sys/time.h>
#include <unistd.h>
#include <sched.h>
#include <signal.h>
#include <omp.h>

#include "utils.h"

#define DEFAULT_THREAD -1
#define DEFAULT_REPEAT 1000000
#define USEC_PER_SEC 1000000
#define NSEC_PER_SEC 1000000000
#define progname "wk-sigbench"

struct experiment {
	int nr_thread;
	int foreach;
	int repeat;
	int verbose;
};

__attribute__((noreturn))
static void usage(void) {
	fprintf(stderr, "Usage: %s [OPTIONS]\n", progname);
	fprintf(stderr, "\nOptions:\n\n");
	fprintf(stderr, "--thread N       number of threads to be spawned\n");
	fprintf(stderr, "--repeat N       number of repetition\n");
	fprintf(stderr, "--foreach        scalability analysis\n");
	fprintf(stderr, "--verbose        be verbose\n");
	fprintf(stderr, "--help           print this message and exit\n");
	fprintf(stderr, "\n");
	exit(EXIT_FAILURE);
}

static void parse_opts(int argc, char **argv, struct experiment *exp) {
	int opt;
	int idx;

	struct option options[] = {
			{ "help",       0, 0, 'h' },
			{ "thread",     1, 0, 't' },
			{ "repeat",     1, 0, 'r' },
			{ "verbose",    0, 0, 'v' },
			{ 0, 0, 0, 0 }
	};

	exp->foreach = 0;
	exp->nr_thread = omp_get_num_procs();
	exp->repeat = DEFAULT_REPEAT;

	while ((opt = getopt_long(argc, argv, "hd:t:r:fv", options, &idx)) != -1) {
		switch (opt) {
		case 't':
			exp->nr_thread = atoi(optarg);
			break;
		case 'r':
			exp->repeat = atoi(optarg);
			break;
		case 'f':
			exp->foreach = 1;
			break;
		case 'h':
			usage();
			break;
		default:
			usage();
			break;
		}
	}

}

void signal_handler(int signum, siginfo_t *info, void *arg)
{
	(void) signum;
	(void) info;
	(void) arg;
}

int sigbench_profile_func(void *args)
{
	(void) args;
	pthread_kill(pthread_self(), SIGUSR1);
	return 0;
}

int sigbench_profile_calibrate(void *args)
{
	(void) args;
	volatile int i = 1;
	while(i > 0)
		i--;
	return 0;
}

void sigbench_once(long nr, long repeat)
{
	double elapsed = 0.0;
	printf("threads=%ld\n", nr);
	#pragma omp parallel num_threads(nr)
	{
		cpu_set_t cpuset;
		struct timespec t1, t2, e;
		int id = omp_get_thread_num();
		//int core = (id + 1) % 4;
		int core = id;
		printf("thread %d on cpu %d\n", id, core);
		CPU_ZERO(&cpuset);
		CPU_SET(core, &cpuset);
		sched_setaffinity(gettid(), sizeof(cpuset), &cpuset);
		struct profile prof = {
		    .name = "sigbench",
			.repeat = repeat,
			.func = sigbench_profile_func,
		};
		asprintf(&prof.name, "sigbench-nr=%ld-id=%d", nr, id);
		profile_init(&prof);
		#pragma omp barrier
		clock_gettime(CLOCK_MONOTONIC, &t1);
		profile_func(&prof);
		clock_gettime(CLOCK_MONOTONIC, &t2);
		#pragma omp barrier
		e = time_sub(&t2, &t1);
		double es = timespec_to_double_ns(&e);
		printf("sec=%ld nsec=%ld es=%f\n", e.tv_sec, e.tv_nsec, es);
		#pragma omp critical
		{
			if (es > elapsed)
				elapsed = es;
		}
		profile_stats(&prof);
		profile_stats_print(&prof, stdout);
		profile_save(&prof);
		profile_destroy(&prof);
	}
	double sec = elapsed / 1000000000.0;
	double total_work = nr * repeat;
	printf("total_work=%f\n", total_work);
	printf("sec=%f, rate=%f\n", sec, total_work / sec);

}

int main(int argc, char **argv)
{
	int i, nr_iter, cpus;
	struct experiment exp;
	struct sigaction sigact;

	parse_opts(argc, argv, &exp);

	sigact.sa_sigaction = signal_handler;
	sigact.sa_flags = SA_SIGINFO;
	sigaction(SIGUSR1, &sigact, NULL);

	if (exp.foreach) {
		for(i = 0; (1 << i) <= exp.nr_thread; i++) {
			sigbench_once((1 << i), exp.repeat);
		}
	} else {
		sigbench_once(exp.nr_thread, exp.repeat);
	}

	sigaction(SIGUSR1, NULL, NULL);
	return 0;
}
