#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <time.h>

/*
Le funzioni per la gestione del tempo in "calendar time (epoch)"  non sono
affatto intuitive qualora si volesse rappresentare una data.

Le date sono gestite mediante l'implementazione "broken-down time", che consente
di suddividere i tempi di "calendar time", in ore, minuti, secondi, etc..., tale
suddivisione viene opportunamente implementata nella struttura 'tm' definita in
<time.h>:

struct tm
{
  int tm_sec;            Seconds.   [0-60] (1 leap second)
  int tm_min;            Minutes.   [0-59]
  int tm_hour;           Hours.     [0-23]
  int tm_mday;           Day.       [1-31]
  int tm_mon;            Month.     [0-11]
  int tm_year;           Year       - 1900.
  int tm_wday;           Day of week.   [0-6]
  int tm_yday;           Days in year.  [0-365]
  int tm_isdst;          DST.       [-101]

  long int tm_gmtoff;        Seconds east of UTC.
  long int __tm_gmtoff;      Seconds east of UTC.
};

HEADER    : <time.h>
PROTOTYPE : struct tm *gmtime(const time_t *tp);
SEMANTICS : La funzione gmtime() converte la variabile 'tp' dal formato calendar
            time al formato broken-down time espresso in UTC,
RETURNS   : Un puntatore alla struttura 'tm' in caso di successo, NULL in caso
            di errore
--------------------------------------------------------------------------------
*/

int main(int argc, char* argv[])
{
    time_t t;
    struct tm* local_gm;

    if ((t = time(NULL)) < 0) {
        fprintf(stderr, "Err.(%s) getting time\n", strerror(errno));
        exit(EXIT_FAILURE);
    }

    /* Si ricorda che la conversione avviene da calendar time a  broken-down time
     espresso in UTC */
    local_gm = gmtime(&t);

    printf("  anno: %d\n  mese: %d\ngiorno: %d\n   ora: %d:%d:%d\n",
           local_gm->tm_year,
           local_gm->tm_mon,
           local_gm->tm_mday,
           local_gm->tm_hour,
           local_gm->tm_min,
           local_gm->tm_sec);

    return (EXIT_SUCCESS);
}
