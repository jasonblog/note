#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<sys/types.h>
#include<sys/socket.h>
#include<sys/un.h>
#include<errno.h>

#define SOCK_FILE "yangzd"
int create_local_sock(char* sockfile)
{
    int local_fd = 0;
    struct sockaddr_un serveraddr;

    if (-1 == (local_fd = socket(AF_LOCAL, SOCK_STREAM, 0))) {
        perror("socket");
        exit(EXIT_FAILURE);
    }

    bzero(&serveraddr, sizeof(serveraddr));
    serveraddr.sun_family = AF_LOCAL;
    strncpy(serveraddr.sun_path, sockfile, strlen(sockfile));

    if (-1 == connect(local_fd, (struct sockaddr*)&serveraddr,
                      sizeof(serveraddr))) {
        perror("connect");
        exit(EXIT_FAILURE);
    }

    return local_fd;
}

static int recv_fd(int fd, int* fd_to_recv, char* buf, int len)
{
    struct cmsghdr* cmsg;

    cmsg = alloca(sizeof(struct cmsghdr) + sizeof(fd_to_recv));
    cmsg->cmsg_len = sizeof(struct cmsghdr) + sizeof(fd_to_recv);
    cmsg->cmsg_level = SOL_SOCKET;
    cmsg->cmsg_type = SCM_RIGHTS;


    struct msghdr msg;
    msg.msg_control = cmsg;
    msg.msg_controllen = cmsg->cmsg_len;
    msg.msg_name = NULL;
    msg.msg_namelen = 0;

    struct iovec iov[3];
    iov[0].iov_base = buf;
    iov[0].iov_len = len;

    msg.msg_iov = iov;
    msg.msg_iovlen = 3;

    if (recvmsg(fd, &msg, 0) < 0) {
        printf("recvmsg error, errno is %d\n", errno);
        fprintf(stderr, "recvmsg failed.  errno : %s\n", strerror(errno));
        return errno;
    }

    memcpy(fd_to_recv, CMSG_DATA(cmsg), sizeof(fd_to_recv));

    if (msg.msg_controllen != cmsg->cmsg_len) {
        *fd_to_recv = -1;
    }

    return 0;
}

int main(int argc, char* argv[])
{
    int sock_fd = 0;
    int file_fd;
    char* ptr = "now write data to file\n";
    char buf[129];
    memset(buf, '\0', 128);
    sock_fd = create_local_sock(SOCK_FILE);
    recv_fd(sock_fd, &file_fd, buf, 128);
    write(file_fd, ptr, strlen(ptr));
    printf("recv message:%s\n", buf);
    unlink(SOCK_FILE);
}