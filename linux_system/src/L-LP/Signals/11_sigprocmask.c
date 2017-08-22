#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>

/*
La "signal mask" o "maschera del segnale" e' l'insieme dei segnali bloccati,
molto utili quando e' necessario proteggere una sezione di codice, inibendo
per l'appunto l'esecuzione di taluni segnali.

Un processo puo' sia esaminare la propria signal mask sia modificarla oppure
puo' eseguire entrambe le operazioni mediante la funzione sigprocmask().

HEADER    : <signal.h>
PROTOTYPE : int sigprocmask(int how, const sigset_t *set, sigset_t *oldset);
SEMANTICS : La funzione sigprocmask() modifica la signal mask di un processo.
            - 'how' indica il tipo di modifica da apportare all'insieme di
           segnali indicato da 'set';
        - 'set' indica l'insieme di segnali su cui intervenerire in base a
           quanto indicato da 'how';
        - 'oldset' riceve l'insieme originale di segnali prima della
           modifica.
RETURNS   : 0 in caso di successo, -1 in caso di errore
--------------------------------------------------------------------------------
- Se 'oldset' e' un puntatore non nullo, la signal mask corrente per il processo
  e' ritornata mediante 'oldset';

- Se 'set' e' un puntatore non nullo, l'argomento 'how' indica come la signal
  mask corrente sara' modificata; i possibili valori di 'how' possono essere:

  SIG_BLOCK  , i segnali che appartengono all'insieme vengono aggiunti
               all'elenco di quelli inibiti. Eventuali segnali generati nel
           corso del periodo di inibizione resteranno pendenti o sospesi
           fino a quando non saranno riattivati;

  SIG_UNBLOCK, i segnali che appartengono all'insieme saranno rimossi dall'
               elenco di quelli inibiti, ovvero riattivati. Se nell'insieme vi
           sono segnali non bloccati non vi saranno effetti negativi;

  SIG_SETMASK, i segnali che appartengono all'insieme saranno indentificati come
               gli unici da bloccare; questo corrisponde alla sostituzione dell'
           insieme corrente di segnali bloccati con l'insieme indicato da
           'set'.

*/

volatile sig_atomic_t quitflag;

int main(void)
{
    void  sig_int(int);
    sigset_t  newmask, oldmask, zeromask;

    if (signal(SIGINT, sig_int) == SIG_ERR) {
        fprintf(stderr, "Err.(%s) signal(SIGINT) failed\n", strerror(errno));
        exit(EXIT_FAILURE);
    }

    if (signal(SIGQUIT, sig_int) == SIG_ERR) {
        fprintf(stderr, "Err.(%s) signal(SIGQUIT) failed\n", strerror(errno));
        exit(EXIT_FAILURE);
    }

    sigemptyset(&zeromask);
    sigemptyset(&newmask);
    sigaddset(&newmask, SIGQUIT);

    /* blocca SIGQUIT e salva la maschera dei segnali corrente */
    if (sigprocmask(SIG_BLOCK, &newmask, &oldmask) < 0) {
        fprintf(stderr, "Err.(%s) SIG_BLOCK failed\n", strerror(errno));
        exit(EXIT_FAILURE);
    }

    while (quitflag == 0) {
        sigsuspend(&zeromask);
    }

    /* SIGQUIT e' stato catturato ed e' ora bloccato */
    quitflag = 0;

    /* resetta la maschera dei segnali che sblocca SIGQUIT */
    if (sigprocmask(SIG_SETMASK, &oldmask, NULL) < 0) {
        fprintf(stderr, "Err.(%s) SIG_SETMASK failed\n", strerror(errno));
        exit(EXIT_FAILURE);
    }

    exit(EXIT_SUCCESS);
}

/* un gestore dei segnali per SIGINT e SIGQUIT */
void sig_int(int signo)
{
    if (signo == SIGINT) {
        printf("\ninterrupt\n");
    } else if (signo == SIGQUIT)
        /* setta il flag per il ciclo principale*/
    {
        quitflag = 1;
    }

    return;
}
