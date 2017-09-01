#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <pthread.h>
#include <unistd.h>
#include <sys/syscall.h>

/* Un thread puo' predisporre la chiamata di talune funzioni alla chiusura del
thread stesso, il funzionamento e' molto simile alla funzione atexit[1]; tali
funzioni sono chiamate "thread cleanup handlers", sono collocate sullo stack
per cui saranno eseguite in ordine inverso rispetto all'invocazione.

HEADER    : <pthread.h>
PROTOTYPE : void pthread_cleanup_push(void (*routine)(void*), void *arg);
            void pthread_cleanup_pop(int execute);
SEMANTICS : La funzione pthread_cleanup_push() registra la funzione di cleanup
            'routine' che sara' invocata con argomento 'arg' in seguito a:
            1 - una chiamata alla funzione pthread_exit();
            2 - come risposta ad una richiesta di cancellazione;
            3 - una chiamata a pthread_cleanup_pop() con argomento non zero.
                'execute' dev'essere diverso da zero, altrimenti la funzione di
                cleanup non sara' invocata.
            La funzione pthread_cleanup_pop() rimuove la connessione stabilita
            dalla funzione pthread_cleanup_push().
RETURNS   : Le funzioni non ritornano
--------------------------------------------------------------------------------
*/

void* thr_func1(void* arg);
void* thr_func2(void* arg);
void cleanup(void* arg);

int main(void)
{
    pthread_t thrID1, thrID2;
    int thr_err;
    void* thr_ret = 0;

    /* Creazione del primo thread */
    if ((thr_err = pthread_create(&thrID1, NULL, thr_func1, (void*)1)) != 0) {
        fprintf(stderr, "Err. pthread_create() %s\n", strerror(thr_err));
        exit(EXIT_FAILURE);
    }

    /* Creazione del secondo thread */
    if ((thr_err = pthread_create(&thrID2, NULL, thr_func2, (void*)1)) != 0) {
        fprintf(stderr, "Err. pthread_create() %s\n", strerror(thr_err));
        exit(EXIT_FAILURE);
    }

    /* Joining del primo thread */
    if ((thr_err = pthread_join(thrID1, &thr_ret)) != 0) {
        fprintf(stderr, "Err. pthread_join() 1 - %s\n", strerror(thr_err));
        exit(EXIT_FAILURE);
    }

    printf("Thread 1 exit code: %d\n", (int)thr_ret);

    /* Joining del secondo thread */
    if ((thr_err = pthread_join(thrID2, &thr_ret)) != 0) {
        fprintf(stderr, "Err. pthread_join() 1 - %s\n", strerror(thr_err));
        exit(EXIT_FAILURE);
    }

    printf("Thread 2 exit code: %d\n", (int)thr_ret);

    return (EXIT_SUCCESS);
}

void* thr_func1(void* arg)
{
    printf("Start Thread 1 \'ThrID1\'\n");

    pthread_cleanup_push(cleanup, "thread 1 first handler");
    pthread_cleanup_push(cleanup, "thread 1 second handler");

    printf("Thread 1 \'ThrID1\' - push complete\n");

    if (arg) {
        return ((void*)1);
    }

    pthread_cleanup_pop(0);
    pthread_cleanup_pop(0);

    return ((void*)1);
}

void* thr_func2(void* arg)
{
    printf("Start Thread 2 \'ThrID2\'\n");

    pthread_cleanup_push(cleanup, "thread 2 first handler");
    pthread_cleanup_push(cleanup, "thread 2 second handler");

    printf("Thread 1 \'ThrID2\' - push complete\n");

    if (arg) {
        pthread_exit((void*)2);
    }

    pthread_cleanup_pop(0);
    pthread_cleanup_pop(0);

    pthread_exit((void*)2);
}

void cleanup(void* arg)
{
    printf("Cleanup: %s\n", (char*)arg);
}
