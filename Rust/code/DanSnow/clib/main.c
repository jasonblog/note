#include <stdio.h>
#include "bindings.h"

// void say_hello(const char *message);
//
// typedef struct _Point {
//   int x;
//   int y;
// } Point;
//
// Point create_point(int x, int y);
// int *alloc_memory(void);
// void free_memory(int *x);
// int *create_vec(size_t *size);
// void free_vec(int *vec, size_t size);

int main() {
  say_hello("Hello");
  Point p = create_point(10, 20);
  printf("x: %d, y: %d\n", p.x, p.y);

  int *x = alloc_memory();
  *x = 123;
  printf("%d\n", *x);
  free_memory(x);

  size_t size;
  int *vec = create_vec(&size);
  for (size_t i = 0; i < size; ++i) {
    printf("%d ", vec[i]);
  }
  puts("");
  free_vec(vec, size);
}
