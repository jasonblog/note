#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <netinet/in.h>         /*socket address struct*/
#include <arpa/inet.h>          /*host to network convertion*/
#include <sys/socket.h>
#include <signal.h>
#define MAX_LENTH 1500

int main(int argc, char* argv[])
{
    struct sockaddr_in addr;
    int sock_fd, ret;
    char snd_buf[MAX_LENTH];


    if (argc != 3) {
        printf("use: %s ip_add port\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    memset(&addr, 0, sizeof(addr));
    addr.sin_family =  AF_INET;

    if (inet_aton(argv[1], (struct in_addr*)&addr.sin_addr.s_addr) == 0) {
        perror(argv[1]);
        exit(EXIT_FAILURE);
    }

    addr.sin_port = htons(atoi(argv[2]));

    if ((sock_fd = socket(AF_INET, SOCK_DGRAM, 0)) == -1) {
        perror("socket");
        exit(EXIT_FAILURE);
    }

    if (ret = connect(sock_fd, (struct sockaddr*)&addr, sizeof(addr)) == -1) {
        perror("Connect");
        exit(EXIT_FAILURE);
    }

    while (1) {
        printf("input msg to send:");
        memset(snd_buf, '\0', MAX_LENTH);
        fgets(snd_buf, MAX_LENTH - 1, stdin);
        write(sock_fd, snd_buf, MAX_LENTH - 1);
    }

}
