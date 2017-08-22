#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <pthread.h>

#define MAX_THREAD  10

char* msg[MAX_THREAD];

/* La funzione pthread_create() consente di passare uno o piu' argomenti alla
funzione che consente l'avvio di un thread, ci sono tuttavia alcuni accorgimenti
da considerare; anzitutto il caso piu' semplice, ossia un solo argomento, caso
in cui non ci sono particolari avvertenze, se invece gli argomenti dovessero
essere piu' d'uno si dovrebbe creare una struttura e all'interno della quale
inserire i vari argomenti, infine passare un puntatore all'intera struttura come
argomento della funzione pthread_create().

Ciascun parametro inviato al thread deve essere passato per riferimento e
castato mediante (void *)
*/

void* thr_func(void* arg);

/* Passaggio di un intero a ciascun thread, che identifica un puntatore ad un
array di 'stringhe' */
int main(int argc, char* argv[])
{
    pthread_t thread[MAX_THREAD];
    int* msgs[MAX_THREAD];
    int thr_ret, i;

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
        msgs[i] = (int*)malloc(sizeof(int));
        *msgs[i] = i;

        printf("Creazione thread: %d -> ", i);

        /* Il passaggio di argomenti in questo caso riguarda un array di
        interi, un valore per volta naturalmente; da notare il cast (void *) */
        thr_ret = pthread_create(&thread[i], NULL, thr_func, (void*)msgs[i]);

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

    int* p_id, n_msg;

    p_id = (int*) arg;
    n_msg = *p_id;

    printf("Thread %d: %s\n", n_msg, msg[n_msg]);

    pthread_exit(NULL);
}
