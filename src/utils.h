/*
 * utils.h
 *
 *  Created on: 2011-12-14
 *    Author: francis
 */

#ifndef UTILS_H_
#define UTILS_H_

void throw(const char *msg);
int do_sleep(int mili);
int gettid();

#define ARG_CHECK(cond, fmt ...)		\
	do {								\
		if (cond) {						\
			msg(fmt);					\
			usage();					\
		}								\
	} while(0)


#endif /* UTILS_H_ */
