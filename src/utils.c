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
#include <math.h>
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

double timespec_to_double_ns(struct timespec *t)
{
	double sec = ((double)t->tv_sec) * 1000000000.0;
	double nsec = (double) t->tv_nsec;
	return sec + nsec;
}

double timespec_to_double_ns_v(struct timespec *t)
{
	double sec = ((double)t->tv_sec) * 1000000000.0;
	double nsec = (double) t->tv_nsec;
	printf("conversion sec=%f nsec=%f\n", sec, nsec);
	return sec + nsec;
}

void profile_init(struct profile *prof)
{
	size_t size = prof->repeat * sizeof(struct timespec);
	/*
	 * allocate memory ahead and writes to it to avoid page fault
	 */
	void *data = malloc(size);
	memset(data, 0, size);
	prof->data = data;
	prof->name = prof->name != NULL ? prof->name : "default";
}

void profile_destroy(struct profile *prof)
{
	free(prof->data);
	prof->data = NULL;
}

void profile_func(struct profile *prof)
{
	int i;
	struct timespec *data = prof->data;

	for (i = 0; i < prof->repeat; i++) {
		clock_gettime(CLOCK_MONOTONIC, &data[i]);
		prof->func(prof->args);
	}
}

void profile_stats(struct profile *prof)
{
	int i;
	struct timespec total = { .tv_sec = 0, .tv_nsec = 0 };

	if (prof->data == NULL)
		return;

	for (i = 0; i < prof->repeat - 1; i++) {
		struct timespec delta = time_sub(&prof->data[i + 1], &prof->data[i]);
		time_add(&total, &delta);
	}
	prof->mean = timespec_to_double_ns(&total) / prof->repeat;
	prof->sd = 0.0;

	for (i = 0; i < prof->repeat - 1; i++) {
		struct timespec delta = time_sub(&prof->data[i + 1], &prof->data[i]);
		double nsec = timespec_to_double_ns(&delta);
		double diff = (nsec - prof->mean);
		prof->sd += diff * diff;
	}
	prof->sd = sqrt(prof->sd / prof->repeat);
	printf("profile %s mean=%f sd=%f\n", prof->name, prof->mean, prof->sd);
}

void profile_save(struct profile *prof)
{
	int i;

	char *fname;
	asprintf(&fname, "%s.csv", prof->name);
	FILE *out = fopen(fname, "w");
	fprintf(out, "iter;nsec;\n");
	for (i = 0; i < prof->repeat - 1; i++) {
		struct timespec delta = time_sub(&prof->data[i + 1], &prof->data[i]);
		long nsec = timespec_to_double_ns(&delta);
		fprintf(out, "%d;%ld;\n", i, nsec);
	}
	free(fname);
	fclose(out);
}
