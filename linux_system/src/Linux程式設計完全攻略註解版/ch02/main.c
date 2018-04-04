#include <stdio.h>
#include <stdlib.h>

int factorial(int n);

int main(int argc, char** argv)
{
    int n;

    if (argc < 2) {
        printf("Usage: %s \n", argv [0]);
        return -1;
    } else {
        n = atoi(argv[1]);
        printf("Factorial of %d is %d.\n", n, factorial(n));
    }

    return 0;
}

