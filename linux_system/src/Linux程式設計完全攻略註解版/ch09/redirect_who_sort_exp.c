#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/wait.h>
#include <stdlib.h>
int main(int argc, char* argv[])
{
    int fds[2];

    if (pipe(fds) == -1) {
        perror("pipe");
        exit(EXIT_FAILURE);
    }

    if (fork() == 0) {
        char buf[128];
        dup2(fds[0], 0);
        close(fds[1]);              //must include ,or block
        execlp("sort", "sort", (char*)0);
        //execlp("cat", "cat", (char *)0);
    } else {
        if (fork() == 0) {
            dup2(fds[1], 1);
            close(fds[0]);
            execlp("who", "who", (char*)0);
        } else {
            close(fds[0]);
            close(fds[1]);
            wait(NULL);
            wait(NULL);
        }
    }

    return 0;
}
