#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <time.h>
#include <unistd.h>

/*
Calendar time: il numero di secondi a partire dalle 00:00 del 01 gennaio 1970
ITC, Epoch; indica il tempo a cui l'orologio del kernel fa riferimento, usato
generalmente per le date dei file oppure per il calcolo di tempi con precisione
di 1 secondo (troppo in realta' se si volesse calcolare un pezzo di codice).

HEADER    : <time.h>
PROTOTYPE : time_t time(time_t *tm);
SEMANTICS : La funzione time() ritorna la data espressa in numero di secondi a
            partire da Epoch, ossia 1970-01-01 00:00:00 +0000 (UTC), e salvata
        in 'tm'.
RETURNS   : Il valore di time in n caso di successo, -1 in caso di errore
--------------------------------------------------------------------------------
*/

int main(int argc, char* argv[])
{
    time_t start, end;

    if ((start = time(&start)) < 0) {
        fprintf(stderr, "Err.(%s) getting time, start\n", strerror(errno));
        exit(EXIT_FAILURE);
    }

    /* Sospendo l'esecuzione del programma per circa 1 secondo e mezzo, espresso
     in millisecondi */
    usleep(1500000);

    if ((end = time(&end)) < 0) {
        fprintf(stderr, "Err.(%s) getting time, end\n", strerror(errno));
        exit(EXIT_FAILURE);
    }

    /* Come si potra' notare non sara' preciso il risultato */
    printf("Tempo trascorso: %f secondi\n", difftime(start, end));

    return (EXIT_SUCCESS);
}
