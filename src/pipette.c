/*
 * pipette.c
 *
 * Trigger all pipe control conditions:
 *  - blocking consumer (pipe empty)
 *  - blocking producer (pipe full)
 *
 *  An anonymous pipe is created by the main process.
 *  The main process spawns the producer, then the consumer.
 *
 *  On Linux, the default pipe size is 64kb
 */

#define _GNU_SOURCE
#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>
#include <errno.h>
#include <error.h>
#include <getopt.h>
#include <limits.h>
#include <utils.h>

#define PROGNAME "wk-pipette"
#define VAL_VERSION "1.0"
#define READ 0
#define WRITE 1

/* Global variables */

static const char *const progname = PROGNAME;
static const int DEFAULT_BLOCK_SIZE = 4096;
static const int PAGE_SIZE = 4096;
static const int DEFAULT_BLOCK_COUNT = 1;
static int verbose = 0;

struct vars {
	int delay_prod;
	int delay_cons;
	int size;
	int count;
	int fd[2];
};

typedef int (*func)(int a, int b, int c, int d);
typedef ssize_t (*exch)(int a, void *b, size_t c);

__attribute__((noreturn))
static void usage(void) {
	fprintf(stderr, "Usage: %s [OPTIONS] [COMMAND]\n", progname);
	fprintf(stderr, "\nOptions:\n\n");
	fprintf(stderr, "  --size		 set size of blocs in bytes\n");
	fprintf(stderr, "  --count		set number of blocs\n");
	fprintf(stderr, "  --delay-prod   delay to produce each block\n");
	fprintf(stderr, "  --delay-cons   delay to consume each block\n");
	fprintf(stderr, "  --verbose	  verbose mode\n");
	exit(EXIT_FAILURE);
}

void msg(const char *format, ...) {
	va_list ap;
	char *str;
	int ret;
	if (verbose && format != NULL) {
		va_start(ap, format);
		ret = vasprintf(&str, format, ap);
		va_end(ap);
		if (ret < 0) {
			return;
		}
		printf("%s\n", str);
		free(str);
	}
}

static void parse_opts(int argc, char **argv, struct vars *vars) {
	int opt;
	size_t loadpathlen = 0;

	struct option options[] = {
		{ "help",			0, 0, 'h' },
		{ "size",			1, 0, 's' },
		{ "count",			1, 0, 'n' },
		{ "delay-prod",		1, 0, 'p' },
		{ "delay-cons",		1, 0, 'c' },
		{ "verbose",		0, 0, 'v' },
		{ 0, 0, 0, 0 }
	};
	int idx;

	while ((opt = getopt_long(argc, argv, "hvs:n:p:c:", options, &idx)) != -1) {
		switch(opt) {
		case 's':
			vars->size = atoi(optarg);
			break;
		case 'n':
			vars->count = atoi(optarg);
			break;
		case 'p':
			vars->delay_prod = atoi(optarg);
			break;
		case 'c':
			vars->delay_cons = atoi(optarg);
			break;
		case 'v':
			verbose = 1;
			break;
		case 'h':
			usage();
			break;
		default:
			usage();
			break;
		}
	}

	/* default values*/
	if (vars->size == 0) {
		vars->size = DEFAULT_BLOCK_SIZE;
		printf("using default block size %d\n", vars->size);
	}
	if (vars->count == 0) {
		vars->count = DEFAULT_BLOCK_COUNT;
		printf("using default count %d\n", vars->count);
	}
	ARG_CHECK(vars->size <= 0, "SIZE must be positive");
	ARG_CHECK(vars->count <= 0, "COUNT must be positive");
	ARG_CHECK((vars->size * vars->count) > INT_MAX, "Total transfer overflows INT_MAX");
}

int producer(int fd, int size, int count, int delay) {
	int i;
	char *buf;

	buf = calloc(1, size);
	if (buf == NULL) {
		return -1;
	}

	msg("producer start");
	for (i = 0; i < count; i++) {
		memset(buf, i, size);
		do_sleep(delay);
		if (do_transfer(write, fd, buf, size) < 0)
			return -1;
		msg("producer %d", i);

	}
	msg("producer exit");
	return 0;
}

int consumer(int fd, int size, int count, int delay) {
	int i;
	int j;
	int ret;
	int sum;
	char *buf;

	buf = calloc(1, size);
	if (buf == NULL) {
		return -1;
	}

	msg("consumer start");
	for (i = 0; i < count; i++) {
		do_sleep(delay);
		if (do_transfer(read, fd, buf, size) < 0)
			return -1;
		sum = 0;
		for (j = 0; j < size; j++) {
			sum += buf[j];
		}
		msg("consumer %d exp=%d act=%d", i, i * size, sum);
	}
	msg("consumer exit");
	return 0;
}

/*
 * Transfer SIZE bytes, by chunks of PAGE_SIZE bytes at a time
 */
int do_transfer(exch f, int fd, char *buf, int size) {
	int ret;
	int chunk;
	int len = 0;
	while(len < size) {
		chunk = PAGE_SIZE;
		if ((size - len) < PAGE_SIZE)
			chunk = size - len;
		if ((ret = f(fd, buf, chunk)) < 0) {
			return -1;
		}
		len += ret;
	}
	return len;
}

int main(int argc, char **argv) {
	int i;
	int ret;
	struct vars *vars = calloc(1, sizeof(struct vars));

	parse_opts(argc, argv, vars);

	ret = pipe(vars->fd);
	if (ret < 0) {
		perror("pipe failed");
		return EXIT_FAILURE;
	}

	if (fork() == 0) {
		close(vars->fd[READ]);
		return producer(vars->fd[WRITE], vars->size, vars->count, vars->delay_prod);
	}

	if (fork() == 0) {
		close(vars->fd[WRITE]);
		return consumer(vars->fd[READ], vars->size, vars->count, vars->delay_cons);
	}
	for (i = 0; i < 2; i++) {
		wait(NULL);
	}
	return EXIT_SUCCESS;
}
