/*
 * inception.c
 *
 * This program creates recursively children process that sleeps for a while
 *
 *  Created on: 2011-02-18
 *      Author: francis
 */

#include <stdio.h>
#include <time.h>
#include <error.h>
#include <errno.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/time.h>
#include "utils.h"

int mili = 0;
int children = 0;

void usage(char *prog) {
	printf("Usage: %s TIME CHILDREN\n\n", prog);
	printf("    TIME     : sleep time in miliseconds\n");
	printf("    CHILDREN : number of children to spawn\n");
}

int do_run() {
	/* self sleep first */
	if (do_sleep(mili) < 0) {
		error(0, errno, "error in do_sleep");
		return -1;
	}

	/* then spawn children */
	if (spawn_child() < 0) {
		error(0, errno, "error in span_children");
		return -1;
	}
}

int spawn_child() {
	int child;
	int stat_loc;

	children--;
	mili = mili * 2;

	if (children == 0)
		return 0;

	if((child = fork()) < 0) {
		return -1;
	} else if (child == 0) {
		do_run();
	} else {
		waitpid(child, &stat_loc, 0);
	}

	return 0;
}

int main (int argc, char **argv) {

	if (argc < 3) {
		usage(argv[0]);
		return 1;
	}

	mili = atoi(argv[1]);
	children = atoi(argv[2]);

	do_run();

	return 0;
}
