#include<stdio.h>
#include<signal.h>
#include<stdlib.h>

int output(sigset_t set)
{
    printf("set.val[0]=%x\n", set.__val[0]);
}
void handler(int sig)
{
    int i;
    sigset_t sysset;
    printf("\nin handler sig=%d\n", sig);
    sigprocmask(SIG_SETMASK, NULL, &sysset);
    output(sysset);         //in handler to see the process mask set
    printf("return\n");
}
int main(int argc, char* argv[])
{
    struct sigaction act;
    sigset_t set, sysset, newset;

    sigemptyset(&set);
    sigemptyset(&newset);
    sigaddset(&set, SIGUSR1);
    sigaddset(&newset, SIGUSR2);

    printf("\nadd SIGUSR1,the value of set:");
    output(set);

    printf("\nadd SIGUSR2,the value of newset:");
    output(newset);

    printf("\nafter set proc block set ,and then read to sysset\n");
    sigprocmask(SIG_SETMASK, &set, NULL);
    sigprocmask(SIG_SETMASK, NULL, &sysset);
    printf("system mask is:\n");
    output(sysset);

    printf("install SIGALRM,and the act.sa_mask is newset(SIGUSR2)\n");
    act.sa_handler = handler;
    act.sa_flags = 0;
    act.sa_mask = newset;
    sigaction(SIGALRM, &act, NULL);
    pause();

    printf("after exec ISR\n");
    sigemptyset(&sysset);
    sigprocmask(SIG_SETMASK, NULL, &sysset);
    output(sysset);
}

