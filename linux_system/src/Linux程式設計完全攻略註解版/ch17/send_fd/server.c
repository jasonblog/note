#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<sys/types.h>
#include<sys/socket.h>
#include<sys/un.h>
#include<errno.h>
#include<fcntl.h>

#define SOCK_FILE "yangzd"
int create_local_sock(char* sockfile)
{
    int local_fd;
    struct sockaddr_un myaddr;

    if (-1 == (local_fd = socket(AF_LOCAL, SOCK_STREAM, 0))) {
        perror("socket");
        exit(EXIT_FAILURE);
    }

    bzero(&myaddr, sizeof(myaddr));
    myaddr.sun_family = AF_LOCAL;
    strncpy(myaddr.sun_path, sockfile, strlen(sockfile));

    if (-1 == bind(local_fd, (struct sockaddr*)&myaddr, sizeof(myaddr))) {
        perror("bind");
        exit(EXIT_FAILURE);
    }

    if (-1 == listen(local_fd, 5)) {
        perror("listen");
        exit(EXIT_FAILURE);
    }

    int new_fd;
    struct sockaddr_un peeraddr;
    int len = sizeof(peeraddr);
    new_fd = accept(local_fd, (struct sockaddr*)&peeraddr, &len);

    if (-1 == new_fd) {
        perror("accept");
        exit(EXIT_FAILURE);
    }

    return new_fd;
}

send_fd(int sock_fd, char* file)
{
    int fd_to_send;

    if (-1 == (fd_to_send = open(file, O_RDWR | O_APPEND))) {
        perror("open");
        exit(EXIT_FAILURE);
    }

    struct cmsghdr* cmsg;

    cmsg = alloca(sizeof(struct cmsghdr) + sizeof(fd_to_send));

    cmsg->cmsg_len = sizeof(struct cmsghdr) + sizeof(fd_to_send);

    cmsg->cmsg_level = SOL_SOCKET;

    cmsg->cmsg_type = SCM_RIGHTS;

    memcpy(CMSG_DATA(cmsg), &fd_to_send, sizeof(fd_to_send));


    struct msghdr msg;

    msg.msg_control = cmsg;

    msg.msg_controllen = cmsg->cmsg_len;

    msg.msg_name = NULL;

    msg.msg_namelen = 0;

    struct iovec iov[3];

    iov[0].iov_base = "hello ";

    iov[0].iov_len = strlen("hello ");

    iov[1].iov_base = "this is yangzd, ";

    iov[1].iov_len = strlen("this is yangzd, ");

    iov[2].iov_base = "and you?";

    iov[2].iov_len = strlen("and you?");

    msg.msg_iov = iov;

    msg.msg_iovlen = 3;

    if (sendmsg(sock_fd, &msg, 0) < 0) {
        printf("sendmsg error, errno is %d\n", errno);
        fprintf(stderr, "sendmsg failed.  errno : %s\n", strerror(errno));
        return errno;
    }

    return 1;
}

int main(int argc, char* argv[])
{
    int sock_fd = 0;
    unlink(SOCK_FILE);

    if (argc != 2) {
        printf("pls usage %s file_send\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    sock_fd = create_local_sock(SOCK_FILE);

    if (send_fd(sock_fd, argv[1]) != 1) {
        printf("send error");
        exit(EXIT_FAILURE);
    }
}