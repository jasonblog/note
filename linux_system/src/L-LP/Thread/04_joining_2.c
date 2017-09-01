#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <pthread.h>
#include <unistd.h>

#define MAX_VALUE 5

void* thr_func(void* arg);

/* Il programma calcola una semplice somma all'interno del thread, dopodiche'
il risultato sara' restituito mediante il valore di ritorno del thread, della
funzione pthread_join(). */
int main(void)
{
    pthread_t thrID;
    int thr_err;
    void* thr_ret;

    if ((thr_err = pthread_create(&thrID, NULL, &thr_func, NULL)) != 0) {
        fprintf(stderr, "Err. pthread_create() (%s)\n", strerror(thr_err));
        exit(EXIT_FAILURE);
    }

    /* Si sarebbe ottenuto il medesimo risultato con:
    'int *thr_ret' e '(void *)&thr_ret' come argomento della funzione. */
    if ((thr_err = pthread_join(thrID, &thr_ret)) != 0) {
        fprintf(stderr, "Err. pthread_join() (%s)\n", strerror(thr_err));
        exit(EXIT_FAILURE);
    }

    printf("Risultato della somma nel thread '%d'\n", (int)thr_ret);

    return (EXIT_SUCCESS);
}

void* thr_func(void* arg)
{
    int i, sum = 0;

    for (i = 0; i < MAX_VALUE; i++) {
        sum += i;
    }

    /* Poiche' la funzione deve ritornare un puntatore a void, si esegue un
    cast della variabile intera 'sum' */
    return ((void*)sum);
}
