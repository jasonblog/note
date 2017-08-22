#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>

/*
 Talune funzioni della libreria standard adottano un intero positivo per
 indicare un eventuale errore nel comportamento della funzione, il relativo
 codice di errore va a collocarsi nella variabile 'errno'.

 Da notare che in linea di massima sia le funzioni della libreria standard sia
 le system call adottano dei valori generici atti ad indicare lo stato di
 errore, ossia '-1', 'NULL' ed 'EOF', ma in questi casi si comprende solo
 l'occorrenza di un errore, ignorandone del tutto la natura.

 Il compito della variabile 'errno', associata a specifiche macro definite in
 errno.h, e' di fornire il tipo di errore occorso; tali macro iniziano con il
 carattere 'E', e sono a tutti gli effetti dei nomi riservati.
*/

int main(int argc, char** argv)
{
    int fd, my_err;
    errno = 0;

    /* All'inizio di una chiamata la variabile 'errno' e' impostato a 0, per
    cui ogni altro valore indichera' un codice di errore */

    if ((fd = open("/dubito/che/esista/questo/file", O_RDONLY)) < 0) {
        my_err = errno;

        if (my_err != 0) {
            fprintf(stderr, "Err.:(%d) Il file non esiste\n", errno);
            exit(my_err);
        }

        exit(EXIT_FAILURE);
    }

    /* Si potrebbe uscire dal programma fornendo alla funzione exit() il
    parametro 'errno'; il codice di uscita del programma peraltro si potra'
    verificare dalla shell mediante il classico `echo $?`. Tale pratica
    tuttavia non e' consigliata.

    Non tutte le funzioni o le system call salvano un codice di errore nella
    variiabile 'errno', per cui e' necessario porre particolare attenzione. */

    close(fd);

    return (EXIT_SUCCESS);
}
