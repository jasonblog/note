#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/wait.h>
#include <unistd.h>
#include <arpa/inet.h>

#define MAXBUF 1024

int main(int argc, char* argv[])
{
    int pid;
    int sockfd, new_fd;
    socklen_t len;
    struct sockaddr_in my_addr, their_addr;
    unsigned int myport, lisnum;
    char buf[MAXBUF + 1];

    if (argv[2]) {
        myport = atoi(argv[2]);
    } else {
        myport = 7575;
    }

    if (argv[3]) {
        lisnum = atoi(argv[3]);
    } else {
        lisnum = 5;
    }

    if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
        perror("socket");
        exit(EXIT_FAILURE);
    }

    bzero(&my_addr, sizeof(my_addr));
    my_addr.sin_family = AF_INET;
    my_addr.sin_port = htons(myport);

    if (argv[1]) {
        my_addr.sin_addr.s_addr = inet_addr(argv[1]);
    } else {
        my_addr.sin_addr.s_addr = INADDR_ANY;
    }

    if (bind(sockfd, (struct sockaddr*) &my_addr, sizeof(struct sockaddr)) == -1) {
        perror("bind");
        exit(EXIT_FAILURE);
    }

    if (listen(sockfd, lisnum) == -1) {
        perror("listen");
        exit(EXIT_FAILURE);
    }

    printf("wait for connect\n");
    len = sizeof(struct sockaddr);

    if ((new_fd = accept(sockfd, (struct sockaddr*) &their_addr, &len)) == -1) {
        perror("accept");
        exit(EXIT_FAILURE);
    } else {
        printf("server: got connection from %s, port %d, socket %d\n",
               inet_ntoa(their_addr.sin_addr), ntohs(their_addr.sin_port), new_fd);
    }

    if (-1 == (pid = fork())) {
        perror("fork");
        exit(EXIT_FAILURE);
    } else if (pid == 0) {
        while (1) {
            bzero(buf, MAXBUF + 1);
            printf("waiting for message:\n");
            fgets(buf, MAXBUF, stdin);

            if (!strncasecmp(buf, "quit", 4)) {
                printf("i will close the connect!\n");
                break;
            }

            len = send(new_fd, buf, strlen(buf) - 1, 0);

            if (len < 0) {
                printf("message'%s' send failure!errno code is %d,errno message is '%s'\n",
                       buf, errno, strerror(errno));
                break;
            }
        }
    } else {
        while (1) {
            bzero(buf, MAXBUF + 1);
            len = recv(new_fd, buf, MAXBUF, 0);

            if (len > 0) {
                printf("message recv successful :'%s',%dByte recv\n", buf, len);
            } else if (len < 0) {
                printf("recv failure!errno code is %d,errno message is '%s'\n",
                       errno, strerror(errno));
                break;
            } else {
                printf("the other one close quit\n");
                break;
            }
        }
    }

    close(new_fd);
    close(sockfd);
    return 0;
}
