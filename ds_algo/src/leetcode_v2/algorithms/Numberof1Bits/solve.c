#include <stdio.h>
#include <stdlib.h>
typedef unsigned int uint32_t;
int hammingWeight(uint32_t n)
{
    int sum = 0;

    while (n) {
        ++sum;
        n &= (n - 1);
    }

    return sum;
}
int main(int argc, char** argv)
{
    uint32_t n;

    while (scanf("%u", &n) != EOF) {
        printf("%d\n", hammingWeight(n));
    }

    return 0;
}
