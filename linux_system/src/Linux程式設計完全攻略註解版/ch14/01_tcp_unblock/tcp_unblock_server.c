#include <sys/types.h>
#include <sys/socket.h>
#include <stdio.h>
#include <string.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <stdlib.h>
#include <errno.h>
#define  BUFSIZE 128
int main(int argc, char* argv[])
{
    int server_sockfd, client_sockfd;
    int server_len, client_len;
    struct sockaddr_in server_address;
    struct sockaddr_in client_address;
    int i, byte;
    char char_send[BUFSIZE];

    server_sockfd = socket(AF_INET, SOCK_STREAM, 0);
    server_address.sin_family = AF_INET;

    if (inet_aton(argv[1], (struct in_addr*) & server_address.sin_addr.s_addr) ==
        0) {
        perror(argv[1]);
        exit(EXIT_FAILURE);
    }

    server_address.sin_port = htons(7838);
    server_len = sizeof(server_address);

    bind(server_sockfd, (struct sockaddr*)&server_address, server_len);

    listen(server_sockfd, 5);

    printf("server waiting for connect\n");

    client_len = sizeof(client_address);
    client_sockfd = accept(server_sockfd, (struct sockaddr*)&client_address,
                           (socklen_t*)&client_len);

    for (i = 0; i < 5; i++) {
        memset(char_send, '\0', BUFSIZE);
        printf("input message to send:");
        fgets(char_send, BUFSIZE, stdin);

        if ((byte = send(client_sockfd, char_send, strlen(char_send), 0)) == -1) {
            perror("send");
            exit(EXIT_FAILURE);
        }

        memset(char_send, '\0', BUFSIZE);
        byte = recv(client_sockfd, char_send, BUFSIZE, MSG_DONTWAIT);

        if (byte > 0) {
            printf("get %d message:%s", byte, char_send);
            byte = 0;
        } else if (byte < 0) {
            if (errno == EAGAIN) {
                errno = 0;
                continue;
            } else {
                perror("recv");
                exit(EXIT_FAILURE);
            }
        }
    }

    shutdown(client_sockfd, 2);
    shutdown(server_sockfd, 2);
}

