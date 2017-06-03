#include<stdio.h>
#include<stdlib.h>
#include<string.h>
int main()
{
    char x[50];
    int i;

    for (i = 0; i < 50; i++) {
        x[i] = i + 1;
    }

    strncpy(x + 20, x, 20);
    strncpy(x + 20, x, 21);
    strncpy(x, x + 20, 20);
    strncpy(x, x + 20, 21);
    x[39] = '\0';
    strcpy(x, x + 20);
    x[39] = 39;
    x[40] = '\0';
    strcpy(x, x + 20);
    return 0;
}
