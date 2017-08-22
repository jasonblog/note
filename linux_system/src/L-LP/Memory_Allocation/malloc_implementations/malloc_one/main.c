#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "malloc_one.h"

int main(void)
{
    const char* title = "The Unix Programming Environment";
    const unsigned int size = strlen(title);

    // malloc_one() testing function
    //void *ptr = (char *)malloc_one(size);
    //strncpy(ptr, title, strlen(title)+1);

    //printf("%s\n", ptr);

    return (EXIT_SUCCESS);
}
