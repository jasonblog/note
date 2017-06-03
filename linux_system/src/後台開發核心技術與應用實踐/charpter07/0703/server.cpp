#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <strings.h>
#include <sys/wait.h>
#include <string.h>
#include <errno.h>
#define DEFAULT_PORT 6666
int main(int argc, char** argv)
{
    int serverfd,
        acceptfd; /* 监听socket: serverfd,数据传输socket: acceptfd */
    struct sockaddr_in my_addr; /* 本机地址信息 */
    struct sockaddr_in their_addr; /* 客户地址信息 */
    unsigned int sin_size, myport = 6666, lisnum = 10;

    if ((serverfd = socket(AF_INET , SOCK_STREAM, 0)) == -1) {
        perror("socket");
        return -1;
    }

    printf("socket ok \n");
    my_addr.sin_family = AF_INET;
    my_addr.sin_port = htons(DEFAULT_PORT);
    my_addr.sin_addr.s_addr = INADDR_ANY;
    bzero(&(my_addr.sin_zero), 0);

    if (bind(serverfd, (struct sockaddr*)&my_addr, sizeof(struct sockaddr)) == -1) {
        perror("bind");
        return -2;
    }

    printf("bind ok \n");

    if (listen(serverfd, lisnum) == -1) {
        perror("listen");
        return -3;
    }

    printf("listen ok \n");

    fd_set client_fdset;    /*监控文件描述符集合*/
    int maxsock;            /*监控文件描述符中最大的文件号*/
    struct timeval tv;      /*超时返回时间*/
    int client_sockfd[5];   /*存放活动的sockfd*/
    bzero((void*)client_sockfd, sizeof(client_sockfd));
    int conn_amount = 0;    /*用来记录描述符数量*/
    maxsock = serverfd;
    char buffer[1024];
    int ret = 0;

    while (1) {
        /*初始化文件描述符号到集合*/
        FD_ZERO(&client_fdset);
        /*加入服务器描述符*/
        FD_SET(serverfd, &client_fdset);
        /*设置超时时间*/
        tv.tv_sec = 30; /*30秒*/
        tv.tv_usec = 0;

        /*把活动的句柄加入到文件描述符中*/
        for (int i = 0; i < 5; ++i) {
            /*程序中Listen中参数设为5,故i必须小于5*/
            if (client_sockfd[i] != 0) {
                FD_SET(client_sockfd[i], &client_fdset);
            }
        }

        /*printf("put sockfd in fdset!\n");*/
        /*select函数*/
        ret = select(maxsock + 1, &client_fdset, NULL, NULL, &tv);

        if (ret < 0) {
            perror("select error!\n");
            break;
        } else if (ret == 0) {
            printf("timeout!\n");
            continue;
        }

        /*轮询各个文件描述符*/
        for (int i = 0; i < conn_amount; ++i) {
            /*FD_ISSET检查client_sockfd是否可读写，>0可读写*/
            if (FD_ISSET(client_sockfd[i], &client_fdset)) {
                printf("start recv from client[%d]:\n", i);
                ret = recv(client_sockfd[i], buffer, 1024, 0);

                if (ret <= 0) {
                    printf("client[%d] close\n", i);
                    close(client_sockfd[i]);
                    FD_CLR(client_sockfd[i], &client_fdset);
                    client_sockfd[i] = 0;
                } else {
                    printf("recv from client[%d] :%s\n", i, buffer);
                }
            }
        }

        /*检查是否有新的连接，如果收，接收连接，加入到client_sockfd中*/
        if (FD_ISSET(serverfd, &client_fdset)) {
            /*接受连接*/
            struct sockaddr_in client_addr;
            size_t size = sizeof(struct sockaddr_in);
            int sock_client = accept(serverfd, (struct sockaddr*)(&client_addr),
                                     (unsigned int*)(&size));

            if (sock_client < 0) {
                perror("accept error!\n");
                continue;
            }

            /*把连接加入到文件描述符集合中*/
            if (conn_amount < 5) {
                client_sockfd[conn_amount++] = sock_client;
                bzero(buffer, 1024);
                strcpy(buffer, "this is server! welcome!\n");
                send(sock_client, buffer, 1024, 0);
                printf("new connection client[%d] %s:%d\n", conn_amount,
                       inet_ntoa(client_addr.sin_addr), ntohs(client_addr.sin_port));
                bzero(buffer, sizeof(buffer));
                ret = recv(sock_client, buffer, 1024, 0);

                if (ret < 0) {
                    perror("recv error!\n");
                    close(serverfd);
                    return -1;
                }

                printf("recv : %s\n", buffer);

                if (sock_client > maxsock) {
                    maxsock = sock_client;
                } else {
                    printf("max connections!!!quit!!\n");
                    break;
                }
            }
        }
    }

    for (int i = 0; i < 5; ++i) {
        if (client_sockfd[i] != 0) {
            close(client_sockfd[i]);
        }
    }

    close(serverfd);
    return 0;
}
