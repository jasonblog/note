#include<stdio.h>
#include<sys/types.h>
#include<unistd.h>
int main(void)
{
    pid_t pid;

    if ((pid = fork()) == -1) {
        printf("fork error");
    } else if (pid == 0) {
        printf("in the child process\n");
    } else {
        printf("in the parent process\n");
    }

    return 0;
}
