/*
 * inception.c
 *
 * This program creates recursively children process that sleeps for a while
 *
 *  Created on: 2011-02-18
 *      Author: francis
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>

void error(const char *msg)
{
    perror(msg);
    exit(0);
}

/*
 * Based on evutil.c from mozilla firefox
 */
int create_socketpair(int fd[2])
{
	int listener = -1;
	int connector = -1;
	int acceptor = -1;
	struct sockaddr_in listen_addr;
	struct sockaddr_in connect_addr;
	int size;
	int saved_errno = -1;

	listener = socket(AF_INET, SOCK_STREAM, 0);
	if (listener < 0)
		return -1;

	memset(&listen_addr, 0, sizeof(listen_addr));
	listen_addr.sin_family = AF_INET;
	listen_addr.sin_addr.s_addr = htonl(INADDR_LOOPBACK);
	listen_addr.sin_port = 0;

	if (bind(listener, (struct sockaddr *) &listen_addr, sizeof(listen_addr)) == -1)
		return -1;

	if (listen(listener, 1) == -1)
		return -1;

	connector = socket(AF_INET, SOCK_STREAM, 0);
	if (connector < 0)
		return -1;

	size = sizeof(connect_addr);
	if (getsockname(listener, (struct sockaddr *) &connect_addr, &size) == -1)
		return -1;

	if (size != sizeof(connect_addr))
		return -1;

	printf("listener addr %d port %d\n", connect_addr.sin_addr.s_addr, connect_addr.sin_port);

	if (connect(connector, (struct sockaddr *) &connect_addr, sizeof(connect_addr)) == -1)
		return -1;

	size = sizeof(listen_addr);
	acceptor = accept(listener, (struct sockaddr *) &listen_addr, &size);
	if (acceptor < 0)
		return -1;

	if (size != sizeof(listen_addr))
		return -1;

	close(listener);

	if (getsockname(connector, (struct sockaddr *) &connect_addr, &size) == -1)
		return -1;

	printf("connector addr %d port %d\n", connect_addr.sin_addr.s_addr, connect_addr.sin_port);

	fd[0] = connector;
	fd[1] = acceptor;

	return 0;
}

int main(int argc, char *argv[])
{
	int fd[2];

	if (create_socketpair(fd) < 0)
		return -1;

    return 0;
}
