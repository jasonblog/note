#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <pthread.h>

#define MAX_THREAD  10

char* msg[MAX_THREAD];

/* I dati da inviare al thread come argomento sono inseriti in una struttura */
struct tdata {
    int tid;
    int sum;
    char* message;
};

struct tdata tdata_arr[MAX_THREAD];

void* thr_func(void* arg);

/* Allorquando gli argomenti da passare sono piu' d'uno e' consigliabile
utilizzare una struttura in cui archiviare i dati; il programma infatti passa
multipli argomenti mediante una struttura e ciascun thread ne ricevera' un'unica
istanza. */

int main(int argc, char* argv[])
{
    pthread_t thread[MAX_THREAD];
    int* msgs[MAX_THREAD];
    int thr_ret, i, sum;

    sum = 0;

    msg[0] = "Assembly programming";
    msg[1] = "C++ Programming";
    msg[2] = "C Programming";
    msg[3] = "Python Programming";
    msg[4] = "Perl Programming";
    msg[5] = "Ruby Programming";
    msg[6] = "Shell Programming";
    msg[7] = "GCC Compiling";
    msg[8] = "GDB Debugging";
    msg[9] = "Network programming";

    for (i = 0; i < MAX_THREAD; i++) {
        sum = sum + i;
        tdata_arr[i].tid = i;
        tdata_arr[i].sum = sum;
        tdata_arr[i].message = msg[i];

        printf("Creazione thread: %d -> ", i);

        /* Il passaggio di argomenti in questo caso riguarda un array di
        interi, un valore per volta naturalmente */
        thr_ret = pthread_create(&thread[i], NULL, thr_func,
                                 (void*)&tdata_arr[i]);

        if (thr_ret != 0) {
            fprintf(stderr, "Err. pthread_create() %s\n", strerror(thr_ret));
            exit(EXIT_FAILURE);
        }

        if ((thr_ret = pthread_join(thread[i], NULL)) != 0) {
            fprintf(stderr, "Err. pthread_join() %s\n", strerror(thr_ret));
            exit(EXIT_FAILURE);
        }
    }

    return (EXIT_SUCCESS);
}

/* La funzione thr_func() riceve un intero dal main thread e stampa il relativo
messaggio dell'array di stringhe */
void* thr_func(void* arg)
{
    sleep(1);

    int ttid, tsum;
    char* tmsg;
    struct tdata* my_data;

    my_data = (struct tdata*)arg;
    ttid = my_data->tid;
    tmsg = my_data->message;
    tsum = my_data->sum;

    printf("Index: %2d - Msg: %20s - Somma = %3d\n", ttid, tmsg, tsum);

    pthread_exit(NULL);
}
