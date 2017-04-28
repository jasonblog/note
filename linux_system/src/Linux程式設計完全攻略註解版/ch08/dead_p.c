#include<stdio.h>
#include<unistd.h>
#include<stdlib.h>

int main()
{
    pid_t pid;

    if ((pid = fork()) == -1) {
        perror("fork");
    } else if (pid == 0) {
        printf("child_pid pid=%d\n", getpid());
        exit(0);
    }

    sleep(3);
    system("ps");
    exit(0);
}
