#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <time.h>

/*
HEADER    : <time.h>
PROTOTYPE : char *asctime(const struct tm *ptm);
SEMANTICS : La funzione asctime() partendo dal parametro broken-down time 'ptm'
            produce una stringa null-terminated, simile al comando 'date'
RETURNS   : Il puntatore alla stringa in caso di successo, NULL in caso di
            errore
--------------------------------------------------------------------------------
*/

int main(int argc, char* argv[])
{
    struct tm* ptm;
    time_t t;
    char* str_time;

    if ((t = time(NULL)) < 0) {
        fprintf(stderr, "Err.(%s) getting time\n", strerror(errno));
        exit(EXIT_FAILURE);
    }

    if ((ptm = gmtime(&t)) == NULL) {
        fprintf(stderr, "Err.(%s) conversion gmtime()\n", strerror(errno));
        exit(EXIT_FAILURE);
    }

    /* La conversione in stringa, partendo da un valore definito in broken-down
     time */
    if ((str_time = asctime(ptm)) == NULL) {
        fprintf(stderr, "Err.(%s) str conversion: asctime()\n", strerror(errno));
        exit(EXIT_FAILURE);
    }

    printf("%s", str_time);

    return (EXIT_SUCCESS);
}
