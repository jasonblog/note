#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <unistd.h>

/*
HEADER    : <stdio.h>
PROTOTYPE : int fileno(FILE *stream);
SEMANTICS : La funzione fileno() ritorna il file descriptor, numero intero
            positivo, di 'stream'.
RETURNS   : Il file descriptor in caso di successo, -1 in caso di errore
--------------------------------------------------------------------------------
*/

int main(int argc, char* argv[])
{
    FILE* stream;
    int n_str;

    if ((stream = fopen("/etc/fstab", "r")) == NULL) {
        fprintf(stderr, "Err.(%s) fopen() failed\n", strerror(errno));
        exit(EXIT_FAILURE);
    }

    if ((n_str = fileno(stream)) < 0) {
        fprintf(stderr, "Err.(%s) fileno() failed\n", strerror(errno));
        exit(EXIT_FAILURE);
    }

    printf("File descriptor, numero:  %d.\n", n_str);

    fclose(stream);

    return (EXIT_SUCCESS);
}
