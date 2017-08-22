#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <sys/syscall.h>

void print_id(const char* str);
void* thr_func(void* arg);

/* Il programma crea un thread, dopodiche' stampa PID, TID di libreria e di
sistema,  nonche' l'indirizzo sia del nuovo thread sia del thread principale. */
int main()
{
    int thr_err;
    pthread_t thrID;

    if ((thr_err = pthread_create(&thrID, NULL, thr_func, NULL)) != 0) {
        fprintf(stderr, "Err. pthread_create() (%s)\n", strerror(thr_err));
        exit(EXIT_SUCCESS);
    }

    print_id("Main Thread: ");
    sleep(1);

    /* Da notare che il Thread-ID del nuovo thread e' collocato nel primo
    paramentro della funzione. Tale dato e' accessibile dal main thread ma non
    dal nuovo thread, dato a cui esso peraltro si riferisce.  */
    printf("\nDal thread principale, TID %lu del nuovo thread\n", thrID);

    return (EXIT_SUCCESS);
}

/* Stampa PID, TIDs e indirizzo Thread */
void print_id(const char* str)
{
    pid_t pid;
    pthread_t tid;
    int stid;

    pid = getpid();             /* Process-ID del processo */
    tid = pthread_self();       /*  Thread-ID di libreria */
    stid = syscall(SYS_gettid); /*  Thread-ID di sistema, assegnato dal OS */

    /* Come si notera' dall'output, il risultato sara' il medesimo PID, poiche'
    ovviamente si tratta dello stesso processo, e TID differenti, poiche'
    all'interno dello stesso processo ci sono due rami di esecuzione differenti,
    i thread per l'appunto. */
    printf("%s - PID: %i - TID(lib): %lu - TID(sys): %lu - (0x%x) \n",
           str, pid, tid, stid, tid);
}

/* La funzione eseguita dal nuovo thread; invoca semplicemente la funzione per
la stampa di taluni dati, con argomento annesso. */
void* thr_func(void* arg)
{
    print_id(" New Thread: ");

    return ((void*)0);
}
