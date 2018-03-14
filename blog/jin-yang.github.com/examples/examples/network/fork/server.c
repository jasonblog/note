#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <errno.h>
#include <sys/wait.h> // waipid()
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h> // sockaddr_in
#include <arpa/inet.h> // inet_pton()
#include "common.h"


// Write "n" bytes to a descriptor.
ssize_t writen(int fd, const void *vptr, size_t n)
{
    size_t      nleft;
    ssize_t     nwritten;
    const char  *ptr;

    ptr = vptr;
    nleft = n;
    while (nleft > 0) {
        if ( (nwritten = write(fd, ptr, nleft)) <= 0) {
            if (nwritten < 0 && errno == EINTR)
                nwritten = 0;  // and call write() again
            else
                return(-1); // error
        }

        nleft -= nwritten;
        ptr   += nwritten;
    }
    return(n);
}
void sig_child(int signo)
{
	pid_t	pid;
	int		stat;

	while ((pid = waitpid(-1, &stat, WNOHANG)) > 0)
		printf("child %d terminated\n", pid); // child quit.
	return;
}

int main(int argc, char **argv)
{
	int					listenfd, sockfd, opt = 1;
    char                cliip[16];
	socklen_t			clilen;
	struct sockaddr_in	cliaddr, servaddr;
    ssize_t             n;
    char                buf[MAXLINE];

	pid_t				childpid;
	struct sigaction	act;

	// Wait the child to quit, or zombine programs occur.
	sigemptyset(&act.sa_mask);
	act.sa_flags = 0;
	act.sa_handler = sig_child;
	if (sigaction(SIGCHLD, &act, NULL) < 0) {
		fprintf(stderr, "sigaction error\n");
		exit(EXIT_FAILURE);
	}

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
	servaddr.sin_family = AF_INET; 	// AF-address family. PF-protocol family.
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
	printf("Listening on port(%d), multi process version.\n", LISTEN_PORT);

	for (;;) {
		clilen = sizeof(cliaddr);
		if ((sockfd = accept(listenfd, (struct sockaddr *) &cliaddr, &clilen)) < 0) {
			if (errno == EINTR) // interrupt by the signal.
				continue; // back to for()
			else {
				perror("accept");
				exit(EXIT_FAILURE);
			}
		}

		if ((childpid = fork()) == 0) {	// child process
			close(listenfd);	// close listening socket
            printf("new client: %s, port %d, id %d\n",
                    inet_ntop(AF_INET, &cliaddr.sin_addr, cliip, 16),
                    ntohs(cliaddr.sin_port), getpid());
            for (;;) {
                n = read(sockfd, buf, MAXLINE);
                if (n == 0) {  // connection closed by client.
                    printf("client %d quit\n", getpid());
                    if (close(sockfd) == -1) {
                        fprintf(stderr, "close error\n");
                        exit(EXIT_FAILURE);
                    }
                    exit(EXIT_SUCCESS);
                } else if (n < 0 && errno == EINTR) { // interrupted by a signal.
                    fprintf(stderr, "interrupted try again\n");
                    continue;
                } else if (n < 0) {
                    fprintf(stderr, "read error\n");
                    exit(EXIT_FAILURE);
                } else {
                    fprintf(stdout, "cli(%d):", getpid()); fflush(stdout);
                    if (writen(1, buf, n) != n) {
                        fprintf(stderr, "writen error\n");
                        exit(EXIT_FAILURE);
                    }

                    if (writen(sockfd, buf, n) != n) {
                        fprintf(stderr, "writen error\n");
                        exit(EXIT_FAILURE);
                    }
                }
            }
			exit(EXIT_SUCCESS);
		}
		close(sockfd);  // parent closes connected socket
	}

	exit(EXIT_SUCCESS);
}
