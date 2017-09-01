#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>

/*
La funzione pause() sospende temporanemante l'esecuzione di un processo,
ponendolo in uno stato di attesa, ossia in sleep, fino alla ricezione di un
segnale.

HEADER    : <unistd.h>
PROTOTYPE : int pause(void);
SEMANTICS : La funzione pause() sospende un processo in attesa di un segnale.
RETURNS   : Ritorna solo dopo la cattura del segnale e dopo che il signal
            handler e' ritornato, in tal caso ritorna -1 e la variabile
        'errno' settata a EINTR.
--------------------------------------------------------------------------------
*/

static void handler(int sig_num);
unsigned int sleep_clone(unsigned int sec);

/* Il programma simula il comportamento della funzione sleep(); anche se in
modo non propriamente elegante ed ineccepibile, ma utile a scopo didattico.
Si deve fornire come parametro il numero di secondi che s'intende porre
il processo in attesa.

Il programma causa una pericolosa race condition, infatti se il processo dovesse
essere interrotto tra la chiamata ad alarm() e pause(), potrebbe capitare che
il tempo di attesa definito in alarm() scada prima dell'esecuzione di pause(),
che sarebbe eseguita dopo l'arrivo del segnale SIGALRM.
*/

int main(int argc, char* argv[])
{
    int ret_func_value, sec_temp;

    if (argc < 2) {
        fprintf(stderr, "Uso: %s <sec. to sleep>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    sec_temp = atoi(argv[1]);

    if (signal(SIGALRM, handler) == SIG_ERR) {
        fprintf(stderr, "Err.(%s) handler() failed\n", strerror(errno));
        exit(EXIT_FAILURE);
    }

    ret_func_value = sleep_clone(sec_temp);
    printf("Returned value clone_sleep(): %d\n", ret_func_value);

    return (EXIT_SUCCESS);
}

static void handler(int sig_num)
{
    return;
}

unsigned int sleep_clone(unsigned int sec)
{
    if (signal(SIGALRM, handler) == SIG_ERR) {
        return (sec);
    }

    alarm(sec);

    pause();

    /* resetta il valore */
    return (alarm(0));
}
