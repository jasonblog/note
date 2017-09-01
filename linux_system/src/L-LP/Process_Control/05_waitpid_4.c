#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>

/* Evitare la creazione di zombie chiamando due volte la funzione fork(), in
modo tale che l'orfano venga subito adottato da init. */

int main(int argc, char* argv[])
{
    pid_t pid;

    if ((pid = fork()) < 0) {
        fprintf(stderr, "Err.(%s) fork() failed\n", strerror(errno));
        exit(EXIT_FAILURE);
    } else if (pid == 0) { /* Primo figlio */
        if ((pid = fork()) < 0) {
            fprintf(stderr, "Err.(%s) fork() failed\n", strerror(errno));
            exit(EXIT_FAILURE);
        } else if (pid > 0) { /* Padre della seconda fork(); primo figlio */
            exit(EXIT_SUCCESS);
        }

        sleep(2);

        printf("Secondo figlio, PPID = %ld\n", (long)getppid());
        exit(EXIT_SUCCESS);
    }

    if (waitpid(pid, NULL, 0) != pid) { /* Attesa che il primo figlio termini */
        fprintf(stderr, "Err.(%s) waitpid() failed\n", strerror(errno));
        exit(EXIT_FAILURE);
    }

    return (EXIT_SUCCESS);
}
