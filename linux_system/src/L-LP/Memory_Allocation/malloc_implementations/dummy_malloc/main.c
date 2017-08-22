#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "basic_malloc.h"

int main(void)
{
    const char* title = "The Unix Programming Environment";
    const unsigned int size = strlen(title);
    char* ptr;

    // basic_malloc() testing function
    // Alloca 'size' byte sullo heap, dopodiche' copia e stampa la stringa
    ptr = basic_malloc(size);
    strcpy(ptr, title);
    printf("%s\n", ptr);

    /* Con la prima funzione per l'allocazione della memoria basic_malloc(),
    non puo' essere invocata free() per liberare la memoria, per cui si
    utilizza' un semplice puntatore a NULL, che peraltro dovrebbe essere una
    buona regola dopo la deallocazione della memoria. */
    ptr = NULL;

    return (EXIT_SUCCESS);
}
