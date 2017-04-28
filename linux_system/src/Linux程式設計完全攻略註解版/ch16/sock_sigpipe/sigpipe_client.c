#include <stdio.h>
#include <signal.h>
#include <string.h>
#include <errno.h>
#include <sys/socket.h>
#include <resolv.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>


int main(int argc, char** argv)
{
    int sockfd, len;
    struct sockaddr_in dest;
    char buf[128];

    if (argc != 3) {
        printf(" error format,it must be:\n\t\t%s IP port\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        perror("Socket");
        exit(errno);
    }

    printf("socket created\n");


    bzero(&dest, sizeof(dest));
    dest.sin_family = AF_INET;
    dest.sin_port = htons(atoi(argv[2]));

    if (inet_aton(argv[1], (struct in_addr*) &dest.sin_addr.s_addr) == 0) {
        perror(argv[1]);
        exit(errno);
    }

    if (connect(sockfd, (struct sockaddr*) &dest, sizeof(dest)) == -1) {
        perror("Connect ");
        exit(errno);
    }

    memset(buf, '\0', 128);
    len = recv(sockfd, buf, 128, 0);
    printf("buf=%s\n", buf);
    close(sockfd);
    return 0;
}
