#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <pthread.h>
#include <unistd.h>
#include <sys/syscall.h>

/* Così come ciascun processo ha il proprio Process-ID, ciascun thread ha il
proprio Thread-ID, da notare tuttavia che il PID ha valenza nel contesto del
sistema - quindi ID di sistema -, mentre il TID ha valenza solo nell'ambito del
processo all'interno del quale il thread e' stato creato.

Nota: Per ottenere il TID di sistema e' necessario utilizzaare la funzione
      syscall(SYS_gettid); in tal modo il Thread-ID sara' visibile mediante
      il classico comando unix ps -L.

Tipo di dato
------------
Anche i thread hanno il proprio tipo di dato, ovvero pthread_t, che al pari di
pid_t dei processi, indica un intero non negativo.

La funzione per ottenere l'ID del Thread e' pthread_self().

Nota: Non c'e' un modo portabile per ottenere il Thread-ID.

HEADER    : <pthread.h>
PROTOTYPE : pthread_t pthread_self(void);
SEMANTICS : Ritorna il Thread ID del thread chiamante
--------------------------------------------------------------------------------
*/

/* Come visualizzare il TID di sistema e interno al processo; poiche' ancora non
si affronta la creazione di nuovi thread il risultato sara' banale */
int main(void)
{
    pthread_t tid;
    int stid;

    /* TID della libreria POSIX pthreads */
    tid = pthread_self();

    /* TID di sistema, assegnato dal Sistema Operativo (assolutamente non
    portabile) */
    if ((stid = syscall(SYS_gettid)) == -1) {
        fprintf(stderr, "Err.(%d) - syscall; %s\n", errno, strerror(errno));
        exit(EXIT_FAILURE);
    }

    printf("Thread %lu (%i) nel processo %i\n", tid, stid, getpid());

    return (EXIT_SUCCESS);
}
