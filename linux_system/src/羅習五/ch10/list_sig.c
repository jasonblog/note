#include <stdlib.h>
#include <signal.h>
#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>

void sighandler(int signumber) {
    printf("get a signal named '%d', '%s'\n", signumber,
           sys_siglist[signumber]);
}

int main(int argc, char **argv) {
    int sig_exist[100];
    int idx = 0;
    for (idx = 0; idx < 100; idx++) {
        if (signal(idx, sighandler) == SIG_ERR) {
            sig_exist[idx] = 0;
        } else {
            sig_exist[idx] = 1;
        }
    }
    for (idx = 0; idx < 100; idx++) {
        if (sig_exist[idx] == 1)
            printf("%2d %s\n", idx, sys_siglist[idx]);
    }
    printf("my pid is %d\n", getpid());
    printf("press any key to resume\n");
    getchar();
    return 0;
}


