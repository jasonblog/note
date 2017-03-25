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
struct sockaddr_un  seraddr;
int clt_afunix_conn_init(const char* pathname)
{
    int                 fd;
    struct sockaddr_un  localaddr;

    printf("===========%s==========\n", __func__);

    fd = socket(AF_UNIX, SOCK_STREAM, 0);

    if (fd < 0) {
        fprintf(stderr, "socket: %s\n", strerror(errno));
        return -1;
    }

    localaddr.sun_family = AF_UNIX;
    snprintf(localaddr.sun_path, sizeof(localaddr.sun_path), "%s-cltid%d", pathname,
             getpid());

    if (bind(fd, (struct sockaddr*)&localaddr, sizeof(struct sockaddr_un)) < 0) {
        fprintf(stderr, "bind: %s\n", strerror(errno));
        return -1;
    }

    seraddr.sun_family = AF_UNIX;
    snprintf(seraddr.sun_path, sizeof(seraddr.sun_path), "%s", pathname);

    return fd;
}

#else

struct sockaddr_in  seraddr;
int clt_afinet_conn_init(int port)
{
    int                 fd;

    printf("===========%s==========\n", __func__);

    fd = socket(AF_INET, SOCK_STREAM, 0);

    if (fd < 0) {
        fprintf(stderr, "socket: %s\n", strerror(errno));
        return -1;
    }

    seraddr.sin_family = AF_INET;
    seraddr.sin_port = port;

    if (inet_pton(AF_INET, "127.0.0.1", &seraddr.sin_addr) < 0) {
        fprintf(stderr, "inet_pton: %s\n", strerror(errno));
        return -1;
    }

    return fd;
}
#endif

int main(int argc, const char* argv[])
{
    int         fd;
    ssize_t     sendlen, recvlen;
    socklen_t   addrlen;
    char        buf[SOCK_IPC_MAX_BUF] = "hello ipc server!";

#if USE_AF_UNIX

    if ((fd = clt_afunix_conn_init(SOCK_IPC_NAME)) < 0) {
        return -1;
    }

    addrlen = sizeof(struct sockaddr_un);
#else

    if ((fd = clt_afinet_conn_init(SOCK_IPC_SER_LISTEN_PORT)) < 0) {
        return -1;
    }

    addrlen = sizeof(struct sockaddr_in);
#endif

    if (connect(fd, (struct sockaddr*)&seraddr, addrlen) < 0) {
        fprintf(stderr,  "connect: %s\n", strerror(errno));
        return -1;
    }

    if ((sendlen = ipc_send(fd, buf, strlen(buf))) < 0) {
        return -1;
    }

    if ((recvlen = ipc_recv(fd, buf, sizeof(buf))) < 0) {
        return -1;
    }

    printf("recv: %s\n", buf);
    return 0;
}
