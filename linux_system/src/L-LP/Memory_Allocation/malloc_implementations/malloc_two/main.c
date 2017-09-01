#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "malloc_two.h"

int main(void)
{
    const char* title = "The Unix Programming Environment";
    const unsigned int size = strlen(title);

    // xmalloc() testing function
    char* ptr;
    ptr = malloc_two(size);
    strncpy(ptr, title, strlen(title) + 1);
    printf("%s\n", ptr);
    free_two(ptr);

    return (EXIT_SUCCESS);
}
