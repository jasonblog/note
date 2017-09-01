#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <fcntl.h>

#define BUF_SIZE 1024

/*
HEADER    : <unistd.h>

PROTOTYPE : int symlink(const char *actualpath, const char symlinkpath);
            ssize_t readlink(const char* restrict pathname, char *restrict buf,
                         size_t bufsize);

SEMANTICS : symlink() crea una nuova voce di directory 'symlinkpath', essa punta
            ad 'actualpath'; readlink() consente la lettura del symbolic link
        'pathname' nel buffer 'buf' di dimensione 'bufsize'.

RETURNS   : symlink()  - 0 In caso di successo, -1 in caso di errore
            readlink() - il numero dei byte letti in caso di successo, 0 in caso
        di errore.
--------------------------------------------------------------------------------
*/

int main(int argc, char* argv[])
{
    int fd;
    char buf[BUF_SIZE];

    if (argc < 3) {
        fprintf(stderr, "Uso: %s <origine> <symbolic-link>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    /* Crea il file di origine */
    if ((fd = open(argv[1], O_CREAT, S_IRWXU | S_IRGRP | S_IROTH)) < 0) {
        fprintf(stderr, "Err.:(%d) - %s: %s\n", errno, strerror(errno), argv[1]);
        exit(EXIT_FAILURE);
    }

    /* Crea il symbolic-link verso il file di origine */
    if (symlink(argv[1], argv[2]) < 0) {
        fprintf(stderr, "Err.:(%d) - Create symbolic-link: %s\n", errno,
                strerror(errno));
        exit(EXIT_FAILURE);
    }

    if (readlink(argv[2], buf, BUF_SIZE) < 0) {
        fprintf(stderr, "Err.:(%d) - Read symbolic-link: %s\n", errno,
                strerror(errno));
        exit(EXIT_FAILURE);
    }

    printf("                       Origine: '%s'\n", argv[1]);
    printf("                 Symbolic-link: '%s'\n", argv[2]);
    printf("Il contenuto del symbolic-link: '%s'\n", buf);

    close(fd);

    return (EXIT_SUCCESS);
}
