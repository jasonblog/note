#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>

/*
HEADER    : <unistd.h>
PROTOTYPE : int execlp(const char *file, const char *arg0, ...);
SEMANTICS : La funzione execl() esegue il programma 'file', a tale programma
            puo' essere passata la una lista di argomenti arg0
RETURNS   : Ritornano solo in caso di errore, restituendo -1
--------------------------------------------------------------------------------
Nota: La desinenza 'l' nel nome della funzione indica che il programma puo'
ricevere una lista di argomenti nella forma arg0, arg1, ... argN, tale lista
deve terminare con un puntatore nullo, possibilmente nella forma '(char *)0'.

La desinenza 'p' indica che il programma da eseguire e' definito mediante il
nome di un 'file', per cui, qualora fosse privo di slash '/', lo si cercherebbe
anzitutto nella directory corrente, se invece lo slash '/' fosse presente, si
porterebbe a termine la ricerca considerando la variabile d'ambiente PATH.
*/

/* Il programma stampa sullo stdout la lista dei file della directory corrente
in ordine inverso e ordinati cronologicamente, utilizzando la funzione execlp()
nel processo figlio */

int main(int argc, char* argv[])
{
    pid_t pid;

    switch (pid = fork()) {
    case -1:
        fprintf(stderr, "Err.(%s) fork() failed\n", strerror(errno));
        exit(EXIT_FAILURE);

    case 0:
        if (execlp("ls", "ls", "-l", "-t", "-r", (char*)0) < 0) {
            fprintf(stderr, "Err.(%s) execlp() failed\n", strerror(errno));
            exit(EXIT_FAILURE);
        }

        exit(EXIT_SUCCESS);

    default:
        waitpid(pid, NULL, 0);
        printf("In esecuzione il padre\n");
    }

    return (EXIT_SUCCESS);
}
