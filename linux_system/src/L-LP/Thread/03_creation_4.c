#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>

int sum; /* variabile globale, condivisa da ciascun thread */

void* thr_fun(void* param);

/* Lo scopo del programma e' il passaggio di un parametro dalla linea di comando
al thread */

int main(int argc, char* argv[], char* envp[])
{
    /* Identificatore del thread */
    pthread_t tid;
    int thr_ret;

    /* Verifica che vi sia un parametro in input */
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <integer value>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    /* Ci si assicura che il parametro sia un intero */
    if (atoi(argv[1]) < 0) {
        fprintf(stderr, "%d deve essere >= 0\n", atoi(argv[1]));
        exit(EXIT_FAILURE);
    }

    /* Creazione del thread e passaggio del parametro */
    if ((thr_ret = pthread_create(&tid, NULL, &thr_fun, argv[1])) != 0) {
        fprintf(stderr, "Err. pthread_create() %s\n", strerror(thr_ret));
        exit(EXIT_FAILURE);
    }

    /* Attende la terminazione del thread */
    if ((thr_ret = pthread_join(tid, NULL)) != 0) {
        fprintf(stderr, "Err. pthread_join() %s\n", strerror(thr_ret));
        exit(EXIT_FAILURE);
    }

    /* stampa il risultato dopo che il thread ha eseguito i calcoli */
    printf("sum = %d\n", sum);

    return (EXIT_SUCCESS);
}

void* thr_fun(void* param)
{
    int i, upper = atoi(param);
    sum = 0;

    for (i = 1; i <= upper; i++) {
        sum += i;
    }

    pthread_exit(0);
}
