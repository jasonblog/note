#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <errno.h>
#include <sys/wait.h> // waipid()
#include <sys/socket.h>
#include <netinet/in.h> // sockaddr_in
#include <arpa/inet.h> // inet_pton()
#include "common.h"


int main(int argc, char **argv)
{
	ssize_t	            n;
	socklen_t	        len;
	char		        buf[MAXLINE];
	int 				sockfd;
	struct sockaddr_in	servaddr, cliaddr;

	// Apply for UDP socket.
	if ((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
		perror("socket");
		exit(EXIT_FAILURE);
	}

	// Set the socket.
	memset(&servaddr, 0, sizeof(servaddr));
	servaddr.sin_family = AF_INET; 	// AF-address family. PF-protocol family.
	servaddr.sin_addr.s_addr = htonl(INADDR_ANY); // "0.0.0.0"
	servaddr.sin_port   = htons(8080);

	if (bind(sockfd, (struct sockaddr *)&servaddr, sizeof(servaddr)) < 0) {
		perror("bind");
		exit(EXIT_FAILURE);
	}
    printf("Listening on port %d, udp version\n", LISTEN_PORT);

	for ( ; ; ) {
		len = sizeof(cliaddr);
		if ((n = recvfrom(sockfd, buf, MAXLINE, 0, (struct sockaddr *)&cliaddr, &len)) < 0) {
			fprintf(stderr, "recvfrom error\n");
			exit(EXIT_FAILURE);
		}
        printf("receive from %s(%d):" , inet_ntoa(cliaddr.sin_addr), cliaddr.sin_port); fflush(stdout);
        if (write(fileno(stdout), buf, n) != n) { // write back
            fprintf(stderr, "writen error\n");
            exit(EXIT_FAILURE);
        }

		if (sendto(sockfd, buf, n, 0, (struct sockaddr*)&cliaddr, len) != n) {
			fprintf(stderr, "sendto error\n");
			exit(EXIT_FAILURE);
		}
	}

	return 0;
}

