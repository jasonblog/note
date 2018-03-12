#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <unistd.h> // read()
#include <sys/socket.h>
#include <netinet/in.h> // sockaddr_in
#include <arpa/inet.h> // inet_pton()
#include <poll.h> /* poll function */
#include "common.h"
//Ctrl-D quit


static int 		read_cnt;
static char 	*read_ptr;
static char		read_buf[4096];
void str_cli(FILE *fp, int sockfd);

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
	fd_set	rset;
	int	            	ret, maxfdp1;
	size_t          	len;
	struct pollfd		client[2];

	client[0].fd = fileno(stdin);
	client[0].events = POLLRDNORM;  // POLLIN
	client[1].fd = sockfd;
	client[1].events = POLLRDNORM;  // POLLIN

	for (;;) {
		if(poll(client, 2, -1) < 0) { // INFTIM = -1
			perror("poll");
			exit(EXIT_FAILURE);
		}



		FD_SET(fileno(fp), &rset);          // this must set every time.
		FD_SET(sockfd, &rset);
		maxfdp1 = MAX(fileno(fp), sockfd) + 1;
		if (select(maxfdp1, &rset, NULL, NULL, NULL) < 0) {
			fprintf(stderr, "select error\n");
			exit(EXIT_FAILURE);
		}

		if (client[1].revents & POLLRDNORM) {	// new client connection
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

		if (client[0].revents & POLLRDNORM) {	// new client connection
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
