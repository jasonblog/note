#include <stdlib.h>
#include <signal.h>
#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>

int nSig[100];

void sighandler(int signumber) {
    nSig[signumber]++;
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
    printf("my pid is %d\n", getpid());
    printf("press any key to count the signal number\n");
    getchar();
    for (idx=0; idx<100; idx++) {
        if (nSig[idx] != 0)
            printf("signal #%d, %d times\n", idx, nSig[idx]);
    }
    
    return 0;
}


