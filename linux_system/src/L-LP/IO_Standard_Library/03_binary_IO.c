#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>

#define MAX_BUF 10

typedef struct OS {
    int num;
    char name[MAX_BUF];
    int year;
} unix_systems;

/*
HEADER    : <stdio.h>

PROTOTYPE : size_t fread(void *ptr, size_t size, size_t nmemb, FILE *stream);
     size_t fwrite(const void *ptr, size_t size, size_t nmemb, FILE *stream);

SEMANTICS : La funzione fread() legge 'nmemb' elementi di dati, ciascuno di essi
            di 'size' bytes di lunghezza, dallo stream puntato da 'stream',
            collocandoli in 'ptr'; la funzione fwrite() scrive 'nmemb' elementi
            di dati collocati in 'ptr', ciascuno di essi di 'size' bytes di
            lunghezza, nello stream puntato da 'stream'.

RETURNS   : Il numero di dati scritti o letti in caso di successo, in caso di
            errore o EOF un numero di elementi minore rispetto al richiesto
--------------------------------------------------------------------------------
*/

int main(void)
{
    FILE* fp;
    char* binary_file = "file.bin";
    int i, ret_func, num_list_elem;

    unix_systems list_unix_os[] = {
        {1, "BSD", 1978},
        {2, "SunOS", 1982},
        {3, "Plan9", 1990},
        {4, "Gnu/Hurd", 1991},
        {5, "Linux", 1991}
    };

    unix_systems new_item, *unix_list = &new_item;
    num_list_elem = sizeof(list_unix_os) / sizeof(unix_systems);

    if ((fp = fopen(binary_file, "w+")) == NULL) {
        fprintf(stderr, "Err.(%d) %s: %s\n", errno, strerror(errno), binary_file);
        exit(EXIT_FAILURE);
    }

    /* Scrive l'array nel file binario puntato da fp */
    ret_func = fwrite(list_unix_os, sizeof(unix_systems), num_list_elem, fp);

    if (feof(fp)) {
        fprintf(stderr, "End of file: fwrite()\n");
        exit(EXIT_FAILURE);
    } else if (ferror(fp)) {
        fprintf(stderr, "Error: fwrite()\n");
        exit(EXIT_FAILURE);
    }

    /* Consente la lettura dall'inizio del file */
    rewind(fp);

    printf("  Version  year\n");

    /* Get and print unix OS list from file */
    for (i = 0; i < num_list_elem; i++) {
        fread(unix_list, sizeof(unix_systems), 1, fp);
        printf("%-2d%-9s%-4d\n", unix_list->num, unix_list->name, unix_list->year);
    }

    fclose(fp);

    return (EXIT_SUCCESS);
}
