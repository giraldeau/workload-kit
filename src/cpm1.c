/*
 * cpm1.c
 *
 *  Created on: 2011-09-28
 *      Author: francis
 *
 *  Produces following execution path:
 *
 *	master  ====+======----+===
 *              |          |
 *  child 1     +=+--+=====+
 *                |  |
 *  child 2       +==+
 *
 *  The resulting critical path is master:child1:child2:child1:master
 *  and not master:child1:master, because if child2 is optimized, the
 *  total execution time will decrease, hence child2 is on the critical path.
 */

#include <stdlib.h>
#include <time.h>
#include "calibrate.h"

long unit = 0;

void master();
void child1();
void child2();

void master()
{
	pid_t res = fork();
	if (res < 0)
		return;
	if (res == 0) {
		child1();
		return;
	}
	do_hog(4 * unit);
	waitpid(res);
	do_hog(unit);
	return;
}

void child1()
{
	do_hog(unit);
	pid_t res = fork();
	if (res < 0)
		return;
	if (res == 0) {
		child2();
		return;
	}
	waitpid(res);
	do_hog(2 * unit);
	return;
}

void child2()
{
	do_hog(2 * unit);
}

int main(int argc, char **argv)
{

	suseconds_t s = 100000;
	unit = calibrate(s);

	master();
	return EXIT_SUCCESS;
}
