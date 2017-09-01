#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>

/*
Talvolta e' necessario esaminare i segnali pendenti.

HEADER    : <signal.h>
PROTOTYPE : int sigpending(sigset_t *set);
SEMANTICS : La funzione sigpending() consente di ricavare i segnali pendenti
            del processo in corso; l'insieme di segnali e' ritornato mediante
        l'argomento 'set'.
RETURNS   : 0 in caso di successo, -1 in caso di errore
--------------------------------------------------------------------------------
*/

static void handler(int sig_num);

int main(int argc, char* argv[])
{
    sigset_t new_mask, old_mask, pend_mask;

    if (signal(SIGQUIT, handler) == SIG_ERR) {
        fprintf(stderr, "Err.(%s) signal(SIGQUIT) failed\n", strerror(errno));
        exit(EXIT_FAILURE);
    }

    /* Il processo blocca il segnale SIGQUIT, salvando la maschera corrente del
    segnale, dopodiche' andra' in sleep per 5 secondi; qualsiasi occorrenza del
    segnale durante questo periodo sara' bloccato e non sara' consegnato fino a
    quando il segnale non sara' sbloccato. */
    if (sigemptyset(&new_mask) < 0) {
        fprintf(stderr, "Err.(%s) sigemptyset() failed\n", strerror(errno));
        exit(EXIT_FAILURE);
    }

    if (sigaddset(&new_mask, SIGQUIT) < 0) {
        fprintf(stderr, "Err.(%s) sigaddset() failed\n", strerror(errno));
        exit(EXIT_FAILURE);
    }

    if (sigprocmask(SIG_BLOCK, &new_mask, &old_mask) < 0) {
        fprintf(stderr, "Err.(%s) sigprocmask(SIG_BLOCK)\n", strerror(errno));
        exit(EXIT_FAILURE);
    }

    sleep(5);

    if (sigpending(&pend_mask) < 0) {
        fprintf(stderr, "Err.(%s) sigpending() failed\n", strerror(errno));
        exit(EXIT_FAILURE);
    }

    if (sigismember(&pend_mask, SIGQUIT) < 0) {
        fprintf(stderr, "Err.(%s) sigismember() failed\n", strerror(errno));
        exit(EXIT_FAILURE);
    }

    /* Resetta la maschera del segnale, sblocca SIGQUIT */
    if (sigprocmask(SIG_SETMASK, &old_mask, NULL) < 0) {
        fprintf(stderr, "Err.(%s) sigprocmask(SIG_SETMASK)\n", strerror(errno));
        exit(EXIT_FAILURE);
    }

    printf("\nSIGQUIT sbloccato\n");

    sleep(5);

    return (EXIT_SUCCESS);
}

static void handler(int sig_num)
{
    printf("Segnale SIGQUIT catturato\n");

    /* Si reinstalla il comportamento di default per il segnale SIGQUIT */
    if (signal(SIGQUIT, SIG_DFL) == SIG_ERR) {
        fprintf(stderr, "Err.(%s) signal(SIGQUIT) failed\n", strerror(errno));
        exit(EXIT_FAILURE);
    }
}
