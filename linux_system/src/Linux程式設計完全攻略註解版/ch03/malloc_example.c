#include<stdio.h>               //printf()
#include<stdlib.h>              //malloc()
int main(int argc, char* argv[], char* envp[])      //(3)
{
    int count;
    int* array;

    if ((array = (int*)malloc(10 * sizeof(int))) == NULL) { //(4)
        printf("malloc memory unsuccessful");
        exit(1);
    }

    for (count = 0; count < 10; count++) {  //(5)
        *array = count;
        array++;
    }

    for (count = 9; count >= 0; count--) {  //(6)
        array--;
        printf("%4d", *array);
    }

    printf("\n");
    free(array);                    //(7)
    array = NULL;                   //(8)
    exit(0);
}

