
#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <sys/epoll.h>
#include <sys/time.h>
#include <sys/types.h>
#include <string.h>
#include <netinet/in.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <fcntl.h>
#include <errno.h>
#include <time.h>

#define MAX_EPOLL_SIZE  1000
#define BUF_SIZE        1024
#define TCP_SERVER_PORT 5000
#define UDP_SERVER_PORT 4000

int setnonblocking(int sockfd)
{
    if (fcntl(sockfd, F_SETFL, fcntl(sockfd, F_GETFD, 0)|O_NONBLOCK) == -1)
    {
        fprintf(stderr, "set socket fd nonblocking errror\n");
        return -1;
    }
    return 0;
}

int open_connections(int conn_count, int *conn_fds, struct sockaddr_in *server_addr);
int epoll_add_connections(int epollfd, int conn_count, int *conn_fds);

int main(int argc, char *argv[])
{
    if (argc < 4)
    {
        fprintf(stderr, "Usage: %s server_ip, server_port, connection_count\n", basename(argv[0]));
        exit(-1);
    }

    struct sockaddr_in server_addr;
    server_addr.sin_family = AF_INET;
    int ret = inet_pton(AF_INET, argv[1], &server_addr.sin_addr);
    if(ret <= 0)
    {
        if (ret == 0)
            fprintf(stderr, "Not in presentation format");
        else
            perror("inet_pton");
        exit(EXIT_FAILURE);
    }
    server_addr.sin_port = htons(atoi(argv[2]));

    int conn_count = atoi(argv[3]);
    int *conn_fds = new int[conn_count];

    struct epoll_event events[MAX_EPOLL_SIZE];
    int epollfd = epoll_create(1024);

    open_connections(conn_count, conn_fds, &server_addr);
    //ADD connection fds to epoll
    epoll_add_connections(epollfd, conn_count, conn_fds);

    //listen epoll events and do response
    for(;;)
    {
        int timeout = -1;
        int num_events = epoll_wait(epollfd, events, MAX_EPOLL_SIZE, timeout);
        if (num_events < 0)
        {
            perror("epoll wait error!\n");
        }

        for(int i = 0; i < num_events; ++i)
        {
            int event_mask = events[i].events;
            int sockfd     = events[i].data.fd;

            if(event_mask & EPOLLHUP)
            {
                fprintf(stderr, "EPOLLHUP, server hung up on connect fd: %d\n", sockfd);
                close(sockfd);
                continue;
            }

            if(event_mask & EPOLLERR)
            {
                fprintf(stderr, "EPOLLERR, server returned error on connect fd: %d\n", sockfd);
                close(sockfd);
                continue;
            }

            if(event_mask & EPOLLIN)
            {
                //recv data from server
                char recv_buf[BUF_SIZE +1];
                bzero(recv_buf, BUF_SIZE + 1);
                int recv_size = recv(sockfd, recv_buf, BUF_SIZE, 0);
                if (recv_size < 0)
                {
                    fprintf(stdout, "recv data from fd(%d) error, errno:%d, strerr:%s\n", sockfd, errno, strerror(errno));
                    close(sockfd);
                    continue;
                }
                if (recv_size == 0)
                {
                    fprintf(stdout, "close socket fd(%d) normally\n", sockfd);
                    close(sockfd);
                    continue;
                }
                fprintf(stdout, "sockfd(%d), RECV DATA: %s\n", sockfd, recv_buf);

                struct epoll_event ev;
                ev.events = EPOLLOUT;
                ev.data.fd = sockfd;
                if(epoll_ctl(epollfd, EPOLL_CTL_MOD, sockfd, &ev) < 0)
                {
                    fprintf(stderr, "epoll_ctl error, sockfd(%d), errno:%d, strerr:%s", sockfd, errno, strerror(errno));
                }
            }

            if(event_mask & EPOLLOUT)
            {
                //send data to server
                char send_buf[BUF_SIZE + 1];
                bzero(send_buf, BUF_SIZE + 1);
                int cur_time = (int) time(NULL);
                snprintf(send_buf, BUF_SIZE, "Hello, I'm client my socket fd is %d, time:%d\n", sockfd, cur_time);
                int send_size = send(sockfd, send_buf, strlen(send_buf), 0);
                if(send_size != (int) strlen(send_buf))
                {
                    fprintf(stderr, "fd(%d) send data to server error!\n", sockfd);
                    close(sockfd);
                    continue;
                }

                struct epoll_event ev;
                ev.events = EPOLLIN;
                ev.data.fd = sockfd;
                if(epoll_ctl(epollfd, EPOLL_CTL_MOD, sockfd, &ev) < 0)
                {
                    fprintf(stderr, "epoll_ctl error, sockfd(%d), errno:%d, strerr:%s", sockfd, errno, strerror(errno));
                }
            }

            sleep(1);
        }//for
    }

    delete[] conn_fds;

    return 0;
}

int open_connections(int conn_count, int *conn_fds, struct sockaddr_in *server_addr)
{
    for(int i = 0; i < conn_count; ++i)
    {
        conn_fds[i] = socket(PF_INET, SOCK_STREAM, 0);
        if(conn_fds[i] < 0)
        {
            fprintf(stderr, "number:%d, open socket error!\n", i);
            continue;
        }
        if(connect(conn_fds[i], (struct sockaddr *)server_addr, sizeof(struct sockaddr_in)) < 0)
        {
            fprintf(stderr, "connect to server error, server:%s, sockfd:%d\n",
                    inet_ntoa(server_addr->sin_addr), conn_fds[i]);
            continue;
        }
    }//for
    return 0;
}

int epoll_add_connections(int epollfd, int conn_count, int *conn_fds)
{
    struct epoll_event ev;

    for(int i = 0; i < conn_count; ++i)
    {
       ev.events = EPOLLIN | EPOLLOUT | EPOLLHUP;
       ev.data.fd = conn_fds[i];
       if(epoll_ctl(epollfd, EPOLL_CTL_ADD, conn_fds[i], &ev) < 0)
       {
           fprintf(stderr, "epoll add fd(%d) error, errno:%d, strerr:%s\n", conn_fds[i], errno, strerror(errno));
           continue;
       }
    }
    return 0;
}
