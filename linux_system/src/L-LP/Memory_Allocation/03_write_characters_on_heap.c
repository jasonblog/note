#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <stdint.h>

/* Lo scopo del programma e' di scrivere la stringa "Hello World" a partire
dall'indirizzo iniziale della heap, ovvero dal program break, dopodiche'
stampare la stringa sia carattere per carattere sia come stringa unica. */

int main(int argc, char* argv[])
{
    void* pb_old_addr, *pb_cur_addr;
    intptr_t size_incr = 12;

    // Salva il valore corrente del 'program break'
    if ((pb_old_addr = sbrk(0)) == (void*) - 1) {
        fprintf(stderr, "Err.(%d) old sbrk(0): %s\n", errno, strerror(errno));
        exit(EXIT_FAILURE);
    }

    // Incrementa il 'program break' di 'size_incr' byte
    if (brk(((uint8_t*)pb_old_addr + size_incr)) == -1) {
        fprintf(stderr, "Err.(%d) brk(): %s\n", errno, strerror(errno));
        exit(EXIT_FAILURE);
    }

    // Salva nuovamente il valore corrente del 'program break'
    if ((pb_cur_addr = sbrk(0)) == (void*) - 1) {
        fprintf(stderr, "Err.(%d) cur sbrk(0): %s\n", errno, strerror(errno));
        exit(EXIT_FAILURE);
    }

    // Setta la stringa carattere per carattere
    *((char*)pb_old_addr) = 'H';
    *((char*)pb_old_addr + 1) = 'e';
    *((char*)pb_old_addr + 2) = 'l';
    *((char*)pb_old_addr + 3) = 'l';
    *((char*)pb_old_addr + 4) = 'o';
    *((char*)pb_old_addr + 5) = ' ';
    *((char*)pb_old_addr + 6) = 'W';
    *((char*)pb_old_addr + 7) = 'o';
    *((char*)pb_old_addr + 8) = 'r';
    *((char*)pb_old_addr + 9) = 'l';
    *((char*)pb_old_addr + 10) = 'd';
    *((char*)pb_old_addr + 11) = '\0';

    //  Stampa 'char by char'
    for (int i = 0; i < size_incr; i++) {
        printf("%c", *((char*)pb_old_addr + i));
    }

    // Stampa la fornendo come indirizzo iniziale il vecchio 'program break'
    printf("\n%s\n", (char*)pb_old_addr);

    // E se si scrivesse oltre il limite dei size_incr?
    *((char*)pb_old_addr + 100) = 'o';
    *((char*)pb_old_addr + 1000) = 'p';
    *((char*)pb_old_addr + 2000) = 's';
    *((char*)pb_old_addr + 3000) = '!';
    *((char*)pb_old_addr + 4095) = '!';

    /* La memoria e' suddivisa in pagine da 4096 byte e il 'program break' non
    e' ubicato sul limite, pertanto vi saranno migliaia di indirizzi di
    memoria liberi pronti per essere sfruttati. Il programma corrente cosi'
    com'e' scritto compilerebbe e sarebbe anche eseguito con successo, un
    'segmentation fault' sarebbe occorso solo se si fosse tentato di scrivere
    anche un solo byte oltre il limite della pagina, ovvero:
    *((char *)pb_old_addr + 4096) = '!'; */

    return (EXIT_SUCCESS);
}
