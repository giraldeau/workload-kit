#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <string.h>
#include <execinfo.h>

volatile static int *x = NULL;
volatile static int action = 0;

#define print(msg) write(2, msg, strlen(msg))

void do_bt()
{
	int n = 10;
	void *buffer[n];
	backtrace((void **)&buffer, n);
	backtrace_symbols_fd(buffer, n, 2);
}

void crash(int signo)
{
	do_bt();
	exit(0);
}

void handler2()
{
	print("handler2\n");
	do_bt();
}

void handler3()
{
	print("handler3\n");
	do_bt();
}

void handler1(int signo)
{
	volatile int boom;
	sigset_t set;

	switch(action) {
	case 0:
		sigemptyset(&set);
		sigaddset(&set, SIGSEGV);
		sigprocmask(SIG_BLOCK, &set, NULL);
		print("before\n");
		boom = *x;
		print("after\n");
		sigprocmask(SIG_BLOCK, &set, NULL);
		break;
	case 1:
		kill(getpid(), SIGUSR2);
		break;
	case 2:
		/* blocked signals are queued and run when unblocked */
		sigemptyset(&set);
		sigaddset(&set, SIGUSR2);
		sigprocmask(SIG_BLOCK, &set, NULL);
		kill(getpid(), SIGUSR2);
		sigprocmask(SIG_UNBLOCK, &set, NULL);
		break;
	case 3:
		/* ignored signals are droped */
		signal(SIGUSR2, SIG_IGN);
		kill(getpid(), SIGUSR2);
		break;
	default:
		break;
	}
}

int main(int argc, char **argv)
{
	sigset_t set;
	int signo = __libc_allocate_rtsig(SIGRTMAX);
	if (argc == 2)
		action = atoi(argv[1]);
	printf("action=%d signo=%d\n", action, signo);
	signal(SIGUSR1, handler1);
	signal(SIGUSR2, handler2);
	signal(signo, handler3);
	signal(SIGSEGV, crash);

	sigemptyset(&set);
	sigaddset(&set, SIGUSR2);
	sigprocmask(SIG_BLOCK, &set, NULL);

	kill(getpid(), SIGUSR2);
	kill(getpid(), SIGUSR2);
	sigprocmask(SIG_UNBLOCK, &set, NULL);
	printf("bye\n");

	/*
	sigemptyset(&set);
	sigaddset(&set, SIGUSR2);
	sigprocmask(SIG_BLOCK, &set, NULL);

	kill(getpid(), SIGUSR1);
	printf("main\n");
	//sigprocmask(SIG_UNBLOCK, &set, NULL);
	printf("bye\n");
	*/
}
