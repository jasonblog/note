#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <time.h>

/*
HEADER    : <time.h>
PROTOTYPE : time_t mktime(struct tm *ptm);
SEMANTICS : Converte 'ptm' da broken-down time a time_t.
RETURNS   : Ritorna il valore in calendar-time in caso di successo, -1 in caso
            di errore
--------------------------------------------------------------------------------
*/

int main(int argc, char* argv[])
{
    struct tm* ptm;
    time_t t, new_time;

    if ((t = time(NULL)) < 0) {
        fprintf(stderr, "Err.(%s) getting time\n", strerror(errno));
        exit(EXIT_FAILURE);
    }

    ptm = localtime(&t);

    /* La funzione mktime(), come accennato nella semantica, converte da
     broken-down time a calendar time */
    new_time = mktime(ptm);

    printf("conversione con mktime(): %ld secondi\n", new_time);

    return (EXIT_SUCCESS);
}
