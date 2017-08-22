#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <signal.h>

/*
Insieme di segnali o "Signal set"[1]
------------------------------------
Per evitare le problematiche relative alla gestione inaffidabile dei segnali
POSIX ha realizzato una nuova interfaccia che consente una gestione ed un
controllo molto piu' accurato; in particolare e' stato introdotto il tipo di
dato 'sigset_t' "signal set" o insieme di segnali, mediante il quale si possono
rappresentare segnali multipli.  Il tipo di dato 'sigset_t' e' utilizzato per
gestire il blocco dei segnali.

Lo standard POSIX definisce cinque funzioni per la gestione e la manipolazione
di un "signal set":

HEADER    : <signal.h>
PROTOTYPE : int sigemptyset(sigset_t *set);
SEMANTICS : La funzione sigemptyset() inizializza l'insieme di segnali puntato
            da 'set', con un insieme vuoto, escludendo tutti i segnali
RETURNS   : 0 in caso di successo, -1 in caso di errore
--------------------------------------------------------------------------------
HEADER    : <signal.h>
PROTOTYPE : int sigfillset(sigset_t *set);
SEMANTICS : La funzione sigfillset() inizializza l'insieme di segnali puntato
            da 'set', con un insieme pieno, includendo tutti i segnali
RETURNS   : 0 in caso di successo, -1 in caso di errore
--------------------------------------------------------------------------------
HEADER    : <signal.h>
PROTOTYPE : int sigaddset(sigset_t *set, int signo);
SEMANTICS : La funzione sigaddset() aggiunge un singolo segnale, indicato dalla
            variabile intera 'signo', a un insieme esistente.
RETURNS   : 0 in caso di successo, -1 in caso di errore
--------------------------------------------------------------------------------
HEADER    : <signal.h>
PROTOTYPE : int sigdelset(sigset_t *set int signo);
SEMANTICS : La funzione sigdelset() rimuove un singolo segnale, indicato dalla
            variabile intera 'signo', da un insieme esistente.
RETURNS   : 0 in caso di successo, -1 in caso di errore
--------------------------------------------------------------------------------
HEADER    : <signal.h>
PROTOTYPE : int sigismember(const sigset_t *set, int signo);
SEMANTICS : La funzione sigismember() verifica se il segnale indicato da 'signo'
            appartiene all'insieme di segnali 'set'.
RETURNS   : 1 se true, 0 se false, -1 in caso di errore
--------------------------------------------------------------------------------
*/

int main(int argc, char* argv[])
{
    sigset_t test_set;

    /* Si provvede alla inizializzazione creando un insieme vuoto */
    sigemptyset(&test_set);


    /* Si aggiunge un segnale all'insieme precedentemente creato */
    sigaddset(&test_set, SIGUSR1);

    if (sigismember(&test_set, SIGUSR1)) {
        printf("Segnale %d presente nel set\n", SIGUSR1);
    } else {
        printf("SIGUSR1 non presente\n");
    }

    /* Si elimina il segnale precedenemente inserito nella lista */
    sigdelset(&test_set, SIGUSR1);

    return (EXIT_SUCCESS);
}
/*
[1] "Set" e "Insieme" sono utilizzati in maniera del tutto intercambiabile ed
equivalente nel corso del sorgente */
