#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>

#define PERMS 0644
#define OFFSET 4096

char buf_1[] = "UNIX PROGRAMMING";
char buf_2[] = "unix programming";

/*
 Mostra come creare un buco, letteralmente, tra le stringhe buf_1 e buf_2
 all'interno del file 'filename'.
*/

int main(int argc, char* argv[])
{
    int fd;
    char* filename = "hole.txt";

    if ((fd = open(filename, O_RDWR | O_CREAT, PERMS)) < 0) {
        fprintf(stderr, "Err. open file\n");
        exit(EXIT_FAILURE);
    }

    /* Scrive la prima stringa 'buf_1' nel file 'filename' */
    if (write(fd, buf_1, sizeof(buf_1)) != sizeof(buf_1)) {
        fprintf(stderr, "Err. write file\n");
        exit(EXIT_FAILURE);
        /* l'offset ora e' "sizeof(buf_1)", ossia 16 */
    }

    if (lseek(fd, OFFSET, SEEK_SET) == -1) {
        fprintf(stderr, "Err. seek\n");
        exit(EXIT_FAILURE);
    }

    if (write(fd, buf_2, sizeof(buf_2)) != sizeof(buf_2)) {
        fprintf(stderr, "Err. write file\n");
        exit(EXIT_FAILURE);
        /* l'offset ora e' "OFFSET", ossia 4096 */
    }

    /*
     Se si aprisse il file 'filename' si notera' la presenza di un buco (hole)
     tra le due stringhe - ancor meglio sarebbe se lo si aprisse mediante 'od'
     o con un qualsivoglia editor in grado di leggere file grezzi e in diversi
     formati, quali l'esadecimale, l'ottale o il binario - e il relativo
     azzeramento di ciascun byte tra le due stringhe. Per azzeramento si intende
     byte posti a zero.

     $ od -c hole.txt
    */

    return (EXIT_SUCCESS);
}
