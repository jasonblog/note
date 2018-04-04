#include <sys/types.h>
#include <sys/socket.h>
#include <stdio.h>
#include <sys/un.h>
#include <unistd.h>
#include <stdlib.h>
int main(int argc, char* argv[])
{
    int sockfd;
    int len;
    struct sockaddr_un address;
    int result;
    int i, byte;
    char ch_recv, ch_send;

    if ((sockfd = socket(AF_UNIX, SOCK_STREAM, 0)) == -1) {
        perror("socket");
        exit(EXIT_FAILURE);
    }

    address.sun_family = AF_UNIX;
    strcpy(address.sun_path, "server_socket");
    len = sizeof(address);

    result = connect(sockfd, (struct sockaddr*)&address, len);

    if (result == -1) {
        printf("ensure the server is up\n");
        perror("connect");
        exit(EXIT_FAILURE);
    }

    for (i = 0, ch_send = 'A'; i < 5; i++, ch_send++) {
        if ((byte = write(sockfd, &ch_send, 1)) == -1) {
            perror("write");
            exit(EXIT_FAILURE);
        }

        if ((byte = read(sockfd, &ch_recv, 1)) == -1) {
            perror("read");
            exit(EXIT_FAILURE);
        }

        printf("receive from server data is %c\n", ch_recv);
    }

    close(sockfd);
    return 0;
}
