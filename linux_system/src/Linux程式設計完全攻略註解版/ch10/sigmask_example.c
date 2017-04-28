#include<stdio.h>
#include<signal.h>
#include<stdlib.h>

static void sig_quit(int);

int main(int argc, char* argv[])
{
    sigset_t    newmask, oldmask, pendmask;

    if (signal(SIGQUIT, sig_quit) == SIG_ERR) {
        perror("signal");
        exit(EXIT_FAILURE);
    }

    printf("install sig_quit\n");

    // Block SIGQUIT and save current signal mask.
    sigemptyset(&newmask);
    sigaddset(&newmask, SIGQUIT);

    if (sigprocmask(SIG_BLOCK, &newmask, &oldmask) < 0) {
        perror("signal");
        exit(EXIT_FAILURE);
    }

    printf("Block SIGQUIT,wait 15 second\n");
    sleep(15);   /* SIGQUIT here will remain pending */

    if (sigpending(&pendmask) < 0) {
        perror("signal");
        exit(EXIT_FAILURE);
    }

    if (sigismember(&pendmask, SIGQUIT)) {
        printf("\nSIGQUIT pending\n");
    }

    if (sigprocmask(SIG_SETMASK, &oldmask, NULL) < 0) {
        perror("signal");
        exit(EXIT_FAILURE);
    }

    printf("SIGQUIT unblocked\n");

    sleep(15);   /* SIGQUIT here will terminate with core file */
    return 0;
}

static void sig_quit(int signo)
{
    printf("caught SIGQUIT,the process will quit\n");

    if (signal(SIGQUIT, SIG_DFL) == SIG_ERR) {
        perror("signal");
        exit(EXIT_FAILURE);
    }
}

