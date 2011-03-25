#include <stdio.h>
#include <pthread.h>

pthread_t t1, t2;

void *child2(void *p)
{
	printf("child2 created : %d (%d)\n", getpid(), getppid());
	sleep(1);
}

void *child1(void *p)
{
	printf("child1 created : %d (%d)\n", getpid(), getppid());
	sleep(1);
	pthread_create(&t2, NULL, child2, (void *)NULL);
	sleep(3);
}

int main()
{
	pthread_create(&t1, NULL, child1, (void *)NULL);

	pthread_join(t1, NULL);
	pthread_join(t2, NULL);

	return 0;
}
