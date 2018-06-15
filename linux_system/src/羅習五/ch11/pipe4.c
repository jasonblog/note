/*
usage: pipe4
*/

#include <assert.h>
#include <unistd.h>
#include <stdio.h>
int main(int argc, char **argv) {
    int pipefd[2];
    char buf[200];
    FILE *in_stream;
    int ret;
    pipe(pipefd);
    ret = fork();
    if (ret==0) {	/*child 1*/
        printf("I am child 1\n");
        close(1); dup(pipefd[1]);
        close(pipefd[1]); close(pipefd[0]);
        printf("send by pipe");
    }
    if (ret>0) {
        ret = fork();
        if (ret==0) {	/*child 2*/
            close(0); dup(pipefd[0]);
            close(pipefd[1]); close(pipefd[0]);
            in_stream=fdopen(0, "r");
            /*用fgets取代gets*/
            fgets(buf, 200, in_stream);
            printf("child 2: %s\n", buf);
        }
    }
}

