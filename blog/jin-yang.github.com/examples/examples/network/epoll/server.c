/* gcc -Wall epoll_examples.c -o example
 * ./example 2345
 * telnet 127.0.0.1 2345
 * NOTE: Telnet should be killed when you try to abort.
 */
#include <fcntl.h>
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
#include <limits.h>     /* for OPEN_MAX */
#include <sys/epoll.h>
#include <sys/types.h>
#include <netdb.h>
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
		if ( (nwritten = write(fd, ptr, nleft)) <= 0) {
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

static int make_socket_non_blocking (int sfd)
{
  int flags, s;

  flags = fcntl (sfd, F_GETFL, 0); // Get the flag.
  if (flags == -1) {
     perror ("fcntl");
     return -1;
  }

  flags |= O_NONBLOCK;
  s = fcntl (sfd, F_SETFL, flags); // And set it.
  if (s == -1) {
     perror ("fcntl");
     return -1;
  }

  return 0;
}


int main (int argc, char *argv[])
{
	int					listenfd, sockfd, opt = 1;
    char                cliip[16];
	socklen_t			clilen;
	struct sockaddr_in	cliaddr, servaddr;
    ssize_t             n;
    char                buf[MAXLINE];

    int                 i, done, efd;
    struct epoll_event  event;
    struct epoll_event* events;

    // Buffer where events are returned
    events = calloc (MAXEVENTS, sizeof event);

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

    if (make_socket_non_blocking (listenfd) == -1) {
		exit(EXIT_FAILURE);
    }

	if (listen(listenfd, 5) < 0) {
		perror("listen");
		exit(EXIT_FAILURE);
	}
    printf("Listening on port %d, epoll version\n", LISTEN_PORT);

    // Same with epoll_create(), except the argument size is ignored.
    if ((efd = epoll_create1(0)) == -1) {
        perror ("epoll_create");
		exit(EXIT_FAILURE);
    }

    event.data.fd = listenfd;
    event.events = EPOLLIN;// | EPOLLET;
    if (epoll_ctl(efd, EPOLL_CTL_ADD, listenfd, &event) == -1) {
        perror("epoll_ctl");
		exit(EXIT_FAILURE);
    }

    // The event loop
    while (1) {
        n = epoll_wait (efd, events, MAXEVENTS, -1);
        for (i = 0; i < n; i++) {
            if ((events[i].events & EPOLLERR) || (events[i].events & EPOLLHUP) || (!(events[i].events & EPOLLIN))) {
                /* An error has occured on this fd, or the socket is not ready for reading (why were we notified then?) */
                fprintf (stderr, "epoll error\n");
                close (events[i].data.fd);
                continue;
            } else if (listenfd == events[i].data.fd) {
                /* We have a notification on the listening socket, which means one or more incoming connections. */
                while (1) {
                    clilen = sizeof cliaddr;
                    if((sockfd = accept (listenfd, (struct sockaddr *)&cliaddr, &clilen)) < 0) {
                        if ((errno == EAGAIN) || (errno == EWOULDBLOCK)) {
                            break;                   // we have processed all incoming connections.
                        } else if (errno == EINTR) { // interrupt by the signal.
                            continue;
                        } else {
                            perror ("accept");
                            exit(EXIT_FAILURE);
                        }
                    }

                    printf("new client: %s, port %d\n",
                        inet_ntop(AF_INET, &cliaddr.sin_addr, cliip, 16), ntohs(cliaddr.sin_port));

                    /* Make the incoming socket non-blocking and add it to the list of fds to monitor. */
                    if(make_socket_non_blocking (sockfd) == -1) exit(EXIT_FAILURE);

                    event.data.fd = sockfd;
                    event.events = EPOLLIN; // | EPOLLET;
                    if(epoll_ctl (efd, EPOLL_CTL_ADD, sockfd, &event) == -1) {
                        perror ("epoll_ctl");
                        exit(EXIT_FAILURE);
                    }
                }
                continue;
            } else {
                /* We have data on the fd waiting to be read. Read and
                   display it. We must read whatever data is available
                   completely, as we are running in edge-triggered mode
                   and won't get a notification again for the same data. */
                done = 0;
                while (1) {
                    n = read (events[i].data.fd, buf, sizeof buf);
                    if (n == 0) { /* End of file. The remote has closed the connection. */
                        done = 1;
                        break;
                    } else if (n < 0) {
                        if (errno == ECONNRESET) { /* connection reset by client */
                            printf("client[%d] aborted connection\n", i);
                            close (events[i].data.fd);
                        } else if (errno == EAGAIN) {
                            /* If errno == EAGAIN, that means we have read all
                               data. So go back to the main loop. */
                            break;
                        } else {
                            perror ("read");
                            exit(EXIT_FAILURE);
                        }
                    } else { /* Write the buffer to standard output */
                        fprintf(stdout, "cli(%d):", events[i].data.fd); fflush(stdout);
                        if (writen(1, buf, n) != n) { // write to screen
                            fprintf(stderr, "writen error\n");
                            exit(EXIT_FAILURE);
                        }
                        if (writen(events[i].data.fd, buf, n) != n) { // write back
                            fprintf(stderr, "writen error\n");
                            exit(EXIT_FAILURE);
                        }
                    }
                }

                if (done) {
                    printf ("Closed connection on descriptor %d\n", events[i].data.fd);

                    /* Closing the descriptor will make epoll remove it
                       from the set of descriptors which are monitored. */
                    close (events[i].data.fd);
                }
            }
        }
    }

    free (events);
    close (listenfd);
    return EXIT_SUCCESS;
}

#if 0
static int create_and_bind (char *port)
{
    struct addrinfo hints;
    struct addrinfo *result, *rp;
    int s, sfd;

    memset (&hints, 0, sizeof (struct addrinfo));
    hints.ai_family = AF_UNSPEC;     // Return IPv4 and IPv6 choices
    hints.ai_socktype = SOCK_STREAM; // We want a TCP socket
    hints.ai_flags = AI_PASSIVE;     // All interfaces

    s = getaddrinfo (NULL, port, &hints, &result);
    if (s != 0) {
        fprintf (stderr, "getaddrinfo: %s\n", gai_strerror (s));
        return -1;
    }

    for (rp = result; rp != NULL; rp = rp->ai_next) {
        sfd = socket (rp->ai_family, rp->ai_socktype, rp->ai_protocol);
        if (sfd == -1) continue;

        s = bind (sfd, rp->ai_addr, rp->ai_addrlen);
        if (s == 0) break; // We managed to bind successfully!

        close (sfd);
    }

    if (rp == NULL) {
        fprintf (stderr, "Could not bind\n");
        return -1;
    }

    freeaddrinfo (result);
    return sfd;
}
#endif
