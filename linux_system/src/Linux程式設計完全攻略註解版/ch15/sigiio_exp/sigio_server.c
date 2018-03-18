#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <string.h>
#include <signal.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/ioctl.h>
#define MAX_LENTH 1500

static int nqueue = 0;
void sigio_handler(int signum)
{
    if (signum == SIGIO) {
        nqueue++;
    }

    printf("signum=%d,nqueue=%d\n", signum, nqueue);

    return;
}

static recv_buf[MAX_LENTH];

int main(int argc, char* argv[])
{
    int sockfd, on = 1;
    struct sigaction action;
    sigset_t newmask, oldmask;
    struct sockaddr_in ser_addr;

    if (argc != 3) {
        printf("use: %s ip_add port\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    memset(&ser_addr, 0, sizeof(ser_addr));
    ser_addr.sin_family = AF_INET;

    if (inet_aton(argv[1], (struct in_addr*) & ser_addr.sin_addr.s_addr) == 0) {
        perror(argv[1]);
        exit(EXIT_FAILURE);
    }

    ser_addr.sin_port = htons(atoi(argv[2]));

    if ((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) == -1) {
        perror("Create socket failed");
        exit(EXIT_FAILURE);
    }

    if (bind(sockfd, (struct sockaddr*)&ser_addr, sizeof(ser_addr)) == -1) {
        perror("Bind socket failed");
        exit(EXIT_FAILURE);
    }

    memset(&action, 0, sizeof(action));
    action.sa_handler = sigio_handler;
    action.sa_flags = 0;
    sigaction(SIGIO, &action, NULL);

    if (fcntl(sockfd, F_SETOWN, getpid()) == -1) {
        perror("Fcntl F_SETOWN ");
        exit(EXIT_FAILURE);
    }

    if (ioctl(sockfd, FIOASYNC, &on) == -1) {
        perror("Ioctl FIOASYNC");
        exit(EXIT_FAILURE);
    }

    sigemptyset(&oldmask);
    sigemptyset(&newmask);
    sigaddset(&newmask, SIGIO);
    printf("get ready\n");

    while (1) {
        int len;
        sigprocmask(SIG_BLOCK, &newmask, &oldmask);

        while (nqueue == 0) {
            sigsuspend(&oldmask);
        }

        memset(recv_buf, '\0', MAX_LENTH);
        len = recv(sockfd, recv_buf, MAX_LENTH, MSG_DONTWAIT);

        if (len == -1 && errno == EAGAIN) {
            nqueue = 0;
        }

        sigprocmask(SIG_SETMASK, &oldmask, NULL);

        if (len >= 0) {
            printf("recv %d byte(s),msg is %s\n", len, recv_buf);
        }
    }
}

