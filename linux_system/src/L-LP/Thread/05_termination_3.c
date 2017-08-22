#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <pthread.h>
#include <unistd.h>
#include <sys/syscall.h>

/* variabile globale condivisa tra i thread */
int global_var = 10;


void* thr_func(void* thr_num);

/* Il programma riceve come argomento il numero di thread da creare
dinamicamente. */
int main(int argc, char* argv[])
{
    int i, thr_err, n_thr;

    if (argc != 2) {
        fprintf(stderr, "Uso: %s <num of thread>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    n_thr = atoi(argv[1]);

    pthread_t thr[n_thr];

    for (i = 0; i < n_thr; i++) {

        if ((thr_err = pthread_create(&thr[i], NULL, thr_func, (void*)0)) != 0) {
            fprintf(stderr, "Err. pthread_create() %s\n", strerror(thr_err));
            exit(EXIT_FAILURE);
        }
    }

    for (i = 0; i < n_thr; i++) {
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

    /* Attenzione, questo non e' il modo corretto, si potrebbe incorrere in
    risultati inaspettati incrementando in tal modo una variabile globale; deve
    essere chiaro che i thread lavorano in multitasking, in simultanea - certo,
    su una macchina monoprocessore sara' una simulazione di multitasking, ma
    comunque ci sara' -, per cui e' possibile che uno o piu' thread leggano o
    scrivano un valore non corretto, o meglio, non congruo,

    Servono dei meccanismi specifici che consentano a ciascun thread di lavorare
    in assoluta sicurezza sui dati, senza la preoccupazione che altri thread nel
    frattempo possano modificarli, nel caso specifico la variabile globale
    'global_var' sarebbe dovuta essere stata modificata da ogni thread in mutua
    esclusione.

    A Titolo di esempio, il problema sarebbe risolto con l'utilizzo delle
    funzioni della famiglia pthread_mutex_, in particolare con la variabile
    globale:

    pthread_mutex_t my_mutex = PTHREAD_MUTEX_INITIALIZER;

    e nel thread:

    pthread_mutex_lock(&my_mutex);
    global_var++;
    pthread_mutex_unlock(&my_mutex);

    Ma non e' ancora il momento di parlare di mutua esclusione; per cui si
    compili questo programma e si faccia attenzione ai risultati non congrui.
    */
    global_var++;

    if ((tid = syscall(SYS_gettid)) == -1) {
        fprintf(stderr, "Err. syscall() %s\n", strerror(errno));
        exit(EXIT_FAILURE);
    }

    printf("thread - TID %lu - Address 0x%x - ", tid, (unsigned int)tid);

    printf("Variabile globale: %d\n", global_var);

    pthread_exit((void*)0);
}
