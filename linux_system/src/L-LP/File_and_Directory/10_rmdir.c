#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <unistd.h>

#define PERMS 0755

/*
HEADER    : #include <unistd.h>

PROTOTYPE : int rmdir(const char *pathname);

SEMANTICS : rmdir() elimina la directory denominata 'pathname'

RETURNS   : 0 In caso di successo, -1 in caso di errore
--------------------------------------------------------------------------------
La directory per essere rimossa deve essere vuota, ossia link count deve avere
il valore 0, naturalmente l'operazione di rimozione comprende anche le voci di
directory "." e ".." .
*/

int main(int argc, char* argv[])
{

    if (argc < 2) {
        fprintf(stderr, "Uso: %s <dirname>\nDir da rimuovere\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    if (rmdir(argv[1]) < 0) {
        fprintf(stderr, "Err.:(%d) - %s: %s\n", errno, strerror(errno), argv[1]);
        exit(EXIT_FAILURE);
    }

    /*
     Un buon uso di questo piccolissimo programma  sarebbe di provarlo simulando
     vari scenari:
     - directory non vuota;
     - directory aperta da un altro processo;
     - directory inesistente.
     - permessi non sufficienti alla rimozione.
     E così via.

     E' sempre una buona idea capire il significato dei vari errori, anche
     perche' in UNIX non vi e' uno standard de facto per gestirli, per cui,
     spesso, ci si imbatte in programmi che li gestiscono in maniera del tutto
     diversa. Negli esempi, invece, e fin'ora, si e' tentato di fornire una
     interfaccia uniforme e coerente.
    */

    return (EXIT_SUCCESS);
}
