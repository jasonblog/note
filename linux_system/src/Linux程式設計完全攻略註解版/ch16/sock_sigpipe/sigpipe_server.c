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

void handler(int sig)
{
    printf("sig=%d\n", sig);
}
int main(int argc, char* argv[])
{
    int pid, bytes;
    int sockfd, new_fd;
    socklen_t len;
    struct sockaddr_in my_addr, their_addr;
    char buffer[128];

    signal(SIGPIPE, handler);

    if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
        perror("socket");
        exit(EXIT_FAILURE);
    }

    bzero(&my_addr, sizeof(my_addr));
    my_addr.sin_family = AF_INET;
    my_addr.sin_port = htons(atoi(argv[2]));

    if (inet_aton(argv[1], (struct in_addr*) &my_addr.sin_addr.s_addr) == 0) {
        perror(argv[1]);
        exit(errno);
    }

    if (bind(sockfd, (struct sockaddr*) &my_addr, sizeof(struct sockaddr)) == -1) {
        perror("bind");
        exit(EXIT_FAILURE);
    }

    if (listen(sockfd, 5) == -1) {
        perror("listen");
        exit(EXIT_FAILURE);
    }

    len = sizeof(struct sockaddr);

    while (1) {
        printf("wait for new connect\n");

        if ((new_fd = accept(sockfd, (struct sockaddr*) &their_addr, &len)) == -1) {
            perror("accept");
            exit(EXIT_FAILURE);
        } else {
            printf("server: got connection from %s, port %d\n",
                   inet_ntoa(their_addr.sin_addr), ntohs(their_addr.sin_port));

            while (1) {
                bzero(buffer, 128);
                printf("pls send message to send:");
                fgets(buffer, 128, stdin);

                if (!strncasecmp(buffer, "quit", 4)) {
                    printf(" i will quit!\n");
                    break;
                }

                bytes = write(new_fd, buffer, strlen(buffer));

                if (bytes < 0) {
                    printf("send failure,errno code is %d,errno message is '%s'\n", errno,
                           strerror(errno));
                    close(new_fd);
                    break;
                } else {
                    printf("message:%s\tsend successful,%dbyte send!\n", buffer, len);
                }
            }
        }
    }

    close(sockfd);
    return 0;
}

