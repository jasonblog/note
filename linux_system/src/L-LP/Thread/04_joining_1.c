#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <pthread.h>
#include <unistd.h>

#define MAX_VALUE 5

/* La funzione pthread_join() si comporta come se fosse una funzione della
famiglia wait(), concettualmente intendo, nel senso che se il main() dovesse
terminare prima del thread, le strutture dati del thread sarebbero deallocate
con il thread stesso ancora in esecuzione, per cui e' necessario un meccanismo
che consenta di far attendere al main() la terminazione del thread.

Il compito della funzione pthread_join() e' di far attendere il thread corrente
fino a quando il thread specificato non cessi l'esecuzione, essa inoltre unifica
due thread in un unico thread, e' come se il nuovo ramo di esecuzione fosse
nuovamente riunito nel ramo principale, questo meccanismo e' estremamente
importante.

HEADER    : <pthread.h>
PROTOTYPE : int pthread_join(pthread_t thread, void **retval);
SEMANTICS : La funzione pthread_join() attende la terminazione del thread
            specificato dal primo argomento 'thread', in retval sara' salvato
            il valore di ritorno del thread terminato.
RETURNS   : 0 in caso di successo, numero di errore in caso di errore
--------------------------------------------------------------------------------
*/

void* thr_func(void* arg);

int main(int argc, char* argv[], char* envp[])
{
    pthread_t thrID;
    int thr_err;
    void* thr_ret;

    if ((thr_err = pthread_create(&thrID, NULL, &thr_func, NULL)) != 0) {
        fprintf(stderr, "Err. pthread_create() (%s)\n", strerror(thr_err));
        exit(EXIT_FAILURE);
    }

    printf("Main Thread: %lu\n", pthread_self());

    /* Negli esempi precedenti e' stata inserita una sleep() per consentire
    l'esecuzione di entrambi i thread, ora invece si usa la funzione
    pthread_join() per far si che il main attenda l'esecuzione del nuovo thread;
    in sostanza tale chiamata manda in sleep() il main() in attesa che la
    funzione thr_func() possa essere completamente eseguita, e non appena cio'
    accadra' pthread_join() ritornera' e il programma avra' nuovamente un solo
    thread.

    Il primo argomento della funzione si riferisce al thread da attendere, il
    secondo argomento invece riguarda il valore di ritorno del thread,
    rispettivamente 'thrID' e 'thr_ret'.

    La funzione pthread_join() e' estremamente importante, ogni nuovo thread
    dovrebbe passare attraverso tale funzione per poi essere ricongiunta al
    thread principale, anche perche' se cio' non accadesse diminuirebbe il
    numero massimo di thread a disposizione del sistema. */
    if ((thr_err = pthread_join(thrID, &thr_ret)) != 0) {
        fprintf(stderr, "Err. pthread_join() (%s)\n", strerror(thr_err));
        exit(EXIT_FAILURE);
    }

    /* Lo stato di uscita di un thread puo' essere ottenuto da un altro thread
    mediante la funzione thread_join(), invocata precedentemente. */
    printf("Valore di ritorno del thread (%lu) : '%d'\n", thrID, (int)thr_ret);

    return (EXIT_SUCCESS);
}

void* thr_func(void* arg)
{
    int i = 0;

    do {
        sleep(1);
        printf(" New Thread: %lu\n", pthread_self());
        i++;
    } while (i < MAX_VALUE);

    return ((void*)0);
}
