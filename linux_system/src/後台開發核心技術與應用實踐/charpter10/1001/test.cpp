#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
int main(void)
{
    pid_t pid;
    pid = fork();

    if (pid < 0) {
        perror("fail to fork");
        exit(-1);
    } else if (pid == 0) {
        /*子进程*/
        printf("Sub-process, PID: %u, PPID: %u\n", getpid(), getppid());
    } else { /*父进程*/
        printf("Parent, PID: %u, Sub-process PID: %u\n", getpid(), pid);
        sleep(2);
    }

    return 0;
}
