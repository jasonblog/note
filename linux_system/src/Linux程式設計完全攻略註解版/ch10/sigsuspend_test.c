//test for sigsuspend
#include<signal.h>
#include<stdlib.h>
#include<errno.h>
#include<stdio.h>
void pr_mask(char* str)
{
    sigset_t sigset01;
    int      errno_save;
    errno_save = errno;

    if (sigprocmask(0, NULL, &sigset01) < 0) {
        perror("sigprocmask erro!");
    }

    printf("%s\n", str);

    if (sigismember(&sigset01, SIGINT)) {
        printf("SIGINT\n");
    }

    if (sigismember(&sigset01, SIGQUIT)) {
        printf("SIGQUIT\n");
    }

    if (sigismember(&sigset01, SIGUSR1)) {
        printf("SIGUSR1\n");
    }

    if (sigismember(&sigset01, SIGALRM)) {
        printf("SIGALRM\n");
    }

    errno = errno_save;
}
static void sig_int(int signo)
{
    printf("signo=%d\n", signo);
    pr_mask("\ntest :in sig_int\n");
}

int main(void)
{
    sigset_t    newmask, oldmask, waitmask;
    pr_mask("program start:");

    if (signal(SIGINT, sig_int) == SIG_ERR) {
        perror("signal(SIGINT) error!!\n");
    }

    if (signal(SIGUSR1, sig_int) == SIG_ERR) {
        perror("signal(SIGUSR1) error");
    }

    sigemptyset(&waitmask);
    sigaddset(&waitmask, SIGUSR1);
    sigemptyset(&newmask);
    sigaddset(&newmask, SIGINT);

    if (sigprocmask(SIG_BLOCK, &newmask, &oldmask) < 0) {
        perror("SIG_BLOCK  erro!!\n");
    }

    pr_mask("in critical region:");

    if (sigsuspend(&waitmask) != -1) {
        perror("sigsuspend  erro!!\n");
    }

    pr_mask("after return from sigsuspend:");

    if (sigprocmask(SIG_SETMASK, &oldmask, NULL) < 0) {
        perror("SIG-SETMASK erro!!\n");
    }

    while (1);

    pr_mask("\nprogram exit:\n");
    exit(0);
}

