#include <stddef.h>
#include <stdio.h>

int putchar(int c) {
    printf("call putchar()\n");
    return c;
}

void* memset(void* s, int c, size_t n) {
    printf("call memset()\n");
    return s;
}
