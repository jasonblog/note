#include <stdio.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <stdlib.h>
#include <fcntl.h>
#include <assert.h>

int main(int argc, const char *argv[])
{
    if (argc <= 2) 
    {
        fprintf(stderr, "usage: %s ip_address port_number\n", basename(argv[0]));
        return 1;
    }

    const char* ip = argv[1];
    int port       = atoi(argv[2]);

    struct sockaddr_in address;
    bzero(&address, sizeof(address));
    address.sin_family = AF_INET;
    inet_pton(AF_INET, ip, &address.sin_addr);
    address.sin_port   = htons(port);

    int sock = socket(PF_INET, SOCK_STREAM, 0);
    assert(sock >=0 );

    int ret = bind(sock, (struct sockaddr *)&address, sizeof(address));
    assert(ret != -1);

    ret = listen(sock, 5);
    assert(ret != -1);

    struct sockaddr_in client;
    socklen_t client_addrlength = sizeof(client);
    int connfd = accept(sock, (struct sockaddr *)&client, &client_addrlength);
    if(connfd < 0)
    {
        fprintf(stderr, "accept error! errno:%d, errstr:%s\n", errno, strerror(errno));
    }
    else
    {
        int pipefd[2];
        assert(ret != -1);
        ret = pipe(pipefd);   /*创建管道, pipefd[0]: read end of the pipe, pipefd[1]:write end of the pipe */
        /*将connfd上 流入的客户数据定向到管道中*/
        ret = splice(connfd, NULL, pipefd[1], NULL, 32768,
                        SPLICE_F_MORE | SPLICE_F_MOVE);
        assert(ret != -1);
        
        /*将管道的输出定向到connfd客户连接文件描述 */
        ret = splice(pipefd[0], NULL, connfd, NULL, 326768, 
                        SPLICE_F_MORE | SPLICE_F_MOVE);
        assert(ret != -1);
        close(connfd);
    }

    close(sock);
    return 0;
}
