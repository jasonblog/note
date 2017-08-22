#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>

#define MAX_BUF 32

FILE* open_file(char* new_file, char* mode);

/*
Il programma copia il contenuto di un file in un altro file, utilizzando solo
funzioni relative ai caratteri; "character at a time I/O".
*/
int main(int argc, char* argv[])
{
    FILE* fp_in;
    FILE* fp_out;
    int c;

    if (argc != 3) {
        fprintf(stderr, "Uso: %s <input_file> <output_file>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    fp_in = open_file(argv[1], "r");
    fp_out = open_file(argv[2], "w");

    while ((c = fgetc(fp_in)) != EOF) {
        if (fputc(c, fp_out) == EOF) {
            fprintf(stderr, "Err(%d) - %s: EOF\n", errno, strerror(errno));
            exit(EOF);
        }
    }

    fclose(fp_in);
    fclose(fp_out);

    return (EXIT_SUCCESS);
}

/* Apre un file in lettura o in scrittura */
FILE* open_file(char* new_file, char* mode)
{
    FILE* fp;

    if ((fp = fopen(new_file, mode)) == NULL) {
        fprintf(stderr, "Err(%d) - %s: %s\n", errno, strerror(errno), new_file);
        exit(EXIT_FAILURE);
    }

    return fp;
}
