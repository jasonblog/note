#include<stdio.h>
#include<sys/wait.h>
#include<sys/types.h>
#include<unistd.h>
#include<sys/errno.h>
#include<stdlib.h>

extern int errno;
int main(int argc, char* argv[])
{
    pid_t pid_one, pid_wait;
    int status;

    if ((pid_one = fork()) == -1) {
        perror("fork");
    }

    if (pid_one == 0) {
        printf("my pid is %d\n", getpid());
        sleep(1);
        exit(EXIT_SUCCESS);
    } else {
        pid_wait = wait(&status);

        if (WIFEXITED(status)) {
            printf("wait on pid:%d,return value is:%4x\n", pid_wait, WEXITSTATUS(status));
        } else if (WIFSIGNALED(status)) {
            printf("wait on pid:%d,return value is:%4x\n", pid_wait, WIFSIGNALED(status));
        }
    }

    return 0;
}
