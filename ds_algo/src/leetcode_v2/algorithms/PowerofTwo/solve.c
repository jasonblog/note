#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
bool isPowerOfTwo(int n)
{
    if (n <= 0) {
        return false;
    }

    return !(n & (n - 1));
}
int main(int argc, char** argv)
{
    int n;

    while (scanf("%d", &n) != EOF) {
        printf("%d\n", isPowerOfTwo(n));
    }

    return 0;
}
