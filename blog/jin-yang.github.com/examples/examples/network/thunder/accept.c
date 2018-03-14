
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <netinet/in.h>

#define PROCESS_NUM 10

int main()
{
    int fd = socket(PF_INET, SOCK_STREAM, 0);
    int connfd;
    int pid, i, status;
    char sendbuff[1024];
    struct sockaddr_in serveraddr;

    printf("Listening 0.0.0.0:1234\n");
    serveraddr.sin_family = AF_INET;
    serveraddr.sin_addr.s_addr = htonl(INADDR_ANY);
    serveraddr.sin_port = htons(1234);
    bind(fd, (struct sockaddr*)&serveraddr, sizeof(serveraddr));
    listen(fd, 1024);
    for(i = 0; i < PROCESS_NUM; i++) {
        pid = fork();
        if(pid == 0) {
            while(1) {
                connfd = accept(fd, (struct sockaddr*)NULL, NULL);
                snprintf(sendbuff, sizeof(sendbuff), "accept PID is %d\n", getpid());

                send(connfd, sendbuff, strlen(sendbuff) + 1, 0);
                printf("process %d accept success!\n", getpid());
                close(connfd);
            }
        }
    }
    wait(&status);
    return 0;
}
