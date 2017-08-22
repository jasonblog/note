#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>

/*
Si e' visto[1] come un processo possa essere definito orfano, e le varie
implicazioni; vi e' anche il caso in cui sia il processo figlio a terminare
prima del processo padre, con il padre in attesa di ricevere informazioni sullo
stato di terminazione del processo figlio.

Nota: Il kernel deve conservare una certa quantita' di informazioni su ogni
processo in fase di terminazione (memoria e file aperti sono chiusi subito):
- PID;
- termination status;
- costi della CPU;

I processi terminati  il cui "termination status" non e' stato ancora inviato al
processo padre, sono detti "processi zombie"; in pratica il processo e' morto,
ma e' ancora 'visibile' (ad esempio col comando 'ps' o con 'top' o con 'htop'
viene indicato con la lettera Z) in attesa di fornire al padre i dati circa la
propria terminaazione.
*/

int main(int argc, char* argv[])
{
    pid_t pid;
    pid = fork();

    if (pid < 0) {
        fprintf(stderr, "Err.(%s) fork() failed\n", strerror(errno));
        exit(EXIT_FAILURE);
    } else if (pid > 0)
        /* Il processo padre va in attesa per 30 secondi dando tutto il tempo
        al processo figlio di essere eseguito */
    {
        sleep(30);
    } else
        /* Il processo figlio esce immediatamente, con il padre che resta ancora
        in attesa di ricevere le informazioni sullo stato di terminazione del
        figlio */
    {
        exit(0);
    }

    return (EXIT_SUCCESS);
}
/*
[1] ../Process-Control/02_fork_4_orfano.c
*/
