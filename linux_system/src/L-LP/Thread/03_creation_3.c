#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <pthread.h>
#include <unistd.h>

#define MAX_VAL 10

static int val = 0;

void* print(void* arg);

/* Si mostra come lo scheduler esegue alternativamente i due thread del
programma. */
int main(int argc, char* argv[], char* envp[])
{
    pthread_t thrID;
    int err;

    err = pthread_create(&thrID, NULL, &print, NULL);

    /* Un altro modo per verificare il valore di ritorno della funzione e' di
    utilizzare direttamente la variabile, dopo l'assegnamento. Personalmente
    preferisco inserire tutto insieme come negli esempi precedenti. */
    if (err != 0) {
        fprintf(stderr, "Err. pthread_create(): %s\n", strerror(err));
        exit(EXIT_FAILURE);
    }

    while (val < MAX_VAL) {
        printf("  Sono nel main: %d (%lu)\n", val++, pthread_self());
        sleep(1);
    }


    return (EXIT_SUCCESS);
}

void* print(void* arg)
{
    while (1) {
        printf("Sono nel thread: %d (%lu)\n", val++, pthread_self());
        sleep(1);
    }

    return ((void*)0);
}
