#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>

/*
HEADER    : <unistd.h>
PROTOTYPE : int execvp(const char *file, const char *arg0, ...);
SEMANTICS : La funzione execl() esegue il programma 'file', a tale programma
            puo' essere passata la una lista di argomenti arg0
RETURNS   : Ritornano solo in caso di errore, restituendo -1
--------------------------------------------------------------------------------
Nota: La desinenza 'v' nel nome della funzione indica che il programma puo'
ricevere gli argomenti mediante il vettore di stringhe argv[], nella forma:
argv[0], argv[1], ... argv[N], tale vettore deve terminare con un
puntatore nullo, possibilmente '(char *)0'.

La desinenza 'p' indica che il programma da eseguire e' definito mediante il
nome di un 'file', per cui, qualora fosse privo di slash '/', lo si cercherebbe
anzitutto nella directory corrente, se invece lo slash '/' fosse presente, si
porterebbe a termine la ricerca considerando la variabile d'ambiente PATH.
*/

/* Il programma stampa sullo stdout la lista dei processi dell'utente loggato al
sistema, utilizzando la funzione execvp() nel processo figlio; la lista degli
argomenti e' passata mediante un vettore di stringhe dichiarato separatamente.
*/

int main(int argc, char* argv[])
{
    pid_t pid;
    char* user_logged;

    if ((user_logged = getenv("USER")) == NULL) {
        fprintf(stderr, "Err.(%s) getenv() failed\n", strerror(errno));
        exit(EXIT_FAILURE);
    }

    char* args[] = {"ps", "-U", user_logged, (char*)0};

    switch (pid = fork()) {
    case -1:
        fprintf(stderr, "Err.(%s) fork() failed\n", strerror(errno));
        exit(EXIT_FAILURE);

    case 0:
        if (execvp("ps", args) < 0) {
            fprintf(stderr, "Err.(%s) execvp() failed\n", strerror(errno));
            exit(EXIT_FAILURE);
        }

        exit(EXIT_SUCCESS);

    default:
        waitpid(pid, NULL, 0);
        printf("In esecuzione il padre\n");
    }

    return (EXIT_SUCCESS);
}
