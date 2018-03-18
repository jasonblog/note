#include<stdio.h>
#include<stdlib.h>
#include<signal.h>
#include<unistd.h>

void func(int signo, siginfo_t* info, void* p)
{
    printf("signo=%d\n", signo);
    printf("sender pid=%d\n", info->si_pid);
}

int main(int argc, char* argv[])
{
    struct sigaction act, oact;

    sigemptyset(&act.sa_mask); /*initial. to empty mask*/
    act.sa_flags = SA_SIGINFO;
    act.sa_sigaction = func;
    sigaction(SIGUSR1, &act, &oact);

    while (1) {
        printf("pid is %d Hello world.\n", getpid());
        pause();
    }
}

