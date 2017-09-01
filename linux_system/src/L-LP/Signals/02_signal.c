#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>

/*
L'interfaccia piu' semplice relativa alla gestione dei segnali nei sistemi unix-
like e' la funzione signal(); tale funzione e' standard ISO C, il quale pero'
non  considera ne' il multitasking, ne' il process group, ne' il terminal I/O e
cosi' via, pertanto la funzione signal() e' diventata nel tempo inutile nei
sistemi unix-like, e poiche' la semantica differisce totalmente da un sistema
all'altro si consiglia di adoperare la funzione sigaction().

La funzione signal() e' trattata per puro scopo didattico.

HEADER    : <signal.h>
PROTOTYPE : typedef void (*sighandler_t)(int);
            sighandler_t signal(int signum, sighandler_t handler);
SEMANTICS : La funzione signal() associa o installa la funzione di gestione
            'handler' al segnale 'signum'
RETURNS   : Il gestore precedente in caso di successo, SIG_ERR in caso di errore
--------------------------------------------------------------------------------
Nota: si e' utilizzata la typedef per rendere il prototipo della funzione
      meno ostico da leggere ed interpretare, il tipo di dato sighandler_t e'
      una estensione GNU delle glibc.

Il prototipo della funzione e' caratterizzato da due argomenti:
- 'signum', un intero indicante il signal number;
- 'handler' un puntatore ad una funzione void che riceve come argomento un
   intero. Il kernel puo' essere informato su una funzione deputata al
   trattamento del segnale, tale funzione e' definita "signal handler", gestore
   del segnale. Non restituisce alcun risultato.

Il valore della funzione handler puo' essere:
- l'indirizzo della funzione da chiamare al verificarsi del segnale;
- la costante SIG_IGN, per ignorare il segnale;
- la costante SIG_DFL, per l'azione di default del segnale.

Se si specifica l'indirizzo della funzione si sta parlando di "signal handler" o
di "signal-catching function" - gestore del segnale o funzione per la cattura
di un segnale.

La funzione restituisce l'indirizzo dell'azione precedente, che puo' essere
salvato per poi essere riutilizzato in un secondo tempo, ovvero quando il
processo tornera' dalla funzione di gestione del segnale e proprio nel punto in
cui era avvenuta l'eccezione.

Dura nel tempo l'associazione segnale/handler?
----------------------------------------------
Talune implementazioni di Unix, tra cui BSD e SyVr.3, prevedono che l'azione
rimanga installata anche dopo la ricezione del segnale, in altre invece, come
SysV prime versioni, dopo l'attivazione dell'handler si ripristina
automaticamente l'azione di default. In questi casi, per riagganciare il segnale
all'handler e' necessario reinstallare nuovamente il segnale nel corpo dell'
handler stesso.
*/

static void signal_handler(int signum);

/* Semplice programma per l'intercettazione di 3 segnali, SIGUSR1, SIGUSR2 e
SIGINT. Una volta compilato lo si esegue in background:
$ ./a.out &
[1] 5960

Con il comando kill si inviano i primi due segnali:
$ kill -USR1 5960
Ricevuto il segnale: SIGUSR1
$ kill -USR2 5960
Ricevuto il segnale: SIGUSR2

Ora si porta il programma da background a foreground, e poiche' vi e' un ciclo
infinito il programma restera' in esecuzione, in attesa di essere terminato;
si prova a terminarlo con la combinazione di tasti Control-C
$ fg %1
./a.out
^C Ricevuto il segnale: SIGINT
^C Ricevuto il segnale: SIGINT

Come si puo' ben notare il segnale SIGINT (^C) e' stato intercettato, a questo
punto per terminarlo non resta altro da fare che stopparlo nuovamente
con Control-Z, dopodiche' terminarlo con il comando kill e il segnale SIGKILL:

^Z
[1]+ Stopped ./a.out
$ kill -9 5960

Nota: D'ora in avanti per indicare, ad esempio, Control-C, si utilizzera' ^C,
      molto piu' chiaro.
*/
int main(int argc, char* argv[])
{
    /* I segnali SIGUSR1 e SIGUSR2 sono segnali a disposizione dell'utente per
    scopi personali, SIGINT invece e' un segnale di terminazione corrispondente
    ai tasti ^C. */
    if (signal(SIGUSR1, signal_handler) == SIG_ERR) {
        fprintf(stderr, "Err.(%s), can't catch SIGUSR1\n", strerror(errno));
        exit(EXIT_FAILURE);
    }

    if (signal(SIGUSR2, signal_handler) == SIG_ERR) {
        fprintf(stderr, "Err.(%s), can't catch SIGUSR2\n", strerror(errno));
        exit(EXIT_FAILURE);
    }

    if (signal(SIGINT, signal_handler) == SIG_ERR) {
        fprintf(stderr, "Err.(%s), can't catch SIGINT\n", strerror(errno));
        exit(EXIT_FAILURE);
    }

    /* La funzione pause() pone in attesa il processo in attesa di un segnale */
    while (1) {
        pause();
    }

    return (EXIT_SUCCESS);
}

static void signal_handler(int signum)
{
    if (signum == SIGUSR1) {

        /* Reinstalla l'azione di default per il segnale SIGUSR1 */
        if (signal(SIGUSR1, SIG_DFL) == SIG_ERR) {
            fprintf(stderr, "Err.(%s), can't catch SIGUSR1\n", strerror(errno));
            exit(EXIT_FAILURE);
        }

        printf("Ricevuto il segnale: SIGUSR1\n");

    } else if (signum == SIGUSR2) {
        printf("Ricevuto il segnale: SIGUSR2\n");
    } else if (signum == SIGINT) {
        printf("Ricevuto il segnale: SIGINT\n");
    } else {
        abort();
    }
}
