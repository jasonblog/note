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
#include <sys/time.h>
#include <sys/types.h>

#define MAXBUF 1024

int main(int argc, char** argv)
{
    int sockfd, new_fd;
    socklen_t len;
    struct sockaddr_in my_addr, their_addr;
    unsigned int myport, lisnum;
    char buf[MAXBUF + 1];
    fd_set rfds;
    struct timeval tv;
    int retval, maxfd = -1;

    if (argv[2]) {
        myport = atoi(argv[2]);
    } else {
        myport = 7838;
    }

    if (argv[3]) {
        lisnum = atoi(argv[3]);
    } else {
        lisnum = 2;
    }

    if ((sockfd = socket(PF_INET, SOCK_STREAM, 0)) == -1) {
        perror("socket");
        exit(EXIT_FAILURE);
    }

    bzero(&my_addr, sizeof(my_addr));
    my_addr.sin_family = PF_INET;
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

    while (1) {
        printf("\n----wait for new connect\n");
        len = sizeof(struct sockaddr);

        if ((new_fd = accept(sockfd, (struct sockaddr*) &their_addr, &len)) == -1) {
            perror("accept");
            exit(errno);
        } else
            printf("server: got connection from %s, port %d, socket %d\n",
                   inet_ntoa(their_addr.sin_addr), ntohs(their_addr.sin_port), new_fd);

        while (1) {
            FD_ZERO(&rfds);
            FD_SET(0, &rfds);
            FD_SET(new_fd, &rfds);
            maxfd = new_fd;
            tv.tv_sec = 1;
            tv.tv_usec = 0;
            retval = select(maxfd + 1, &rfds, NULL, NULL, &tv);

            if (retval == -1) {
                perror("select");
                exit(EXIT_FAILURE);
            } else if (retval == 0) {
                continue;
            } else {
                if (FD_ISSET(0, &rfds)) {
                    bzero(buf, MAXBUF + 1);
                    fgets(buf, MAXBUF, stdin);

                    if (!strncasecmp(buf, "quit", 4)) {
                        printf("i will quit!\n");
                        break;
                    }

                    len = send(new_fd, buf, strlen(buf) - 1, 0);

                    if (len > 0) {
                        printf("send successful,%d byte send!\n", len);
                    } else {
                        printf("send failure!");
                        break;
                    }
                }

                if (FD_ISSET(new_fd, &rfds)) {
                    bzero(buf, MAXBUF + 1);
                    len = recv(new_fd, buf, MAXBUF, 0);

                    if (len > 0) {
                        printf("recv success :'%s',%dbyte recv\n", buf, len);
                    } else {
                        if (len < 0) {
                            printf("recv failure\n");
                        } else {
                            printf("the ohter one end ,quit\n");
                            break;
                        }
                    }
                }
            }
        }

        close(new_fd);
        printf("need othe connecdt (no->quit)");
        fflush(stdout);
        bzero(buf, MAXBUF + 1);
        fgets(buf, MAXBUF, stdin);

        if (!strncasecmp(buf, "no", 2)) {
            printf("quit!\n");
            break;
        }
    }

    close(sockfd);
    return 0;
}

