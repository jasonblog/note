#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define BUFLEN 255
int main(int argc, char** argv)
{
    struct sockaddr_in peeraddr, myaddr;
    int sockfd;
    char recmsg[BUFLEN + 1];
    unsigned int socklen;
    sockfd = socket(AF_INET, SOCK_DGRAM, 0);

    if (sockfd < 0) {
        printf("socket creating error\n");
        exit(EXIT_FAILURE);
    }

    socklen = sizeof(struct sockaddr_in);
    memset(&peeraddr, 0, socklen);
    peeraddr.sin_family = AF_INET;
    peeraddr.sin_port = htons(7838);

    if (argv[1]) {
        if (inet_pton(AF_INET, argv[1], &peeraddr.sin_addr) <= 0) {
            printf("wrong group address!\n");
            exit(EXIT_FAILURE);
        }
    } else {
        printf("no group address!\n");
        exit(EXIT_FAILURE);
    }

    memset(&myaddr, 0, socklen);
    myaddr.sin_family = AF_INET;
    myaddr.sin_port = htons(23456);

    if (argv[2]) {
        if (inet_pton(AF_INET, argv[2], &myaddr.sin_addr) <= 0) {
            printf("self ip address error!\n");
            exit(EXIT_FAILURE);
        }
    } else {
        myaddr.sin_addr.s_addr = INADDR_ANY;
    }

    if (bind(sockfd, (struct sockaddr*) &myaddr,
             sizeof(struct sockaddr_in)) == -1) {
        printf("Bind error\n");
        exit(EXIT_FAILURE);
    }

    for (;;) {
        bzero(recmsg, BUFLEN + 1);
        printf("input message to send:");

        if (fgets(recmsg, BUFLEN, stdin) == (char*) EOF) {
            exit(EXIT_FAILURE);
        };

        if (sendto(sockfd, recmsg, strlen(recmsg), 0, (struct sockaddr*) &peeraddr,
                   sizeof(struct sockaddr_in)) < 0) {
            printf("sendto error!\n");
            exit(EXIT_FAILURE);;
        }

        printf("sned message:%s", recmsg);
    }
}
