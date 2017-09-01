#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>

/* Allorquando si lavora con thread multipli si possono verificare dei casi in
cui alcuni di essi condividano la stessa variabile o la medesima struttura dati,
ed e' proprio in tali contesti che la gestione della consistenza dei dati
diviene fondamentale.

Cos'e' la consistenza dei dati? Poniamo il caso di avere due thread che
condividono la stessa variabile in sola lettura, in questo specifico ambito vi
e' consistenza poiche' nessuno dei due thread puo' modificare la variabile, per
cui entrambi possono visualizzarla nel suo valore originario; se invece uno dei
due thread avesse accesso alla variabile anche in scrittura e dovesse
modificarne il valore, la variabile stessa si troverebbe in uno stato
inconsistente, proprio perche' il thread in sola lettura leggerebbe prima un
valore e poi un altro. Vi dev'essere coerenza per ciascun thread.

Per risolvere il problema si deve disporre di un meccanismo di lock, che
consenta ad un thread per volta l'accesso alla variabile, in pratica vi deve
essere sincronizzazione.

Mutex (Mutual exclusion - Mutua esclusione)
-------------------------------------------
I dati possono essere protetti assicurandosi che solo un thread per volta possa
avere l'accesso per la modifica di una risorsa condivisa, a tale scopo si usano
sono i 'mutex', che consentono di operare un lock sulla risorsa condivisa e di
rilasciarla 'unlock' ad operazione ultimata.

Un mutex e' una variabile di tipo pthread_mutex_t.

Quando un thread blocca 'lock' un mutex diviene il proprietario di quel mutex e
solo il proprietario stesso potra' sbloccarlo, gli altri thread che tenteranno
di usare la risorsa condivisa la troveranno bloccata e andranno in sleep fino a
quando la risorsa non sara' stata sbloccata, ossia ad 'unlock' avvenuto.

Un mutex puo' essere allocato sia staticamente sia dinamicamente, per cui prima
di essere utilizzato deve essere allocato; l'inizializzazione di un mutex
allocato staticamente puo' essere effettuata assegnando
PTHREAD_MUTEX_INITIALIZER ad una variabile mutex:

pthread_mutex_t mutex_var = PTHREAD_MUTEX_INITIALIZER;

Per l'inizializzazione di un mutex allocato dinamicamente si una invece la
funzione pthread_mutex_init().

Per il 'lock' e 'unlock' di un mutex si usano le funzioni pthread_mutex_lock() e
pthread_mutex_unlock() rispettivamente:

HEADER    : <pthread.h>
PROTOTYPE : int pthread_mutex_lock(pthread_mutex_t *mutex);
            int pthread_mutex_unlock(pthread_mutex_t *mutex);
SEMANTICS : La funzione pthread_mutex_lock() blocca 'mutex';
            La funzione pthread_mutex_unlock() sblocca 'mutex';
RETURNS   : 0 in caso di successo, numero di errore in caso di errore
--------------------------------------------------------------------------------
*/

void* thr_func(void* arg);

static int globvar = 0;
/* Inizializzazione statica mutex */
static pthread_mutex_t mtx = PTHREAD_MUTEX_INITIALIZER;

int main(int argc, char* argv[])
{
    int nloop;
    int thr_ret, i;
    pthread_t thrID1, thrID2, thrID3;

    if (argc != 2) {
        fprintf(stderr, "Uso: %s <num threads>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    nloop = atoi(argv[1]);

    if ((thr_ret = pthread_create(&thrID1, NULL, thr_func, &nloop)) != 0) {
        fprintf(stderr, "Err. pthread_create() %s\n", strerror(thr_ret));
        exit(EXIT_FAILURE);
    }

    if ((thr_ret = pthread_join(thrID1, NULL)) != 0) {
        fprintf(stderr, "Err. pthread_join() %s\n", strerror(thr_ret));
        exit(EXIT_FAILURE);
    }

    printf("Variabile globale: %d\n", globvar);

    return (EXIT_SUCCESS);
}

void* thr_func(void* arg)
{
    int tot_loops = *((int*)arg);
    int temp, i;

    for (i = 0; i < tot_loops; i++) {
        pthread_mutex_lock(&mtx);
        temp = globvar;
        temp++;
        globvar = temp;
        pthread_mutex_unlock(&mtx);
    }

    return (NULL);
}
