#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <unistd.h>

#define MAX_BUF 64
#define MAX_LINE 128

/*
Il programma copia il contenuto di un file in un altro file, utilizzando solo
funzioni relative alle righe; "line at a time I/O".
*/

void remove_new_line(char buf[], int size);
FILE* open_file(char* new_file, char* mode);

int main(int argc, char* argv[])
{
    FILE* fp_in, *fp_out;
    char str_from[MAX_BUF], str_to[MAX_BUF], buf_line[MAX_LINE];

    fputs("File sorgente da cui copiare i dati: ", stdout);
    fgets(str_from, MAX_BUF, stdin);

    remove_new_line(str_from, sizeof(str_from));

    fputs("File destinazione in cui inserire i dati: ", stdout);
    fgets(str_to, MAX_BUF, stdin);

    remove_new_line(str_to, sizeof(str_to));

    fp_in = open_file(str_from, "r");
    fp_out = open_file(str_to, "w");

    while (fgets(buf_line, MAX_LINE, fp_in) != NULL) {
        if (fputs(buf_line, fp_out) == EOF) {
            fprintf(stderr, "Err: fputs(), %s: %s\n", strerror(errno), buf_line);
            exit(EXIT_FAILURE);
        }
    }

    fclose(fp_in);
    fclose(fp_out);

    return (EXIT_SUCCESS);
}

/*
Elimina il carattere di new-line, qualora vi fosse, inserendo al suo posto
il carattere null-teriminated; senza questa funzione sia la lettura sia la
scrittura di un file non andrebbero a buon fine.
*/
void remove_new_line(char buf[], int size)
{
    int i;

    for (i = 0; i < size; i++) {
        if (buf[i] == '\n') {
            buf[i] = '\0';
        }
    }

    /* Apre un file in lettura o in scrittura */
    FILE* open_file(char* new_file, char* mode) {
        FILE* fp;

        if ((fp = fopen(new_file, mode)) == NULL) {
            fprintf(stderr, "Err(%d) - %s: %s\n", errno, strerror(errno), new_file);
            exit(EXIT_FAILURE);
        }

        return fp;
    }
