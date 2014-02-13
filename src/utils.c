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
#include <sys/syscall.h>
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
