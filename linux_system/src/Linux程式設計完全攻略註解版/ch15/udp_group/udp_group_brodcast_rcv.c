#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <netdb.h>
#include <errno.h>

#define BUFLEN 255

int main(int argc, char** argv)
{
    struct sockaddr_in peeraddr;
    struct in_addr ia;
    int sockfd;
    char recmsg[BUFLEN + 1];
    unsigned int socklen, n;
    struct hostent* group;
    struct ip_mreq mreq;

    sockfd = socket(AF_INET, SOCK_DGRAM, 0);

    if (sockfd < 0) {
        printf("socket creating err in udptalk\n");
        exit(EXIT_FAILURE);
    }


    bzero(&mreq, sizeof(struct ip_mreq));

    if (argv[1]) {
        if ((group = gethostbyname(argv[1])) == (struct hostent*) 0) {
            perror("gethostbyname");
            exit(EXIT_FAILURE);
        }
    } else {
        printf("you should give me a group address, 224.0.0.0-239.255.255.255\n");
        exit(EXIT_FAILURE);
    }

    bcopy((void*) group->h_addr, (void*) &ia, group->h_length);

    bcopy(&ia, &mreq.imr_multiaddr.s_addr, sizeof(struct in_addr));


    //mreq.imr_interface.s_addr = htonl(INADDR_ANY);
    if (argv[2]) {
        if (inet_pton(AF_INET, argv[2], &mreq.imr_interface.s_addr) <= 0) {
            printf("Wrong dest IP address!\n");
            exit(EXIT_FAILURE);
        }
    }


    if (setsockopt(sockfd, IPPROTO_IP, IP_ADD_MEMBERSHIP, &mreq,
                   sizeof(struct ip_mreq)) == -1) {
        perror("setsockopt");
        exit(EXIT_FAILURE);
    }

    socklen = sizeof(struct sockaddr_in);
    memset(&peeraddr, 0, socklen);
    peeraddr.sin_family = AF_INET;
    peeraddr.sin_port = htons(7838);

    if (argv[1]) {
        if (inet_pton(AF_INET, argv[1], &peeraddr.sin_addr) <= 0) {
            printf("Wrong dest IP address!\n");
            exit(EXIT_FAILURE);
        }
    } else {
        printf("no group address given, 224.0.0.0-239.255.255.255\n");
        exit(EXIT_FAILURE);
    }


    if (bind(sockfd, (struct sockaddr*) &peeraddr,
             sizeof(struct sockaddr_in)) == -1) {
        printf("Bind error\n");
        exit(EXIT_FAILURE);
    }


    for (;;) {
        bzero(recmsg, BUFLEN + 1);
        n = recvfrom(sockfd, recmsg, BUFLEN, 0, (struct sockaddr*) &peeraddr, &socklen);

        if (n < 0) {
            printf("recvfrom err in udptalk!\n");
            exit(EXIT_FAILURE);
        } else {

            recmsg[n] = 0;
            printf("peer:%s", recmsg);
        }
    }
}
