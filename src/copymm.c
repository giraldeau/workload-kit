/*
 * copymm.c
 *
 *  Created on: Nov 20, 2014
 *      Author: francis
 */

#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <time.h>
#include <signal.h>
#include <sched.h>
#include <sys/mman.h>
#include <utils.h>

#define PAGE_SIZE 4096
#define BIGBUF_SIZE (1 << 28)

struct cnt {
	char *buf1;
	char *buf2;
	char *bigbuf;
	char *stack;
	long idx;
	int count;
};

int profile_wrapper_empty(void *args) {
	//struct timespec s = { .tv_sec = 0, .tv_nsec = 1000000 };
	//nanosleep(&s, NULL);
	return 0;
}

int profile_wrapper_memcpy(void *args) {
	void *tmp;
	struct cnt *data = args;
	memcpy(data->buf2, data->buf1, PAGE_SIZE);
	tmp = data->buf1;
	data->buf2 = data->buf1;
	data->buf1 = tmp;
	return 0;
}

void handle_signal(int signum) {
	// do nothing
	return;
}

int profile_wrapper_signal(void *args) {
	kill(getpid(), SIGUSR1);
	return 0;
}

int profile_wrapper_printf(void *args) {
	char buf[256];
	sprintf(buf, "%s %ld %p %f\n", "bidon", (unsigned long) &buf, &buf, 3.1416);
	return 0;
}

int profile_wrapper_pagefault(void *args) {
	struct cnt *data = args;
	data->bigbuf[data->idx * PAGE_SIZE] = data->idx;
	data->idx++;
	return 0;
}

int profile_wrapper_condition(void *args) {
	struct cnt *data = args;
	if (data->bigbuf[data->idx] > 64 ||
		data->bigbuf[data->idx] < -64) {
		data->count++;
	}
	data->idx++;
	return 0;
}

int worker_stub(void *data) {
	(void) data;
	//printf("inside thread\n");
	return 0;
}

int profile_wrapper_clone(void *args) {
	struct cnt *data = args;
/*
int clone(int (*fn)(void *), void *child_stack,
int flags, void *arg, ...
// pid_t *ptid, struct user_desc *tls, pid_t *ctid );
*/

	printf("%p %p\n", worker_stub, data->stack);
	int tid = clone(worker_stub, data->stack, CLONE_THREAD | CLONE_SIGHAND, NULL);
	perror("error");
	printf("tid=%d\n", tid);
	return 0;
}

int main(int argc, char **argv)
{
	int i;
	struct profile prof;
	struct cnt data;
	void *tmp;

/*
	// profile overhead
	prof.func = profile_wrapper_empty;
	prof.args = NULL;
	prof.repeat = BIGBUF_SIZE;
	profile_func(&prof);

	// profile condition
	data.bigbuf = mmap(NULL, BIGBUF_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, 0, 0);
	int shift = 0;
	for (i = 0; i < BIGBUF_SIZE; i++) {
		data.bigbuf[i] = i;
	}
	data.idx = 0;
	data.count = 0;
	prof.func = profile_wrapper_condition;
	prof.args = &data;
	prof.repeat = BIGBUF_SIZE;
	profile_func(&prof);
	printf("count=%d\n", data.count);
	munmap(data.bigbuf, BIGBUF_SIZE);
	return 0;

	// profile signal
	signal(SIGUSR1, handle_signal);
	prof.func = profile_wrapper_signal;
	prof.args = NULL;
	prof.repeat = 1000000;
	profile_func(&prof);
	signal(SIGUSR1, SIG_IGN);

	// profile printf
	prof.func = profile_wrapper_printf;
	prof.args = NULL;
	prof.repeat = 1000000;
	profile_func(&prof);

*/
	/*
	data.bigbuf = mmap(NULL, 1L << 32, PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, 0, 0);
	data.idx = 0;
	prof.func = profile_wrapper_pagefault;
	prof.args = &data;
	prof.repeat = (1L << 32) / PAGE_SIZE;
	profile_func(&prof);
	data.idx = 0;
	profile_func(&prof);
	munmap(data.bigbuf, 1L << 32);
	*/

	/*
	data.buf1 = mmap(NULL, PAGE_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, 0, 0);
	data.buf2 = mmap(NULL, PAGE_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, 0, 0);
	memset(data.buf1, 42, PAGE_SIZE);
	memset(data.buf2, 82, PAGE_SIZE);

	prof.func = profile_wrapper_memcpy;
	prof.args = &data;
	profile_func(&prof);

	munmap(data.buf1, PAGE_SIZE);
	munmap(data.buf2, PAGE_SIZE);
	 */
	int stack_size = 1024*1024;
	data.stack = malloc(stack_size);
	data.stack += stack_size;
	//data.bigbuf = mmap(NULL, 1L << 30, PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, 0, 0);
	prof.func = profile_wrapper_clone;
	prof.args = &data;
	prof.repeat = 1;
	profile_func(&prof);
	//munmap(data.bigbuf, 1L << 32);


	return 0;
}
