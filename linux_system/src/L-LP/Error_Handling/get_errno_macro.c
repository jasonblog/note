#include <stdio.h>
#include <stdlib.h>
#include <errno.h>

/* L'argomento fornito in input fornira' informazioni sulla macro associata
alla variabile 'errno'. */

int main(int argc, char* argv[])
{

    if (argc < 2) {
        fprintf(stderr, "Uso: %s <errno_number>\n", argv[0]);
    } else {
        //errno = atoi(argv[1]);
        perror(argv[1]);
    }

    // medesimo risultato con perror("");

    return (EXIT_SUCCESS);
}
