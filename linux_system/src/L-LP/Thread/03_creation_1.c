#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>

/*
I thread sono detti anche "leightweight process" processi leggeri, in virtu' del
fatto che sono particolari 'sotto-processi' senza un vero e proprio spazio di
indirizzamento, in quanto utilizzano quello del processo chiamante.

Thread addizionali possono essere creati mediante la funzione pthread_create():

HEADER    : <pthread.h>
PROTOTYPE : int pthread_create(pthread_t *thread, const pthread_attr_t *attr,
                               void *(*start_routine) (void *), void *arg);
SEMANTICS : La funzione pthread_create() prende 4 argomenti:
            - pthread_t *thread, e' un puntatore a pthread_t;
            - pthread_attr_t *attr, gli attributi del nuovo thread, qualora non
              si volessero modificare, sara' sufficiente inserire NULL;
            - void *(*start_routine) (void *), il codice da eseguire. Trattasi
              di un puntatore a funzione che prende un puntatore a void e
              restituisce un puntatore a void;
            - void *arg, sono gli eventuali argomenti da passare, qualora non
              si volessero passare argomenti sara' sufficiente inserire NULL.
RETURNS   : 0 in caso di successo, numero di errore in caso di errore
--------------------------------------------------------------------------------
Nota: I thread in Linux sono implementati da clone(), i processi invece da
fork(); e' molto interessante notare che la creazione di thread e' ben 10 volte
piu' veloce della creazione di processi, perche' nella fork() si "perde tempo"
alla duplicazione degli attributi del parent process, mentre invece nella
creazione di thread vi e' condivisione delle risorse.
*/

void* thr_func(void* arg);

int main()
{
    pthread_t thrID;
    int thr_err;
    char str[] = "POSIX thread";

    /* Si invoca la funzione pthread_create() per creare un nuovo thread:
    - Il primo parametro e' un puntatore a thrID;
    - il secondo parametro e' definito NULL, per cui saranno utilizzati gli
      attributi di default;
    - Il terzo parametro thr_func() e' la funzione che il thread eseguira'
      allorquando sara' avviato, non appena la funzione terminera' vorra' dire
      che anche il thread sara' terminato. In questo caso la funzione stampa
      semplicemente un messaggio;
    - Il quarto parametro e' l'argomento passato alla funzione, e' possibile
      inviare una quantita' arbitraria di dati mediante una struttura, nel
      caso specifico tuttavia l'argomento e' una semplice stringa.

    Il nuovo thread creato inizia la propria esecuzione dall'indirizzo della
    funzione, nel caso specifico da thr_func.

    Da notare che se non fosse stato creato un nuovo thread il programma
    sarebbe stato composto da un singolo thread; vale a dire che ogni programma
    corrisponde comunque ad un singolo thread, definito peraltro "thread
    principale", per cui mediante la funzione pthread_create() si sarebbe creato
    il secondo.

    Gli errori, come accennato nell'esempio sul supporto[1], conviene gestirli
    senza variabili esterne, utilizzando il valore restituito dalle funzioni,
    nel caso specifico 'thr_err'. */
    if ((thr_err = pthread_create(&thrID, NULL, thr_func, str)) != 0) {
        fprintf(stderr, "Err. pthread_create() %s\n", strerror(thr_err));
        exit(EXIT_SUCCESS);
    }

    /* Non vi e' alcuna garanzia che il nuovo thread creato venga eseguito per
    primo, per cui c'e' bisogno di sleep() senza di essa infatti il processo
    potrebbe uscire prima che il nuovo thread possa essere eseguito. */
    sleep(1);

    printf("Main thread\n");

    return (EXIT_SUCCESS);
}

void* thr_func(void* arg)
{
    printf("New Thread: %s\n", arg);
}

/* COMPILAZIONE: $ gcc programma.c -lpthread

[1] Programma relativo al supporto dei POSIX THREADS: 01_threads_support.c */
