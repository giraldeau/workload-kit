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
#define SIZE 1024
#define SERVER "server"
#define CLIENT "client"
#define DELAY 10
long unit = 0;

void server();
void client();

typedef struct ctx {
	int id;
	const char *who;
	int sock;
	char *buf;
	int size;
	long int mili;
} ctx_t;

typedef void (*func)(void);
typedef void (*tx_hdl)(ctx_t *, int, int);

ctx_t *make_ctx(int size)
{
	ctx_t *ctx = calloc(1, sizeof(ctx_t));
	ctx->buf = malloc(size);
	ctx->size = size;
	return ctx;
}

void free_ctx(ctx_t *ctx)
{
	if (ctx != NULL) {
		if (ctx->buf != NULL)
			free(ctx->buf);
		free(ctx);
	}
}

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
	do_sleep(10);

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

void do_send(ctx_t *ctx, int size, int id)
{
	ssize_t n = send(ctx->sock, ctx->buf, size, 0);
	printf("%d %s send %ld\n", id, 	ctx->who, n);
	do_hog(ctx->mili);
}

void do_recv(ctx_t *ctx, int size, int id)
{
	ssize_t n = recv(ctx->sock, ctx->buf, size, 0);
	printf("%d %s recv %ld\n", id, 	ctx->who, n);
	do_hog(ctx->mili);
}

void do_server_run(ctx_t *ctx)
{
	int full = ctx->size;
	int half = ctx->size >> 1;
	int cnt = 0;

	/* 1 */
	do_send(ctx, full, cnt++);

	/* 3 */
	do_send(ctx, half, cnt);
	do_send(ctx, half, cnt++);

	/* 4 */
	do_send(ctx, full, cnt++);

	do_sleep(50);
}

void do_client_run(ctx_t *ctx)
{
	int full = ctx->size;
	int half = ctx->size >> 1;
	int cnt = 0;

	/* 1 */
	do_recv(ctx, full, cnt++);

	/* 3 */
	do_recv(ctx, full, cnt);

	/* 4 */
	do_recv(ctx, half, cnt++);
	do_recv(ctx, half, cnt++);

	do_sleep(50);
}

void server()
{
	ctx_t *ctx = make_ctx(SIZE);
	ctx->sock = make_server_socket();

	/* both delay */
	ctx->mili = unit;
	do_server_run(ctx);

	/* server delay */
	do_server_run(ctx);

	/* client delay */
	ctx->mili = 0;
	do_server_run(ctx);

	shutdown(ctx->sock, 0);
	free_ctx(ctx);
	return;
}

void client()
{
	ctx_t *ctx = make_ctx(SIZE);
	ctx->sock = make_client_socket();

	/* both delay */
	ctx->mili = unit;
	do_client_run(ctx);

	/* server delay */
	ctx->mili = 0;
	do_client_run(ctx);

	/* client delay */
	ctx->mili = unit;
	do_client_run(ctx);

	shutdown(ctx->sock, 0);
	free_ctx(ctx);
	return;
}

int main(int argc, char **argv)
{

	suseconds_t s = 10000;
	unit = calibrate(s);
	master();
	return EXIT_SUCCESS;
}
