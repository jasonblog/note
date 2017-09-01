#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <unistd.h>

/* I Thread sono stati standardizzati nel 1995, IEEE POSIX 1003.1c ne specifica
l'interfaccia di programmazione - (API - Application Program Interface) -, i
thread POSIX vanno sotto il nome di Pthreads, ossia POSIX threads.

Le API possono essere classificate in:
- Thread management     funzioni per creare, eliminare, terminare e attendere
                        thread, in pratica la gestione;
- Mutex                 funzioni per la gestione della mutua esclusione, ossia
                        la sincronizzazione semplice;
- Condition variables   funzioni a supporto della sincronizzazione basata sul
                        settaggio di specifiche variabili, dette di condizione.

Gli identificatori della libreria Pthreads hanno adottato il suffisso pthread_
che contraddistingue ciascun nome di funzione, inoltre vi sono ulteriori
differenziazioni sui nomi stessi delle funzioni:
- pthread_              gestione dei thread in generale;
- pthread_attr_         gestione relative alla proprieta' dei thread;
- pthread_mutex_        gestione della mutua esclusione;
- pthread_mutexattr_    proprieta' delle strutture per la mutua esclusione;
- pthread_cond_         gestione delle variabili di condizione;
- pthread_condattr_     proprieta' delle variabili di condizione;
- pthread_key_          dati speciali dei thread.

Gestione degli errori
---------------------
Poiche' i thread condividono la memoria, e' altamente consigliato di non
utilizzare una variabile globale come 'errno' per la gestione degli errori, ma
piuttosto di adoperare il codice d'errore restituito da ciascuna funzione della
famiglia POSIX THREADS.

Per verificare se il sistema supporta i thread vi sono due strade:

1 - a compile-time, utilizzando le direttive al preprocessore per la
    compilazione condizionale, nel caso specifico la macro _POSIX_THREADS;

2 - a run-time, mediante la funzione sysconf e la costante _SC_THREADS. */

/* Il programma verifica se il sistema supporta i thread mediante ambedue le
strade */

int main(void)
{
    long res;

#ifdef _POSIX_THREADS
    printf("IEEE POSIX 1003.1c \"pthreads\" supportati\n");
#endif

    if ((res = sysconf(_SC_THREADS)) == -1) {
        fprintf(stderr, "Err.(%d) - sysconf; %s\n", errno, strerror(errno));
        exit(EXIT_FAILURE);
    }

    printf("System support threads: %li\n", res);

    return (EXIT_SUCCESS);
}
