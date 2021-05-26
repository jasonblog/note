#ifndef INCLUDE_BINDINGS_H
#define INCLUDE_BINDINGS_H

#include <stdint.h>
#include <stdlib.h>
#include <stdbool.h>

typedef struct Point {
  int32_t x;
  int32_t y;
} Point;

int32_t *alloc_memory(void);

Point create_point(int32_t x, int32_t y);

int32_t *create_vec(uintptr_t *size);

void free_memory(int32_t *x);

void free_vec(int32_t *vec, uintptr_t size);

void say_hello(const char *message);

#endif /* INCLUDE_BINDINGS_H */
