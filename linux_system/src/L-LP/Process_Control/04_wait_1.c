#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <sys/wait.h>

/*
Lo scopo della funzione wait() e' di attendere che il processo figlio termini la
prpria esecuzione, dopodiche' anche il processo padre puo' essere terminato;
e' molto utile per la prevenzione dei processi zombie.

Se un figlio e' gia' terminato, la funzione wait() ritorna immediatamente; al
ritorno della funzione il "termination status" del figlio e' salvato nella
variabile 'status' e tutte le risorse del processo sono rilasciate.

La wait() in definitiva ritorna non appena un processo figlio termina.

HEADER    : <sys/wait.h>
PROTOTYPE : pid_t wait(int *status);
SEMANTICS : La funzione wait() sospende l'esecuzione del processo chimante fino
            a quando uno dei suoi figli termina, o finche' non riceve un segnale
        per la terminazione del processo. Nel puntatore ad intero 'status'
        viene salvato il "termination status" del processo figlio.
RETURNS   : Il Process ID del figlio terminato in caso di successo, -1 in caso
            di errore
--------------------------------------------------------------------------------
*/

int main(int argc, char* argv[])
{
    pid_t pid, wpid;
    int status;

    /* Si crea un processo figlio */
    pid = fork();

    switch (pid) {
    case -1:
        fprintf(stderr, "Err.(%s) fork() failed\n", strerror(errno));
        exit(EXIT_FAILURE);

    case 0:
        printf("(PID %ld) Figlio in esecuzione, il Padre: %ld - pid=%ld\n",
               (long)getpid(), (long)getppid(), (long)pid);

        sleep(3);
        printf("Figlio terminato ....\n");
        exit(EXIT_SUCCESS);

    default:
        printf("(PID %ld) Padre  in esecuzione, il Padre: %ld - pid=%ld\n",
               (long)getpid(), (long)getppid(), (long)pid);

        /* Il padre resta in attesa che il figlio termini, si sarebbe potuto
        passare anche un puntatore nullo, qualora non vi fosse la necessita' di
        ottenere il "termination status".
        wpid = wait(NULL);
        */
        wpid = wait(&status);

        printf("PID figlio terminato: %ld\n", (long)wpid);
        printf("Padre terminato ....\n");

        exit(EXIT_SUCCESS);
    }


    return (EXIT_SUCCESS);
}
