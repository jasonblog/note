#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>

#define MAX_BUF 512

/*
HEADER    : <stdio.h>

PROTOTYPE : FILE *tmpfile(void);

SEMANTICS : La funzione tmpfile() apre un file temporaneo unico, binario in
            lettura e scrittura.

RETURNS   : Lo stream relativo al file in caso di successo, NULL in caso di
            errore
--------------------------------------------------------------------------------
Nota: La caratteristica della funzione tmpfile() e' che il file temporaneo sara'
automaticamente eliminato alla chiusura del programma
*/

int main(int argc, char* argv[])
{
    char buf[MAX_BUF];
    FILE* ftmp;

    /* Crea il file temporaneo mediante tmpfile() */
    if ((ftmp = tmpfile()) == NULL) {
        fprintf(stderr, "%s :generazione file temp.", strerror(errno));
        exit(EXIT_FAILURE);
    }

    /* Scrive nel file temp */
    fputs("Sto scrivendo nel file temporaneo\n", ftmp);

    /* Torna il puntatore all'inizio del file */
    rewind(ftmp);

    /* Legge dal file temp */
    if ((fgets(buf, MAX_BUF, ftmp)) == NULL) {
        fprintf(stderr, "%s lettura da file temp.\n", strerror(errno));
        exit(EXIT_FAILURE);
    }

    fputs(buf, stdout);

    /* Chiusura del file */
    fclose(ftmp);

    return (EXIT_SUCCESS);
}
