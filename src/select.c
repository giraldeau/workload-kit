/*
 * select.c
 *
 * Execution example of the select system call. Waits on the standard input
 * for data, or until timeout occurs. Based on the example in man SELECT(2).
 *
 *  Created on: Jan 29, 2013
 *      Author: francis
 */

#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>

#define BUFSIZE 128

int main(int argc, char **argv)
{
	char buf[BUFSIZE];
	fd_set rfds;
	struct timeval tv;
	int ret;

	FD_ZERO(&rfds);
	FD_SET(0, &rfds);

	tv.tv_sec = 1;
	tv.tv_usec = 0;
	ret = select(1, &rfds, NULL, NULL, &tv);

	if (ret == -1) {
		perror("select()");
	} else if (ret) {
		printf("ready\n");
		ret = read(0, buf, BUFSIZE - 1);
		buf[ret] = '\0';
		printf("read (%d): %s\n", ret, buf);
	} else {
		printf("timeout\n");
	}

	return EXIT_SUCCESS;
}
