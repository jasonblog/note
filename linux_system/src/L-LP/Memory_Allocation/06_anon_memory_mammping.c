#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <unistd.h>
#include <sys/mman.h>

/* L'allocazione della memoria puo' essere gestita anche mediante una mappatura
della memoria stessa utilizzando le syscall mmap() e munmap().

Una mappa o mappatura (o mapping) puo' essere sostanzialmente di due tipi:
1) File mapping.
2) Anonymous mapping.

HEADER   : <sys/mman.h>
PROTOTYPE: void *mmap(void *addr, size_t length, int prot, int flags,
                      int fd, off_t offset);

mmap() crea una nuova mappa di memoria nello spazio virtuale degli indirizzi
del processo chiamante. L'indirizzo di partenza e' indicato da 'addr', mentre
'length' e' il numero di byte da mappare.

Se 'addr' vale NULL sara' il kernel a scegliere l'indirizzo dal quale creare la
mappa, che corrisponde peraltro alla modalita' piu' portabile per la creazione
di una nuova mapppa; se invece 'addr' non e' impostato a NULL allora il
kernel prendera' in considerazione tale indirizzo per posizionare la mappa; il
kernel linux iniziera' la mappatura dalla pagina "confinante"  - da ricordare
che una pagina e' di 4096 Kb.

La variabile intera 'prot' indica la protezione della mappa, ovvero se i dati
mappati devono essere letti 'PROT_READ', scritti 'PROT_WRITE', eseguiti
'PROT_EXEC', oppure se non e' possibile acceddervi 'PROT_NONE'.
PROT_NONE deve essere usato da solo, mentre gli altri flags possono essere
combinati tra loro mediante un 'bitwise-inclusive OR'.

La variabile intera 'flags' determina se i cambiamenti ai dati mappati devono
essere visibili agli altri processi e se le eventuali modifiche devono essere
scritte su disco 'MAP_SHARED', oppure no 'MAP_PRIVATE'. Anche in questo
caso e' possibile combinare diversi flags con un 'bitwise-inclusive OR', ad
esempio 'MAP_ANON' deve essere associato a 'MAP_PRIVATE'.

Il file-descriptor 'fd' indica il file da mappare. Infine 'offset' riguarda il
punto a partire dal quale iniziare la mappatura.

In caso di successo, ritorna un puntatore all'area mappata; in caso di errore
restituisce MAP_FAILED, ovvero (void *)-1.

Nota: Riguardo l'eliminazione del mapping.
      Allorquando un processo termina la propria esecuzione, il mapping viene
      cancellato automaticamente, comprese eventuali modifiche salvate su
      disco nel caso in cui fosse stata utilizzata la protezione MAP_SHARED.

HEADER   : <sys/mman.h>
PROTOTYPE: int munmap(void *addr, size_t length);

Qualora si volesse eliminare il mapping manualmente, si dovra' invocare la
system call munmap(), che elimina il mapping specificato nel range compreso
tra l'indirizzo 'addr' e il numero di byte 'length'. Per chiudere il file
mappato, qualora ci fosse, dovra' essere invocata specificatamente la syscall
close() sul file stesso. Ritorna 0 in caso di successo, -1 altrimenti.

Nota: Una regione di memoria puo' essere contrassegnata o MAP_PRIVATE o
      MAP_SHARED, tutto il resto e' opzionale e non vincolante.

MAP_ANON non comporta file su disco, alloca la memoria per l'utilizzo privato
del processo, e' come se si utilizzasse la malloc() in un certo senso.
*/


/* The program's purpose:
Lo scopo del programma e' di allocare una pagina di memoria - tipicamente di
4096 Kb - dopodiche' prelevare l'indirizzo iniziale e finale del range di
memoria allocato e salvare i valori in variabili distinte, inizializzarle
con degli interi, stampare gli indirizzi e i valori, e infine rilasciare la
memoria */

int main(int argc, char* argv[])
{
    void* addr;

    // Inizializza len a 'memory page size'
    size_t len = sysconf(_SC_PAGESIZE);
    int protection = PROT_READ | PROT_WRITE;
    int flags = MAP_PRIVATE | MAP_ANON;

    if ((addr = mmap(NULL, len, protection, flags, -1, 0)) == MAP_FAILED) {
        fprintf(stderr, "Err.(%d) mmap(): %s\n", errno, strerror(errno));
        exit(EXIT_FAILURE);
    }

    // Indirizzo iniziale del range allocato (0)
    char* start_memaddr = (char*)addr;
    // Indirizzo finale del range allocato (4095)
    char* end_memaddr   = (char*)(addr) + 4095;

    /* Se si aggiungesse un solo byte in piu' si otterrebbe un
    'segmentation fault' */

    // Inizializzazione degli interi
    *start_memaddr = 10;
    *end_memaddr = 20;

    printf("       Allocated Memory: %d Kb\n", len);
    printf("Allocated Memory starts: %p - Val: %d\n", \
           & (*start_memaddr), *start_memaddr);
    printf("  Allocated Memory ends: %p - Val: %d\n", \
           & (*end_memaddr), *end_memaddr);

    // Elimina la memoria allocata
    if (munmap(addr, len) == -1) {
        fprintf(stderr, "Err.(%d) munmap(): %s\n", errno, strerror(errno));
        exit(EXIT_FAILURE);
    }

    return (EXIT_SUCCESS);
}
