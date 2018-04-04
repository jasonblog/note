#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>
#include <sys/socket.h>
#include <netinet/in.h>

int main(int argc, char* argv[])
{
    int client_fd;

    if (-1 == (client_fd = socket(AF_INET, SOCK_STREAM, 0))) {
        perror("socket");
        exit(EXIT_FAILURE);
    }

    struct  sockaddr_in server_addr;

    server_addr.sin_family = AF_INET;

    server_addr.sin_port = htons(atoi(argv[1]));

    server_addr.sin_addr.s_addr = inet_addr("10.34.130.31");

    if (-1 == connect(client_fd, (struct sockaddr*)&server_addr,
                      sizeof(server_addr))) {
        perror("connect");
        exit(EXIT_FAILURE);
    }

    struct info {
        char flag ;         //1:file attribute; 2:file content
        char buf[256];      //require file name
        int local_begin;    //if get attribute, no used
        int length;         //if get attribute, no used
    };

    struct info sendinfo;

    sendinfo.flag = 1;

    sprintf(sendinfo.buf, "%s", argv[2]);

    if (-1 == send(client_fd, &sendinfo, sizeof(sendinfo), 0)) {
        perror("send");
        exit(EXIT_FAILURE);
    }

    char buf[1024];
    memset(buf, '\0', 1024);

    if (-1 == recv(client_fd, buf, 1024, 0)) {
        perror("recv");
        exit(EXIT_FAILURE);
    }

    close(client_fd);

    if (-1 == (client_fd = socket(AF_INET, SOCK_STREAM, 0))) {
        perror("socket");
        exit(EXIT_FAILURE);
    }


    if (-1 == connect(client_fd, (struct sockaddr*)&server_addr,
                      sizeof(server_addr))) {
        perror("connect");
        exit(EXIT_FAILURE);
    }

    sendinfo.flag = 2;
    sendinfo.local_begin = 0;
    sendinfo.length = atoi(buf);

    if (-1 == send(client_fd, &sendinfo, sizeof(sendinfo), 0)) {
        perror("send");
        exit(EXIT_FAILURE);
    }

    int file_fd;
    file_fd = open(argv[3], O_WRONLY | O_CREAT, 0644);

    lseek(file_fd, sendinfo.local_begin, SEEK_SET);

    printf("length = %d\n", ntohl(sendinfo.length));
    int ret = 0;
    int sum = 0;

    do {
        ret = recv(client_fd, buf, 1024, 0);
        write(file_fd, buf, ret);
        sum  = sum + ret;
        printf("sum=%d\n", sum);
        getchar();
    } while (sum != ntohl(sendinfo.length));

    close(client_fd);
    close(file_fd);
}




