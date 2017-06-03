#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
int main()
{
    /*fork一个子进程*/
    pid_t pid = fork();

    if (pid < 0) {
        perror("fork error\n");
        return 0;
    } else if (pid > 0) { /*父进程*/
        printf("Parent process\n");
        pid_t pr = wait(NULL);
        printf("Parent process, I catched a child process with pid of %d\n", pr);
    } else if (pid == 0) {
        printf("Sub-process, PID: %u, PPID: %u\n", getpid(), getppid());
        exit(0);
    }

    return 0;
}
