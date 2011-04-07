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

#define PROGNAME "iosplit"
#define VAL_VERSION "1.0"

/* Global variables */

static const char *const progname = PROGNAME;

struct vars {
	char *workdir;
	int size;
	int count;
};

__attribute__((noreturn))
static void usage(void) {
    fprintf(stderr, "Usage: %s [OPTIONS] [COMMAND]\n", progname);
    fprintf(stderr, "\nOptions:\n\n");
    fprintf(stderr, "  --size     set size of blocs\n");
    fprintf(stderr, "  --count    set number of blocs\n");
    fprintf(stderr, "  --dir      set the working dir\n");
    exit(EXIT_FAILURE);
}

static void parse_opts(int argc, char **argv, struct vars *vars) {
    int opt;
    size_t loadpathlen = 0;

    struct option options[] = {
        { "help",      0, 0, 'h' },
        { "size",      0, 0, 's' },
        { "count",     0, 0, 'c' },
        { "dir",       0, 0, 'd' },
        { 0, 0, 0, 0}
    };
    int idx;

    while ((opt = getopt_long(argc, argv, "hs:c:d:", options, &idx)) != -1) {
        switch(opt) {
        case 's':
            vars->size = atoi(optarg);
            break;
        case 'c':
            vars->count = atoi(optarg);
            break;
        case 'd':
            vars->workdir = optarg;
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
    if (vars->workdir == NULL) {
    	vars->workdir = PROGNAME "-data";
    }
}

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

int main(int argc, char **argv) {

	struct vars vars;

	parse_opts(argc, argv, &vars);

	if (make_work_dir(vars.workdir) < 0)
		return EXIT_FAILURE;

	return EXIT_SUCCESS;

}
