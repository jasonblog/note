#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>

#define MAX_SIZE_ALLOC 1000000

/* Lo scopo del programma e' di verificare le conseguenze della chiamata free()
in relazione al 'program break'. Il programma alloca blocchi di memoria
multipli, dopodiche' alcuni o tutti i blocchi saranno liberati, a seconda degli
argomenti forniti dalla linea di comando.
- Il primo argomento specifica il numero dei blocchi.
- Il secondo argomento indica la grandezza dei blocchi.
- Il terzo argomento riguarda gli step del loop da attivare per liberare la
  memoria: 1 libera ogni blocco (default), 2, libera ogni secondo blocco e
  cosi' via.
- Il quarto e il quinto argomento indicano il range di blocchi da liberare; se
  sono omessi, tutti i blocchi allocati saranno liberati.
*/

int main(int argc, char* argv[])
{
    char* ptr[MAX_SIZE_ALLOC];
    int free_min, free_step, free_max, block_size, n_alloc;

    if (argc < 3 || strcmp(argv[1], "--help") == 0) {
        fprintf(stderr, "Usage: %s num-allocs block-size [step [min [max]]]\n", \
                argv[0]);
        exit(EXIT_FAILURE);
    }

    // Il numero dei blocchi di memoria
    n_alloc = strtol(argv[1], NULL, 10);

    if (n_alloc > MAX_SIZE_ALLOC) {
        fprintf(stderr, "Err. num-alloc > %d\n", MAX_SIZE_ALLOC);
        exit(EXIT_FAILURE);
    }

    // La grandezza dei blocchi di memoria
    block_size = strtol(argv[2], NULL, 10);

    free_step = (argc > 3) ? strtol(argv[3], NULL, 10) : 1;
    free_min = (argc > 4) ? strtol(argv[4], NULL, 10) : 1;
    free_max = (argc > 5) ? strtol(argv[5], NULL, 10) : n_alloc;

    if (free_max > n_alloc) {
        fprintf(stderr, "Err. free_max > num_allocs\n");
        exit(EXIT_FAILURE);
    }

    printf("current Program break: %10p\n", sbrk(0));
    printf("Allocate %d*%d bytes\n", n_alloc, block_size);

    for (int i = 0; i < n_alloc; i++) {
        ptr[i] = malloc(block_size);

        if (ptr[i] == NULL) {
            fprintf(stderr, "Err. malloc(), %s\n", strerror(errno));
            exit(EXIT_FAILURE);
        }
    }

    printf("current Program break: %10p\n", sbrk(0));
    printf("free blocks from %d to %d in steps of %d\n", \
           free_min, free_max, free_step);

    for (int i = free_min - 1; i < free_max; i += free_step) {
        free(ptr[i]);
    }

    printf("current Program break: %10p\n", sbrk(0));

    return (EXIT_SUCCESS);
}
