#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>

/*
Per inviare un segnale ad un processo le funzioni che possono essere adoperate,
tra le altre, sono la funzione kill() - man 2 kill - che invia un segnale ad un
processo o ad un gruppo di processi, e la funzione raise() che consente ad un
processo di inviare un  segnale a se stesso.

HEADER    : <signal.h>
PROTOTYPE : int kill(pid_t pid, int sig);
            int raise(int sig);
SEMANTICS : La funzione kill() invia il segnale 'sig' al processo o gruppo di
            processi con PID 'pid';
        la funzione raise() invia al processo corrente il segnale 'sig'.
RETURNS   : 0 in caso di successo, -1 in caso di errore
--------------------------------------------------------------------------------
Nota: raise(sig) e kill(getpid(), sig) sono equivalenti.

I valori dell'argomento pid della funzione kill() possono essere:
  pid > 0, il segnale e' inviato al processo indicato da pid;

 pid == 0, il segnale e' inviato ad ogni processo del processo group del
           processo chiamante;

  pid < 0, il segnale e' inviato ad ogni processo del process group indicato
           da pid;

pid == -1, il segnale e' inviato ad ogni processo, fatta eccezione per init.

Permessi
--------
Naturalmente un processo per poter inviare un segnale ad un altro processo ha
bisogno di avere i permessi necessari, solo al superuser e' conisentito inviare
segnali a ciascun processo del sistema, per gli altri utenti vi sono delle
regole di base da rispettare:

- Il RUID o l'EUID del processo chiamante deve corrispondere al RUID o all'EUID
  del processo di destinazione[1];

- Se l'implementazione supporta _POSIX_SAVED_IDS, si deve far corrispondere il
  saved SUID invece dell'EUID;

- Se e' stato inviato il segnale SIGCONT, allora un processo puo' inviare tale
  segnale a tutti i processi della medesima sessione.

Signal number 0
---------------
POSIX definisce il numero di segnale 0 come un segnale nullo, per cui qualora
si utilizzasse kill(pid, 0), non sara' inviato nessun segnale.
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
        pause();

        exit(EXIT_SUCCESS);

    default:
        printf("Padre, PID: %ld\n", (long)getpid());

        /* Si invia il segnale generico SIGURG1 al processo figlio */
        if (kill(pid, SIGUSR1) == -1) {
            fprintf(stderr, "Err.(%s) kill() failed()\n", strerror(errno));
            exit(EXIT_FAILURE);
        }

        sleep(1);

        exit(EXIT_SUCCESS);
    }

    return (EXIT_SUCCESS);
}

static void signal_handler(int signum)
{
    printf("Ricevuto segnale %d\n", signum);

    if (signal(SIGUSR1, signal_handler) == SIG_ERR) {
        fprintf(stderr, "Err.(%s) signal() main failed\n", strerror(errno));
        exit(EXIT_FAILURE);
    }
}
/*
[1] L'identificazione dei processi e' trattata nel sorgente:
../Process-Control/01_process_indentifiers.c
*/
