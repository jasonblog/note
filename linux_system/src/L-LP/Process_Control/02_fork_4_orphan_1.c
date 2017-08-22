#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>

/* Lo terminazione dei processi insegna[1] che se alla sua 'nascita' ogni nuovo
processo ha sempre un padre, non e' detto che questo sia vero anche alla
conclusione del processo stesso; cosa succederebbe se morisse il padre lasciando
il figlio in piena salute? Il figlio sarebbe un orfano, ma non per molto,
poiche' init() lo accoglierebbe immediatamente a braccia aperte.

Sebbene romanzato, e' proprio cio' che succederebbe, ossia se il processo padre
dovesse terminare con il processo figlio ancora in esecuzione, il figlio sarebbe
per un lasso di tempo un processo orfano, dopodiche' init() diventerebbe il
nuovo processo genitore; tecnicamente si direbbe che il processo figlio e' stato
ereditato da init().

Nota: Il kernel, quando un processo termina, provvede a verificare se tale
processo e' il padre di altri processi in esecuzione, in caso di esito positivo
il PPID (Parent PID) di questi specifici processi viene posto al PID di init(),
ovvero 1; in tal modo ciascun processo avra' sempre un padre a cui riporftare il
proprio stato di terminazione.
*/

void child_process(pid_t pid);
void parent_process(pid_t pid);

int main(int argc, char* argv[])
{
    /* Programma che genera un processo figlio e che poi termina prima del figlio
    stesso, rendendolo orfano; attenzione, non si sta parlando di zombie, lo
    scopo e' di far adottare l'orfano da init. */

    pid_t pid;
    pid = fork();

    if (pid == -1) {
        fprintf(stderr, "Err.(%s) fork() failed\n", strerror(errno));
        exit(EXIT_FAILURE);
    }

    if (pid == 0) {
        child_process(pid);
    } else {
        parent_process(pid);
    }

    return (EXIT_SUCCESS);
}

void child_process(pid)
{
    /* Si attendono due secondi in modo tale da far terminare il padre, cosicche'
    il processo figlio diventi orfano e pertanto adottabile da init */
    sleep(2);

    if (getppid() == 1) {
        printf("Ho perso il padre\n");
    }

    printf("(PID %ld) Figlio in esecuzione, il Padre: %ld - pid=%ld\n",
           (long)getpid(), (long)getppid(), (long)pid);

    exit(EXIT_SUCCESS);
}

void parent_process(pid_t pid)
{
    printf("(PID %ld) Padre  in esecuzione, il Padre: %ld - pid=%ld\n",
           (long)getpid(), (long)getppid(), (long)pid);

    exit(EXIT_SUCCESS);
}
/*
[1] ../Process-Environment/01_process_termination.c
*/
