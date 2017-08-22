#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>

/*
I processi in unix sono utilizzati per diversi motivi, uno dei principali e' di
utilizzarli per lanciare nuovi programmi, tale caratteristica e' propria della
famiglia delle funzioni exec.

Allorquando un processo viene chiamato da una delle funzioni exec, tale processo
e' completamente rimpiazzato da un nuovo programma - in pratica l'immagine del
processo corrente viene rimpiazzata da una nuova immagine - , il quale inizia
l'esecuzione dalla funzione main() - come qualsiasi altro programma del resto -,
il PID non cambia poiche' non e' stato creato un nuovo processo, ma si "limita"
a rimpiazzare testo, dati, heap e il segmento dello stack.

La famiglia delle funzioni exec comprende sei differenti funzioni, tuttavia
solo la funzione execve() e' una system call vera e propria, le altre cinque
sono semplicemente funzioni di libreria che si appoggiano ad essa.

HEADER    : <unistd.h>
PROTOTYPE :
 int execl(const char *path, const char *arg0, ... );
 int execv(const char *path, char *const argv[]);
int execle(const char *path, const char *arg0, ... , char *const envp[]);
int execve(const char *path, char *const argv[], char *const envp[]);
int execlp(const char *file, const char *arg0, ...);
int execvp(const char *file, char *const argv[]);
SEMANTICS : Eseguono il programma definito in 'path' on in 'file', con o senza
            argomenti, nell'ambiente di default o definito da envp[].
RETURNS   : Ritornano solo in caso di errore, restituendo -1
--------------------------------------------------------------------------------
Nota: I prototipi delle funzioni, seppur piuttosto complessi da ricordare, hanno
il vantaggio che possono essere facilmente memorizzati grazie a specifiche
lettere che compongono la desinenza di ciascun nome di funzione:

-        p (path) = Il programma 'file' da eseguire va ricercato nella directory
                    corrente;
-        l (list) = Il programma utilizza una lista di argomenti che termina con
                    un puntatore nullo.
            (arg0, arg1, ... argvN) - Termina con (char *)0;
-      v (vector) = Il programma utilizza un array di stringhe che termina con
                    un puntatore nullo.
            (argv[0], arg[1], ... argv[N]) - Termina con (char *)0;
- e (environment) = L'ambiente del processo e' gestito mediante l'array envp[];

Differenze tra le varie funzioni:
1 - La modalita' di esecuzione del programma.
    Le prime quattro funzioni eseguiranno il programma definito da un 'path',
    le ultime due funzioni invece eseguiranno il programma specificato mediante
    un 'file', lettera 'p'.

    'path' e' il nome del programma che deve essere eseguito. Se in 'path'
           vi e' uno slash '/' si considera tale percorso e la variabile
       d'ambiente PATH non viene considerata, altrimenti il programma lo si
       cerca nella directory corrente.
    'file' e' il nome del programma che deve essere eseguito. Non richiede
       un percorso completo poiche' utilizza la variabile d'ambiente PATH.

2 - Il passaggio dei parametri.
    Si considerano le lettere all'interno della funzione 'v' ed 'l', la prima e'
    un vettore la seconda una lista; nel primo caso gli argomenti sono passati
    mediante il vettore di puntatori ad argv[], seguite da un puntatore nullo,
    nel secondo caso gli argomenti sono passati come una lista di puntatori,
    terminata da un puntatore nullo.

3 - Il passaggio della lista delle variabili d'ambiente.
    Le funzioni con la lettera 'e' utilizzeranno l'array envp[] per interagire
    con l'ambiente, le altre utilizzeranno la variabile globale esterna environ.
*/


int main(int argc, char* argv[])
{
    pid_t pid;

    if ((pid = fork()) < 0) {
        fprintf(stderr, "Err.(%s) fork() failed\n", strerror(errno));
        exit(EXIT_FAILURE);
    } else if (pid == 0) { /* Il figlio compila un programma */
        if (execl("/usr/bin/gcc", "gcc", "-v", "02_fork_1.c", (char*)0) < 0) {
            fprintf(stderr, "Err.(%s) execl() failed\n", strerror(errno));
            exit(EXIT_FAILURE);
        }
    }

    /* Si attende la terminazione del processo figlio */
    if (waitpid(pid, NULL, 0) < 0) {
        fprintf(stderr, "Err.(%s) waitpid() failed\n", strerror(errno));
        exit(EXIT_FAILURE);
    }

    /* In questo caso si esegue il programma precedentemente compilato
    if ((pid = fork()) < 0) {
        fprintf(stderr,"Err.(%s) fork() failed\n", strerror(errno));
    exit(EXIT_FAILURE);
    } else if (pid == 0) {
        if (execl("./a.out","a.out","-1",(char*)0) < 0) {
        fprintf(stderr,"Err.(%s)execl() failed\n", strerror(errno));
        exit(EXIT_FAILURE);
    }
    }
    if (waitpid(pid, NULL, 0) < 0) {
       fprintf(stderr,"Err.(%s) waitpid() failed\n", strerror(errno));
       exit(EXIT_FAILURE);
    }
    */
    return (EXIT_SUCCESS);
}
