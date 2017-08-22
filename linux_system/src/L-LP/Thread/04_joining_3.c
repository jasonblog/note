#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <pthread.h>
#include <unistd.h>

void* thr_func1(void* arg);
void* thr_func2(void* arg);

/* Il programma mostra come recuperare lo stato di uscita di due thread
terminati; stato di uscita ottenuto mediante il secondo parametro della funzione
pthread_join(). */
int main(void)
{
    pthread_t thrID1, thrID2;
    int thr_err;
    void* thr_ret;

    /* Si crea il primo thread */
    if ((thr_err = pthread_create(&thrID1, NULL, &thr_func1, NULL)) != 0) {
        fprintf(stderr, "Err. pthread_create() 1(%s)\n", strerror(thr_err));
        exit(EXIT_FAILURE);
    }

    /* Si crea il secondo thread */
    if ((thr_err = pthread_create(&thrID2, NULL, &thr_func2, NULL)) != 0) {
        fprintf(stderr, "Err. pthread_create() 2(%s)\n", strerror(thr_err));
        exit(EXIT_FAILURE);
    }

    /* join del primo thread */
    if ((thr_err = pthread_join(thrID1, &thr_ret)) != 0) {
        fprintf(stderr, "Err. pthread_join() 1(%s)\n", strerror(thr_err));
        exit(EXIT_FAILURE);
    }

    /* Lo stato di uscita dei thread viene salvato nella variabile thr_ret */
    printf("thread 1 exit code %d\n", (int)thr_ret);

    /* join del secondo thread */
    if ((thr_err = pthread_join(thrID2, &thr_ret)) != 0) {
        fprintf(stderr, "Err. pthread_join() 1(%s)\n", strerror(thr_err));
        exit(EXIT_FAILURE);
    }

    printf("thread 2 exit code %d\n", (int)thr_ret);

    return (EXIT_SUCCESS);
}

void* thr_func1(void* arg)
{
    printf("Thread 1 ritorno\n");

    /* Si imposta ad 1 il valore di ritorno */
    return ((void*)1);
}

void* thr_func2(void* arg)
{
    printf("Thread 2 uscita\n");

    /* Si imposta a 2 il valore di ritorno */
    return ((void*)2);
}
