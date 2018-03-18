#include <stdio.h>
#include <string.h>
int main(void)
{
    int i;
    char a[10];
    printf("input string:");
    scanf("%s", a);
    i = printf("the string is:%s\n", a); //
    printf("the last printf return %d\n", i);
    return 0;
}


