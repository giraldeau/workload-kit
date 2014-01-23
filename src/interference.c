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

#include <stdio.h>
#include <stdlib.h>
#include <inttypes.h>
#include <pthread.h>
#include <getopt.h>
#include "spin.h"

#define DEFAULT_SPIN 100000 // 100ms
#define DEFAULT_DURATION 1000000 // 1s
#define DEFAULT_THREAD 2
#define DEFAULT_FREQ 4
#define USEC_PER_SEC 1000000
#define NSEC_PER_SEC 1000000000
#define progname "wk-interference"

struct experiment {
    int nr_thread;
    long spin;
    int verbose;
    int freq;
    long duration;
};

__attribute__((noreturn))
static void usage(void) {
    fprintf(stderr, "Usage: %s [OPTIONS] [COMMAND]\n", progname);
    fprintf(stderr, "\nOptions:\n\n");
    fprintf(stderr, "--spin S         spin duration in microseconds (default = 100 000 us)\n");
    fprintf(stderr, "--thread N       number of threads to be spawned (default = 2)\n");
    fprintf(stderr, "--frequency      frequency in HZ of the CPU bursts per thread (default = 4)\n");
    fprintf(stderr, "--duration       duration in microseconds of the experiment (default = 1 000 000 us\n");
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
            { "duration",   1, 0, 'd' },
            { "thread",     1, 0, 't' },
            { "freq",       1, 0, 'f' },
            { "spin",       1, 0, 's' },
            { "verbose",    0, 0, 'v' },
            { 0, 0, 0, 0 }
    };

    exp->nr_thread = DEFAULT_THREAD;
    exp->spin = DEFAULT_SPIN;
    exp->freq = DEFAULT_FREQ;
    exp->duration = DEFAULT_DURATION;

    while ((opt = getopt_long(argc, argv, "hd:t:f:s:v", options, &idx)) != -1) {
        switch (opt) {
        case 'd':
            exp->duration = atol(optarg);
            break;
        case 't':
			exp->nr_thread = atoi(optarg);
			break;
		case 'f':
			exp->freq = atoi(optarg);
			break;
		case 's':
			exp->spin = atoi(optarg);
			break;
        case 'h':
            usage();
            break;
        default:
            usage();
            break;
        }
    }

    if(exp->spin * exp->freq > 900000) {
    	printf("Warning: Spin duration is too long for requested frequency."
    			"CPU may spin all the time.\n");
    }
}

int main(int argc, char **argv) {
	int i, nr_iter;
	struct experiment *exp = calloc(1, sizeof(struct experiment));
	struct timespec ts;
	parse_opts(argc, argv, exp);

	nr_iter = (exp->duration / USEC_PER_SEC) * exp->freq;
	ts.tv_sec = 0;
	ts.tv_nsec = (NSEC_PER_SEC / exp->freq) - exp->spin * 1000;

	spin_init(exp->nr_thread);
	for (i = 0; i < nr_iter; i++) {
		printf("before_spin=%d\n", i);
		spin(exp->spin);
		printf("after_spin=%d\n", i);
		nanosleep(&ts, NULL);
		printf("after_sleep=%d\n", i);
	}
	spin_exit();
	return 0;
}
