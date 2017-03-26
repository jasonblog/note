#include <stdio.h>
#include <sys/types.h> //socket()
#include <sys/socket.h>//socket()
#include <string.h> //strerror()
#include <errno.h>//errno

#include <linux/un.h> // struct sockaddr_un
#include <netinet/in.h> //struct sockaddr_in

#include "sock_ipc.h"

int ipc_send(int sockfd, const void* msg, size_t send_size)
{
    int         sendlen;

    while ((sendlen = send(sockfd, msg, send_size, 0)) < 0) {
        if (errno == EINTR) {
            continue;
        }

        printf("send: %s\n", strerror(errno));
        return -1;
    }

    return sendlen;
}

int ipc_recv(int sockfd, void* msg, size_t recv_size)
{
    int         recvlen;

    while ((recvlen = recv(sockfd, msg, recv_size, 0)) < 0) {
        if (errno == EINTR) {
            continue;
        }

        printf("recv: %s\n", strerror(errno));
        return -1;
    }

    return recvlen;
}

#if USE_AF_UNIX
int ser_afunix_listen(const char* pathname)
{
    int                 listenfd, on;
    struct sockaddr_un  seraddr;

    printf("===========%s==========\n", __func__);

    listenfd = socket(AF_UNIX, SOCK_STREAM, 0);

    if (listenfd < 0) {
        fprintf(stderr, "socket: %s\n", strerror(errno));
        return -1;
    }

    unlink(pathname);
    seraddr.sun_family = AF_UNIX;
    snprintf(seraddr.sun_path, sizeof(seraddr.sun_path), "%s", pathname);

    if (bind(listenfd, (struct sockaddr*)&seraddr,
             sizeof(struct sockaddr_un)) < 0) {
        fprintf(stderr, "bind: %s\n", strerror(errno));
        return -1;
    }

    if (listen(listenfd, SOCK_IPC_MAX_CONN) < 0) {
        fprintf(stderr, "listen: %s\n", strerror(errno));
        return -1;
    }

    return listenfd;
}
#else

int ser_afinet_listen(int port)
{
    int                 listenfd, on;
    struct sockaddr_in  seraddr;

    printf("===========%s==========\n", __func__);

    listenfd = socket(AF_INET, SOCK_STREAM, 0);

    if (listenfd < 0) {
        fprintf(stderr, "socket: %s\n", strerror(errno));
        return -1;
    }

    on = 1;
    setsockopt(listenfd, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on));

    seraddr.sin_family = AF_INET;
    seraddr.sin_port = port;
    seraddr.sin_addr.s_addr = htonl(INADDR_ANY);

    if (bind(listenfd, (struct sockaddr*)&seraddr,
             sizeof(struct sockaddr_in)) < 0) {
        fprintf(stderr, "bind: %s\n", strerror(errno));
        return -1;
    }

    if (listen(listenfd, SOCK_IPC_MAX_CONN) < 0) {
        fprintf(stderr, "listen: %s\n", strerror(errno));
        return -1;
    }

    return listenfd;
}
#endif

int ser_accept(int listenfd)
{
    int                 connfd;
    ssize_t             recvlen, sendlen;
    char                buf[SOCK_IPC_MAX_BUF];
    socklen_t           addrlen;
#if USE_AF_UNIX
    struct sockaddr_un  cltaddr;
#else
    struct sockaddr_in  cltaddr;
#endif

    addrlen = sizeof(cltaddr);

    for (;;) {
        connfd = accept(listenfd, (struct sockaddr*)&cltaddr, &addrlen);

        if (connfd < 0) {
            fprintf(stderr, "accept: %s\n", strerror(errno));
            continue;
        }

        if (recvlen = ipc_recv(connfd, buf, sizeof(buf)) < 0) {
            continue;
        }

        printf("recv: %s\n", buf);
        snprintf(buf, sizeof(buf), "hello, ipc client!");

        if (ipc_send(connfd, buf, strlen(buf)) < 0) {
            continue;
        }

        close(connfd);
    }
}

int main(int argc, const char* argv[])
{
    int fd;

#if USE_AF_UNIX
    fd = ser_afunix_listen(SOCK_IPC_NAME);

    if (fd < 0) {
        return -1;
    }

#else
    fd = ser_afinet_listen(SOCK_IPC_SER_LISTEN_PORT);

    if (fd < 0) {
        return -1;
    }

#endif

    if (ser_accept(fd) < 0) {
        return -1;
    }

    return 0;
}
