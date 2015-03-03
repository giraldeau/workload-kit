/*
 * utils.h
 *
 *  Created on: 2011-12-14
 *    Author: francis
 */

#ifndef UTILS_H_
#define UTILS_H_

struct profile {
	char *name;
	int (*func)(void *args);
	void *args;
	int repeat;
	double mean;
	double sd;
	int nr_thread;
	struct timespec *data;
};

void throw(const char *msg);
int do_sleep(int mili);
int gettid();
int profile(struct profile *prof);
struct timespec time_sub(struct timespec *x, struct timespec *y);

#define ARG_CHECK(cond, fmt ...)		\
	do {								\
		if (cond) {						\
			msg(fmt);					\
			usage();					\
		}								\
	} while(0)


#endif /* UTILS_H_ */
