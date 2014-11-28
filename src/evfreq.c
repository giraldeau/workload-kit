/*
 * evfreq.c
 *
 *  Created on: Nov 21, 2014
 *      Author: francis
 */

#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <unistd.h>
#include <fcntl.h>

#include "utils.h"

int profile_clock(void *args) {
	int fd = *((int *)args);
	char buf;
	int ret = read(fd, &buf, 1);
	return ret;
}

int main(int argc, char **argv)
{
	struct profile p;

	int fd = open("/dev/zero", O_RDONLY);
	p.func = profile_clock;
	p.repeat = 10000000;
	p.args = &fd;

	profile_func(&p);
}
