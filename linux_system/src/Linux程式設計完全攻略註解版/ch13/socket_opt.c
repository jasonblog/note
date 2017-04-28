#include<sys/socket.h>
#include<stdio.h>
#include<sys/types.h>
#include<stdlib.h>
#include<netinet/in.h>
#include<netinet/tcp.h>
int main(int argc, char* argv[])
{
    int rcvbuf_size;
    int sndbuf_size;
    int type = 0;
    socklen_t size;
    int sock_fd;
    struct timeval set_time, ret_time;

    if ((sock_fd = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
        perror("socket");
        exit(EXIT_FAILURE);
    }

    size = sizeof(sndbuf_size);
    getsockopt(sock_fd, SOL_SOCKET, SO_SNDBUF, &sndbuf_size, &size);
    printf("sndbuf_size=%d\n", sndbuf_size);
    printf("size=%d\n", size);
    size = sizeof(rcvbuf_size);
    getsockopt(sock_fd, SOL_SOCKET, SO_RCVBUF, &rcvbuf_size, &size);
    printf("rcvbuf_size=%d\n", rcvbuf_size);

    size = sizeof(type);
    getsockopt(sock_fd, SOL_SOCKET, SO_TYPE, &type, &size);
    printf("socket type=%d\n", type);
    size = sizeof(struct timeval);

    getsockopt(sock_fd, SOL_SOCKET, SO_SNDTIMEO, &ret_time, &size);
    printf("default:time out is %ds,and %dns\n", ret_time.tv_sec, ret_time.tv_usec);

    set_time.tv_sec = 10;
    set_time.tv_usec = 100;
    setsockopt(sock_fd, SOL_SOCKET, SO_SNDTIMEO, &set_time, size);
    getsockopt(sock_fd, SOL_SOCKET, SO_SNDTIMEO, &ret_time, &size);
    printf("after modify:time out is %ds,and %dns\n", ret_time.tv_sec,
           ret_time.tv_usec);

    int ttl = 0;
    size = sizeof(ttl);
    getsockopt(sock_fd, IPPROTO_IP, IP_TTL, &ttl, &size);
    printf("the default ip  ttl is %d\n", ttl);
    int maxseg = 0;
    size = sizeof(maxseg);
    getsockopt(sock_fd, IPPROTO_TCP, TCP_MAXSEG, &maxseg, &size);
    printf("the TCP max seg is %d\n", maxseg);
}
