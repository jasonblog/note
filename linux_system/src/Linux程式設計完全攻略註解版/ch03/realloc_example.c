#include <stdio.h>
#include <stdlib.h>
int main(int argc, char* argv[], char* envp[])          //(1)
{
    int input;
    int n;
    int* numbers1;
    int* numbers2;
    numbers1 = NULL;

    if ((numbers2 = (int*)malloc(5 * sizeof(int))) == NULL) { //(2)
        printf("malloc memory unsuccessful");
        //free(numbers2);
        numbers2 = NULL;
        exit(1);
    }

    for (n = 0; n < 5; n++) {           //(3)
        *(numbers2 + n) = n;
        printf("numbers2's data: %d\n", *(numbers2 + n));
    }

    printf("Enter an integer value you want to remalloc ( enter 0 to stop)\n");
    //(4)
    scanf("%d", &input);

    numbers1 = (int*)realloc(numbers2, (input + 5) * sizeof(int)); //(5)

    if (numbers1 == NULL) {
        printf("Error (re)allocating memory");
        exit(1);
    }

    for (n = 0; n < 5; n++) {           //(6)
        printf("the numbers1s's data copy from numbers2: %d\n", *(numbers1 + n));
    }

    for (n = 0; n < input; n++) {           //(7)
        *(numbers1 + 5 + n) = n * 2;
        printf("nummber1's new data: %d\n", *(numbers1 + 5 + n)); // numbers1++;
    }

    printf("\n");
    free(numbers1);                     //(8)
    numbers1 = NULL;
    // free(numbers2);                  //(9)
    return 0;
}


