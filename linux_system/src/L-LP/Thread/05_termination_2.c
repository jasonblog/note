#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <pthread.h>
#include <unistd.h>
#include <sys/syscall.h>

struct data {
    int var_a, var_b, var_c, var_d;
    char* str_1, *str_2, *str_3, *str_4;
};

void print_data(const char* str, const struct data* fp);
void* thr_func1(void* arg);
void* thr_func2(void* arg);

void* thr_func(void* thr_num);

/* Il programma mostra il problema che puo' capitare alle variabili allocate
sullo stack usate come argomento della funzione pthread_exit(), e come lo stack
del secondo thread venga sovrascritto dallo stack del primo thread; la soluzione
potrebbe essere l'allocazione dinamica della struttura con malloc. */
int main(void)
{
    pthread_t thrID1, thrID2;
    int thr_err;
    struct data* fp;

    if ((thr_err = pthread_create(&thrID1, NULL, thr_func1, NULL)) != 0) {
        fprintf(stderr, "Err. pthread_create() 1 - %s\n", strerror(thr_err));
        exit(EXIT_FAILURE);
    }

    if (thr_err = pthread_join(thrID1, (void*)&fp) != 0) {
        fprintf(stderr, "Err. pthread_join() 1 - %s\n", strerror(thr_err));
        exit(EXIT_FAILURE);
    }

    sleep(1);

    printf("\nDal main si crea un nuovo thread\n\n");

    if ((thr_err = pthread_create(&thrID2, NULL, thr_func2, NULL)) != 0) {
        fprintf(stderr, "Err. pthread_create() 2 - %s\n", strerror(thr_err));
        exit(EXIT_FAILURE);
    }

    sleep(1);

    print_data("\n", fp);

    return (EXIT_SUCCESS);
}

void print_data(const char* str, const struct data* fp)
{
    printf(str);
    printf("struttura: 0x%x\n", (unsigned)fp);
    printf("    data.var_a = %d - data.str_1 = %s\n", fp->var_a, fp->str_1);
    printf("    data.var_b = %d - data.str_2 = %s\n", fp->var_b, fp->str_2);
    printf("    data.var_c = %d - data.str_3 = %s\n", fp->var_c, fp->str_3);
    printf("    data.var_d = %d - data.str_4 = %s\n", fp->var_d, fp->str_4);
}

void* thr_func1(void* arg)
{
    struct data data = {100, 200, 300, 400, "Thread", "Programming", "on Unix",
        "System"
    };
    print_data("thread 1:\n", &data);

    /* In questa occasione vi e' la sovrascrittura della struttura */
    pthread_exit((void*)&data);
}

void* thr_func2(void* arg)
{
    printf("thread 2: ");

    pthread_exit((void*)0);
}
