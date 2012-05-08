/*
 * Copyright (C) 2011-2012  Matthew Khouzam <matthew.khouzam@ericsson.com>
 * Copyright (C) 2012  Mathieu Desnoyers <mathieu.desnoyers@efficios.com>
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 */

#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
/*
 * We need to define TRACEPOINT_DEFINE in one C file in the program
 * before including provider headers.
 */
#define TRACEPOINT_DEFINE
#include "sample_component_provider.h"

int main(int argc, char **argv)
{
	int i = 0, j = 0;
	char c;

	/* sleep before starting experiment */
	usleep(1000000);
	for (i = 0; i < 16; i++) {
		printf("experiment %i\n", i);
		c = 'a';
		size_t msg_len = 1 << i;
		char *buf = malloc(sizeof(char) * msg_len);
		for (j = 0; j < msg_len; j++) {
			buf[j] = c++;
			if (c > 'z')
				c = 'a';
		}
		buf[msg_len-1] = '\0';
		tracepoint(sample_component, message, buf);
		free(buf);
		usleep(1000000);

	}
	return 0;
}
