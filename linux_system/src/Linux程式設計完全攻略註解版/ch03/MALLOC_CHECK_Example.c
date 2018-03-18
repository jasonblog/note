#include <stdio.h>
#include <stdlib.h>

int main(int argc, char* argv[])
{
    int* p, *q;
    p = (int*)malloc(sizeof(int));
    q = (int*)malloc(sizeof(int));
    free(p);
    //         free(p);
    printf("hello world\n");
    free(p);
    return 0;
}
