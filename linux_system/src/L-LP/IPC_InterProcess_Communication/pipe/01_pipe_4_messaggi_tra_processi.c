#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>

#define MAX_BUF 10

/* Il programma principale (padre) crea una pipe e subito dopo crea anche un
nuovo processo (figlio) lanciando una fork; il processo padre scrive nella pipe
una stringa recevuta in input, dopodiche' il processo figlio legge la medesima
stringa dalla pipe stessa e la stampa in otput. */

int main(int argc, char* argv[])
{
    int pi_fd[2];
    char buf[MAX_BUF];
    ssize_t nread;

    if (argc != 2 || strcmp(argv[1], "--help") == 0) {
        fprintf(stderr, "Uso: %s <stringa>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    /* Creazione della pipe */
    if (pipe(pi_fd) == -1) {
        fprintf(stderr, "Err.: %d pipe() - %s\n", errno, strerror(errno));
        exit(EXIT_FAILURE);
    }

    switch (fork()) {
    case -1:
        fprintf(stderr, "Err.: %d fork() - %s\n", errno, strerror(errno));
        exit(EXIT_FAILURE);

    case 0: /* Creazione del processo figlio */

        /* Chiude il file descriptor in scrittura */
        if (close(pi_fd[1]) == -1) {
            fprintf(stderr, "Err: %d close() %s\n", errno, strerror(errno));
            exit(EXIT_FAILURE);
        }

        /* Lettura dei dati dalla pipe e stampa sullo stdout */
        for (;;) {
            if ((nread = read(pi_fd[0], buf, MAX_BUF)) == -1) {
                fprintf(stderr, "Err.: %d read() - %s\n",
                        errno, strerror(errno));
                exit(EXIT_FAILURE);
            }

            if (nread == 0) {
                break;
            }

            if (write(STDOUT_FILENO, buf, nread) != nread) {
                fprintf(stderr, "Err.: %d write() - %s\n",
                        errno, strerror(errno));
                exit(EXIT_FAILURE);
            }
        }

        write(STDOUT_FILENO, "\n", 1);

        if (close(pi_fd[0]) == -1) {
            fprintf(stderr, "Err: %d close() %s\n", errno, strerror(errno));
            exit(EXIT_FAILURE);
        }

        exit(EXIT_SUCCESS);

    default: /* Il processo padre scrive nella pipe*/

        /* Chiude il file descriptor in lettura */
        if (close(pi_fd[0]) == -1) {
            fprintf(stderr, "Err: %d close() %s\n", errno, strerror(errno));
            exit(EXIT_FAILURE);
        }

        if (write(pi_fd[1], argv[1], strlen(argv[1])) != strlen(argv[1])) {
            fprintf(stderr, "Err: %d write() %s\n", errno, strerror(errno));
            exit(EXIT_FAILURE);
        }

        if (close(pi_fd[1]) == -1) {
            fprintf(stderr, "Err: %d close() %s\n", errno, strerror(errno));
            exit(EXIT_FAILURE);
        }

        /* Attende che il processo figlio finisca di eseguire le proprie
        istruzioni */
        wait(NULL);

        exit(EXIT_SUCCESS);
    }

    return (EXIT_SUCCESS);
}
