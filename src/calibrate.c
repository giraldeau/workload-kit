/*
 * calibrate.c
 *
 *  Created on: 2011-02-18
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
#include "calibrate.h"

#define HIST 10

static volatile sig_atomic_t run;

int do_hog(long niters) {
    volatile sig_atomic_t dummy = 1;
    while (dummy && --niters)
        ;

    return 0;
}

static void signalhandler (int signr) {
    (void) signr;
    run = 0;
}

static unsigned long counter (unsigned long niters) {
    run = 1;
    while (run && --niters)
        ;
    return niters;
}

static void error (int status, const char *fmt, ...) {
    va_list ap;
    int errno_code = errno;

    va_start (ap, fmt);
    vfprintf (stderr, fmt, ap);
    va_end (ap);
    fprintf (stderr, ": %s\n", strerror (errno_code));
    exit (status);
}

unsigned long calibrate(suseconds_t usec) {

	unsigned long val[HIST];
    struct itimerval timer;
    struct sigaction action;
    sigset_t set;
    unsigned long result = 0;
    int i;

    action.sa_handler = signalhandler;
    if (sigemptyset (&action.sa_mask)) {
        error(EXIT_FAILURE, "sigemptyset failed");
    }
    action.sa_flags = 0;

    timer.it_interval.tv_sec = 0;
    timer.it_interval.tv_usec = usec;
    timer.it_value.tv_sec = 0;
    timer.it_value.tv_usec = usec;

    if (sigaction (SIGALRM, &action, NULL)) {
        error(EXIT_FAILURE, "sigaction failed");
    }

    if (setitimer (ITIMER_REAL, &timer, NULL)) {
        error(EXIT_FAILURE, "setitimer failed");
    }
    // warm-up
    counter(ULONG_MAX);

    // gather values
    for (i = 0; i < HIST; ++i) {
    	result += ULONG_MAX - counter(ULONG_MAX);
	}

    timer.it_interval.tv_usec = 0;
    timer.it_value.tv_usec = 0;
    if (setitimer (ITIMER_REAL, &timer, NULL)) {
        error(EXIT_FAILURE, "setitimer stop failed");
    }

    result /= HIST;

    return result;
}
