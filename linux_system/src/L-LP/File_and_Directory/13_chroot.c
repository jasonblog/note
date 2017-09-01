#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/stat.h>

/*
HEADER    : <unistd.h>
PROTOTYPE : int chroot(const char *path);
SEMANTICS : La funzione chroot() cambia la root directory del processo chiamante
            al pathname 'path' specificato
RETURNS   : 0 in caso di successo, -1 in caso di errore
--------------------------------------------------------------------------------
Ogni processo ha una root directory, e' il punto di partenza dal quale ciascun
pathname assoluto viene interpretato, nel caso specifico '/'; la root directory
e' ereditata da tutti i figli del processo chiamante e naturalmente puo' essere
invocata da processi con privilegi da super-utente.

Un esempio comune del suo utilizzo e' col protocollo ftp, quando un utente
si logga come anonimo, al server viene invocata una chroot() su quel processo
in modo tale da restringere il campo di azione dell'utente alla sola directory
acceduta.
*/

/* Il programma crea una nuova root directory fornita come argomento */
int main(int argc, char* argv[])
{
    struct stat statbuf;
    pid_t pid;

    if (argc < 2) {
        fprintf(stderr, "Uso: %s <dirname>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    if (stat(argv[1], &statbuf) < 0) {
        if (mkdir(argv[1], 0755) == -1) {
            fprintf(stderr, "Err.(%s) mkdir() failed\n", strerror(errno));
            exit(-1);
        }
    } else if (S_ISDIR(statbuf.st_mode)) {
        printf("New dir: %s\n", argv[1]);
    }

    /* Si imposta la nuova root directory con l'argomento fornito in input */
    if (chroot(argv[1]) == -1) {
        fprintf(stderr, "Err.(%s) chroot() failed\n", strerror(errno));
        exit(-1);
    }

    /* Si accede alle nuova root directory in modo tale che il nuovo processo
    la intepreti come la current working directory */
    if (chdir(argv[1]) == -1) {
        fprintf(stderr, "Err.(%s) chdir() failed\n", strerror(errno));
        exit(-1);
    }

    switch (pid = fork()) {
    case -1:
        fprintf(stderr, "Err.(%s) fork() failed\n", strerror(errno));
        exit(EXIT_FAILURE);

    case 0:
        printf("Current working dir: %s\n", get_current_dir_name());

        /* questa chiamata fallira', poiche' la root directory ora e'
        cambiata, per cui il pathname /etc non sara' raggiungibile mediante
        l'attuale configurazione. La root directory e la current working
        directory ora corrispondono.
        */
        if (chdir("/etc") == -1) {
            fprintf(stderr, "Err: invalid path chdir()\n", strerror(errno));
            exit(-1);
        }

    /* testare questo programma commentando la chiamata a chroot() e
    verificare l'output relativo al pathname in cui ci si trova */

    default:
        waitpid(pid, NULL, 0);
    }

    return (EXIT_SUCCESS);
}
