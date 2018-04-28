#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>

#define MAX_BUF 1024

int main(void)
{
    char finput[] = "/etc/fstab";
    int fd;
    char buffer[MAX_BUF];

    if ((fd = open(finput, O_RDONLY)) < 0) {
        fprintf(stderr, "Err. open file\n");
        exit(EXIT_FAILURE);
    }

    /*
     Legge il contenuto del file indicato dal file descriptor 'fd',
     copiandolo in 'buffer'; ovviamente la lettura riguarda MAX_BUF byte.
    */
    if (read(fd, buffer, MAX_BUF) < 0) {
        fprintf(stderr, "Err. read file\n");
        exit(EXIT_FAILURE);
    }

    /*
     Come si puo' notare si utilizza la printf per scrivere i dati, ma non e'
     efficiente come se si fosse utilizzato la write()
    */
    printf("%s", buffer);
    close(fd);

    return (EXIT_SUCCESS);
}
