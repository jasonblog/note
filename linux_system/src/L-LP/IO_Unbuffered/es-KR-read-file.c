#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main(int argc, char* argv[])
{
    char c;

    /*
     * Come si puo' notare, il parametro della read() che prevede la lunghezza
     * del buffer è impostato ad 1, ciò vuol dire che la lettura dell'input sarà
     * carattere per carattere.
     *
     * Codice estremamente efficiente
     */
    while (read(0, &c, 1) > 0) {
        write(STDOUT_FILENO, &c, sizeof(c));
    }

    return (EXIT_SUCCESS);
}
