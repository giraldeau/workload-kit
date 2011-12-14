/*
 * cpm3.c
 *
 *  Created on: 2011-10-25
 *      Author: francis
 *
 *  Exchanges packets from a client and server
 *
 *                          waitpid
 *                         /
 *	master  ====+==========+===
 *            |  |         |
 *  client    |  +=========+
 *            |            |
 *  server    +============+
 *
 *  The master spawn a server and a client. The the server listen for incoming
 *  connection. The client connects to the server. A pattern of send and
 *  receive is performed.
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include "utils.h"
#include "calibrate.h"

#define DEFAULT_HOST "localhost"
#define DEFAULT_PORT 7373
#define SIZE1 1024
#define SIZE2 512
#define SERVER "server"
#define CLIENT "client"

long unit = 0;

void server();
void client();

typedef void (*func)(void);

void spawn(func f, pid_t *pid)
{
	*pid = fork();
	if (*pid < 0)
		return;
	if (*pid == 0) {
		f();
		exit(0);
	}
}

void master()
{
	pid_t server_pid, client_pid;

	// handle error properly, otherwise
	/* start the server */
	spawn(server, &server_pid);

	/* start the client */
	spawn(client, &client_pid);

	/* wait all */
	waitpid(client_pid);
	waitpid(server_pid);
	return;
}

int make_client_socket()
{
    int sockfd, portno, n;
    struct sockaddr_in serv_addr;
    struct hostent *server;

    portno = DEFAULT_PORT;
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0)
        throw("ERROR opening socket");
    server = gethostbyname(DEFAULT_HOST);
    if (server == NULL) {
        fprintf(stderr,"ERROR, no such host\n");
        exit(0);
    }
    bzero((char *) &serv_addr, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    bcopy((char *)server->h_addr,
         (char *)&serv_addr.sin_addr.s_addr,
         server->h_length);
    serv_addr.sin_port = htons(portno);
    if (connect(sockfd,(struct sockaddr *) &serv_addr,sizeof(serv_addr)) < 0)
        throw("ERROR connecting");
    return sockfd;
}

int make_server_socket()
{
    int sockfd, newsockfd, portno;
    struct hostent *server;
    int yes = 1;
    socklen_t clilen;
    struct sockaddr_in serv_addr, cli_addr;

	sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0)
       throw("ERROR opening socket");

    if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, (char *) &yes, sizeof(yes)) < 0)
   	 throw("ERROR while setting socket option");

    server = gethostbyname(DEFAULT_HOST);
    if (server == NULL) {
        fprintf(stderr,"ERROR, no such host\n");
        exit(0);
    }
    bzero((char *) &serv_addr, sizeof(serv_addr));
    portno = DEFAULT_PORT;
    serv_addr.sin_family = AF_INET;
    bcopy((char *)server->h_addr,
         (char *)&serv_addr.sin_addr.s_addr,
         server->h_length);
    serv_addr.sin_port = htons(portno);
    if (bind(sockfd, (struct sockaddr *) &serv_addr,
             sizeof(serv_addr)) < 0)
             throw("ERROR on binding");
    printf("server waiting\n");
    listen(sockfd,5);
    clilen = sizeof(cli_addr);
    newsockfd = accept(sockfd,
                (struct sockaddr *) &cli_addr,
                &clilen);
    if (newsockfd < 0)
         throw("ERROR on accept");
    return newsockfd;
}

void do_send(int id, char *who, int sock, char *buf, int size)
{
	ssize_t n = send(sock, buf, size, 0);
	printf("%d %s send %ld\n", id, who, n);
	do_sleep(50);
}

void do_recv(int id, char *who, int sock, char *buf, int size)
{
	ssize_t n = recv(sock, buf, size, 0);
	printf("%d %s recv %ld\n", id, 	who, n);
	do_sleep(50);
}

void server()
{
	int sock;
	ssize_t n;
	char buf[SIZE1];
	pid_t pid;
	int cnt = 0;

	pid = getpid();
	sock = make_server_socket();
	memset(buf, 0, SIZE1);
	printf("server pid=%d\n", pid);

	/* 1 */
	do_recv(cnt++, SERVER, sock, buf, SIZE1);

	/* 2 */
	do_send(cnt++, SERVER, sock, buf, SIZE1);

	/* 3 */
	do_recv(cnt, SERVER, sock, buf, SIZE2);
	do_recv(cnt++, SERVER, sock, buf, SIZE2);

	/* 4 */
	do_recv(cnt, SERVER, sock, buf, SIZE1);

	shutdown(sock, 0);
	return;
}

void client()
{
	int sock;
	ssize_t n;
	char buf[SIZE1];
	pid_t pid;
	int cnt = 0;

	pid = getpid();
	sock = make_client_socket();
	memset(buf, 0, SIZE1);
	printf("client pid=%d\n", pid);
	do_sleep(50);

	/* 1 */
	do_send(cnt++, CLIENT, sock, buf, SIZE1);

	/* 2 */
	do_recv(cnt++, CLIENT, sock, buf, SIZE1);

	/* 3 */
	do_send(cnt++, CLIENT, sock, buf, SIZE1);

	/* 4 */
	do_send(cnt, CLIENT, sock, buf, SIZE2);
	do_send(cnt, CLIENT, sock, buf, SIZE2);

	shutdown(sock, 0);
	return;
}

int main(int argc, char **argv)
{

	suseconds_t s = 100000;
	unit = calibrate(s);

	master();
	return EXIT_SUCCESS;
}
