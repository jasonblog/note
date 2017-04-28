#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <sys/socket.h>
#include <resolv.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <fcntl.h>

#define MAXBUF 128

int main(int argc, char** argv)
{
    int sockfd, ret, i;
    struct sockaddr_in dest, mine;
    char buffer[MAXBUF + 1];

    if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        perror("Socket");
        exit(EXIT_FAILURE);
    }

    bzero(&dest, sizeof(dest));
    dest.sin_family = AF_INET;
    dest.sin_port = htons(7838);

    if (inet_aton(argv[1], (struct in_addr*) &dest.sin_addr.s_addr) == 0) {
        perror(argv[1]);
        exit(EXIT_FAILURE);
    }

    bzero(&mine, sizeof(mine));
    mine.sin_family = AF_INET;
    mine.sin_port = htons(7839);

    if (inet_aton(argv[2], (struct in_addr*) &mine.sin_addr.s_addr) == 0) {
        perror(argv[2]);
        exit(EXIT_FAILURE);
    }

    if (bind(sockfd, (struct sockaddr*) &mine, sizeof(struct sockaddr)) == -1) {
        perror(argv[3]);
        exit(EXIT_FAILURE);
    }

    if (connect(sockfd, (struct sockaddr*) &dest, sizeof(dest)) != 0) {
        perror("Connect ");
        exit(EXIT_FAILURE);
    }

    if (fcntl(sockfd, F_SETFL, O_NONBLOCK) == -1) {
        perror("fcntl");
        exit(EXIT_FAILURE);
    }

    while (1) {
        bzero(buffer, MAXBUF + 1);
        ret = recv(sockfd, buffer, MAXBUF, 0);

        if (ret > 0) {
            printf("get %d message:%s", ret, buffer);
            ret = 0;
        } else if (ret < 0) {
            if (errno == EAGAIN) {
                errno = 0;
                continue;
            } else {
                perror("recv");
                exit(EXIT_FAILURE);
            }
        }

        memset(buffer, '\0', MAXBUF + 1);
        printf("input message to send:");
        fgets(buffer, MAXBUF, stdin);

        if ((ret = send(sockfd, buffer, strlen(buffer), 0)) == -1) {
            perror("send");
            exit(EXIT_FAILURE);
        }

    }

    close(sockfd);
    return 0;
}
