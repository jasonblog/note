#include <stdio.h>
#include <sys/types.h> //socket()
#include <sys/socket.h>//socket()
#include <string.h> //strerror()
#include <errno.h>//errno
#include <unistd.h>

#include <linux/un.h> // struct sockaddr_un
#include <netinet/in.h> //struct sockaddr_in

#include "sock_ipc.h"
#include "ser_do.h"

static int
(*sln_ipc_ser_func[SLN_IPC_MAX_TYPE])(
    void* recvbuf, int recv_size,
    void* sendbuf, int* send_size);

static int
sln_ipc_send(int sockfd, const void* msg, size_t send_size)
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
sln_ipc_recv(int sockfd, void* msg, size_t recv_size)
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
static int
sln_ipc_ser_afunix_listen(const char* pathname)
{
    int                 listenfd;
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

static int
sln_ipc_ser_afinet_listen(int port)
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

static int
sln_ser_handle(int sockfd, ipc_sock_msg_t* recv_msg)
{
    ipc_sock_msg_t  send_msg;

    memset(&send_msg, 0, sizeof(ipc_sock_msg_t));

    send_msg.msg_type = recv_msg->msg_type;

    if ((recv_msg->msg_type >= SLN_IPC_MAX_TYPE)
        && (recv_msg->msg_rc < 0)) {
        send_msg.msg_rc = SLN_IPC_RC_TYPE;
    } else if (NULL == sln_ipc_ser_func[recv_msg->msg_type]) {
        send_msg.msg_rc = SLN_IPC_RC_FUNC;
    } else {
        send_msg.msg_rc
            = sln_ipc_ser_func[recv_msg->msg_type](
                  recv_msg->msg_buf,
                  recv_msg->msg_buflen,
                  send_msg.msg_buf,
                  &send_msg.msg_buflen);
    }

    if (sln_ipc_send(sockfd, &send_msg, sizeof(ipc_sock_msg_t)) < 0) {
        return -1;
    }

    return 0;
}

static int
sln_ipc_ser_accept(int listenfd)
{
    int                 connfd;
    ssize_t             recvlen;
    ipc_sock_msg_t      recv_msg;
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

        if ((recvlen = sln_ipc_recv(connfd, &recv_msg, sizeof(ipc_sock_msg_t))) < 0) {
            continue;
        }

        sln_ser_handle(connfd, &recv_msg);

        close(connfd);
    }

    return 0;
}

int sln_ipc_ser_loop(void)
{
    int fd;

#if USE_AF_UNIX
    fd = sln_ipc_ser_afunix_listen(SOCK_IPC_NAME);

    if (fd < 0) {
        return -1;
    }

#else
    fd = sln_ipc_ser_afinet_listen(SOCK_IPC_SER_LISTEN_PORT);

    if (fd < 0) {
        return -1;
    }

#endif

    if (sln_ipc_ser_accept(fd) < 0) {
        return -1;
    }

    return 0;
}

int sln_ipc_ser_func_init(void)
{
    int     i;

    for (i = 0; i < SLN_IPC_MAX_TYPE; i++) {
        sln_ipc_ser_func[i] = NULL;
    }

    sln_ipc_ser_func[SLN_IPC_TYPE_0x1] = sln_ipc_handle_0x1;
    sln_ipc_ser_func[SLN_IPC_TYPE_0x2] = sln_ipc_handle_0x2;

    return 0;
}

int main(int argc, const char* argv[])
{
    sln_ipc_ser_func_init();
    sln_ipc_ser_loop();

    return 0;
}
