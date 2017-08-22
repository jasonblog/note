#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <stdint.h>

/* Lo scopo del programma e' di allocare 32 byte di memoria, dopodiche'
deallocarli */

int main(int argc, char* argv[])
{
    void* addr_a, *addr_b, *addr_c;
    intptr_t size_incr = 32;

    // Salva il valore corrente del 'program break'
    if ((addr_a = sbrk(0)) == (void*) - 1) {
        fprintf(stderr, "Err.(%d) sbrk(0): %s\n", errno, strerror(errno));
        exit(EXIT_FAILURE);
    }

    printf("%p - Current program break address (starting program) \n", \
           addr_a);

    // Incrementa il 'program break' di 'size_incr' byte
    if (brk(((uint8_t*)addr_a + size_incr)) == -1) {
        fprintf(stderr, "Err.(%d) brk(): %s\n", errno, strerror(errno));
        exit(EXIT_FAILURE);
    }

    // Salva nuovamente il valore corrente del 'program break'
    if ((addr_b = sbrk(0)) == (void*) - 1) {
        fprintf(stderr, "Err.(%d) sbrk(0): %s\n", errno, strerror(errno));
        exit(EXIT_FAILURE);
    }

    printf("%p - Current program break address (after %d byte allocation) \n", \
           addr_b, size_incr);

    /* Libera la memoria precedentemente allocata, fornendo a brk() l'indirizzo
    del 'program break' iniziale */
    if (brk(addr_a) == -1) {
        fprintf(stderr, "Err.(%d) brk(): %s\n", errno, strerror(errno));
        exit(EXIT_FAILURE);
    }

    /* Dopo aver aver liberato la memoria salva ancora una volta il valore
    corrente del 'program break' */
    if ((addr_c = sbrk(0)) == (void*) - 1) {
        fprintf(stderr, "Err.(%d) sbrk(0): %s\n", errno, strerror(errno));
        exit(EXIT_FAILURE);
    }

    printf("%p - Current program break address (after %d byte deallocation)\n", \
           addr_c, size_incr);

    return (EXIT_SUCCESS);
}
