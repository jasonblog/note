#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <errno.h>
#include <sys/wait.h>   // waipid()
#include <sys/socket.h>
#include <netinet/in.h> // sockaddr_in
#include <arpa/inet.h> // inet_pton()
#include <limits.h>		/* for OPEN_MAX */
#include <poll.h> /* poll function */
#include "common.h"


#define MAXEVENTS 64

// Write "n" bytes to a descriptor.
ssize_t	writen(int fd, const void *vptr, size_t n)
{
	size_t		nleft;
	ssize_t		nwritten;
	const char	*ptr;

	ptr = vptr;
	nleft = n;
	while (nleft > 0) {
		if ((nwritten = write(fd, ptr, nleft)) <= 0) {
			if (nwritten < 0 && errno == EINTR)
				nwritten = 0;  // and call write() again
			else
				return(-1);	// error
		}

		nleft -= nwritten;
		ptr   += nwritten;
	}
	return(n);
}


int main(int argc, char **argv)
{
	int					listenfd, sockfd, opt = 1;
    char                cliip[16];
	socklen_t			clilen;
	struct sockaddr_in	cliaddr, servaddr;
    ssize_t             n;
    char                buf[MAXLINE];

	int					i, maxi,  connfd;
	int					nready;
	struct pollfd		client[MAXEVENTS];


	// Apply for TCP socket.
	if ((listenfd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
		perror("socket");
		exit(EXIT_FAILURE);
	}

    // When server quit first, the address can be used immediately.
    if (setsockopt(listenfd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(int)) == -1) {
        perror("setsockopt");
		exit(EXIT_FAILURE);
    }

	// Set the socket.
	memset(&servaddr, 0, sizeof(servaddr));
	servaddr.sin_family = AF_INET; 	              // AF-address family. PF-protocol family.
	servaddr.sin_addr.s_addr = htonl(INADDR_ANY); // "0.0.0.0"
	servaddr.sin_port   = htons(LISTEN_PORT);

	if (bind(listenfd, (struct sockaddr *)&servaddr, sizeof(servaddr)) < 0) {
		perror("bind");
		exit(EXIT_FAILURE);
	}

	if (listen(listenfd, 5) < 0) {
		perror("listen");
		exit(EXIT_FAILURE);
	}
    printf("Listening on port %d, select version\n", LISTEN_PORT);

	client[0].fd = listenfd;
	client[0].events = POLLRDNORM;  // POLLIN
	for (i = 1; i < MAXEVENTS; i++)
		client[i].fd = -1;		/* -1 indicates available entry */
	maxi = 0;					/* max index into client[] array */

	while(1) {
		if((nready = poll(client, maxi+1, -1)) < 0) { // INFTIM = -1
			perror("poll");
			exit(EXIT_FAILURE);
		}

		if (client[0].revents & POLLRDNORM) {	// new client connection
			clilen = sizeof(cliaddr);
			if ((connfd = accept(listenfd, (struct sockaddr *) &cliaddr, &clilen)) < 0) {
				if (errno == EINTR) // interrupt by the signal.
					continue; // back to for()
				else {
					perror("accept");
					exit(EXIT_FAILURE);
				}
			}
			printf("new client: %s, port %d\n",
					inet_ntop(AF_INET, &cliaddr.sin_addr, cliip, 16), ntohs(cliaddr.sin_port));

			for (i = 1; i < MAXEVENTS; i++)
				if (client[i].fd < 0) {
					client[i].fd = connfd;	/* save descriptor */
					break;
				}
			if (i == MAXEVENTS) {
				fprintf(stderr, "too many clients\n");
				exit(EXIT_FAILURE);
			}

			client[i].events = POLLRDNORM;
			if (i > maxi)
				maxi = i;				/* max index in client[] array */

			if (--nready <= 0)
				continue;				/* no more readable descriptors */
		}

		for (i = 1; i <= maxi; i++) {	/* check all clients for data */
			if ( (sockfd = client[i].fd) < 0)
				continue;

			if (client[i].revents & (POLLRDNORM | POLLERR)) {
				n = read(sockfd, buf, MAXLINE);
				if (n == 0) { // connection closed by client
					printf("client[%d] closed connection\n", i);
					if (close(sockfd) == -1) {
						fprintf(stderr, "close error\n");
						exit(EXIT_FAILURE);
					}
					client[i].fd = -1;
				} else if (n < 0) {
					if (errno == ECONNRESET) { /* connection reset by client */
						printf("client(%d) aborted connection\n", i);
						if (close(sockfd) == -1) {
							fprintf(stderr, "close error\n");
							exit(EXIT_FAILURE);
						}
						client[i].fd = -1;
					} else {
						fprintf(stderr, "read error\n");
						exit(EXIT_FAILURE);
					}
				} else {
                    fprintf(stdout, "cli(%d):", i); fflush(stdout);
					if (writen(fileno(stdout), buf, n) != n) { // write to screen
						fprintf(stderr, "writen error\n");
						exit(EXIT_FAILURE);
					}
					if (writen(sockfd, buf, n) != n) { // Write back.
						fprintf(stderr, "writen error\n");
						exit(EXIT_FAILURE);
					}
				}

				if (--nready <= 0)
					break;				/* no more readable descriptors */
			}
		}
	}
}

