#include<stdio.h>
int main()
{
    char a[20];
    int i = snprintf(a, 9, "%012d", 12345);
    printf("i = %d, a = %s", i, a);
    return 0;
}
