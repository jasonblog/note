#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <fcntl.h>

/*
HEADER    : <unistd.h>
PROTOTYPE : int dup(int fd);
            int dup2(int fd, int fd2);
SEMANTICS : La funzione dup(), duplica il file descriptor 'fd'; la funzione
            dup2() duplica il file descriptor 'fd' in 'fd2'.
RETURNS   : Enrambe ritornanano il nuovo file descriptor, -1 in caso di errore
--------------------------------------------------------------------------------
*/

int main(int argc, char* argv[])
{
    int fd, fd2, fd3, nr;
    char buf;

    if (argc < 2) {
        fprintf(stderr, "Uso: %s <filename>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    if ((fd = open(argv[1], O_RDONLY)) < 0) {
        fprintf(stderr, "Err.:(%d) - %s: %s\n", errno, strerror(errno), argv[1]);
        exit(EXIT_FAILURE);
    }

    /*
     Duplica mediante la funzione dup() il file descriptor 'fd', dopodiche' lo si
     chiude.
    */
    fd2 = dup(fd);
    close(fd);

    /*
     Una nuova duplicazione del file descriptor utilizzando la funzione dup2(),
     si duplica 'fd2' in 'fd3'.
    */
    dup2(fd2, fd3);
    close(fd2);

    /*
     Si provvede alla lettura del file utilizzando l'ultimo file descriptor
    */

    while ((nr = read(fd3, &buf, 1)) > 0) {
        write(STDOUT_FILENO, &buf, nr);
    }

    close(fd3);

    return (EXIT_SUCCESS);
}
