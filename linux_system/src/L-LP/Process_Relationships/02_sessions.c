#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>

/*
Una sessione (Session-ID, SID) e' una collezione di uno o piu' gruppi di
processi, anch'essa utilizza il tipo di dato pid_t.

HEADER    : <unistd.h>
PROTOTYPE : pid_t setsid(void);
SEMANTICS : La funzione setsid() consente ad un processo di creare o stabilire
            una nuova sessione.
RETURNS   : Il PGID in caso di successo, -1 in caso di errore
--------------------------------------------------------------------------------
Se il processo chiamante non e' il process group leader, la funzione setsid()
creera' una nuova sessione, altrimenti ritornera' un errore qualora il processo
chiamante dovesse essere il process group leader.

Una chiamata alla funzione setsid() comporta:
1 - Il processo diventa session leader della sessione; il session leader e'
    il processo che crea la sessione. Tale processo e' l'unico processo nella
    nuova sessione;

2 - Il processo diventa process group leader del nuovo gruppo di processi; il
    PGID e' il PID del processo chiamante;

3 - Il processo non ha terminale di controllo; se il processo ha un terminale
    di controllo prima della chiamata a setsid(), l'associazione e' interrotta.

Cosi' come per i gruppi di processi anche per le sessioni e' possibile ottenere
l'identificativo relativo al processo; la funzione che si occupa di restituire
il PGID del leaader della sessione e' getsid().

HEADER    : <unistd.h>
PROTOTYPE : pid_t getsid(pid_t pid);
SEMANTICS : La funzione getsid() restituisce il SID del processo 'pid'.
RETURNS   : Il SID in caso di successo, -1 in caso di errore
--------------------------------------------------------------------------------
*/

void* err(char* str_err);

int main(int argc, char* argv[])
{
    pid_t pid;
    pid_t sid, nsid;

    switch (pid = fork()) {
    case -1:
        err("fork() failed");

    case 0:

        /* Si invoca la funzione getsid() per ottenere il valore della
        sessione di default del processo figlio */
        if ((sid = getsid(pid)) == -1) {
            err("getsid() failed");
        }

        printf("Child process, Sessione iniziale - SID = %ld\n", (long)sid);

        /* Si invoca la funzione setsid() per cambiare la sessione */
        if ((nsid = setsid()) == -1) {
            err("setsid() failed");
        }

        /* Si invoca ancora una volta la funzione getsid() per ottenere il
        nuovo valore della sessione */
        if ((sid = getsid(pid)) == -1) {
            err("getsid() failed");
        }

        printf("  Child process, Cambio sessione - SID = %ld\n", (long)sid);

        exit(EXIT_SUCCESS);

    default:
        waitpid(pid, NULL , 0);

        /* Questa chiamata fallisce poiche' il processo padre e' anche
        process group leader; inserito solo a scopo didattico */
        if ((sid = setsid()) == -1) {
            err("setsid() failed, parent is process group leader");
        }
    }

    return (EXIT_SUCCESS);
}

void* err(char* str_err)
{
    fprintf(stderr, "Err.(%s) - %s\n", strerror(errno), str_err);
    exit(EXIT_FAILURE);
}
