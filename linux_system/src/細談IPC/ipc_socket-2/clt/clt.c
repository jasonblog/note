#include <stdio.h>
#include <sys/types.h> //socket()
#include <sys/socket.h>//socket()
#include <string.h> //strerror()
#include <errno.h>//errno

#include <linux/un.h> // struct sockaddr_un
#include <netinet/in.h> //struct sockaddr_in

#include "sock_ipc.h"


static int
ipc_send(int sockfd, const void* msg, size_t send_size)
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

static int
ipc_recv(int sockfd, void* msg, size_t recv_size)
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
int
sln_ipc_clt_afunix_conn_init(const char* pathname)
{
    int                 fd;

    printf("===========%s==========\n", __func__);

    fd = socket(AF_UNIX, SOCK_STREAM, 0);

    if (fd < 0) {
        fprintf(stderr, "socket: %s\n", strerror(errno));
        return -1;
    }

    seraddr.sun_family = AF_UNIX;
    snprintf(seraddr.sun_path, sizeof(seraddr.sun_path), "%s", pathname);

    return fd;
}

#else

struct sockaddr_in  seraddr;
int
sln_ipc_clt_afinet_conn_init(int port)
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

int
sln_ipc_clt_conn(
    int msg_type,
    int* ret_code,
    void* sendbuf,
    int sendlen,
    void* recvbuf,
    int* recvlen)
{
    int                 connfd;
    ssize_t             ret_size;
    socklen_t           addrlen;
    ipc_sock_msg_t      send_msg, recv_msg;

#if USE_AF_UNIX

    if ((connfd = sln_ipc_clt_afunix_conn_init(SOCK_IPC_NAME)) < 0) {
        return -1;
    }

    addrlen = sizeof(struct sockaddr_un);
#else

    if ((connfd = sln_ipc_clt_afinet_conn_init(SOCK_IPC_SER_LISTEN_PORT)) < 0) {
        return -1;
    }

    addrlen = sizeof(struct sockaddr_in);
#endif

    if (connect(connfd, (struct sockaddr*)&seraddr, addrlen) < 0) {
        fprintf(stderr,  "connect: %s\n", strerror(errno));
        return -1;
    }

    memset(&send_msg, 0, sizeof(ipc_sock_msg_t));
    send_msg.msg_type = msg_type;

    if (NULL != sendbuf) {
        send_msg.msg_buflen = sendlen;
        memcpy(send_msg.msg_buf, sendbuf, sendlen);
    }

    if ((ret_size = ipc_send(connfd, &send_msg, 3 * sizeof(int) + sendlen)) < 0) {
        return -1;
    }

    if ((ret_size = ipc_recv(connfd, &recv_msg, sizeof(ipc_sock_msg_t))) < 0) {
        return -1;
    }

    if (recv_msg.msg_type != send_msg.msg_type) {
        printf("Error msg type!\n");
        return -1;
    }

    *ret_code = recv_msg.msg_rc;

    if (NULL != recvbuf) {
        *recvlen = recv_msg.msg_buflen;
        memcpy(recvbuf, recv_msg.msg_buf, recv_msg.msg_buflen);
    }

    return 0;
}

int main(int argc, const char* argv[])
{
    int         rc, ret, recvlen;
    char        sendbuf[SOCK_IPC_MAX_BUF];
    char        recvbuf[SOCK_IPC_MAX_BUF];

    if (argc != 2) {
        printf("Usage: %s <send information>\n", argv[0]);
        return -1;
    }

    strcpy(sendbuf, argv[1]);

    ret = sln_ipc_clt_conn(SLN_IPC_TYPE_0x1, &rc, NULL, 0, recvbuf, &recvlen);

    if (ret < 0) {
        printf("clt conn error!");
    }

    if (rc != SLN_IPC_RC_OK) {
        printf("get failed!");
    }

    printf("recv-1: %s\n", recvbuf);

    ret = sln_ipc_clt_conn(SLN_IPC_TYPE_0x2, &rc, sendbuf, strlen(sendbuf) + 1,
                           NULL, NULL);

    if (ret < 0) {
        printf("clt conn error!");
    }

    if (rc != SLN_IPC_RC_OK) {
        printf("set failed!");
    }

    ret = sln_ipc_clt_conn(SLN_IPC_TYPE_0x1, &rc, NULL, 0, recvbuf, &recvlen);

    if (ret < 0) {
        printf("clt conn error!");
    }

    if (rc != SLN_IPC_RC_OK) {
        printf("get failed!");
    }

    printf("recv-2: %s\n", recvbuf);
    return 0;
}
