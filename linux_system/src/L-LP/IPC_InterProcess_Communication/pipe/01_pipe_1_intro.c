#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <unistd.h>
#include <limits.h>

#define MAX_BUF 1024

/*
La pipe crea un flusso di comunicazione unidirezionale half-duplex, per cui un
processo scrive - mediante write() -  e un altro processo legge - mediante
read().

Nota: POSIX.1 consente anche la creazione di pipe bidirezionali, full duplex,
      il canale di comunicazione aperto sara' in lettura e scrittura su
      entrambi i file descriptor. Questa soluzione tuttavia risulta essere non
      portabile quanto la pipe unidirezionale half-duplex.

Le pipe possono essere utilizzate solo da processi che hanno antenati in comune,
generalmente viene creata da un processo che successivamente eseguira' una
chiamata a fork(), per cui la pipe sara' utilizzata per la comunicazione tra il
processo padre e il processo figlio.

Per creare una pipe si utilizza la sistem call pipe().

HEADER    : <unistd.h>
PROTOTYPE : int pipe(int fd[2]);
SEMANTICS : Il parametro della funzione pipe() e' il file descriptor 'fd', un
            array con due interi:
            - fd[0] e' il file descriptor da usare per la lettura della pipe;
            - fd[1] e' il file descriptor da usare per la scrittura nella pipe.
            L'output di fd[0] e' l'input di fd[1].
RETURNS   : 0 in caso di successo, -1 in caso di errore
--------------------------------------------------------------------------------
In sostanza la funzione pipe() non fa altro che chiedere al kernel la creazione
di un canale o buffer di comunicazione, per cui puo' essere considerata come un
file a cui si accede in lettura e scrittura.

La write() aggiunge dati sulla pipe; la read() legge i dati dalla pipe,
togliendoli da essa.
*/


/* Si utilizza una pipe per l'invio di un messaggio dal processo padre al
processo figlio, ossia il padre scrive e il figlio legge e stampa in output; si
tratta di una pipe unidirezionale. */

int main(int argc, char* argv[])
{
    int n, fd[2];
    pid_t pid;
    char buffer[MAX_BUF];

    /* In caso di esito positivo si aprira' una pipe, in cui fd[0] potra' essere
    utilizzato per la lettura e fd[1] per la scrittura.

    Nota: In questo contesto la chiamata della pipe deve essere precedente alla
          a quella della fork(), altrimenti si potrebbe incorrere ad un
          risultato non previsto. */
    if (pipe(fd) < 0) {
        fprintf(stderr, "Err.: %d pipe() - %s\n", errno, strerror(errno));
        exit(EXIT_FAILURE);
    }

    /* Si crea un processo figlio mediante la chiamata fork() */
    if ((pid = fork()) < 0) {
        fprintf(stderr, "Err.: %d fork() - %s\n", errno, strerror(errno));
        exit(EXIT_FAILURE);
    } else if (pid > 0) {
        /* Il processo padre chiude anzitutto l'end point in lettura */
        close(fd[0]);

        /* dopodiche' scrive nella pipe  */
        write(fd[1], "IPC - messaggio inviato mediante pipe\n", 38);
    } else {
        /* Il processo figlio chiude l'end point in scrittura */
        close(fd[1]);

        /* dopodiche' legge i dati scritti dal processo padre nella pipe e
        mediante una successiva write() li invia allo standard output */
        if ((n = read(fd[0], buffer, MAX_BUF)) < 0) {
            fprintf(stderr, "Err.: %d read() - %s\n", errno, strerror(errno));
            exit(EXIT_FAILURE);
        }

        if (write(STDOUT_FILENO, buffer, n) < 0) {
            fprintf(stderr, "Err.: %d write() - %s\n", errno, strerror(errno));
            exit(EXIT_FAILURE);
        }
    }

    return (EXIT_SUCCESS);
}
