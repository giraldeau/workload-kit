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

struct timespec time_sub(struct timespec *x, struct timespec *y);
void time_add(struct timespec *x, struct timespec *y);
double timespec_to_double_ns(struct timespec *t);


void profile_init(struct profile *prof);
void profile_destroy(struct profile *prof);
void profile_func(struct profile *prof);
void profile_stats(struct profile *prof);
void profile_save(struct profile *prof);
void profile_combo(struct profile *prof);

#define ARG_CHECK(cond, fmt ...)		\
	do {								\
		if (cond) {						\
			msg(fmt);					\
			usage();					\
		}								\
	} while(0)


#endif /* UTILS_H_ */
