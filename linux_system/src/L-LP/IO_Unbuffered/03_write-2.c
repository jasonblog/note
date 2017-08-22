#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>

#define DIMBUF 2048

int main(int argc, char* argv[], char* envp[])
{
    int fd, nread;
    char* filename = "/etc/passwd";
    char buf[DIMBUF];

    if ((fd = open(filename, O_RDONLY)) == -1) {
        printf("Errore in apertura del file\n");
        exit(1);
    }

    /* Con la dimensione di DIMBUF a 1024 leggo tranquillamente
     * l'intero file linkato da *filename, tuttavia, se avessi
     * inserito DIMBUF a 1 e un while al posto dell'if seguente
     * il risultato sarebbe stato lo stesso e probabilmente piu'
     * efficiente
     * # DIMBUF 1
     * while ( (nread = read(fd, buf, DIMBUF )) >0 )
     * ...
     */
    if ((nread = read(fd, buf, DIMBUF)) > 0) {
        write(STDOUT_FILENO, buf, nread);
    }

    close(fd);

    return (EXIT_SUCCESS);
}
