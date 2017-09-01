#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <sys/time.h>
#include <unistd.h>

/*
HEADER    : <sys/time.h>
PROTOTYPE : int gettimeofday(struct timeval *tv, struct timezone *tz);
SEMANTICS : La funzione gettimeofday() salva nella struttura timeval 'tv' la
            data corrente, a partire da  "Epoch" e puntata da 'tz.
RETURNS   : 0 In caso di successo, -1 in caso di errore
--------------------------------------------------------------------------------
Nota: Le funzione utilizza una struttura 'timeval' e una struttura 'timezone'
definite in <sys/time.h>, il secondo parametro tuttavia e' obsoleto e deve
essere sempre posto a NULL.

Precisione al microsecondo:

struct timeval {
     long tv_sec;   "Secondi"
     long tv_usec;  "Microsecondi"
};

Precisione al nanosecondo:

struct timespec {
     time_t tv_sec; "Secondi"
     long   tv_nsec;    "Nanosecondi"
};

Tale funzione puo' essere indicata per verificare i tempi di reazione di un
programma o di un pezzo di codice e con opportuni accorgimenti anche per
stampare la data corrente.
*/

int main(int argc, char* argv[])
{
    struct timeval start, end;
    double time_elapsed;

    /* Rispetto all'esempio con time() il risultato sara' sicuramente molto
     piu' accurato, poiche' esresso in millisecondi */
    if (gettimeofday(&start, NULL) < 0) {
        fprintf(stderr, "Err.(%s) getting time, start\n", strerror(errno));
        exit(EXIT_FAILURE);
    }

    /* Sospendo l'esecuzione del programma per circa 1 secondo e mezzo, espresso
     in millisecondi */
    usleep(1500000);

    if (gettimeofday(&end, NULL) < 0) {
        fprintf(stderr, "Err.(%s) getting time, end\n", strerror(errno));
        exit(EXIT_FAILURE);
    }

    time_elapsed = ((((end.tv_sec - start.tv_sec) * 1000000.) +
                     ((end.tv_usec - start.tv_usec))) / 1000000.);

    printf("Tempo trascorso: %f secondi\n", time_elapsed);

    return (EXIT_SUCCESS);
}
