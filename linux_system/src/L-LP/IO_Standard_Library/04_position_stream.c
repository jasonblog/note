#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

/*
HEADER    : <stdio.h>

PROTOTYPE : int fseek(FILE *fp, long offset, int whence);
            long ftell(FILE *fp);

SEMANTICS : La funzione fseek() sposta ad 'offset' la posizione corrente
            all'interno del file 'fp', partendo da  'whence' (whence + offset);
        la funzione ftell() restituisce la posizione corrente all'interno
        del file 'fp'.

RETURNS   : 0 In caso di successo, -1 in caso di errore
--------------------------------------------------------------------------------
*/

int main(int argc, char* argv[])
{
    FILE* fp;
    struct stat bufstat;
    int c, cur_pos;
    char* input_file;

    if (argc < 2) {
        fprintf(stderr, "Uso: %s <input_file>\n", argv[0]);
        exit(-1);
    }

    input_file = argv[1];

    if ((fp = fopen(input_file, "r")) == NULL) {
        fprintf(stderr, "Err(%d) %s: %s", errno, strerror(errno), input_file);
        exit(EXIT_FAILURE);
    }

    /*
     La la posizione corrente all'interno del file viene spostata sino alla fine,
     in modo tale che la successiva lettura del file stesso non possa essere
     effettuata.
    */
    if (fseek(fp, 0L, SEEK_END) < 0) {
        fprintf(stderr, "Err(%d) %s: fseek() SEEK_END", errno, strerror(errno));
        exit(EXIT_FAILURE);
    }

    /*
     Poiche' la funzione ftell() consente di ottenere la posizione corrente
     all'interno del file, e avendola posizionata al termine del file stesso,
     se ne deduce che 'cur_pos', oltre ad avere la posizione corrente all'interno
     del file, indichi anche la grandezza del file stesso espresso in bytes.
    */
    cur_pos = ftell(fp);

    /*
     Si verifica, tramite la funzione stat(), se effettivamente la grandezza
     del file 'input_file' corrisponde alla grandezza ottenuta grazie alla
     funzione ftell(), in caso di esito positivo essa sara' stampata su stdout
    */

    if (stat(input_file, &bufstat) < 0) {
        fprintf(stderr, "Err(%d) %s: %s", errno, strerror(errno), input_file);
        exit(EXIT_FAILURE);
    }

    if (bufstat.st_size == cur_pos) {
        printf("%d bytes\n", (int)bufstat.st_size);
    }

    /*
     La lettura, ovviamente, non andra' a buon fine, o meglio, sara' letto solo
     il carattere EOF ... */
    while ((c = fgetc(fp)) != EOF) {
        fputc(c, stdout);
    }

    /*
     ...per poter leggere e stampare su stdout il file e' necessario riportare la
     posizione corrente all'interno del file all'inizio del file stesso; e'
     possibile farlo sia con la fseek() sia con rewind():
     fseek(fp, 0L, SEEK_SET);
     rewind(fp);
    */
    if (fseek(fp, 0L, SEEK_SET) < 0) {
        fprintf(stderr, "Err(%d) %s: fseek() SEEK_SET", errno, strerror(errno));
        exit(EXIT_FAILURE);
    }

    while ((c = fgetc(fp)) != EOF) {
        fputc(c, stdout);
    }

    fclose(fp);

    return (EXIT_SUCCESS);
}
