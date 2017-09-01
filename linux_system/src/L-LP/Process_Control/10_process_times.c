#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <unistd.h>
#include <sys/times.h>

/*
I sistemi unix-like hanno sempre mantenuto due diverse modalita' di gestione del
tempo, Calendar Time[1] e Process Time; della prima, ci siamo gia' occupati, e
riguarda ad esempio le date di modifica dei file, l'orologio del kernel o la
data di avvio dei processi, Process Time invece e' un concetto nuovo, talvolta
e' identificato come il tempo del processore.

Su ogni processo il kernel calcola tre tempi:
- wall clock time , il tempo trascorso per eseguire un processo, tale valore
                    dipende dal numeri dei processi che sono stati eseguiti
            sul sistema;
- user CPU time   , il tempo assegnato ed impiegato dalla CPU per l'esecuzione
                    delle istruzioni del processo utente;
- system CPU time , il tempo assegnato ed impiegato dalla CPU per l'esecuzione
                    del codice del processo nel kernel;

Ciascun processo puo' invocare la funzione times() per ottenere tali valori per
se stesso e per i procesi figli; la somma di user CPU time e system CPU time
equivale al tempo effettivo che il sistema ha impiegato per eseguire uno
specifico processo.

La differenziazione tra Calendar Time e Process Time e' evidente, le operazioni
del sistema fanno sempre riferimento al primo, mentre per conoscere i tempi
di esecuzione di un processo si ricorre al secondo.

La funzione times() interagisce con la struttura 'tms' definita nell'header
<sys/times.h>:

struct tms
{
    clock_t tms_utime;       User CPU time.
    clock_t tms_stime;       System CPU time.

    clock_t tms_cutime;      User CPU time of dead children.
    clock_t tms_cstime;      System CPU time of dead children.
};

Si noti che la struttura 'tms' non contiene valori "wall clock time", tuttavia
allorquando si invoca la funzione times(), ritorna un valore di tipo clock_t,
ossia "wall clock time".

HEADER    : <sys/times.h>
PROTOTYPE : clock_t times(struct tms *buf);
SEMANTICS : Legge in buf il valore corrente dei tempi utilizzati dal processore
RETURNS   : Il tempo trascorso in tick in caso di successo, -1 in caso di errore
--------------------------------------------------------------------------------
I valori ritornati dalla funzione times() sono espressi in "clock tick", tali
valori sono convertiti in secondi calcolando il numero di clock tick al secondo,
_SC_CLK_TCK ritornato da sysconf().
*/

typedef struct tms tms_time;
static void execute_command(char* cmd);
static void process_times(clock_t real, tms_time* start_tms, tms_time* end_tms);

/* Il programma simula il comando time dei sistemi unix-like; calcola il tempo
di CPU impegato per eseguire ciascun comando passato come argomento */
int main(int argc, char* argv[])
{
    int i;
    setbuf(stdout, NULL);

    if (argc < 2) {
        fprintf(stderr, "Uso: %s <comando 1, ..., comando N>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    for (i = 1; i < argc; i++) {
        execute_command(argv[i]);
    }

    return (EXIT_SUCCESS);
}

static void execute_command(char* cmd)
{
    tms_time start_tms, end_tms;
    clock_t start, end;
    int status;

    printf("\nComando: %s\n\n", cmd);

    /* Inizia il calcolo del tempo di CPU */
    if ((start = times(&start_tms)) == -1) {
        fprintf(stderr, "Err.(%s) start times() failed\n", strerror(errno));
        exit(EXIT_FAILURE);
    }

    /* Si esegue il comando da monitorare */
    if ((status = system(cmd)) < 0) {
        fprintf(stderr, "Err.(%s) system() failed\n", strerror(errno));
        exit(EXIT_FAILURE);
    }

    /* Finisce il calcolo del tempo di CPU */
    if ((end = times(&start_tms)) == -1) {
        fprintf(stderr, "Err.(%s) end times() failed\n", strerror(errno));
        exit(EXIT_FAILURE);
    }

    /* Si passano i valori alla funzione per essere formattati e stampati sullo
    stdout */
    process_times(end - start, &start_tms, &end_tms);
}

static void process_times(clock_t real, tms_time* start_tms, tms_time* end_tms)
{
    static long clock_tick;

    if (clock_tick == 0)
        if ((clock_tick = sysconf(_SC_CLK_TCK)) < 0) {
            fprintf(stderr, "Err.(%s) sysconf() failed\n", strerror(errno));
            exit(EXIT_FAILURE);
        }

    printf("      real: %7.2f\n", real / (double) clock_tick);

    printf("      user: %7.2f\n",
           (end_tms->tms_utime - start_tms->tms_utime) / (double) clock_tick);

    printf("       sys: %7.2f\n",
           (end_tms->tms_stime - start_tms->tms_stime) / (double) clock_tick);

    printf("child user: %7.2f\n",
           (end_tms->tms_cutime - start_tms->tms_cutime) / (double) clock_tick);

    printf(" child sys: %7.2f\n",
           (end_tms->tms_cstime - start_tms->tms_cstime) / (double) clock_tick);
}
