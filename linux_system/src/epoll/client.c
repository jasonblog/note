#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <netdb.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/socket.h>

#define MAXDATASIZE 100 // max number of bytes we can get at once


int main(int argc, char* argv[]) {
    int sockfd, numbytes;
    char buf[MAXDATASIZE];
    struct hostent* he;
    struct sockaddr_in their_addr; // connector'saddress information

    if ((argc == 1) || (argc == 2)) {
        fprintf(stderr, "usage: client hostname\nEx:\n$./client01 ip port\n");
        exit(1);
    }

    if ((he = gethostbyname(argv[1])) == NULL) { // get the host info
        herror("gethostbyname");
        exit(1);
    }

    if ((sockfd = socket(PF_INET, SOCK_STREAM, 0)) == -1) {
        perror("socket");
        exit(1);
    }

    their_addr.sin_family = AF_INET; // host byte order
    their_addr.sin_port = htons(atoi(argv[2])); // short, network byte order
    their_addr.sin_addr = *((struct in_addr*)he->h_addr);
    memset(their_addr.sin_zero, '\0', sizeof their_addr.sin_zero);

    if (connect(sockfd, (struct sockaddr*)&their_addr,
                sizeof their_addr) == -1) {
        perror("connect");
        exit(1);
    }

    while (1) {
        if (send(sockfd, "hello, this is client message!", strlen("hello, this is client message!"), 0) == -1) {
            perror("send");
        }

        if ((numbytes = recv(sockfd, buf, MAXDATASIZE - 1, 0)) == -1) {
            perror("recv");
        } else if (numbytes == 0) {
            printf("Remote server has shutdown!\n");
            break;
        }

        buf[numbytes] = '\0';
        printf("Received: %s \n", buf);
        sleep(1);
    }

    close(sockfd);
    return 0;
}
