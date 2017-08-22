#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>

#define MAX_BUF 512

/*
HEADER    : <stdio.h>

PROTOTYPE : char *tmpnam(char *str);

SEMANTICS : Restituisce il puntatore ad una stringa, che dev'essere un file name
            valido e non esistente al momento della creazione.

RETURNS   : Il puntatore al file temporaneo in caso di successo, NULL in caso di
            esito negativo. Non sono definiti errori.
--------------------------------------------------------------------------------
Nota: L'uso di tmpnam() e' assolutamente non raccomandato; POSIX infatti
consiglia di utilizzare mkstemp()

L'esempio a scopo didattico tuttavia deve essere proposto.
*/

int main(int argc, char* argv[])
{
    char buf[MAX_BUF];
    char tmp_pathname[L_tmpnam];
    FILE* ftmp;

    /* Oppure con NULL al posto di una stringa
    char *tmpfile = tmpnam(NULL);
    */
    tmpnam(tmp_pathname);

    printf("1° file temporaneo: %s\n", tmp_pathname);

    if ((ftmp = fopen(tmp_pathname, "w+")) == NULL) {
        fprintf(stderr, "%s: creazione file temporaneo %s\n",
                strerror(errno), tmp_pathname);
        exit(EXIT_FAILURE);
    }

    /* Si scrive sul file temporaneo */
    fputs("The art of UNIX programming", ftmp);

    /* Si riporta la posizione corrente all'interno del file all'inizio, per
     poter leggere il file */
    fseek(ftmp, 0L, SEEK_SET);

    /* Si legge la riga pocanzi scritta sull file temporaneo */
    if (fgets(buf, sizeof(buf), ftmp) == NULL) {
        fprintf(stderr, "Err. %s reading file temp\n", strerror(errno));
        exit(EXIT_FAILURE);
    }

    fputs(buf, stdout);

    fclose(ftmp);

    return (EXIT_SUCCESS);
}
