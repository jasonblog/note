#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
uint32_t reverseBits(uint32_t n)
{
    uint32_t m = 0;

    for (int i = 0; i < 31; ++i) {
        m |= (n & 0x1);
        m <<= 1;
        n >>= 1;
    }

    return m | (n & 0x1);
}
int main(int argc, char** argv)
{
    uint32_t i;

    while (scanf("%u", &i) != EOF) {
        printf("%x = %x\n", i, reverseBits(i));
    }

    return 0;
}

