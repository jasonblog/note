#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <time.h>

/*
HEADER    : <time.h>
PROTOTYPE : struct tm *localtime(cons time_t *tp);
SEMANTICS : La funzione localtime() converte la variabile 'tp' dal formato
            calendar time al formato broken-down time, espresso nell'ora locale
RETURNS   : Un puntatore alla struttura 'tm' in caso di successo, NULL in caso
            di errore
--------------------------------------------------------------------------------
*/

int main(int argc, char* argv[])
{
    time_t t;
    struct tm* local_time;

    if ((t = time(NULL)) < 0) {
        fprintf(stderr, "Err.(%s) getting time\n", strerror(errno));
        exit(EXIT_FAILURE);
    }

    /* Si ricorda che la conversione avviene da calendar time a broken-down time
    espresso nell'ora locale */
    local_time = localtime(&t);

    printf("  Anno: %d\n  Mese: %d\nGiorno: %d\n   Ora: %d:%d:%d\n",     \
           local_time->tm_year,    \
           local_time->tm_mon,     \
           local_time->tm_mday,    \
           local_time->tm_hour,    \
           local_time->tm_min,     \
           local_time->tm_sec);

    return (EXIT_SUCCESS);
}
