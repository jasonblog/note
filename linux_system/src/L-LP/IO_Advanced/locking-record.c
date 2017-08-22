/*
 * Programma che accede in scrittura ad un file di caratteri per modificare uno
 * specifico carattere in una posizione data; il programma inoltre deve
 * consentire l'accesso esclusivo al file, applicando un blocco sulla sola
 * porzione da modificare.
 */

#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>

int main(int argc, char* argv[])
{
    int fd;
    struct flock fl;
    char* filename = "test.txt";

    if ((fd = open(filename, O_RDWR | O_CREAT)) == -1) {
        fprintf(stderr, "Uso: %s 'filename'\n", argv[0]);
        exit(EXIT_FAILURE);
    }


    fl.l_type = F_WRLCK;     /* Imposta il lock in scrittura */
    fl.l_whence = SEEK_SET;
    fl.l_start = 4;      /* lock in scrittura dal byte 4 ... */
    fl.l_len = 2;        /* ... al byte 6, ovvero di due byte */

    if (fcntl(fd, F_SETLK, &fl) == -1) {
        printf("Bloccato...");
    } else {
        getchar();
        /* Mantiene il blocco */
        lseek(fd, (long)2 * sizeof(char), SEEK_SET);
        write(fd, "*", 2);    /* Effettua la modifica ... */
        fl.l_type = F_UNLCK;  /* ... sblocca la risorsa ... */
        fl.l_whence = SEEK_SET;
        fl.l_start = 4;
        fl.l_len = 2;
    }

    close(fd);

    return (EXIT_SUCCESS);
}
