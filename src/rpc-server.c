/* A simple server in the internet domain using TCP
   The port number is passed as an argument */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h> 
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include "utils.h"
#include "calibrate.h"
#include "rpc.h"

int main(int argc, char *argv[])
{
	int sockfd, newsockfd, portno;
	int yes = 1;
	socklen_t clilen;
	char buffer[256];
	struct sockaddr_in serv_addr, cli_addr;
	int ret;
	struct hostent *server;

	if (argc < 3) {
		fprintf(stderr,"ERROR, no bind address and/or port provided\n");
		exit(1);
	}

	printf("server starting\n");
	/* 100us * 10 = 1ms */
	unsigned long count = calibrate(10000) / 10;

	sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if (sockfd < 0)
		throw("ERROR opening socket");

	if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int)) < 0)
		throw("ERROR while setting socket option");

	server = gethostbyname(argv[1]);
	if (server == NULL) {
		fprintf(stderr,"ERROR, no such host\n");
		exit(0);
	}
	memset(&serv_addr, 0, sizeof(serv_addr));
	portno = atoi(argv[2]);
	serv_addr.sin_family = AF_INET;
	memcpy(&serv_addr.sin_addr.s_addr, server->h_addr, server->h_length);
	serv_addr.sin_port = htons(portno);
	ret = bind(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr));
	if (ret < 0)
		throw("ERROR on binding");
	printf("server waiting cmd\n");
	listen(sockfd,5);
	clilen = sizeof(cli_addr);
	while(1) {
		struct message msg;

		newsockfd = accept(sockfd, (struct sockaddr *) &cli_addr, &clilen);
		if (newsockfd < 0)
			throw("ERROR on accept");
		do {
			memset(&msg, 0, sizeof(msg));
			ret = read(newsockfd, &msg, sizeof(msg));
			if (ret < 0)
				throw("ERROR reading from socket");
			//printf("server recv cmd: %d arg: %d\n", msg.cmd, msg.arg);
			switch (msg.cmd) {
			case RPC_HOG:
				do_hog(msg.arg * count);
				break;
			case RPC_SLEEP:
				do_sleep(msg.arg);
				break;
			case RPC_PING:
				// do nothing
			default:
				break;
			}
			msg.ret = 42;
			ret = write(newsockfd, &msg, sizeof(msg));
			if (ret < 0)
				throw("ERROR writing to socket");
		} while(msg.cnt > 0);
		close(newsockfd);
	}
	close(sockfd);
	return 0;
}
