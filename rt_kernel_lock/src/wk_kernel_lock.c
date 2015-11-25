#define _GNU_SOURCE
#include <pthread.h>
#include <sched.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/prctl.h>
#include <urcu/uatomic.h>

int debug = 1;
static volatile int test_go;

int set_affinity_prio(int cpu, int prio)
{
	cpu_set_t mask;
	struct sched_param p;
	int ret = 0;

	p.sched_priority = prio;

	CPU_ZERO(&mask);
	CPU_SET(cpu, &mask);
	ret = sched_setaffinity(0, sizeof(mask), &mask);
	if (ret)
		goto end;

	ret = sched_setscheduler(0, SCHED_RR, &p);
	if (ret)
		goto end;
end:
	return ret;
}

void print_debug(char *str)
{
	if (debug)
		fprintf(stdout, "%s\n", str);
}

/*
 * Tries to acquire the kernel lock, releases it and returns.
 * Started after the lock has already been acquired, so it should
 * hang while trying to open take it.
 */
void *highprio0(void *arg)
{
	int fd, ret;

	ret = prctl(PR_SET_NAME, "highprio0", 0, 0, 0);
	if (ret) {
		perror("set name");
	}

	ret = set_affinity_prio(0, 80);
	if (ret) {
		fprintf(stderr, "highprio0: error setting the priority "
				"or affinity\n");
		goto end;
	}

	/* Wait for lowprio0 to be ready */
	while(!test_go)
		usleep(1);
	cmm_smp_mb();

	print_debug("highprio0 trying to acquire the lock");
	fd = open("/proc/wk_lock", O_RDONLY);
	if (fd < 0) {
		perror("open /proc/wk_lock");
		goto end;
	}
	print_debug("highprio0 got the lock");

	ret = close(fd);
	if (ret)
		perror("close /proc/wk_lock");
	print_debug("highprio0 released the lock");

end:
	return;
}

/*
 * Takes the lock and tries to do active work but has a lower priority
 * than highprio1, when it's work is done, release the lock.
 */
void *lowprio1(void *arg)
{
	int fd, ret;
	volatile int cnt;

	ret = prctl(PR_SET_NAME, "lowprio1", 0, 0, 0);
	if (ret) {
		perror("set name");
	}

	ret = set_affinity_prio(1, 30);
	if (ret) {
		fprintf(stderr, "lowprio1: error setting the priority "
				"or affinity\n");
		goto end;
	}

	print_debug("lowprio1 trying to acquire the lock");
	fd = open("/proc/wk_lock", O_RDONLY);
	if (fd < 0) {
		perror("open /proc/wk_lock");
		goto end;
	}
	print_debug("lowprio1 got the lock");

	cmm_smp_mb();
	/* Allow the other threads to start their work */
	test_go = 1;

	while (cnt < 100000000)
		cnt++;

	print_debug("lowprio1 busy work done, releasing the lock");
	ret = close(fd);
	if (ret)
		perror("close /proc/wk_lock");
	print_debug("lowprio1 released the lock");

end:
	return;
}

/*
 * Only busy looping with higher priority than lowprio1.
 */
void *highprio1(void *arg)
{
	int ret;
	volatile int cnt;

	ret = prctl(PR_SET_NAME, "highprio1", 0, 0, 0);
	if (ret) {
		perror("set name");
	}

	ret = set_affinity_prio(1, 80);
	if (ret) {
		fprintf(stderr, "highprio1: error setting the priority "
				"or affinity\n");
		goto end;
	}

	/* Wait for lowprio0 to be ready */
	while(!test_go)
		usleep(1);
	cmm_smp_mb();

	print_debug("highprio1 busy looping");
	while (cnt < 100000000)
		cnt++;
	print_debug("highprio1 done");

end:
	return;
}

int main()
{
	int ret;
	pthread_t highprio0_t, highprio1_t, lowprio1_t;

	ret = set_affinity_prio(0, 99);
	if (ret) {
		fprintf(stderr, "main: error setting the priority "
				"or affinity\n");
		goto end;
	}
	
	ret = pthread_create(&lowprio1_t, NULL, &lowprio1, NULL);
	if (ret != 0) {
		perror("pthread_create");
		goto end;
	}

	/* Wait for lowprio0 to be ready */
	while(!test_go)
		usleep(1);
	cmm_smp_mb();

	ret = pthread_create(&highprio1_t, NULL, *highprio1, NULL);
	if (ret != 0) {
		perror("pthread_create");
		goto error1;
	}

	ret = pthread_create(&highprio0_t, NULL, &highprio0, NULL);
	if (ret != 0) {
		perror("pthread_create");
		goto error2;
	}

	ret = 0;

	pthread_join(highprio0_t, NULL);
error2:
	pthread_join(highprio1_t, NULL);
error1:
	pthread_join(lowprio1_t, NULL);
end:
	return ret;
}
