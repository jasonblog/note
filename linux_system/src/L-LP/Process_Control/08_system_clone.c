#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>

/*
HEADER    : <stdlib.h>
PROTOTYPE : int system(const char *cmdstring);
SEMANTICS : La funzione system() esegue il comando definito in 'cmdstring'.
RETURNS   : La funzione system() e' implementata utilizzando fork(), exec e
            waitpid(), pertanto e' dotata di tre valori di ritorno:
        1 - Se la fork() fallisce o la waitpid() ritorna un errore diverso
            da EINTR, restituira' -1;
            2 - Se la exec fallisce impedendo cosi' l'esecuzione di /bin/sh
            restitura' exit(127);
            3 - Se non ci sono errori in fork(), exec e waitpid(), restituira'
            il termination status della shell, nella forma specificata per
        waitpid().
--------------------------------------------------------------------------------
La funzione esegue un comando specificato in 'cmdstring' nella forma
/bin/sh -c 'cmdstring', e ritorna solo dopo che il comando sara' completamente
portato a termine; il parametro -c sta a significare di prendere il prossimo
argomento dalla linea di comando, ossia 'cmdstring'.

'cmdstring' puo' essere qualsiasi comando da eseguirsi nella shell.

La funzione system() e' standard ISO C, tuttavia il suo comportamento e'
fortemente dipendente dal sistema; durante l'esecuzione del comando 'cmdstring'
il segnale SIGCHLD sara' bloccato, mentre i segnali SIGINT e SIGQUIT saranno
ignorati.

Sicurezza: Potrebbe rappresentare un grosso problema di sicurezza qualora si
           chiamasse la funzione system() all'interno di un programma setuid,
       per cui e' estremamente sconsigliato tale utilizzo.
*/

/* Si provvede ora alla implementazione di un clone della funzione system(),
la differenza sostanziale riguarda la gestione dei segnali che in questo caso e'
inesistente, tuttavia anche in questo modo dovrebbe essere comprensibile il
il comportamento (concettuale) della funzione in oggetto. */
int system_clone(const char* cmd_string);

int main(int argc, char* argv[])
{

    if (argc < 2) {
        fprintf(stderr, "Uso: %s \"<command/s>\"\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    system_clone(argv[1]);

    return (EXIT_SUCCESS);
}

int system_clone(const char* cmd_string)
{
    pid_t pid;
    int status;

    if (cmd_string == NULL) {
        return (1);
    }

    if ((pid = fork()) < 0) {
        status = -1;
    } else if (pid == 0) {
        if (execl("/bin/sh", "sh", "-c", cmd_string, (char*)0) < 0) {
            _exit(127);
        }
    } else {
        while (waitpid(pid, &status, 0) < 0) {
            if (errno != EINTR) {
                status = -1;
                break;
            }
        }
    }

    /* Implementazione con la switch
    switch (pid = fork()) {
        case -1:
        status = -1;
    case 0:
        if (execl("/bin/sh", "sh", "-c", cmd_string, (char *)0) < 0)
            _exit(127);
    default:
        while (waitpid(pid, &status, 0) < 0) {
            if (errno != EINTR) {
            status = -1;
            break;
        }
        }
    }
    */

    return (status);
}
