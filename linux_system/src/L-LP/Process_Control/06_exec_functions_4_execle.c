#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>

/*
HEADER    : <unistd.h>
PROTOTYPE : int execle(const char *path, const char *arg0, ...,
                       char *const envp[]);
SEMANTICS : La funzione execle() esegue il programma 'path', a tale programma
            possono essere passati gli argomenti definiti nella lista arg0 ed
        ulteriori argomenti definiti nel vettore di stringhe envp[].
RETURNS   : Ritornano solo in caso di errore, restituendo -1
--------------------------------------------------------------------------------
Nota: La desinenza 'l' nel nome della funzione indica che il programma puo'
ricevere una lista di argomenti nella forma:
arg0, arg1, ... argN, tale lista deve terminare con un puntatore nullo,
possibilmente '(char *)0'.

La desinenza 'e' indica che al programma possono essere passate le variabili di
ambiente gestite dal vettore di stringhe envp[], nella forma:
envp[0], envp[1], ... envp[N], tale vettore deve terminare con un puntatore
nullo.

L'eseguibile va cercato nel PATH indicato da 'path'.
*/

/* Il programma stampa sullo stdout la lista delle variabili di ambiente
utilizzando la funzione execl() nel processo figlio; poiche' si tratta di un
nuovo programma con una nuova immagine di memoria, l'ambiente e' vuoto
per cui stampera' solo le variabili passate come argomento.

Si e' adoperato il programma 'env' della FSF, qualora non dovesse essere
presente sul sistema unix in uso, si puo' adoperare 'printenv', entrambi hanno
lo scopo di stampare sullo stdout la lista delle variabili d'ambiente.

La lista delle variabili di ambiente e' passata mediante un vettore di stringhe
dichiarato separatamente.
*/

int main(int argc, char* argv[])
{
    pid_t pid;
    char* env_vars[] = {"EDITOR=vim", "SHELL=/bin/bash", "USER=b3h3m0th", NULL};

    switch (pid = fork()) {
    case -1:
        fprintf(stderr, "Err.(%s) fork() failed\n", strerror(errno));
        exit(EXIT_FAILURE);

    case 0:
        if (execle("/usr/bin/env", "printenv", (char*)0, env_vars) < 0) {
            fprintf(stderr, "Err.(%s) execle() failed\n", strerror(errno));
            exit(EXIT_FAILURE);
        }

        exit(EXIT_SUCCESS);

    default:
        waitpid(pid, NULL, 0);
        printf("In esecuzione il padre\n");
    }

    return (EXIT_SUCCESS);
}
