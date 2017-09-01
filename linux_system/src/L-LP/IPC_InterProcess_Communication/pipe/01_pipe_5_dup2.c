#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>


int main(void)
{
    int fd[2];
    pid_t pid;

    /* Creazione della pipe */
    pipe(fd);

    /* fork del proceosso figlio e verifica dello stesso*/

    if ((pid = fork()) == 0) { /* Child Process */
        /* chiusura del file descriptor 1 in scrittura */
        close(fd[1]);

        /* Si duplica la lettura della pipe allo stdin */
        dup2(fd[0], STDIN_FILENO);

        /* Il processo figlio rimpiazzato dal programma 'sort',  */
        execlp("sort", "sort", 0);
    } else { /* Parent Process */
        FILE* stream;
        /* chiusura del file descriptor 0 in lettura */

        close(fd[0]);

        /* Il file descripror in scrittura associato a 'stream' */
        stream = fdopen(fd[1], "w");

        /* L'output sara' ordinato dal programma 'sort' eseguito dal processo
        figlio */
        fprintf(stream, "BBBBBBBBBBBB\n");
        fprintf(stream, "FFFFFFFFFFFF\n");
        fprintf(stream, "AAAAAAAAAAAA\n");
        fprintf(stream, "CCCCCCCCCCCC\n");
        fprintf(stream, "DDDDDDDDDDDD\n");
        fprintf(stream, "EEEEEEEEEEEE\n");

        fflush(stream);

        close(fd[1]);

        /* Attende l'esecuzione del processo figlio */
        waitpid(pid, NULL, 0);
    }

    return (EXIT_SUCCESS);
}
