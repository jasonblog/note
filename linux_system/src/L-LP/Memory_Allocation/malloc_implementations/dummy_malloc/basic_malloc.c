#include <stdio.h>
#include <stdlib.h>
#include <unistd.h> // sbrk()
#include <errno.h>
#include <string.h>
#include <assert.h>
#include "basic_malloc.h"

/* my malloc (a)
utilizza la syscall sbrk() per la manipolazione dello heap; sbrk(0) ritorna un
puntatore a 'break', ovvero in cima allo heap, sbrk(n) incrementa lo heap di
'n' byte e ritorna un puntatore al nuovo indirizzo. In caso di fallimento
riorna (void *)-1, con il settaggio appropriato della variabile 'errno'. */

void* basic_malloc(size_t size)
{
    void* ptr = sbrk(0);
    void* increment = sbrk(size);

    if (increment == (void*) - 1) {
        fprintf(stderr, "Err.(%d) sbrk(): %s\n", errno, strerror(errno));
        exit(EXIT_FAILURE);
    } else {
        assert(ptr == increment);
        return ptr;
    }
}
