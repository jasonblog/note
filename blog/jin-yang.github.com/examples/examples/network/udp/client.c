#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <unistd.h> // read()
#include <sys/socket.h>
#include <netinet/in.h> // sockaddr_in
#include <arpa/inet.h> // inet_pton()
#include "common.h"

// recvfrom() return 0 is normal.
int main(int argc, char **argv)
{
	int		n;
	size_t	len;
	char	sendline[MAXLINE], recvline[MAXLINE + 1];

	int					ret, sockfd;
    struct sockaddr_in	servaddr;


	// UDP socket.
	if ((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
		perror("socket");
		exit(EXIT_FAILURE);
	}

	// Set the socket.
	memset(&servaddr, 0, sizeof(servaddr));
	servaddr.sin_family = AF_INET;
	servaddr.sin_port   = htons(8080);	// daytime server
	// inet_pton(presentation-numeric) is available to ipv4/6.
	// AF-address family. PF-protocol family.
	ret = inet_pton(AF_INET, "127.0.0.1", &servaddr.sin_addr); // ipv4
	if (ret <= 0) {
		if (ret == 0)
            fprintf(stderr, "Not in presentation format.\n");
		else
			perror("inet_pton");
		exit(EXIT_FAILURE);
	}

	while (fgets(sendline, MAXLINE, stdin) != NULL) {
        fputs("stdin : ", stdout);
		if (fputs(sendline, stdout) == EOF) {
			fprintf(stderr, "fputs error\n");
			exit(EXIT_FAILURE);
		}

		len = strlen(sendline);
		if (sendto(sockfd, sendline, len, 0, (struct sockaddr *)&servaddr, sizeof(servaddr)) != len) {
			fprintf(stderr, "sendto error\n");
			exit(EXIT_FAILURE);
		}

		if ((n = recvfrom(sockfd, recvline, MAXLINE, 0, NULL, NULL)) < 0) {
			fprintf(stderr, "recvfrom error\n");
			exit(EXIT_FAILURE);
		}
		recvline[n] = '\0'; // null terminate.

        fputs("socket: ", stdout);
		if (fputs(recvline, stdout) == EOF) {
			fprintf(stderr, "fputs error\n");
			exit(EXIT_FAILURE);
		}
	}
	return 0;
}
