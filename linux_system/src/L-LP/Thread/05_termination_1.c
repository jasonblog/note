#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <pthread.h>
#include <unistd.h>
#include <sys/syscall.h>

#define MAX_THREAD 5

/* Se un thread all'interno di un processo dovesse invocare exit(), _Exit(),
oppure _exit(), l'intero processo sarebbe terminato, e questo non va bene
perche' thread e processi dovrebbero essere indipendenti per cio' che concerne
lo stato di uscita.

I thread contemplano un meccanismo di terminazione meno invasivo rispetto alle
funzioni precedenti, supportano infatti tre modalita' di terminazione del loro
flusso di esecuzione, senza peraltro terminare anche il processo:

1 - Un thread semplicemente ritorna dalla routine di avvio;
2 - Un thread puo' essere cancellato da un altro thread nello stesso processo;
3 - Un thread puo' invocare la funzione pthread_exit().

La funzione pthread_exit() termina il thread chiamante, ritornando, volendo, lo
stato di terminazione.

HEADER    : <pthread.h>
PROTOTYPE : void pthread_exit(void *ret_val);
SEMANTICS : La funzione pthread_exit() termina il thread chiamante e ritorna il
            relativo valore mediante la variabile 'retval'.
RETURNS   : Questa funzione non ritorna
--------------------------------------------------------------------------------
*/

void* thr_func(void* thr_num);

int main(void)
{
    pthread_t thr[MAX_THREAD];
    int i, thr_err;

    /* Si provvede alla creazione di MAX_THREAD thread */
    for (i = 0; i < MAX_THREAD; i++) {

        if ((thr_err = pthread_create(&thr[i], NULL, thr_func, (void*)i)) != 0) {
            fprintf(stderr, "Err. pthread_create() %s\n", strerror(thr_err));
            exit(EXIT_FAILURE);
        }
    }

    for (i = 0; i < MAX_THREAD; i++) {
        if ((thr_err = pthread_join(thr[i], NULL)) != 0) {
            fprintf(stderr, "Err. pthread_join() %s\n", strerror(thr_err));
            exit(EXIT_FAILURE);
        }
    }

    return (EXIT_SUCCESS);
}

void* thr_func(void* thr_num)
{
    pthread_t tid;

    if ((tid = syscall(SYS_gettid)) == -1) {
        fprintf(stderr, "Err. syscall() %s\n", strerror(errno));
        exit(EXIT_FAILURE);
    }

    printf("thread '%d' - TID %lu - Address 0x%x\n",
           (int)thr_num, tid, (unsigned int)tid);

    /* La funzione pthread_exit() termina il thread chiamante */
    pthread_exit((void*)0);
}
