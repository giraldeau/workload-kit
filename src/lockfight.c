/*
 * lockfight.c
 *
 *  Created on: 2011-04-13
 *      Author: francis
 */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include <getopt.h>

char *progname = "lockfight";

struct vars {
	int mili;
	char *path;
};

int lock_file(char *path) {

	int fd;
	struct flock lock;

	lock.l_type = F_WRLCK;
	lock.l_start = 0;
	lock.l_whence = SEEK_SET;
	lock.l_len = 0;


	if ((fd = open(path, O_RDWR | O_CREAT, S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH)) == -1) {
		perror("Can't open output file");
		return -1;
	}

	printf("Try to lock file...\n");
	if (fcntl(fd, F_SETLKW, &lock) < 0) {
		perror("fcntl error");
		return -1;
	}
	printf("File lock succeeded\n");

	return 0;
}

int do_sleep(int mili) {

	struct timespec t;
	t.tv_sec = mili / 1000;
	t.tv_nsec = (mili % 1000) * 1000000;

	if (nanosleep(&t, NULL) < 0) {
		return -1;
	}
}

__attribute__((noreturn))
static void usage(void) {
    fprintf(stderr, "Usage: %s [OPTIONS] [COMMAND]\n", progname);
    fprintf(stderr, "\nOptions:\n\n");
    fprintf(stderr, "  --help     this help\n");
    fprintf(stderr, "  --file     file to lock\n");
    fprintf(stderr, "  --sleep    sleep time after acquiring the lock in miliseconds\n");
    exit(EXIT_FAILURE);
}

static int parse_opts(int argc, char **argv, struct vars *vars) {
    int opt;
    size_t loadpathlen = 0;

    struct option options[] = {
        { "help",      0, 0, 'h' },
        { "file",      1, 0, 'f' },
        { "sleep",     1, 0, 's' },
        { 0, 0, 0, 0}
    };
    int idx;

    while ((opt = getopt_long(argc, argv, "hs:f:", options, &idx)) != -1) {
        switch(opt) {
        case 's':
            vars->mili = atoi(optarg);
            break;
        case 'f':
			vars->path = strdup(optarg);
            break;
        case 'h':
        	usage();
            break;
        default:
        	usage();
        	break;
        }
    }

    if (vars->mili == 0 || vars->path == NULL) {
    	return -1;
    }
    return 0;
}


int main(int argc, char **argv) {

	struct vars v;

	if (parse_opts(argc, argv, &v) < 0) {
		printf("Error: missing arguments");
		usage();
		return -1;
	}

	lock_file(v.path);
	do_sleep(v.mili);

	return 0;
}
