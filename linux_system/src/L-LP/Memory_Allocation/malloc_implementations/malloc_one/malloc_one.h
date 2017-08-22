#ifndef MALLOC_ONE_H
#define MALLOC_ONE_H

#include <stdio.h>

struct memory_block {
    int free;
    size_t size;
};

// Prototipi
void init_alloc(void);
void free_mem(void* first_byte);
void malloc_one(size_t size);

#endif
