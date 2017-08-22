#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>

/*
HEADER    : <unistd.h>
PROTOTYPE : int execv(const char *path, const *const argv[]);
SEMANTICS : La funzione execv() esegue il programma 'path', a tale programma
            possono essere passati gli argomenti definiti nel vettore di
        stringhe argv[].
RETURNS   : Ritornano solo in caso di errore, restituendo -1
--------------------------------------------------------------------------------
Nota: La desinenza 'v' nel nome della funzione indica che il programma puo'
ricevere gli argomenti mediante il vettore di stringhe argv[], nella forma:
argv[0], argv[1], ... argv[N], tale vettore deve terminare con un
puntatore nullo, possibilmente '(char *)0'.

L'eseguibile va cercato nel PATH indicato da 'path'.
*/

/* Il programma stampa sullo stdout la lista dei processi dell'utente loggato
al sistema, utilizzando la funzione execv() nel processo figlio; la lista degli
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
        if (execv("/bin/ps", args) < 0) {
            fprintf(stderr, "Err.(%s) execv() failed\n", strerror(errno));
            exit(EXIT_FAILURE);
        }

        exit(EXIT_SUCCESS);

    default:
        waitpid(pid, NULL, 0);
        printf("In esecuzione il padre\n");
    }

    return (EXIT_SUCCESS);
}
