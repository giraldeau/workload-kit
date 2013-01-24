/*
 * cpm2.c
 *
 *  Created on: 2011-10-25
 *      Author: francis
 *
 *  Produces following execution path:
 *
 *                          waitpid
 *                         /
 *	master  ====+==========+===
 *              |
 *  child 1     +======+
 *
 *  The master creates a child, but perform the wait only after the child exits.
 *  The child remains in the zombie state in the millisecond range.
 *  The resulting critical path is always in master, the master never wait nor
 *  receives wakeup.
 */

#include <stdlib.h>
#include <time.h>
#include "calibrate.h"

long unit = 0;

void master();
void child1();

void master()
{
	pid_t res = fork();
	if (res < 0)
		return;
	if (res == 0) {
		child1();
		return;
	}
	do_hog(2 * unit);
	waitpid(res);
	return;
}

void child1()
{
	do_hog(unit);
	return;
}

int main(int argc, char **argv)
{

	suseconds_t s = 100000;
	unit = calibrate(s);

	master();
	return EXIT_SUCCESS;
}
