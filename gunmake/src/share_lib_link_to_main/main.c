#include <stdio.h>

int a = 100;
int b = 200;

int add(int,int);
int sub(int,int);

int main(int argc, char* argv[])
{
    printf("add=%d\n", add(a, b));
    printf("sub=%d\n", sub(a, b));
    return 0;
}
