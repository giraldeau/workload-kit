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
#include <getopt.h>

#define TRACEPOINT_DEFINE
#include "heartbeat_tp_provider.h"

#define DEFAULT_MSG 	"heartbeat"
#define DEFAULT_DELAY	1000000
#define DEFAULT_REP	10
#define DEFAULT_NB_THD  1

struct command_opts {
	char *message;
	int delay;
	int rep;
	int nb_thread;
	int verbose;
	int quiet;
};

__attribute__((noreturn))
static void usage(void)
{
	fprintf(stderr, "wk-heartbeat\n");
	fprintf(stderr, "Usage: wk-heartbeat [OPTIONS] [COMMAND]\n");
	fprintf(stderr, "\nOptions:\n");
	fprintf(stderr, "  --help	this help\n");
	fprintf(stderr, "  --message	custom message to write as heartbeat, default heartbeat\n");
	fprintf(stderr, "  --nb_thread	set number of threads\n");
	fprintf(stderr, "  --delay 	delay between heartbeats (us), default 1s\n");
	fprintf(stderr, "  --rep	number of heartbeats to emit, default 10\n");
	fprintf(stderr, "  --verbose	verbose mode, display option values\n");
	fprintf(stderr, "  --quiet      quiet mode, do not display heartbeat\n");
	fprintf(stderr, "\n");
	exit(EXIT_FAILURE);
}

static void dump_opts(struct command_opts *opts)
{
	printf("%10s %s\n", "option", "value");
	printf("%10s %s\n", "message", opts->message);
	printf("%10s %d\n", "delay", opts->delay);
	printf("%10s %d\n", "rep", opts->rep);
	printf("%10s %d\n", "nb_thread", opts->nb_thread);
	printf("%10s %d\n", "verbose", opts->verbose);
	printf("%10s %d\n", "quiet", opts->quiet);
}

void default_int_value(int *val, int def)
{
	if (*val == 0)
		*val = def;
}

static int parse_opts(int argc, char **argv, struct command_opts *opts)
{
	int idx;
	int opt;
	int ret = 0;

	struct option options[] = {
			{ "help",        0, 0, 'h' },
			{ "message",     1, 0, 'm' },
			{ "delay",       1, 0, 'd' },
			{ "rep",         1, 0, 'r' },
			{ "nb_thread",   1, 0, 'n' },
			{ "verbose",     0, 0, 'v' },
			{ "quiet",       0, 0, 'q' },
			{ 0, 0, 0, 0}
	};

	memset(opts, 0, sizeof(struct command_opts));

	while ((opt = getopt_long(argc, argv, "hvm:d:r:n:q:", options, &idx)) != -1) {
		switch(opt) {
		case 'm':
			opts->message = strdup(optarg);
			break;
		case 'd':
			opts->delay = atoi(optarg);
			break;
		case 'r':
			opts->rep = atoi(optarg);
			break;
		case 'n':
			opts->nb_thread = atoi(optarg);
			break;
		case 'h':
			usage();
			break;
		case 'v':
			opts->verbose = 1;
			break;
		case 'q':
			opts->quiet = 1;
			break;
		default:
			printf("unknown option %c\n", opt);
			ret = -1;
			break;
		}
	}

	/* default values*/
	if (opts->message == NULL)
		opts->message = strdup(DEFAULT_MSG);
	if (opts->delay ==  0)
		opts->delay = DEFAULT_DELAY;
	if (opts->rep == 0)
		opts->rep = DEFAULT_REP;
	if (opts->nb_thread == 0)
		opts->nb_thread = DEFAULT_NB_THD;
	if (opts->verbose) {
		dump_opts(opts);
	}
done:
	return ret;
err:
	ret = -1;
	goto done;
}

void *heartbeat_thread(void *arg) {
	int i;
	pthread_t id;
	struct command_opts *opts = arg;

	if (opts == NULL)
		return NULL;

	id = pthread_self();
	usleep(opts->delay);
	for (i = 0; i < opts->rep; i++) {
		if (!opts->quiet)
			printf("0x%-16lx %-10d %s\n", id, i, opts->message);
		tracepoint(heartbeat, msg, opts->message);
		usleep(opts->delay);
	}
	return NULL;
}

int main(int argc, char **argv)
{
	struct command_opts opts;
	int ret = 0;
	int i = 0;
	pthread_t *thds;

	ret = parse_opts(argc, argv, &opts);
	if (ret < 0)
		goto error;

	thds = calloc(sizeof(thds), opts.nb_thread);

	if (!opts.quiet)
		printf("%-18s %-10s %s\n", "id", "rep", "message");
	for (i = 0; i < opts.nb_thread; i++) {
		pthread_create(&thds[i], NULL, heartbeat_thread, &opts);
	}
	for (i = 0; i < opts.nb_thread; i++) {
		pthread_join(thds[i], NULL);
	}

error:
	if (opts.message != NULL)
		free(opts.message);
	return ret;
}
