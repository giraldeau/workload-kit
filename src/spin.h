/*
 * spin.h
 *
 *  Created on: Jan 23, 2014
 *      Author: francis
 */

#ifndef SPIN_H_
#define SPIN_H_

struct spin;

struct spin_args {
	int id;
	struct spin *spin;
};

typedef void (*init_t)(struct spin_args *);
typedef void (*done_t)(struct spin_args *);

struct spin {
	int n;
	init_t init;
	done_t done;
	void *data;
};

void spin_init(struct spin *spin);
void spin(long usec);
void spin_exit();

#endif /* SPIN_H_ */
