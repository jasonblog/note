#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>

/*
HEADER    : <unistd.h>
PROTOTYPE : int execl(const char *path, const char *arg0, ...);
SEMANTICS : La funzione execl() esegue il programma 'path', a tale programma
            possono essere passati gli argomenti definiti nella lista arg0.
RETURNS   : Ritornano solo in caso di errore, restituendo -1
--------------------------------------------------------------------------------
Nota: La desinenza 'l' nel nome della funzione indica che il programma puo'
ricevere una lista di argomenti nella forma:
arg0, arg1, ... argN, tale lista deve terminare con un puntatore nullo,
possibilmente '(char *)0'.

L'eseguibile va cercato nel PATH indicato da 'path'.
*/

/* Il programma stampa sullo stdout la lista dei file della directory corrente
utilizzando la funzione execl() nel processo figlio */

int main(int argc, char* argv[])
{
    pid_t pid;

    switch (pid = fork()) {
    case -1:
        fprintf(stderr, "Err.(%s) fork() failed\n", strerror(errno));
        exit(EXIT_FAILURE);

    case 0:
        if (execl("/bin/ls", "ls", "-l", (char*)0) < 0) {
            fprintf(stderr, "Err.(%s) execl() failed\n", strerror(errno));
            exit(EXIT_FAILURE);
        }

        exit(EXIT_SUCCESS);

    default:
        waitpid(pid, NULL, 0);
        printf("In esecuzione il padre\n");
    }

    return (EXIT_SUCCESS);
}
