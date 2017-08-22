#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <sys/wait.h>

/*
SIGCHLD: E' il segnale che il kernel invia al processo padre per notificarlo
         sulla terminazione del figlio

Quando un processo termina o si ferma, il segnale SIGCHLD e' inviato al processo
padre, da notare tuttavia che il comportamento di default consiste nell'ignorare
il segnale, per cui il processo padre deve catturare il segnale se vuole essere
notificato sul cambiamento di stato del processo figlio.

Il comportamento solito del signal handler prevede l'invocazione di una delle
funzioni della famiglia wait() affinche' si possa acquisire lo stato e il PID
del processo figlio.

Nota storica: Le prime versioni di UNIX System V comprendevano il segnale SIGCLD
              con una semantica del tutto differente a SIGCHLD, il suo utilizzo
          non e' consigliato. Linux, BSD e POSIX non lo supportano e
          oltretutto il segnale SIGCLD talvolta e' indicato come un sinonimo
          di SIGCHLD stesso.

Semantica
---------
Quando occorre un segnale SIGCHLD, lo stato del processo figlio cambia, vi e'
pertanto la necessita' di invocare una delle funzioni wait() affinche' si possa
determinare l'accaduto.

Tramite l'uso di segnali e' possibile svincolare il padre da un'attesa esplicita
della terminazione del figlio, mediante un'apposita funzione handler per la
gestione di SIGCHLD; la funzione handler verrà attivata in modo asincrono alla
ricezione del segnale, l'handler chiamerà wait() con cui il padre potra'
raccogliere ed eventualmente gestire lo stato di terminazione del figlio.
*/

static void signal_handler(int sig_num);

int main(int argc, char* argv[])
{
    pid_t pid;

    if (signal(SIGCHLD, signal_handler) == SIG_ERR) {
        fprintf(stderr, "Err.(%s) signal() 'main' error\n", strerror(errno));
        exit(EXIT_FAILURE);
    }

    if ((pid = fork()) < 0) {
        fprintf(stderr, "Err.(%s) fork() failed\n", strerror(errno));
        exit(EXIT_FAILURE);
    } else if (pid == 0) {
        printf("Figlio, PID: %ld\n", (long)getpid());
        sleep(2);
        exit(EXIT_SUCCESS);
    }

    pause();
    printf("Padre, PID: %d\n", getpid());

    return (EXIT_SUCCESS);
}

static void signal_handler(int sig_num)
{
    pid_t pid;
    int child_status;

    if ((pid = waitpid(-1, &child_status, 0)) < 0) {
        fprintf(stderr, "Err.(%s) waitpid() error\n", strerror(errno));
        exit(EXIT_FAILURE);
    }

    printf("Segnale '%d' verso '%d' - child status: %d\n",
           sig_num, pid, child_status);

    /* Si provvede alla reinstallazione  */
    if (signal(SIGCHLD, signal_handler) == SIG_ERR) {
        fprintf(stderr, "Err.(%s) signal() 'sig-han' error\n", strerror(errno));
        exit(EXIT_FAILURE);
    }
}
