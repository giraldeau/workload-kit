/*
 * reparent.c
 *
 *  Created on: Jan 24, 2013
 *      Author: francis
 *
 *  The program spawn a child that sleeps for 1 second, but the parent
 *  immediately exits. Thus, the child is reparented to init and is not
 *  on the critical path.
 */

#include <stdio.h>
#include <stdlib.h>
#include <time.h>

int main(int args, char **argv) {
	pid_t pid = fork();
	if (pid < 0)
		return EXIT_FAILURE;
	if (pid == 0)
		sleep(1);
	return EXIT_SUCCESS;
}
