#include <stdio.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>

#define BUFFER_SIZE   1024

int main(int argc, const char *argv[])
{
    if (argc < 2) 
    {
        printf("Usage: %s ip_address port_number recv_buffer_size\n", basename(argv[0]));
        return 1;
    }

    const char *ip = argv[1];
    int port       = atoi(argv[2]);
    int recvbuf    = atoi(argv[3]);

    struct sockaddr_in address;
    bzero(&address, sizeof(address));
    address.sin_family = AF_INET;
    inet_pton(AF_INET, ip, &address.sin_addr);
    address.sin_port   = htons(port);

    int sock = socket(PF_INET, SOCK_STREAM, 0);
    assert(sock >= 0);

    int len = sizeof(recvbuf);
    /*先设置TCP接收缓冲区的大小，然后立即读取之*/
    setsockopt(sock, SOL_SOCKET, SO_RCVBUF, &recvbuf, sizeof(recvbuf));
    getsockopt(sock, SOL_SOCKET, SO_RCVBUF, &recvbuf, (socklen_t *)&len);
    printf("the tcp receive buffer size after setting is %d\n", recvbuf);

    int ret = bind(sock, (struct sockaddr *)&address, sizeof(address));
    assert(ret != -1);

    ret = listen(sock, 5);
    assert(ret != -1);

    struct sockaddr_in client;
    socklen_t  client_addrlength = sizeof(client);
    int connfd = accept(sock, (struct sockaddr *)&client, &client_addrlength);
    if (connfd < 0) 
    {
        printf("errno is:  %d, errstr:%s\n", errno, strerror(errno));
    }
    else
    {
        char buffer[BUFFER_SIZE];
        memset(buffer, '\0', BUFFER_SIZE);
        while(recv(connfd, buffer, BUFFER_SIZE, 0) > 0) {}
        close(connfd);
        char client_ip[INET_ADDRSTRLEN];
        inet_ntop(AF_INET, (void *)&client.sin_addr, client_ip, (socklen_t )INET_ADDRSTRLEN);
        printf("Recv data from %s, data content is:%s\n", client_ip, buffer);
    }

    close(sock);
    return 0;
}
