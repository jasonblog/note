#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>

/* Sui sistemi UNIX il modo piu' semplice per allocare memoria dinamicamente
e' mediante le syscalls brk() e sbrk() che consentono la gestione e il relativo
innalzamento del 'program break'.

Nota: Il 'program break' e' la prima locazione dopo la fine del segmento BSS,
      ovvero il segmento dei dati non inizializzati.

Nota: Sia brk() sia sbrk() creano allocazioni per pagine (4096 KB)

HEADER    : <unistd.h>
PROTOTYPE : void *sbrk (intptr_t increment);

La syscall sbrk() consente di incrementare e/o decrementare il valore corrente
del 'program break' mediante un offset intero specificato da 'increment';
va da se che incrementare sta per allocare memoria, decrementare invece sta per
deallocare. Ritorna il valore del 'program break' in caso di esito positivo,
(void*)-1 nel caso fallisse. La chiamata sbrk(0) restituisce la locazione
corrente del 'program break'.

E' necessario aggiungere in fase di compilazione o la macro BSD_SOURCE oppure
la macro SVID_SOURCE, altrimenti il programma non compilerebbe:
$ (gcc o clang) -std=c11 -Wall -pedantic -D_BSD_SOURCE file.c
$ (gcc o clang) -std=c11 -Wall -pedantic -D_SVID_SOURCE file.c
*/

/* Lo scopo del programma e' di incrementare il 'program break' di 10 byte,
di salvare il suo valore prima e dopo la chiamata alla system call e infine
stampare in output i relativi valori. */

int main(void)
{
    static const intptr_t size_incr = 10;
    void* pb_old_addr, *pb_current_addr;

    // Si ottiene il valore corrente del 'program 'break'
    if ((pb_old_addr = sbrk(0)) == (void*) - 1) {
        fprintf(stderr, "Err.(%d) old sbrk(0): %s\n", errno, strerror(errno));
        exit(EXIT_FAILURE);
    }

    // Si aumenta il 'program break' di 'size_incr' byte
    if (sbrk(size_incr) == (void*) -  1) {
        fprintf(stderr, "Err.(%d) sbrk(): %s\n", errno, strerror(errno));
        exit(EXIT_FAILURE);
    }

    // Si ottiene nuovamente il valore corrente del 'program 'break'
    if ((pb_current_addr = sbrk(0)) == (void*) - 1) {
        fprintf(stderr, "Err.(%d) cur sbrk(0): %s\n", errno, strerror(errno));
        exit(EXIT_FAILURE);
    }

    printf("    (old) 'program break' address: %p\n", pb_old_addr);
    printf("(current) 'program break' address: %p\n", pb_current_addr);

    return (EXIT_SUCCESS);
}
