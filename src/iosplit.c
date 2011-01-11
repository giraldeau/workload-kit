#define _GNU_SOURCE
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>
#include <errno.h>

int main(int argc, char **argv) {

	int buf_size = 1048576;
	int nb_blocs = 20;
	int max_size = buf_size * nb_blocs;
	blkcnt_t blk1, blk2, blk3;

	// | O_DIRECT doesn't work

	const char *path1 = "bidon1.data";
	int fd1 = open(path1, O_CREAT | O_RDWR | O_SYNC, S_IRUSR|S_IWUSR);

	int x;
	char *buf = calloc(buf_size, 1);

	/* if the file exists, truncate it */
	struct stat st1;
	int res;
	if ((res = stat(path1, &st1)) < 0) {
		if (res != ENOENT) {
			perror("stat");
		}
	} else {
		if(truncate(path1, 0) < 0) {
			perror("truncate");
		}
	}

	for(x=0; x<nb_blocs; x++) {
		if (write(fd1, buf, buf_size) < 0) {
			perror("write zero");
		}
	}

	off_t offset;
	if ((offset = lseek(fd1, max_size, SEEK_SET)) < 0) {
		perror("seek");
	}

	int i = 40;

	if (write(fd1, &i, sizeof(int))<0) {
		perror("write");
	}

	/* check the size on disk */
	if ((res = stat(path1, &st1)) < 0) {
		perror("stat");
	}

	printf("st_blocks = %d\n", (int)st1.st_blocks * 512);
	printf("st_size   = %d\n", (int)st1.st_size);

	close(fd1);
	free(buf);

	return 0;
}
