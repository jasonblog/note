#include <signal.h>
#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>
#include <assert.h>
int main() {
    sigset_t sigset;
    int signo;
    sigfillset(&sigset);
    sigprocmask(SIG_SETMASK, &sigset, NULL);
    printf("pid = %d\n", getpid());
    while(1) {
        assert(sigwait(&sigset, &signo) == 0);
        printf("recv sig#%d\n", signo);
    }
}