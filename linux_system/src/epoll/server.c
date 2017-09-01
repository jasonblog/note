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
#include <openssl/ssl.h>
#include <openssl/err.h>
#include <fcntl.h>
#include <sys/epoll.h>
#include <sys/time.h>
#include <sys/resource.h>
#define MAXBUF 1024
#define MAXEPOLLSIZE 10000
/*
   setnonblocking - 设置句柄为非阻塞方式
   */
int setnonblocking(int sockfd) {
    if (fcntl(sockfd, F_SETFL, fcntl(sockfd, F_GETFD, 0) | O_NONBLOCK) == -1) {
        return -1;
    }

    return 0;
}
/*
   handle_message - 处理每个 socket 上的消息收发
   */
int handle_message(int new_fd) {
    char buf[MAXBUF + 1];
    int len;
    /* 开始处理每个新连接上的数据收发 */
    bzero(buf, MAXBUF + 1);
    /* 接收客户端的消息 */
    len = recv(new_fd, buf, MAXBUF, 0);

    if (len > 0) {
        printf("%d接收消息成功:'%s'，共%d个字节的数据\n",
               new_fd, buf, len);
    } else {
        if (len < 0)
            printf
            ("消息接收失败！错误代码是%d，错误信息是'%s'\n",
             errno, strerror(errno));

        close(new_fd);
        return -1;
    }

    /* 处理每个新连接上的数据收发结束 */
    return len;
}
int main(int argc, char** argv) {
    int listener, new_fd, kdpfd, nfds, n, ret, curfds;
    socklen_t len;
    struct sockaddr_in my_addr, their_addr;
    unsigned int myport, lisnum;
    struct epoll_event ev;
    struct epoll_event events[MAXEPOLLSIZE];
    struct rlimit rt;
    myport = 5000;
    lisnum = 2;
    /* 设置每个进程允许打开的最大文件数 */
    rt.rlim_max = rt.rlim_cur = MAXEPOLLSIZE;

    if (setrlimit(RLIMIT_NOFILE, &rt) == -1) {
        perror("setrlimit");
        exit(1);
    } else {
        printf("设置系统资源参数成功！\n");
    }

    /* 开启 socket 监听 */
    if ((listener = socket(PF_INET, SOCK_STREAM, 0)) == -1) {
        perror("socket");
        exit(1);
    } else {
        printf("socket 创建成功！\n");
    }

    setnonblocking(listener);
    bzero(&my_addr, sizeof(my_addr));
    my_addr.sin_family = PF_INET;
    my_addr.sin_port = htons(myport);
    my_addr.sin_addr.s_addr = INADDR_ANY;

    if (bind(listener, (struct sockaddr*) &my_addr, sizeof(struct sockaddr)) == -1) {
        perror("bind");
        exit(1);
    } else {
        printf("IP 地址和端口绑定成功\n");
    }

    if (listen(listener, lisnum) == -1) {
        perror("listen");
        exit(1);
    } else {
        printf("开启服务成功！\n");
    }

    /* 创建 epoll 句柄，把监听 socket 加入到 epoll 集合里 */
    kdpfd = epoll_create(MAXEPOLLSIZE);
    len = sizeof(struct sockaddr_in);
    ev.events = EPOLLIN | EPOLLET;
    ev.data.fd = listener;

    if (epoll_ctl(kdpfd, EPOLL_CTL_ADD, listener, &ev) < 0) {
        fprintf(stderr, "epoll set insertion error: fd=%d\n", listener);
        return -1;
    } else {
        printf("监听 socket 加入 epoll 成功！\n");
    }

    curfds = 1;

    while (1) {
        /* 等待有事件发生 */
        nfds = epoll_wait(kdpfd, events, curfds, -1);

        if (nfds == -1) {
            perror("epoll_wait");
            break;
        }

        /* 处理所有事件 */
        for (n = 0; n < nfds; ++n) {
            if (events[n].data.fd == listener) {
                new_fd = accept(listener, (struct sockaddr*) &their_addr, &len);

                if (new_fd < 0) {
                    perror("accept");
                    continue;
                } else {
                    printf("有连接来自于： %d:%d， 分配的 socket 为:%d\n",
                           inet_ntoa(their_addr.sin_addr), ntohs(their_addr.sin_port), new_fd);
                }

                setnonblocking(new_fd);
                ev.events = EPOLLIN | EPOLLET;
                ev.data.fd = new_fd;

                if (epoll_ctl(kdpfd, EPOLL_CTL_ADD, new_fd, &ev) < 0) {
                    fprintf(stderr, "把 socket '%d' 加入 epoll 失败！%s\n",
                            new_fd, strerror(errno));
                    return -1;
                }

                curfds++;
            } else {
                ret = handle_message(events[n].data.fd);

                if (ret < 1 && errno != 11) {
                    epoll_ctl(kdpfd, EPOLL_CTL_DEL, events[n].data.fd, &ev);
                    curfds--;
                }
            }
        }
    }

    close(listener);
    return 0;
}
