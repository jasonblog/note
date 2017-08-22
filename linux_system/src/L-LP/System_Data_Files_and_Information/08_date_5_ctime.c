#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <time.h>

/*
HEADER    : <time.h>
PROTOTYPE : char *ctime(const time_t *pt);
SEMANTICS : La funzione ctime() partendo da un parametro di tipo time_t 'pt'
            produce una stringa null-terminated, simile al comando 'date'
RETURNS   : Il puntatore alla stringa in caso di successo, NULL in caso di
            errore
--------------------------------------------------------------------------------
*/

int main(int argc, char* argv[])
{
    time_t t;
    char* str_time;

    if (time(&t) < 0) {
        fprintf(stderr, "Err.(%s) getting time()\n", strerror(errno));
        exit(EXIT_FAILURE);
    }

    /* La conversione in stringa, partendo da un valore di tipo time_t */
    if ((str_time = ctime(&t)) == NULL) {
        fprintf(stderr, "Err.(%s) str conversion: ctime()\n", strerror(errno));
        exit(EXIT_FAILURE);
    }

    printf("%s", str_time);

    return (EXIT_SUCCESS);
}
