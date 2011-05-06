#define _GNU_SOURCE
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>
#include <errno.h>
#include <error.h>
#include <getopt.h>

#define PROGNAME "ioburst"
#define VAL_VERSION "1.0"

/* Global variables */

static const char *const progname = PROGNAME;
static const int DEFAULT_BLOCK_SIZE = 4096;
static const int DEFAULT_BLOCK_COUNT = 1;

struct vars {
	char *file;
	int size;
	int count;
	int sync;
};

__attribute__((noreturn))
static void usage(void) {
    fprintf(stderr, "Usage: %s [OPTIONS] [COMMAND]\n", progname);
    fprintf(stderr, "\nOptions:\n\n");
    fprintf(stderr, "  --size     set size of blocs in bytes\n");
    fprintf(stderr, "  --count    set number of blocs\n");
    fprintf(stderr, "  --file     set the working file\n");
    fprintf(stderr, "  --sync     synchrone write\n");
    exit(EXIT_FAILURE);
}

static void parse_opts(int argc, char **argv, struct vars *vars) {
    int opt;
    size_t loadpathlen = 0;

    struct option options[] = {
        { "help",      0, 0, 'h' },
        { "size",      1, 0, 's' },
        { "count",     1, 0, 'c' },
        { "file",      1, 0, 'f' },
        { "sync",      0, 0, 'z' },
        { 0, 0, 0, 0}
    };
    int idx;

    while ((opt = getopt_long(argc, argv, "hzs:c:f:", options, &idx)) != -1) {
        switch(opt) {
        case 's':
            vars->size = atoi(optarg);
            break;
        case 'c':
            vars->count = atoi(optarg);
            break;
        case 'f':
            vars->file = optarg;
            break;
        case 'z':
            vars->sync = 1;
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
    if (vars->file == NULL) {
    	vars->file = PROGNAME "-data";
    	printf("using default file name %s\n", vars->file);
    }
    if (vars->size == 0) {
    	vars->size = DEFAULT_BLOCK_SIZE;
    	printf("using default block size %d\n", vars->size);
    }
    if (vars->count == 0) {
		vars->count = DEFAULT_BLOCK_COUNT;
		printf("using default count %d\n", vars->count);
	}
}
/*
int make_work_dir(char *dir) {
	int status;
	struct stat st;

	if(stat(dir, &st) == 0 && S_ISDIR(st.st_mode))
		return 0;

	status = mkdir(dir, S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
	if (status < 0) {
		error(0, errno, "error creating directory %s", dir);
		return -1;
	}

}
*/

int do_io(int fd, int size, int count) {

	int i;
	char *buf;

	buf = calloc(1, size);
	if (buf == NULL) {
		return -1;
	}

	for (i = 0; i < count; i++) {
		if (write(fd, buf, size) < 0) {
			error(0, errno, "error in writing");
			return -1;
		}
	}

	return 0;
}

int main(int argc, char **argv) {

	struct vars *vars = calloc(1, sizeof(struct vars));

	parse_opts(argc, argv, vars);

	/*
	if (make_work_dir(vars.file) < 0)
		return EXIT_FAILURE;
	*/

	mode_t perms = S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH | S_IWOTH;
	int out_flags = O_CREAT | O_TRUNC | O_WRONLY;
	if (vars->sync) {
		out_flags = out_flags | O_SYNC;
	}
	int out = open(vars->file, out_flags, perms);
	if (out < 0) {
		error(0, errno, "error opening output file %s", vars->file);
		return EXIT_FAILURE;
	}

	if (do_io(out, vars->size, vars->count) < 0) {
		printf("error occured while performing I/O\n");
		return EXIT_FAILURE;
	}

	return EXIT_SUCCESS;
}
