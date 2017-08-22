#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>

/*
E' possibile utilizzare la signal mask per bloccare o sbloccare specifici
segnali, tale tecnica e' adoperata per proteggere, dalla eventuale interruzione
di segnali, talune sezioni critiche di codice.

Una volta appurato, invocando sigpending(), che un segnale e' pendente, e'
necessario un modo affidabile per riattivare il segnale in oggetto e
consentirne la generazione; la funzione sigsuspend() si occupa proprio di
questo.

HEADER    : <signal.h>
PROTOTYPE : int sigsuspend(const sigset_t *mask);
SEMANTICS : Imposta la signal mask specificata, mettendo in pausa il processo
RETURNS   : -1
--------------------------------------------------------------------------------
La funzione sigsuspend() attiva temporaneamente la maschera dei segnali definita
nel parametro 'mask', dopodiche' attende che il segnale venga elevato; se la
maschera consente la generazione del segnale, avviene immediatamente.

Una volta completata l'azione associata al segnale, si ripristina la maschera
dei segnali originale nello stato in cui era prima della chiamata a
sigsuspend(); tale funzione e' un modo estremamente efficace per per verificare
quando un segnale viene generato.
*/

static void sig_int(int signo);
void pr_mask(const char* str);

/* Il programma mostra il modo corretto di proteggere una sezione critica di
codice da uno specifico segnale */

int main(int argc, char* argv[])
{
    sigset_t new_mask, old_mask, wait_mask;

    pr_mask("program start: ");

    if (signal(SIGINT, sig_int) == SIG_ERR) {
        fprintf(stderr, "Err.(%s) signal(SIGINT)\n", strerror(errno));
        exit(EXIT_FAILURE);
    }

    sigemptyset(&wait_mask);
    sigaddset(&wait_mask, SIGUSR1);
    sigemptyset(&new_mask);
    sigaddset(&new_mask, SIGUSR1);

    /* Blocca SIGINT e salva la maschera corrente del segnale */
    if (sigprocmask(SIG_BLOCK, &new_mask, &old_mask) < 0) {
        fprintf(stderr, "Err.(%s) SIG_BLOCK\n", strerror(errno));
        exit(EXIT_FAILURE);
    }

    /* Regione critica */
    pr_mask("critical region: ");

    /* Pausa, tutti i segnali tranne SIGUSR1 */
    if (sigsuspend(&wait_mask) != -1) {
        fprintf(stderr, "Err.(%s) sigsuspend()\n", strerror(errno));
        exit(EXIT_FAILURE);
    }

    pr_mask("after sigsuspend return: ");

    /* Resetta la signal mask */
    if (sigprocmask(SIG_SETMASK, &old_mask, NULL) < 0) {
        fprintf(stderr, "Err.(%s) SIG_SETMASK\n", strerror(errno));
        exit(EXIT_FAILURE);
    }

    pr_mask("program exit: ");

    return (EXIT_SUCCESS);
}

static void sig_int(int signo)
{
    pr_mask("\n in sig_int: ");
}

void pr_mask(const char* str)
{
    sigset_t sigset;
    int errno_save;
    errno_save = errno;

    if (sigprocmask(0, NULL, &sigset) < 0) {
        fprintf(stderr, "sigprocmask error");
        exit(EXIT_FAILURE);
    }

    printf("%s", str);

    if (sigismember(&sigset, SIGINT)) {
        printf("SIGINT ");
    }

    if (sigismember(&sigset, SIGQUIT)) {
        printf("SIGQUIT ");
    }

    if (sigismember(&sigset, SIGUSR1)) {
        printf("SIGUSR1 ");
    }

    if (sigismember(&sigset, SIGALRM)) {
        printf("SIGALRM ");
    }

    printf("\n");
    errno = errno_save;
}
