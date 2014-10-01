#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <getopt.h>
#include "utils.h"
#include "rpc.h"

#define progname "wk-rpc-client"

#define DEFAULT_ASYNC 0
#define DEFAULT_DELAY 10
#define DEFAULT_PORT 9876

struct opts {
	int async;
	int delay;
	int cmd;
	char *server;
	int port;
};

struct cx {
	int sockfd;
	struct sockaddr_in serv_addr;
	struct hostent *server;
};


__attribute__((noreturn))
static void usage(void) {
    fprintf(stderr, "Usage: %s [OPTIONS] [COMMAND]\n", progname);
    fprintf(stderr, "\nOptions:\n\n");
    fprintf(stderr, "--server         server address\n");
    fprintf(stderr, "--port           port (default 9876)\n");
    fprintf(stderr, "--delay          server side operation delay (ms)\n");
    fprintf(stderr, "--async          amount of asynchronous processing (ms)\n");
    fprintf(stderr, "--command        command to execute on the server [ hog, sleep ]\n");
    fprintf(stderr, "--verbose        be more verbose\n");
    fprintf(stderr, "--help           print this message and exit\n");
    fprintf(stderr, "\n");
    exit(EXIT_FAILURE);
}

static void parse_opts(int argc, char **argv, struct opts *opts) {
	int opt;
	int idx;

	struct option options[] = {
		{ "help",       0, 0, 'h' },
		{ "server",     1, 0, 's' },
		{ "port",       1, 0, 'p' },
		{ "async",      1, 0, 'a' },
		{ "delay",      1, 0, 'd' },
		{ "command",    1, 0, 'c' },
		{ "verbose",    0, 0, 'v' },
		{ 0, 0, 0, 0 }
	};

	opts->cmd = RPC_HOG;
	opts->delay = DEFAULT_DELAY;
	opts->async = DEFAULT_ASYNC;
	opts->port = DEFAULT_PORT;
	opts->server = NULL;

	while ((opt = getopt_long(argc, argv, "hva:d:c:s:p:", options, &idx)) != -1) {
		switch (opt) {
		case 'c':
			if (strcmp(optarg, "hog") == 0) {
				opts->cmd = RPC_HOG;
			} else if (strcmp(optarg, "sleep") == 0) {
				opts->cmd = RPC_SLEEP;
			}
			break;
		case 'a':
			opts->async = atoi(optarg);
			break;
		case 'd':
			opts->delay = atoi(optarg);
			break;
		case 's':
			opts->server = strdup(optarg);
			break;
		case 'p':
			opts->port = atoi(optarg);
			break;
		case 'h':
			usage();
			break;
		default:
			usage();
			break;
		}
	}

	if (opts->server == NULL) {
		printf("error: server address must be specified\n");
		usage();
	}
}

int rpc_connect(struct opts *opts, struct cx *cx) {
	int ret;

	cx->sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if (cx->sockfd < 0) {
		printf("socket() failed\n");
		return -1;
	}
	cx->server = gethostbyname(opts->server);
	if (cx->server == NULL) {
		printf("gethostbyname() failed\n");
		return -1;
	}
	memset(&cx->serv_addr, 0, sizeof(cx->serv_addr));
	cx->serv_addr.sin_family = AF_INET;
	memcpy(&cx->serv_addr.sin_addr.s_addr, cx->server->h_addr,
			sizeof(cx->server->h_length));
	cx->serv_addr.sin_port = htons(opts->port);

	ret = connect(cx->sockfd, (struct sockaddr *) &cx->serv_addr,
			sizeof(cx->serv_addr));
	if (ret < 0) {
		printf("connect() failed\n");
		return -1;
	}

	return 0;
}

int rpc_command(struct opts *opts, struct cx *cx, struct message *msg, struct message *ans) {
	int ret;
	ret = write(cx->sockfd, msg, sizeof(struct message));
	if (ret < 0) {
		printf("write() failed\n");
		return ret;
	}
	ret = read(cx->sockfd, ans, sizeof(struct message));
	if (ret < 0) {
		printf("read() failed\n");
		return ret;
	}
	printf("status: %d\n", ans->ret);
	return ret;
}

int rpc_terminate(struct cx *cx) {
	close(cx->sockfd);
	return 0;
}

int main(int argc, char *argv[]) {
	struct opts opts;
	struct cx cx;
	struct message msg;
	struct message ans;
	int ret;

	parse_opts(argc, argv, &opts);
	ret = rpc_connect(&opts, &cx);
	if (ret < 0)
		return -1;
	msg.cmd = opts.cmd;
	msg.arg = opts.delay;
	msg.ret = 0;
	memset(&ans, 0, sizeof(struct message));
	rpc_command(&opts, &cx, &msg, &ans);
	rpc_terminate(&cx);
	return 0;

	/*
	char buffer[256];
	if (argc < 4) {
		fprintf(stderr,"usage %s hostname port\n", argv[0]);
		exit(0);
	}
	portno = atoi(argv[2]);
	sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if (sockfd < 0)
		throw("ERROR opening socket");
	server = gethostbyname(argv[1]);
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
	//printf("Please enter the message: ");
	//bzero(buffer,256);
	//fgets(buffer,255,stdin);
	printf("client send cmd %s\n", argv[3]);
	n = write(sockfd,argv[3],strlen(argv[3]));
	if (n < 0)
		throw("ERROR writing to socket");
	bzero(buffer,256);
	n = read(sockfd,buffer,255);
	if (n < 0)
		throw("ERROR reading from socket");
	printf("client recv result %s\n", buffer);
	close(sockfd);
	return 0;
	*/
}
