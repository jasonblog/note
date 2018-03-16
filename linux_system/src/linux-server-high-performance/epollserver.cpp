//#include "epollserver.hpp"

//epollserver::epollserver()
//{
//}

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/wait.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <fcntl.h>
#include <sys/epoll.h>
#include <sys/time.h>
#include <sys/resource.h>

#define LISTEN_BACKLOG 20
#define UDP_SERVER_PORT   4000
#define TCP_SERVER_PORT   5000
#define BUFFER_SIZE       1024
#define MAX_EPOLL_SIZE    1024

int setnonblocking(int sockfd)
{
    if (fcntl(sockfd, F_SETFL, fcntl(sockfd, F_GETFD, 0)|O_NONBLOCK) == -1)
    {
        fprintf(stderr, "set socket fd to nonbloack error!\n");
        return -1;
    }
    return 0;
}

int main(int argc, char* argv[])
{
    struct sockaddr_in tcp_serveraddr;
    struct sockaddr_in udp_serveraddr;

    int tcpfd_listener = socket(AF_INET, SOCK_STREAM, 0);
    if(tcpfd_listener < 0)
        perror("tfd_listener create error\n");

    int udpfd_listener = socket(AF_INET, SOCK_DGRAM, 0);
    if(udpfd_listener < 0)
        perror("ufd_listener create error\n");

    int opt=SO_REUSEADDR; //socket address reuse option
    setsockopt(udpfd_listener,SOL_SOCKET,SO_REUSEADDR,&opt,sizeof(opt));
    setsockopt(tcpfd_listener,SOL_SOCKET,SO_REUSEADDR,&opt,sizeof(opt));

    setnonblocking(udpfd_listener);
    setnonblocking(tcpfd_listener);

    bzero(&udp_serveraddr, sizeof(udp_serveraddr));
    udp_serveraddr.sin_family = AF_INET;
    udp_serveraddr.sin_port = htons(UDP_SERVER_PORT);
    udp_serveraddr.sin_addr.s_addr = INADDR_ANY;


    bzero(&tcp_serveraddr, sizeof(tcp_serveraddr));
    tcp_serveraddr.sin_family = AF_INET;
    tcp_serveraddr.sin_addr.s_addr = INADDR_ANY;
    tcp_serveraddr.sin_port = htons(TCP_SERVER_PORT);

    fprintf(stdout, "UDP PORT:%d,       TCP PORT:%d\n", UDP_SERVER_PORT, TCP_SERVER_PORT);

    //bind udp socket server address and port
    if (bind(udpfd_listener, (struct sockaddr *) &udp_serveraddr, sizeof(struct sockaddr)) == -1)
        perror("udpfd_listener bind error\n");

    //bind tcp socket server address and port
    if(bind(tcpfd_listener,(struct sockaddr *)&tcp_serveraddr, sizeof(struct sockaddr))== -1)
        perror("tcpfd_listener bind error\n");

    int epollfd = epoll_create(1024); //1024 is just a hint to kernel, has no actual meaning
    struct epoll_event epoll_ev;

    epoll_ev.data.fd = udpfd_listener;
    epoll_ev.events = EPOLLIN|EPOLLET;
    epoll_ctl(epollfd, EPOLL_CTL_ADD, udpfd_listener, &epoll_ev);

    epoll_ev.data.fd = tcpfd_listener;
    epoll_ev.events = EPOLLIN|EPOLLET;
    epoll_ctl(epollfd, EPOLL_CTL_ADD, tcpfd_listener, &epoll_ev);

    int ret = listen(tcpfd_listener, LISTEN_BACKLOG);  //listen tcp socket fd
    if(ret < 0)
    {
        perror("listen tcp socket fd error\n");
    }

    for ( ; ; )
    {
        struct epoll_event events[MAX_EPOLL_SIZE];
        int nfds = epoll_wait(epollfd, events, MAX_EPOLL_SIZE, -1); //epollfd, epoll events, max events, timeout
        printf("wait end nfds = %d\n",nfds);

        for(int i=0; i < nfds; ++i)
        {
            if(events[i].data.fd == tcpfd_listener)  //if it's tcp epoll event fd, means need to create a new tcp connection
            {
                struct sockaddr_in clientaddr;
                socklen_t socklen = sizeof(clientaddr);
                int connfd = accept(tcpfd_listener,(struct sockaddr *)&clientaddr, &socklen);
                if(connfd<0)
                {
                    perror("connfd<0");
                    exit(1);
                }
                setnonblocking(connfd);
                char *str_ip = inet_ntoa(clientaddr.sin_addr);
                printf("FILE:%s, LINE:%d, accept a connection from %s, created sockfd:%d\n", __FILE__, __LINE__, str_ip, connfd);
                struct epoll_event ev;
                ev.data.fd = connfd;   //set connected socket fd to epoll struct
                ev.events  = EPOLLIN;
                epoll_ctl(epollfd, EPOLL_CTL_ADD, connfd, &ev);   //add a new tcp connection to epoll
            }
            else if(events[i].data.fd == udpfd_listener) //if it's udp epoll event fd
            {
                printf("udp\n");
                if( events[i].events && EPOLLIN )
                    printf( "udp receive begin!\n" );

                char recv_buffer[BUFFER_SIZE + 1];
                char send_buffer[BUFFER_SIZE + 1];

                struct sockaddr_in clientaddr;
                socklen_t sock_len = sizeof(clientaddr);
                bzero(recv_buffer, BUFFER_SIZE + 1);
                bzero(send_buffer, BUFFER_SIZE + 1);

                int recv_size = recvfrom(udpfd_listener, recv_buffer, BUFFER_SIZE, 0,
                                         (struct sockaddr *)&clientaddr, &sock_len);
                if(recv_size < 0)
                {
                    fprintf(stderr, "received failed! error code %d, error message : %s \n", errno, strerror(errno));
                    continue;
                }

                //get client address and set it as echo content
                char *client_ip = inet_ntoa(clientaddr.sin_addr);
                int   client_port = ntohs(clientaddr.sin_port);
                snprintf(send_buffer, BUFFER_SIZE, "I've got your address:%s, port:%d\n", client_ip, client_port);
                fprintf(stdout, "socket %d recv from : %s :%d message: %s , %d bytes\n",udpfd_listener, client_ip,
                        client_port, recv_buffer, recv_size);

                //send(echo) content to client
                int send_size = sendto(udpfd_listener, send_buffer, strlen(send_buffer), 0,
                                       (struct sockaddr *)&clientaddr, sock_len);
                if(send_size < 0)
                {
                    fprintf(stderr, "send failed! error code %d, error message : %s \n", errno, strerror(errno));
                    continue;
                }
            }

            //do process with different event type fot tcp connections
            //如果是已经连接的用户，并且收到数据，那么进行读入。
            else if(events[i].events & EPOLLIN)
            {
                int sockfd = -1;
                if ((sockfd = events[i].data.fd) < 0)
                {
                    fprintf(stdout, "invalid sockfd:%d\n", sockfd);
                    continue;
                }

                //read data from sockfd
                char recv_buf[BUFFER_SIZE + 1];
                bzero(recv_buf, BUFFER_SIZE + 1);
                int recv_size = recv(sockfd, recv_buf, BUFFER_SIZE, 0);
                if (recv_size < 0)
                {
                    if (errno == ECONNRESET)
                    {
                        close(sockfd);
                        events[i].data.fd = -1;
                    }
                    else
                    {
                        fprintf(stderr, "recv data from sockfd(%d) error, errno:%d, errstr:%s\n",
                                sockfd, errno, strerror(errno));
                    }
                }
                else if (recv_size == 0)
                {
                    struct sockaddr_in client_addr;
                    socklen_t socklen = sizeof(client_addr);
                    getpeername(sockfd, (struct sockaddr *)&client_addr, &socklen);
                    fprintf(stdout, "client:%s, close tcp connection, sockfd:%d\n",
                            inet_ntoa(client_addr.sin_addr), sockfd);
                    close(sockfd);
                    events[i].data.fd = -1;
                }
                else
                {
                    fprintf(stdout, "read OK %s\n", recv_buf);
                    recv_buf[recv_size] = '\0';
                }

                struct epoll_event ev;
                ev.data.fd = sockfd;
                ev.events  = EPOLLOUT;
                epoll_ctl(epollfd, EPOLL_CTL_MOD, sockfd, &ev);
            }
            else if(events[i].events & EPOLLOUT) // 如果有数据发送
            {
                printf("write \n");
                int sockfd = events[i].data.fd;
                //write(sockfd, line, n);
                char send_buf[BUFFER_SIZE + 1];
                bzero(send_buf, BUFFER_SIZE + 1);
                //TODO: send back processdata
                struct sockaddr_in client_addr;
                socklen_t socklen = sizeof(client_addr);
                getpeername(sockfd, (struct sockaddr *)&client_addr, &socklen);

                snprintf(send_buf, BUFFER_SIZE, "server fd:%d, I've got your address:%s, port:%d\n",
                         sockfd, inet_ntoa(client_addr.sin_addr), ntohs(client_addr.sin_port));
                fprintf(stdout, "WRITE: %s", send_buf);

                int send_size = send(sockfd, send_buf, BUFFER_SIZE, 0);
                if(send_size < 0)
                {
                    fprintf(stderr, "send data to client(%s) error, errno:%d, errstr:%s\n",
                            inet_ntoa(client_addr.sin_addr), errno, strerror(errno));
                }

                struct epoll_event ev;
                ev.data.fd=sockfd;
                ev.events = EPOLLIN;
                epoll_ctl(epollfd, EPOLL_CTL_MOD, sockfd, &ev);
            }//
        }//for(int i; i < nfds; ++i)
    }
    return 0;
}
