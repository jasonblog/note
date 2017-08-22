#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <assert.h>
#include "malloc_two.h"

/* L'allocazione della memoria sara' effettuata mediante una linked-list,
'base_heap' e' una variabile puntatore globale che punta alla testa della
lista */

void* base_heap = NULL;

/* Cerca un blocco di memoria libero sufficientemene grande, iniziando la
ricerca dall'indirizzo di base dello heap.
Ritorna o un puntatore al blocco di memoria libero oppure NULL. */
t_mem_block find_block(t_mem_block* last, size_t size)
{
    t_mem_block cur = base_heap;

    while (cur && !(cur->free && cur->size >= size)) {
        // Dopo l'esecuzione last puntera' all'ultimo blocco visitato
        *last = cur;
        cur = cur->next;
    }

    return cur;
}

/* Se non si dovesse trovare un blocco libero, la memoria sara' richiesta al
sistema operativo mediante sbrk(), aggiungendo in tal modo un nuovo blocco
di memoria alla fine della linked-list */
t_mem_block request_heap(t_mem_block last, size_t size)
{
    t_mem_block block;

    // block ora punta a 'break', la cima dello heap
    block = sbrk(0);

    /* Incrementa lo heap mediante una chiamata a sbrk(), naturalmente vi e'
    dello spazio aggiuntivo per la struttura mem_block */
    if (sbrk(META_BLOCK_SIZE + size) == (void*) - 1) {
        fprintf(stderr, "Err.(%d) sbrk() failed: %s\n", errno, strerror(errno));
        return (NULL);
    }

    block->size = size;
    block->next = NULL;

    if (last) {
        last->next = block;
    }

    block->free = 0;

    return block;
}

// Mediante le due funzioni precedenti, alloca la memoria necessaria
void* malloc_two(size_t size)
{
    t_mem_block block, last;

    if (size <= 0) {
        return NULL;
    }

    if (!base_heap) { // Riguarda la prima chiamata
        block = request_heap(NULL, size);

        if (!block) {
            return NULL;
        }

        base_heap = block;
    } else {
        last = base_heap;
        block = find_block(&last, size);

        if (!block) {
            // La ricerca di un blocco libero e' fallita
            block = request_heap(last, size);

            if (!block) {
                return NULL;
            }
        } else
            // Blocco libero trovato
        {
            block->free = 0;
        }
    }

    return (block + 1);
}

// Ritorna l'indirizzo della struttura
t_mem_block get_ptr_block(void* ptr)
{
    return (t_mem_block)ptr - 1;
}

// Libera i blocchi di memoria utilizzati
void free_two(void* ptr)
{
    if (!ptr) {
        return;
    }

    t_mem_block pblock = get_ptr_block(ptr);
    assert(pblock->free == 0);
    pblock->free = 1;
}
