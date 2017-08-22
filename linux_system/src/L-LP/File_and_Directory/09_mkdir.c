#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <errno.h>
#include <string.h>

#define PERMS 0755

/*
HEADER    : #include <sys/stat.h>
            #include <sys/types.h>

PROTOTYPE : int mkdir(const char *pathname, mode_t mode);

SEMANTICS : mkdir() crea una nuova directory vuota denominata 'pathname', con i
            permessi definiti da 'mode'.

RETURNS   : 0 In caso di successo, -1 in caso di errore
--------------------------------------------------------------------------------
Nella directory vuota saranno presenti le due canoniche voci di directory:
- .  (current path);
- .. (parent directory).

Da notare che una directory non e' un file, per cui i permessi dovranno essere
gestiti con particolare attenzione, ad esempio se non saranno settati i permessi
di esecuzione, essa non potra' essere acceduta; per cio' che concerne user-id e
group-id si seguono sostanzialmente le regole relative ai file.
*/

int main(int argc, char* argv[])
{

    if (argc < 2) {
        fprintf(stderr, "Uso: %s <dirname>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    if (mkdir(argv[1], PERMS) < 0) {
        fprintf(stderr, "Err.:(%d) - %s on %s\n", errno, strerror(errno), argv[1]);
        exit(EXIT_FAILURE);
    }

    return (EXIT_SUCCESS);
}
