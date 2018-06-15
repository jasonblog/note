#include <stdlib.h>
#include <signal.h>
#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>
#include <assert.h>

volatile char *c; 

void sighandler(int signumber) {
    printf("get a signal named '%d', '%s'\n", signumber,
           sys_siglist[signumber]);
    c=malloc(sizeof(char));
}

int main(int argc, char **argv) {
    assert(signal(SIGSEGV, sighandler) != SIG_ERR);  
    *c = 'a';
    printf("my pid is %d\n", getpid());
    printf("press any key to resume\n");
    getchar();
    return 0;
}


