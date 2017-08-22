#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#if defined(BSD) || defined(MACOS)
#include <sys/time.h>
#define FMT "%10lld  "
#else
#define FMT "%10ld  "
#endif
#include <sys/resource.h>

#define doit(name)  pr_limits(#name, name)

static void pr_limits(char*, int);

/* Ciascun processo e' in grado sia di ottenere sia di settare i limiti delle
proprie risorse, in particolare la funzione getrlimit() viene utilizzata per
l'ottenimento delle informazioni mentre la funzione setrlimit() e' deputata
al settaggio.

Entrambe le funzioni puntano alla struttura rlimit:
struct rlimit {
    rlim_t rlim_cur;    // limite corrente; soft limit
    rlim_t rlim_max;    // limite/valore massimo per rlim_cur; hard limit
};

HEADER    : <sys/resource.h>
PROTOTYPE : int getrlimit(int resource, struct rlimit *prl);
SEMANTICS : La funzione getrlimit() ottiene informazioni su 'resource', che
            punta alla struttura di tipo rlimit *prl.
RETURNS   : 0 in caso di successo, non zero in caso di errore
--------------------------------------------------------------------------------
HEADER    : <sys/resource.h>
PROTOTYPE : int setrlimit(int resource, const struct rlimit *prl);
SEMANTICS : La funzione setrlimit() setta i limiti di 'resource', che
            punta alla struttura di tipo rlimit *prl.
RETURNS   : 0 in caso di successo, non zero in caso di errore
--------------------------------------------------------------------------------
resource puo' essere uno dei seguenti valori:
- RLIMIT_AS - Massimo valore in byte della memoria;
- RLIMIT_CORE - Massimo valore in byte del core file;
- RLIMIT_CPU - Massimo ammontare del tempo di CPU in secondo;
- RLIMIT_DATA - Massimo valore in byte del segmento dati o data segment;
- RLIMIT_FSIZE - Massimo valore in byte di un file da creare;
- RLIMIT_LOCKS (Linux 2.4 solo) - Massimo numero di file lock per processo;
- RLIMIT_MEMLOCK - Massimo ammontare di memoria che un processo puo' bloccare
                   in memoria usando  mlock();
- RLIMIT_MSGQUEUE (Da Linux 2.6.8) - Limite massimo in byte per una coda di
                                     messaggi per un Real User ID del processo
                                     chiamante;
- RLIMIT_NICE (Da Linux 2.6.12) - valore di nice per processo;
- RLIMIT_NOFILE - Massimo numero di file aperti per processo;
- RLIMIT_NPROC - Massimo numero di processi figli per Real User ID;
- RLIMIT_RSS - Limite massimo RSS;
- RLIMIT_RTPRIO (Da Linux 2.6.12) - Limite massimo priorita' in real time;
- RLIMIT_RTTIME (Da Linux 2.6.25) - Limite in microsecondi dell'ammontare del
                                    tempo di CPU che un processo schedulato in
                                    real time puo' consumare semza essere
                                    bloccato da una system call;
- RLIMIT_SIGPENDING (Da Linux 2.6.8) - Limite sul numero dei segnali che puo'
                                       essere messo in cosa per un Real user ID
                                       del processo chiamante;
- RLIMIT_STACK - Massimo valore in byte dello stack;
- RLIMIT_VNEM - Sinonimo di RLIMIT_AS.
*/

int main(void)
{
#ifdef  RLIMIT_AS
    doit(RLIMIT_AS);
#endif
    doit(RLIMIT_CORE);
    doit(RLIMIT_CPU);
    doit(RLIMIT_DATA);
    doit(RLIMIT_FSIZE);
#ifdef  RLIMIT_LOCKS
    doit(RLIMIT_LOCKS);
#endif
#ifdef  RLIMIT_MEMLOCK
    doit(RLIMIT_MEMLOCK);
#endif
    doit(RLIMIT_NOFILE);
#ifdef  RLIMIT_NPROC
    doit(RLIMIT_NPROC);
#endif
#ifdef  RLIMIT_RSS
    doit(RLIMIT_RSS);
#endif
#ifdef  RLIMIT_SBSIZE
    doit(RLIMIT_SBSIZE);
#endif
    doit(RLIMIT_STACK);
#ifdef  RLIMIT_VMEM
    doit(RLIMIT_VMEM);
#endif
    exit(EXIT_SUCCESS);
}

static void pr_limits(char* name, int resource)
{
    struct rlimit   limit;

    if (getrlimit(resource, &limit) < 0) {
        fprintf(stderr, "Err. getrlimit() %s\n", name);
        exit(EXIT_FAILURE);
    }

    printf("%-14s  ", name);

    if (limit.rlim_cur == RLIM_INFINITY) {
        printf("(infinite)  ");
    } else {
        printf(FMT, limit.rlim_cur);
    }

    if (limit.rlim_max == RLIM_INFINITY) {
        printf("(infinite)");
    } else {
        printf(FMT, limit.rlim_max);
    }

    putchar((int)'\n');
}
