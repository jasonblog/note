#include <stdio.h>
int sum(int a)
{
    auto int c = 0;
    static int b = 5;
    c++;
    b++;
    printf("c=%d\t,b=%d\t", c, b);
    return (a + b + c);
}
int main(void)
{
    int i;
    int a = 2;

    for (i = 0; i < 5; i++) {
        printf("time %d sum(a)=%d\n", i, sum(a));
    }

    return 0;
}

