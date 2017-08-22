#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <malloc.h>

int main(void)
{
    char text[] = "The art of Unix Programming";
    char* str = malloc(strlen(text));

    malloc_stats();

    return (EXIT_SUCCESS);
}
