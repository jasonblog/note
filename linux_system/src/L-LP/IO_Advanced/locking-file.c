#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>

void printout(char* s);

int main(int argc, char* argv[], char* envp[])
{
    char* filename = argv[1];
    int fd;
    struct flock lock;

    printout("Apertura");

    if ((fd = open(filename, O_WRONLY)) == -1) {
        printf("Errore in open())\n");
        exit(1);
    }

    /* Azzera l'area di memoria per flock */
    memset(&lock, 0, sizeof(lock));
    printout("Sto bloccando....");

    /* Setta il write lock */
    lock.l_type = F_WRLCK;

    /* Imposta il lock in scrittura */
    fcntl(fd, F_SETLKW, &lock);
    printout("Bloccato....");

    getchar();

    lock.l_type = F_UNLCK;

    /* unlock del file */
    fcntl(fd, F_SETLKW, &lock);

    close(fd);


    return (EXIT_SUCCESS);
}

void printout(char* s)
{
    printf("%s\n", s);
}
