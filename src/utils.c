/*
 * utils.c
 *
 *  Created on: 2011-12-14
 *      Author: francis
 */

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#include <string.h>
#include <sys/syscall.h>
#include <time.h>
#include "utils.h"

void throw(const char *msg)
{
	perror(msg);
	exit(1);
}

int do_sleep(int mili) {

	struct timespec t;
	struct timeval t1, t2;
	t.tv_sec = mili / 1000;
	t.tv_nsec = (mili % 1000) * 1000000;

	if (mili == 0)
		return 0;

	gettimeofday(&t1, NULL);
	if (nanosleep(&t, NULL) < 0) {
		return -1;
	}
	gettimeofday(&t2, NULL);
	return 0;
}

int gettid() {
    return (int) syscall(SYS_gettid);
}

/*
 * computes time difference (x - y)
 */
struct timespec time_sub(struct timespec *x, struct timespec *y)
{
    struct timespec res;
    res.tv_sec  = x->tv_sec  - y->tv_sec;
    res.tv_nsec = x->tv_nsec - y->tv_nsec;
    if(x->tv_nsec < y->tv_nsec) {
        res.tv_sec--;
        res.tv_nsec += 1000000000;
    }
    return res;
}

void time_add(struct timespec *x, struct timespec *y)
{
    x->tv_sec  = x->tv_sec + y->tv_sec;
    x->tv_nsec = x->tv_nsec + y->tv_nsec;
    if(x->tv_nsec >= 1000000000) {
        x->tv_sec++;
        x->tv_nsec -= 1000000000;
    }
}

void profile_func(struct profile *prof) {
	int i;
	struct timespec *data;
	/* allocate memory ahead and writes to it to avoid page fault */
	data = malloc(prof->repeat * sizeof(struct timespec));
	memset(data, 0, prof->repeat * sizeof(struct timespec));

	for (i = 0; i < prof->repeat; i++) {
		clock_gettime(CLOCK_MONOTONIC, &data[i]);
		prof->func(prof->args);
	}

	struct timespec total = { .tv_sec = 0, .tv_nsec = 0 };
	for (i = 0; i < prof->repeat - 1; i++) {
		struct timespec delta = time_sub(&data[i + 1], &data[i]);
		time_add(&total, &delta);
	}
	prof->mean = (double)(total.tv_sec * 1000000000 + total.tv_nsec) / prof->repeat;
	printf("mean=%f\n", prof->mean);
}
