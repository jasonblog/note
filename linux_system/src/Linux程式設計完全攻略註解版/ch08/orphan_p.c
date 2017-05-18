#include <sys/types.h>
#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>

int main()
{
    pid_t pid;

    if ((pid = fork()) == -1) {
        perror("fork");
    } else if (pid == 0) {
        printf("pid=%d,ppid=%d\n", getpid(), getppid());
        sleep(2);
        printf("pid=%d,ppid=%d\n", getpid(), getppid());
    } else {
        exit(0);
    }
}
