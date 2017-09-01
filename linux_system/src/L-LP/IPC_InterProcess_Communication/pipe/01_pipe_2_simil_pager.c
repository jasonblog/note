#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <unistd.h>
#include <wait.h>

#define DEFAULT_PAGER   "/bin/less"
#define MAX_LINE        512

/* Stampa l'output una pagina per volta, invocando il pager preferito
dall'utente. si utilizzera' una pipe per redirigere l'output direttamente al
pager. */

/* I pager piu' utilizzati in ambiente UNIX sono less, pager e more. */

int main(int argc, char* argv[])
{
    int n, fd[2];
    pid_t pid;
    char* pager, *argv0;
    char line[MAX_LINE];
    FILE* fp;

    /* Si verifica anzitutto che si passi un argomento a linea di comando */
    if (argc != 2) {
        fprintf(stderr, "Uso: %s <pathname>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    /* Si apre il file definito dall'utente come argomento */
    if ((fp = fopen(argv[1], "r")) == NULL) {
        fprintf(stderr, "Err.(%s) can't open\n", argv[1]);
        exit(EXIT_FAILURE);
    }

    /* Si apre una pipe, che in caso di esito positivo avra' fd[0] in lettura e
    fd[1] in scrittura */
    if (pipe(fd) < 0) {
        fprintf(stderr, "Err.(%s) - opening pipe\n", strerror(errno));
        exit(EXIT_FAILURE);
    }

    /* Si crea un processo figlio mediante la chiamata fork() */
    if ((pid = fork()) < 0) {
        fprintf(stderr, "Err.(%s) - fork()\n", strerror(errno));
        exit(EXIT_FAILURE);
    } else if (pid > 0) { /* Processo Padre */
        /* Il processo padre chiude l'end point in lettura */
        close(fd[0]);

        /* Il processo padre copia l'argomento argv[1] nella pipe */
        while (fgets(line, MAX_LINE, fp) != NULL) {
            n = strlen(line);

            if (write(fd[1], line, n) != n) {
                fprintf(stderr, "Err.(%s) - write pipe\n", strerror(errno));
                exit(EXIT_FAILURE);
            }
        }

        if (ferror(fp)) {
            fprintf(stderr, "Err.(%s) - fgets error\n", strerror(errno));
            exit(EXIT_FAILURE);
        }

        /* Il processo padre chiude l'end point della pipe in scrittura */
        close(fd[1]);

        /* Il processo padre attende l'esecuzione del processo figlio */
        if (waitpid(pid, NULL, 0) < 0) {
            fprintf(stderr, "Err.(%s) - waitpid()\n", strerror(errno));
            exit(EXIT_FAILURE);
        }
    } else { /* Processo Figlio */
        /* Il processo figlio chiude l'end point in scrittura */
        close(fd[1]);

        /* Mediante la funzione dup2() si fa in modo che lo standard input
        divenga l'end point in lettura della pipe */
        if (fd[0] != STDIN_FILENO) {
            if (dup2(fd[0], STDIN_FILENO) != STDIN_FILENO) {
                fprintf(stderr, "Err.(%s) - dup2()\n", strerror(errno));
                exit(EXIT_FAILURE);
            }

            close(fd[0]); /* in realta' dopo dup2() non sarebbe necessrio */
        }

        /* Si verifica se il pager e' definito nella variabile d'ambiente,
        altrimenti gli si assegna il valore di DEFAULT_PAGER */
        if ((pager = getenv("PAGER")) == NULL) {
            pager = DEFAULT_PAGER;
        }

        /* Assegna ad argv0 il valore '/pager', dopodiche' gli toglie lo
        slash iniziale */
        if ((argv0 = strrchr(pager, '/')) != NULL) {
            argv0++;
        } else {
            argv0 = pager;
        }

        /* Esegue il programma pager */
        if (execl(pager, argv0, (char*)0) < 0) {
            fprintf(stderr, "Err.(%s) - execl()\n", strerror(errno));
            exit(EXIT_FAILURE);
        }
    }

    return (EXIT_SUCCESS);
}
