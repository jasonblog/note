#include <unistd.h>
#include <signal.h>
#include <stdio.h>
#include <assert.h>
void sighandler(int signumber) {
    printf("get a signal named '%d', '%s'\n", signumber, sys_siglist[signumber]);
}

int main(int argc, char **argv) {
    sigset_t sigset;
    assert(signal(SIGQUIT, sighandler) != SIG_ERR);
    /*終止所有的signal*/
    sigfillset(&sigset);
    sigprocmask(SIG_SETMASK, &sigset, NULL);
    /*睡10秒鐘*/
    printf("sleep 10sec\n");
    for(int i=0; i<10; i++) {
    	sleep(1); write(1, "*", 1);
    }
    printf("\n");
    /*重新啓動所有的signal*/
    sigemptyset(&sigset);
    sigprocmask(SIG_SETMASK, &sigset, NULL);
    while (1) {
	    pause();
    }
}
