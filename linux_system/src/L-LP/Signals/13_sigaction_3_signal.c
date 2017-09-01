#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>

typedef void SigFunc(int);

/* Versione affidabile della funzione signal(), implementata mediante
sigaction() */
SigFunc* signal_rel(int signo, SigFunc* func);
static void handler(int signo);

int main(int argc, char* argv[])
{

    /* Si utilizza la versione affidabile della funzione signal() per la
    cattura del segnale SIGUSR1 */
    if (signal_rel(SIGUSR1, handler) == SIG_ERR) {
        fprintf(stderr, "Err.(%s) can't catch SUGUSR1\n", strerror(errno));
        exit(EXIT_FAILURE);
    }

    while (1) {
        pause();
    }

    return (EXIT_SUCCESS);
}

SigFunc* signal_rel(int signo, SigFunc* func)
{
    struct sigaction new_act, old_act;

    new_act.sa_handler = func;
    sigemptyset(&new_act.sa_mask);
    new_act.sa_flags = 0;

    if (sigaction(signo, &new_act, &old_act) < 0) {
        return (SIG_ERR);
    }

    return (old_act.sa_handler);
}

static void handler(int signo)
{
    if (signo == SIGUSR1) {
        write(0, "SIGUSR1 captured\n", 17);
    }
}
