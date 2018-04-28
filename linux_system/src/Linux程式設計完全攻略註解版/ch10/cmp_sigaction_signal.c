#include <stdlib.h>
#include <signal.h>
#include <stdio.h>

static void sig_usr1(signo)
{
    printf("SIGUSR1 function\n");
}
static void sig_usr2(signo)
{
    printf("SIGUSR2 function\n");
}

static void sig_alarm(signo)
{
    printf("SIGALRM function\n");
}
int main(void)
{

    sigset_t newmask, oldmask;

    /*-----signal ------------------*/
    if (signal(SIGUSR1, sig_usr1) < 0 | signal(SIGUSR2,
            sig_usr2) < 0 | signal(SIGALRM, sig_alarm) < 0) {
        perror("signal\n");
    }

    sigemptyset(&newmask);
    sigaddset(&newmask, SIGUSR1);
    sigaddset(&newmask, SIGUSR2);
    sigaddset(&newmask, SIGALRM);
    /*---------signal end--------------*/

    /*-----sigaction------------------*/
    /*struct sigaction act1,act2,act3;
    act1.sa_handler=sig_usr1;
    sigemptyset(&act1.sa_mask);
    act2.sa_handler=sig_usr2;
    sigemptyset(&act2.sa_mask);
    act3.sa_handler=sig_alarm;
    sigemptyset(&act3.sa_mask);

    sigaction(SIGUSR1,&act1,NULL);
    sigaction(SIGUSR2,&act2,NULL);
    sigaction(SIGALRM,&act3,NULL);
    */
    /*-----sigaction--end----------------*/

    sigprocmask(SIG_BLOCK, &newmask, &oldmask);
    printf("SIGUSR is blocked\n");
    kill(getpid(), SIGUSR2);
    kill(getpid(), SIGUSR1);
    kill(getpid(), SIGALRM);
    sigprocmask(SIG_SETMASK, &oldmask, NULL);
}



