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

#define MSLEEP 100
#define UHOG 100000

int main(int argc, char *argv[])
{
     int sockfd, newsockfd, portno;
     int yes = 1;
     socklen_t clilen;
     char buffer[256];
     struct sockaddr_in serv_addr, cli_addr;
     int n;
     struct hostent *server;

     if (argc < 3) {
         fprintf(stderr,"ERROR, no bind address and/or port provided\n");
         exit(1);
     }

     printf("server starting\n");
	 // 100ms by default
	 unsigned long count = calibrate(UHOG);

	 sockfd = socket(AF_INET, SOCK_STREAM, 0);
     if (sockfd < 0) 
        throw("ERROR opening socket");

     if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, (char *) &yes, sizeof(yes)) < 0)
    	 throw("ERROR while setting socket option");

     server = gethostbyname(argv[1]);
     if (server == NULL) {
         fprintf(stderr,"ERROR, no such host\n");
         exit(0);
     }
     bzero((char *) &serv_addr, sizeof(serv_addr));
     portno = atoi(argv[2]);
     serv_addr.sin_family = AF_INET;
     //serv_addr.sin_addr.s_addr = INADDR_ANY;
     bcopy((char *)server->h_addr,
          (char *)&serv_addr.sin_addr.s_addr,
          server->h_length);
     serv_addr.sin_port = htons(portno);
     if (bind(sockfd, (struct sockaddr *) &serv_addr,
              sizeof(serv_addr)) < 0) 
              throw("ERROR on binding");
     printf("server waiting cmd\n");
     listen(sockfd,5);
     clilen = sizeof(cli_addr);
     newsockfd = accept(sockfd, 
                 (struct sockaddr *) &cli_addr, 
                 &clilen);
     if (newsockfd < 0) 
          throw("ERROR on accept");
     bzero(buffer,256);
     n = read(newsockfd,buffer,255);
     if (n < 0) throw("ERROR reading from socket");
     printf("server recv cmd: %s (%d)\n",buffer,n);
     if (strncmp(buffer, "sleep", n) == 0) {
    	 printf("do_sleep\n");
    	 do_sleep(MSLEEP);
     } else if (strncmp(buffer, "hog", n) == 0) {
    	 printf("do_hog\n");
    	 do_hog(count);
     } else {
    	 printf("unknown command\n");
     }
     n = write(newsockfd,"done",18);
     if (n < 0) throw("ERROR writing to socket");
     close(newsockfd);
     close(sockfd);
     return 0; 
}
