#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <limits.h>

void get_working_dir(void);
void change_dir(void);

/*
HEADER    : <unistd.h>
PROTOTYPE : int chdir(const char *pathname);
SEMANTICS : Cambia la directory di lavoro in 'pathname'
RETURNS   : 0 In caso di successo, -1 in caso di errore
--------------------------------------------------------------------------------
HEADER    : <unistd.h>
PROTOTYPE : int fchdir(int fd);
SEMANTICS : Cambia la directory di lavoro tramite il file descriptor 'fd'
RETURNS   : 0 In caso di successo, -1 in caso di errore
--------------------------------------------------------------------------------
HEADER    : <unistd.h>
PROTOTYPE : char *getcwd(char *buf, size_t size);
SEMANTICS : Legge il pathname della current working directory; restituisce il
            pathname assoluto mediante la stringa 'buf' - null terminated - ,
        di dimensione 'size'
RETURNS   : buf in caso di successo, NULL in caso di errore
--------------------------------------------------------------------------------
Quando un utente si logga in un sistema UNIX, la "current working directory" -
directory di lavoro corrente - iniziale e' indicata dal sesto campo di
/etc/passwd, ossia la HOME directory dell'utente stesso.

Ad ogni processo e' associata una "current working directory", che sta ad
indicare il pathname relativo, puo' essere cambiata via terminale mediante
il comando cd, oppure stampata in output con il comando pwd; ciascun
nuovo processo eredita la "current working directory" dal proprio genitore.

L'argomento 'size' della funzione getcwd() ha un valore compreso tra:
0 e PATH_MAX.

Nota: La funzione fchdir() non e' parte delle specifiche POSIX.1, ma e' una
estensione XSI della SUS.
*/


int main(int argc, char* argv[])
{
    char* buf;

    get_working_dir();
    change_dir();

    /*
     Se si utilizza un puntatore nullo e size 0, sara' allocata automaticamente
     una stringa pari alla lunghezza del pathname corrente, tuttavia questa
     pratica e' sconsigliata poiche' non e' ne' POSIX ne' SUS, e quindi non
     portabile.
    */
    if ((buf = getcwd(NULL, 0)) == NULL) {
        fprintf(stderr, "Err.:(%d) - %s: buf\n", errno, strerror(errno));
        exit(EXIT_FAILURE);
    }

    get_working_dir();
    change_dir();
    get_working_dir();

    /*
     La funzione getcwd() e' utile per memorizzare la "current working directory"
     iniziale, utilizzandola come parametro per chdir() allorquando si vuole che
     l'applicazione torni al "punto di partenza".
    */
    if (chdir(buf) < 0) {
        fprintf(stderr, "Err.:(%d) - %s; %s\n", errno, strerror(errno), buf);
        exit(EXIT_FAILURE);
    }

    printf("%s\n", buf);

    free(buf);

    return (EXIT_SUCCESS);
}

void get_working_dir(void)
{
    /*
     PATH_MAX e' definito in <limits.h> e corrisponde all'ampiezza massima di un
     pathname
    */
    char buf[PATH_MAX];

    /* La directory di lavoro corrente in forma assoluta */
    if (getcwd(buf, sizeof(buf)) == NULL) {
        fprintf(stderr, "Err.:(%d) - %s: curr. workdir\n", errno, strerror(errno));
        exit(EXIT_FAILURE);
    }

    printf("%s\n", buf);
}

/* Salgo l'albero della directory */
void change_dir(void)
{
    if (chdir("..") < 0) {
        fprintf(stderr, "Err.:(%d) - %s: \"..\"\n", errno, strerror(errno));
        exit(EXIT_FAILURE);
    }
}
