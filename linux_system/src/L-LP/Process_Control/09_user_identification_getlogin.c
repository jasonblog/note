#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <unistd.h>
#include <pwd.h>

/*
HEADER    : <unistd.h>
PROTOTYPE : char *getlogin(void);
SEMANTICS : La funzione getlogin() server per ottenere il login name
RETURNS   : Un puntatore alla stringa con il login name in caso di successo,
            NULL in caso di errore
--------------------------------------------------------------------------------
Sui sistemi unix-like uno stesso utente puo' avere diversi login-name, per cui
puo' essere collegato a piu' di un terminale, la funzione getlogin() identifica
un utente loggato ad un terminale del processo corrente, fornendo un puntatore
alla stringa relativa al login-name correntemente in uso.

La chiamata puo' fallire qualora il processo non fosse associato al terminale in
cui l'utente e' loggato.
*/

int main(int argc, char* argv[])
{
    char* login_name;
    struct passwd* pw;

    if ((login_name = getlogin()) == NULL) {
        fprintf(stderr, "Err.(%s) getlogin() failed\n", strerror(errno));
        exit(EXIT_FAILURE);
    }

    /* Si puo' utilizzare con la struttura passwd, per la verifica dei campi
    contenuti in /etc/passwd */
    pw = getpwnam(login_name);

    printf("       login name: %s\n", login_name);
    printf("/etc/passwd shell: %s\n", pw->pw_shell);
    /* E via via tutti i campi[1] */

    return (EXIT_SUCCESS);
}
/*
[1] ../System-Data-Files-and-Information/01-password_database_*
*/
