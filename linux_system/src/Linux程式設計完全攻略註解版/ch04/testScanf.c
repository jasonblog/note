#include  <stdio.h>
int main(void)
{
    int a, b;
    scanf("%2d%*2d%1d", &a, &b);
    printf("%d\n", a - b);
    return 0;
}


