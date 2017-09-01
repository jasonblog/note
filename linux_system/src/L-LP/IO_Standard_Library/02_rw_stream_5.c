#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>

#define MAX_BUF 4096

int main(int argc, char* argv[])
{
    FILE* fp_in;
    int c;

    if (argc != 2) {
        fprintf(stderr, "Uso: %s <input_file>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    if ((fp_in = fopen(argv[1], "r")) == NULL) {
        fprintf(stderr, "Err(%d) %s: %s\n", errno, strerror(errno), argv[1]);
        exit(EXIT_FAILURE);
    }

    while (!feof(fp_in)) {
        if ((c = fgetc(fp_in)) != EOF) {
            fputc(c, stdout);
        }
    }

    if (ferror(fp_in)) {
        fprintf(stderr, "Err(%d) %s: stream error\n", errno, strerror(errno));
        exit(EXIT_FAILURE);
    }

    fclose(fp_in);

    return (EXIT_SUCCESS);
}
