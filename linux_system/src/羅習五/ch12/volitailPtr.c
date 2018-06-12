#include <stdio.h>
#include <stdlib.h>

volatile int global;

int main(int argv, char** argc) {
    volatile int* volPtr;
    int* ptr;
    volPtr = &global;
    ptr = &global;
    *volPtr = 0xc0fe;
    *ptr = 0xdead;
    for (int i=0; i< 100; i++) {
        (*volPtr)++;
        (*ptr)++;
    }
    return *volPtr|*ptr;
}
