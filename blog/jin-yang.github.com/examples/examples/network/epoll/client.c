#include <stdio.h>
#include <fcntl.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <unistd.h> // read()
#include <sys/socket.h>
#include <netinet/in.h> // sockaddr_in
#include <sys/epoll.h>
#include <arpa/inet.h> // inet_pton()
#include "common.h"
//Ctrl-D quit


static int 		read_cnt;
static char 	*read_ptr;
static char		read_buf[4096];
void str_cli(FILE *fp, int sockfd);

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

int main(int argc, char **argv)
{
	int					sockfd, ret;
	struct sockaddr_in	servaddr;

	// Apply for TCP socket.
	if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
		perror("socket");
		exit(EXIT_FAILURE);
	}

	// Set the socket.
	memset(&servaddr, 0, sizeof(servaddr));
	servaddr.sin_family = AF_INET;
	servaddr.sin_port   = htons(LISTEN_PORT);	// echo server

	// inet_pton(presentation-numeric) is available to ipv4/6.
	// AF-address family. PF-protocol family.
	ret = inet_pton(AF_INET, "127.0.0.1", &servaddr.sin_addr); // ipv4 or AF_INET6
	if (ret <= 0) {
		if (ret == 0)
			fprintf(stderr, "Not in presentation format.\n");
		else
			perror("inet_pton");
		close(sockfd);
		exit(EXIT_FAILURE);
	}

    printf("connecting to 127.1, port: %d\n", LISTEN_PORT);

	// Try to connet to the server.
	if (connect(sockfd, (struct sockaddr *)&servaddr, sizeof(servaddr)) < 0) {
		perror("connect");
		exit(EXIT_FAILURE);
	}

	str_cli(stdin, sockfd);

	exit(EXIT_SUCCESS);
}

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

static ssize_t my_read(int fd, char *ptr)
{
	if (read_cnt <= 0) {
again:
		if ((read_cnt = read(fd, read_buf, sizeof(read_buf))) < 0) {
			if (errno == EINTR)
				goto again;
			return -1;
		} else if (read_cnt == 0)
			return 0;
		read_ptr = read_buf;
	}

	read_cnt--;
	*ptr = *read_ptr++;
	return 1;
}

ssize_t readline(int fd, void *vptr, size_t maxlen)
{
	ssize_t 	n, rc;
	char		c, *ptr;

	ptr = vptr;
	for (n = 1; n < maxlen; n++) {
		if ((rc = my_read(fd, &c)) == 1) {
			*ptr++ = c;
			if (c == '\n')
				break;      // newline is stored, like fgets()
		} else if (rc == 0) {
			*ptr = '\0';
			return(n-1);    // EOF, n-1 bytes were read
		} else
			return -1;      // error, errno set by read()
	}

	*ptr = '\0';            // null terminate like fgets()
	return n;
}

#define MAX(a, b)	((a) > (b)? (a): (b))
void str_cli(FILE *fp, int sockfd)
{
	char 	sendline[MAXLINE], recvline[MAXLINE];
	int		ret, n, i;
	size_t	len;

    int                 efd;
    struct epoll_event  event;
    struct epoll_event  events[2];


    // Same with epoll_create(), except the argument size is ignored.
    if ((efd = epoll_create1(0)) == -1) {
        perror ("epoll_create");
		exit(EXIT_FAILURE);
    }

    if(make_socket_non_blocking (fileno(stdin)) == -1) exit(EXIT_FAILURE);
    if(make_socket_non_blocking (sockfd) == -1) exit(EXIT_FAILURE);

    event.data.fd = fileno(stdin);   // stdin
    event.events = EPOLLIN;// | EPOLLET;
    if (epoll_ctl(efd, EPOLL_CTL_ADD, fileno(stdin), &event) == -1) {
        perror("epoll_ctl");
		exit(EXIT_FAILURE);
    }
    event.data.fd = sockfd;   // stdin
    event.events = EPOLLIN;// | EPOLLET;
    if (epoll_ctl(efd, EPOLL_CTL_ADD, sockfd, &event) == -1) {
        perror("epoll_ctl");
		exit(EXIT_FAILURE);
    }

	for (;;) {
        n = epoll_wait (efd, events, 2, -1);  // only stdin+sockfd
        for (i = 0; i < n; i++) {
            if ((events[i].events & EPOLLERR) || (events[i].events & EPOLLHUP) || (!(events[i].events & EPOLLIN))) {
                /* An error has occured on this fd, or the socket is not ready for reading (why were we notified then?) */
                fprintf (stderr, "epoll error\n");
                close (events[i].data.fd);
                exit(EXIT_FAILURE);
            }

            if (sockfd == events[i].data.fd) {   // socket is readable.
                ret = readline(sockfd, recvline, MAXLINE);
                if (ret == 0) {
                    fprintf(stderr, "str_cli: server terminated prematurely\n");
                    exit(EXIT_FAILURE);
                } else if (ret < 0) {
                    fprintf(stderr, "str_cli error\n");
                    exit(EXIT_FAILURE);
                }

                fputs("socket: ", stdout);
                if (fputs(recvline, stdout) == EOF) {
                    fprintf(stderr, "fputs error\n");
                    exit(EXIT_FAILURE);
                }
            }

            if (fileno(stdin) == events[i].data.fd) {   // input is readable.
                if (fgets(sendline, MAXLINE, fp) == NULL) {
                    if (ferror(fp)) {           // stream errors.
                        fprintf(stderr, "fgets error\n");
                        exit(EXIT_FAILURE);
                    } else {                    // end of file.
                        return;
                    }
                }
                fputs("stdin : ", stdout);
                if (fputs(sendline, stdout) == EOF) {
                    fprintf(stderr, "fputs error\n");
                    exit(EXIT_FAILURE);
                }

                len = strlen(sendline);
                if (writen(sockfd, sendline, len) != len) {
                    fprintf(stderr, "writen error\n");
                    exit(EXIT_FAILURE);
                }
            }
        }
    }
}
