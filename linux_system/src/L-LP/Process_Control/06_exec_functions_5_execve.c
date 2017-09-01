#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>

/*
HEADER    : <unistd.h>
PROTOTYPE : int execve(const char *path, char *const argv[],
                       char *const envp[]);
SEMANTICS : La funzione execve() esegue il programma 'path', a tale programma
            possono essere passati gli argomenti definiti nel vettore di
        stringhe argv[] ed ulteriori argomenti definiti nel vettore di
        stringhe envp[].
RETURNS   : Ritornano solo in caso di errore, restituendo -1
--------------------------------------------------------------------------------
Nota: La desinenza 'v' nel nome della funzione indica che il programma puo'
ricevere gli argomenti mediante il vettore di stringhe argv[], nella classica
forma argv[0], argv[1], ... argv[N], tale vettore deve terminare con un
puntatore nullo, possibilmente nella forma '(char *)0'.

La desinenza 'e' indica che al programma possono essere passate le variabili di
ambiente gestite dal vettore di stringhe envp[], nella forma:
envp[0], envp[1], ... envp[N], tale vettore deve terminare con un puntatore
nullo.

L'eseguibile va cercato nel PATH indicato da 'path'.
*/

/* Il programma stampa sullo stdout la lista delle variabili di ambiente
utilizzando la funzione execve() nel processo figlio; poiche' si tratta di un
nuovo programma con una nuova immagine di memoria, l'ambiente e' vuoto
per cui stampera' solo le variabili passate come argomento.

Si e' adoperato il programma 'env' della FSF, qualora non dovesse essere
presente sul sistema unix in uso, si puo' adoperare 'printenv', entrambi hanno
lo scopo di stampare sullo stdout la lista delle variabili d'ambiente.

La lista degli argomenti e' passata mediante un vettore di stringhe dichiarato
separatamente, cosi' come la lista delle variabili di ambiente. */

int main(int argc, char* argv[])
{
    pid_t pid;
    char* env_vars[] = {"EDITOR=vim", "SHELL=/bin/bash", "USER=b3h3m0th", NULL};
    char* args[] = {"/usr/bin/env", (char*)0};


    switch (pid = fork()) {
    case -1:
        fprintf(stderr, "Err.(%s) fork() failed\n", strerror(errno));
        exit(EXIT_FAILURE);

    case 0:
        if (execve("/usr/bin/env", args, env_vars) < 0) {
            fprintf(stderr, "Err.(%s) execve() failed\n", strerror(errno));
            exit(EXIT_FAILURE);
        }

        exit(EXIT_SUCCESS);

    default:
        waitpid(pid, NULL, 0);
        printf("In esecuzione il padre\n");
    }

    return (EXIT_SUCCESS);
}
