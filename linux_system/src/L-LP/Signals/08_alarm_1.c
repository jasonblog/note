#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <sys/wait.h>

#define BELL "\007"

static void handler(int sig_num);

/*
La funzione alarm() fornisce un metodo che permette ad un processo di
predisporre una interruzione in un futuro prossimo, mediante il segnale SIGALRM
inviato dopo un preciso numero di secondi.

Nota: I secondi sono del tipo clock time.

Semantica
---------
- Vi e' un solo alarm clock per processo;

- Se quando si invoca un alarm clock per un processo, ci si imbatte in un altro
  alarm clock precedentemente registrato e non ancora scaduto per il processo
  stesso, il numero di secondi rimanenti dell'alarm clock e' ritornato come
  valore della funzione. Il precedente alarm clock e' rimpiazzato dal nuovo
  valore;

- Se un alarm clock precedentemente registrato per un processo non e' ancora
  scaduto e il valore di 'seconds' e' uguale a zero, tale alarm clock e'
  cancellato. Il numero di secondi rimanenti dell'alarm clock registrato in
  precedenza e' riornato come valore della funzione;

- L'azione di default corrispondente e' la terminazione del processo;

- Il segnale SIGALRM puo' essere catturato, ma in tal caso il signal handler
  deve essere installato prima di chiamare la funzione alarm(), in caso
  contrario il processo sara' terminato.

HEADER    : <unistd.h>
PROTOTYPE : unsigned int alarm(unsigned int seconds);
SEMANTICS : La funzione alarm() invia un segnale SIGALRM dopo 'seconds' secondi
RETURNS   : 0 se non vi erano time-out impostati in precedenza oppure il numero
            di secondi mancante allo scadere del time-out precedente
--------------------------------------------------------------------------------
*/

/* Il programma riceve come parametro il numero di secondi dopo dei quali
inviare un segnale al processo figlio mediante la funzione alarm(). */

int main(int argc, char* argv[])
{
    pid_t pid;
    int seconds;

    if (argc < 2) {
        fprintf(stderr, "Uso: %s <seconds>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    /* La funzione atoi() non ritorna errori, per cui risulta inutile un
    eventuale controllo */
    seconds = atoi(argv[1]);

    if ((pid = fork()) < 0) {
        fprintf(stderr, "Err.(%s) fork() failed\n", strerror(errno));
        exit(EXIT_FAILURE);
    } else if (pid == 0) {
        printf("Figlio, PID: %ld\n", (long)getpid());


        if (signal(SIGALRM, handler) == SIG_ERR) {
            fprintf(stderr, "Err.(%s) sigalarm() failed\n", strerror(errno));
            exit(EXIT_FAILURE);
        }

        /* Sara' inviato un segnale al processo dopo seconds secondi */
        alarm(seconds);

        pause();
    } else {
        waitpid(-1, NULL, 0);
        printf("Padre, PID: %ld\n", (long)getpid());
    }


    return (EXIT_SUCCESS);
}

static void handler(int sig_num)
{
    printf("Segnale '%d' al PID: %ld\n", sig_num, (long)getpid());
    printf("%s", BELL);
}
