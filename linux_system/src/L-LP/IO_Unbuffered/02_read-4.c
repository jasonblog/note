#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <unistd.h>

#define MAX_BUF 30

/* Leggere dati da input mediante read() e stamparli in output  */

int main(int argc, char* argv[])
{
    char buf[MAX_BUF + 1];  /* E' necessario riservare spazio per la newline */
    ssize_t num_read;       /* Si usa ssize_t per la portabilita' */

    if (argc <= 1) {
        fprintf(stderr, "Usage: %s <input data>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    if ((num_read = read(STDIN_FILENO, buf, MAX_BUF)) == -1) {
        fprintf(stderr, "Err. reading file: %s\n", strerror(errno));
        exit(EXIT_FAILURE);
    }

    /* Si aggiunge il null byte terminator di fine stringa */
    buf[num_read] = '\0';

    /* Ora si possono stampare i dati ricevuti in input */
    printf("Data: %s\n", buf);

    return (EXIT_SUCCESS);
}
