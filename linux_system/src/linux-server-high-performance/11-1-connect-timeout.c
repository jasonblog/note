#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <errno.h>
#include <fcntl.h>
#include <string.h>
#include <unistd.h>
#include <assert.h>


/*超时连接函数*/
int timeout_connect(const char * ip, int port, int time)
{
    int ret = 0;
    struct sockaddr_in address;
    bzero(&address, sizeof(address));
    address.sin_family = AF_INET;
    inet_pton(AF_INET, ip, &address.sin_addr);
    address.sin_port = htons(port);
    
    int sockfd = socket(PF_INET, SOCK_STREAM, 0);
    assert(sockfd >= 0);

    /*通过选项SO_RCVTIMEO和SO_SNDTIMEO所设置的超时时间的类型timeval，这和select系统调用
     * 的超时参数类型相同
     */
    struct timeval timeout;
    timeout.tv_sec = time;
    timeout.tv_usec = 0;
    socklen_t len = sizeof(timeout);

    ret = setsockopt(sockfd, SOL_SOCKET, SO_SNDTIMEO, &timeout, len);
    assert(ret != -1);

    ret = connect(sockfd, (struct sockaddr *)&address, sizeof(address));
    if (ret == -1) 
    {
        /*超时对应的错误是EINPROGRESS,下面这个条件如果成立，
         * 我们就可以处理定时任务了
         */
        if (errno == EINPROGRESS) 
        {
            fprintf(stderr, "connecting timeout process timeout logic \n");
            return -1;
        }
        fprintf(stderr, "error occur when connecting to server\n");
        return -1;
    }
    return sockfd;
}

int main(int argc, const char *argv[])
{
    if (argc <= 2) 
    {
        fprintf(stderr, "usage: %s ip_address port_number\n", basename(argv[0]));
        return 1;
    }
    const char *ip = argv[1];
    int port       = atoi(argv[2]);

    int sockfd   = timeout_connect(ip, port, 10);
    if (sockfd < 0) 
    {
        return 1;
    }

    return 0;
}
