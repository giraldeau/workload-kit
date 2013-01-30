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

#include "utils.h"

char *progname = "lockfight";

struct vars {
	int mili;
	char *path;
};

int lock_file(int fd) {
	struct flock lock;

	lock.l_type = F_WRLCK;
	lock.l_start = 0;
	lock.l_whence = SEEK_SET;
	lock.l_len = 0;

	printf("Try to lock file...\n");
	if (fcntl(fd, F_SETLKW, &lock) < 0) {
		perror("fcntl error");
		return -1;
	}
	printf("File lock succeeded\n");

	return 0;
}

int unlock_file(int fd) {
	struct flock lock;

	lock.l_type = F_UNLCK;
	lock.l_start = 0;
	lock.l_whence = SEEK_SET;
	lock.l_len = 0;

	printf("Try to unlock file...\n");
	if (fcntl(fd, F_SETLKW, &lock) < 0) {
		perror("fcntl error");
		return -1;
	}
	printf("File unlock succeeded\n");

	return 0;
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
    memset(vars, 0, sizeof(struct vars));

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

	int fd;
	struct vars v;

	if (parse_opts(argc, argv, &v) < 0) {
		printf("Error: missing arguments\n");
		usage();
		return -1;
	}

	if ((fd = open(v.path, O_RDWR | O_CREAT, S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH)) == -1) {
		perror("Can't open output file");
		return -1;
	}

	lock_file(fd);
	do_sleep(v.mili);
	unlock_file(fd);
	do_sleep(v.mili);

	return 0;
}
