#include <unistd.h>
#include "malloc_one.h"

// L'allocatore e' inizializzato
int init_allocator = 0;

// Puntatore all'inizio dell'area di memoria da gestire
void* beginning_memory;

// Individua l'ultimo indirizzo valido, riconducibile a 'break'
void* last_address;

// Inizializza l'allocatore di memoria
void init_alloc(void)
{
    // Salva l'ultimo indirizzo valido puntato da 'break'
    last_address = sbrk(0);

    beginning_memory = last_address;

    init_allocator = 1;
}

// Libera la memoria utilizzata
void free_mem(void* first_byte)
{
    struct memory_block* block;

    // backup
    block = first_byte - sizeof(struct memory_block);

    // Marca il blocco come disponibile
    block->free = 1;
}

// Assegna tanta memoria quanta richiesta da 'size'
void malloc_one(size_t size)
{
    void* cur_location;
    struct memory_block* cur_block;
    void* location;
    size_t fixed_size;

    if (!init_allocator) {
        init_alloc();
    }

    fixed_size = size + sizeof(struct memory_block);

    location = 0;

    cur_location = beginning_memory;

    while (cur_location != last_address) {
        cur_block = (struct memory_block*)cur_location;

        if (cur_block->free) {
            if (cur_block->size >= fixed_size) {
                cur_block->free = 0;
                location = cur_location;
                break;
            }
        }

        cur_location = (char*)cur_location + cur_block->size;
    }

    if (!location) {
        sbrk(fixed_size);
        location = last_address;
        last_address = (char*)last_address + fixed_size;
        cur_block = location;
        cur_block->free = 0;
        cur_block->size = fixed_size;
    }

    location = (char*)location + sizeof(struct memory_block);
}
