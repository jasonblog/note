#include <stdio.h>
int main(int argc, char** argv) {
    volatile int a;
    int b;
    b = 0xdead;
    a = 0xc0fe;
    printf("a = %x, b = %x\n", a, b);
}