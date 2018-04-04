#include <stdio.h>
#include <stdlib.h>
union word {
    int     a;
    char        b;
} c;
int checkCPU(void)
{
    c.a = 1;
    printf("c.b=%d\n", c.b);
    return (c.b == 1);
}
int main(void)
{
    int i;
    i = checkCPU();

    if (i == 0) {
        printf("this is Big_endian\n");
    } else if (i == 1) {
        printf("this is Little_endian\n");
    }

    return 0;
}
