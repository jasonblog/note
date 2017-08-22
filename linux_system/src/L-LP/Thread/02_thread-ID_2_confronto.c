#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>

/* Per verificare se due thread sono gli stessi si usa la funzione
pthread_equal(), che confronta ciascun thread-ID.

HEADER    : <pthread.h>
PROTOTYPE : int pthread_equal(pthread_t tid1, pthread_t tid2);
SEMANTICS : La funzione pthread_equal() serve per verificare se il thread-ID
            tid1 e' uguale o diverso dal thread-ID tid2.
RETURNS   : Non 0 se t1 e t2 sono uguali, 0 altrimenti
*/

void* thr_func(void* arg);

int main()
{
    pthread_t thrID;
    pthread_t main_thrID = pthread_self();
    int thr_err;

    if ((thr_err = pthread_create(&thrID, NULL, thr_func, NULL)) != 0) {
        fprintf(stderr, "Err. pthread_create() %s\n", strerror(thr_err));
        exit(EXIT_SUCCESS);
    }

    sleep(1);

    printf("Thread principale: %u\n", main_thrID);

    if (pthread_equal(thrID, main_thrID) == 0) {
        printf("\nI Thread sono diversi");
    }

    return (EXIT_SUCCESS);
}

void* thr_func(void* arg)
{
    pthread_t TID = pthread_self();

    printf("     Nuovo thread: %u\n", TID);
}
