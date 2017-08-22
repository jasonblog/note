#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>

/*
Il programma e' identico al precedente con kill(), si e' solo sostituita la
funzione.
*/

static void signal_handler(int signum);

int main(int argc, char* argv[])
{
    pid_t pid;

    if (signal(SIGUSR1, signal_handler) == SIG_ERR) {
        fprintf(stderr, "Err.(%s) signal() main failed\n", strerror(errno));
        exit(EXIT_FAILURE);
    }

    switch (pid = fork()) {
    case -1:
        fprintf(stderr, "Err.(%s) fork() failed\n", strerror(errno));
        exit(EXIT_FAILURE);

    case 0:
        printf("Figlio, PID: %ld\n", (long)getpid());

        /* La funzione raise() invia al processo corrente il segnale
        SIGUSR1 */
        if (raise(SIGUSR1) == -1) {
            fprintf(stderr, "Err.(%s) raise() failed()\n", strerror(errno));
            exit(EXIT_FAILURE);
        }

        exit(EXIT_SUCCESS);

    default:
        printf("Padre, PID: %ld\n", (long)getpid());

        sleep(1);

        exit(EXIT_SUCCESS);
    }

    return (EXIT_SUCCESS);
}

static void signal_handler(int signum)
{
    printf("PID: %ld - Ricevuto segnale %d\n", (long)getpid(), signum);
}
