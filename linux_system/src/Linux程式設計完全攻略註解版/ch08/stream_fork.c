#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>

int main(int argc, char* argv[])
{
    pid_t pid;
    printf("before fork,have enter\n");
    printf("before fork,no enter:pid=%d\t", getpid());
    pid = fork();

    if (pid == 0) {
        printf("\nchild,after fork:pid=%d\n", getpid());
    } else {
        printf("\nparent,after fork:pid=%d\n", getpid());
    }
}
