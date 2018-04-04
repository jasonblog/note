#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>

int main(void)
{
    char buf[] = "UNIX Programming";


    /*
     Questo metodo di 'stampare' sullo stdout e' piu' efficiente, poiche' lavora
     mediante system call di basso livello, e oltre all'efficienza e'
     naturalmente piu' veloce se si fosse utilizzata la classica printf() */

    write(fileno(stdout), buf, sizeof(buf));

    printf("\n");

    /*
     Un ulteriore esempio con  una espressione condizionale per verificare la
     correttezza della buf in output, o meglio, del vettore di caratteri.
    */
    if (write(STDOUT_FILENO, buf, sizeof(buf) - 1) != sizeof(buf) - 1) {
        fprintf(stderr, "%d: %s err.\n", errno, strerror(errno));
    }

    return (EXIT_SUCCESS);
}
