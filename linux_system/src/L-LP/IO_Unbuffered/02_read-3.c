#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>

int main(int argc, char* argv[])
{
    int fd, n;
    char buf;

    if (argc < 2) {
        fprintf(stderr, "Uso: %s <file name>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    if ((fd = open(argv[1], O_RDONLY)) < 0) {
        fprintf(stderr, "Err. open file\n");
        exit(EXIT_FAILURE);
    }

    /*
     In questo caso, a differenza degli esempi precedenti, si legge un carattere
     per volta, utilizzando un ciclo while naturalmente e non una espressione
     condizionale.
    */
    while ((n = read(fd, &buf, 1)) > 0) {
        write(STDOUT_FILENO, &buf, n);
    }

    close(fd);

    return (EXIT_SUCCESS);
}
