#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <sys/socket.h>
#include <resolv.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <sys/time.h>
#include <sys/types.h>

#define MAXBUF 1024
int main(int argc, char** argv)
{
    int sockfd, len;
    struct sockaddr_in dest;
    char buffer[MAXBUF + 1];
    fd_set rfds;
    struct timeval tv;
    int retval, maxfd = -1;

    if (argc != 3) {
        printf("argv format errno,pls:\n\t\t%s IP port\n", argv[0], argv[0]);
        exit(EXIT_FAILURE);
    }

    if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        perror("Socket");
        exit(EXIT_FAILURE);
    }

    bzero(&dest, sizeof(dest));
    dest.sin_family = AF_INET;
    dest.sin_port = htons(atoi(argv[2]));

    if (inet_aton(argv[1], (struct in_addr*) &dest.sin_addr.s_addr) == 0) {
        perror(argv[1]);
        exit(EXIT_FAILURE);
    }

    if (connect(sockfd, (struct sockaddr*) &dest, sizeof(dest)) != 0) {
        perror("Connect ");
        exit(EXIT_FAILURE);
    }

    printf("\nget ready pls chat\n");

    while (1) {
        FD_ZERO(&rfds);
        FD_SET(0, &rfds);
        FD_SET(sockfd, &rfds);
        maxfd = sockfd;
        tv.tv_sec = 1;
        tv.tv_usec = 0;
        retval = select(maxfd + 1, &rfds, NULL, NULL, &tv);

        if (retval == -1) {
            printf("select %s", strerror(errno));
            break;
        } else if (retval == 0) {
            continue;
        } else {
            if (FD_ISSET(sockfd, &rfds)) {
                bzero(buffer, MAXBUF + 1);
                len = recv(sockfd, buffer, MAXBUF, 0);

                if (len > 0) {
                    printf("recv message:'%s',%d byte recv\n", buffer, len);
                } else {
                    if (len < 0) {
                        printf("message recv failure\n");
                    } else {
                        printf("the othe quit ,quit\n");
                        break;
                    }
                }
            }

            if (FD_ISSET(0, &rfds)) {
                bzero(buffer, MAXBUF + 1);
                fgets(buffer, MAXBUF, stdin);

                if (!strncasecmp(buffer, "quit", 4)) {
                    printf("i will quit\n");
                    break;
                }

                len = send(sockfd, buffer, strlen(buffer) - 1, 0);

                if (len < 0) {
                    printf("message send failure");
                    break;
                } else
                    printf
                    ("send success,%d byte send\n", len);
            }
        }
    }

    close(sockfd);
    return 0;
}
