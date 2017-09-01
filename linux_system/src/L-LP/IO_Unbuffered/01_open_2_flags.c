#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>

int main(int argc, char* argv[])
{
    int input_fd, file_flags;
    mode_t file_perms;

    /* Il file sara' creato, o per lo meno dovrebbe esserlo, con i seguenti
    permessi: rw-rw-rw (0666).

    Solo uno tra O_RDWR, O_RDONLY e O_WRONLY deve essere specificato in
    flags; inoltre se O_CREAT non e' stato inserito gli eventuali permessi
    saranno ignorati, nel caso specifico 'file_perms'.  */

    file_flags = O_RDWR | O_CREAT | O_TRUNC;
    file_perms = S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH | S_IWOTH;

    if (argc != 2) {
        fprintf(stderr, "Usage: %s <pathname>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    if ((input_fd = open(argv[1], file_flags, file_perms)) < 0) {
        fprintf(stderr, "Err. opening %s\n", argv[1]);
        exit(EXIT_FAILURE);
    }

    if ((close(input_fd)) < 0) {
        fprintf(stderr, "Err. close(%s)\n", argv[1]);
        exit(EXIT_FAILURE);
    }

    return (EXIT_SUCCESS);
}
