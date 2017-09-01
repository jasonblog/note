#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>

/*
La funzioni wait() e waitpid() restituiscono il "termination status" (lo stato
di terminazione del processo) mediante il puntatore ad intero 'status', di esso
infatti alcuni bit sono riservati per la memorizzazione:

- dell'"exit status" (uscita normale);
- del segnale che ha causato la terminazione (uscita anomala);
- di 1 bit ad indicare la generazione o meno del core file;
- etc...

Lo standard POSIX definisce una serie di macro in <sys/wait.h> atte allo scopo
di analizzare l'"exit status", e non solo:

    WIFEXITED(status), vera per un figlio terminato normalmente;

  WIFSIGNALED(status), vera se il figlio e' terminato in maniera anomala a causa
                       di un segnale non catturato;

   WIFSTOPPED(status), vera se il processo che ha causato il ritorno di watpid()
                       e' bloccato. Uso possibile solo con WUNTRACED attivato;

    WCOREDUMP(status), vera se il processo terminato ha generato un file "core
                       dump". Valutata solo se WIFSIGNALED restituisce un valore
               non nullo;

  WEXITSTATUS(status), restituisce gli 8 bit meno significativi dell'exit status
                       Valutata solo se WIFEXITED restituisce un valore non
               nullo;

     WTERMSIG(status), restituisce il numero del segnale che ha causato la
                       terminazione anomala;


     WSTOPSIG(status), restituisce il numero del segnale che ha bloccato il
                       processo. Valutata solo se WIFSTOPPED restituisce un
               valore non nullo.
*/

void process_exitstatus(int status);

int main(int argc, char* argv[])
{
    pid_t pid;
    int status;

    if ((pid = fork()) < 0) {
        fprintf(stderr, "Err.(%s) fork failed\n", strerror(errno));
        exit(EXIT_FAILURE);
    } else if (pid == 0) {   /* Processo Figlio */
        exit(7);
    }

    if (wait(&status) != pid) {  /* wait() per il figlio */
        fprintf(stderr, "Err.(%s) wait failed\n", strerror(errno));
        exit(EXIT_FAILURE);
    }

    process_exitstatus(status);  /* stampa lo stato del figlio */

    if ((pid = fork()) < 0) {
        fprintf(stderr, "Err.(%s) fork failed\n", strerror(errno));
        exit(EXIT_FAILURE);
    } else if (pid == 0) {   /* Processo Figlio */
        abort();    /* Genera SIGABRT */
    }

    if (wait(&status) != pid) {  /* wait() per il figlio */
        fprintf(stderr, "Err.(%s) wait failed\n", strerror(errno));
        exit(EXIT_FAILURE);
    }

    process_exitstatus(status);  /* stampa lo stato del figlio */

    if ((pid = fork()) < 0) {
        fprintf(stderr, "Err.(%s) fork failed\n", strerror(errno));
        exit(EXIT_FAILURE);
    } else if (pid == 0) {   /* Processo Figlio */
        status /= 0;    /* divide per 0, genera SIGFPE */
    }

    if (wait(&status) != pid) {  /* wait() per il figlio */
        fprintf(stderr, "Err.(%s) wait failed\n", strerror(errno));
        exit(EXIT_FAILURE);
    }

    process_exitstatus(status);  /* stampa lo stato del figlio */

    return (EXIT_SUCCESS);
}

/* Stampa una descrizione dello stato di uscita */
void process_exitstatus(int status)
{
    if (WIFEXITED(status)) {
        printf("Terminazione normale, exit status: %d\n", WEXITSTATUS(status));
    }

    else if (WIFSIGNALED(status)) {
        printf("Terminazione anomala, exit status: %d\n", WTERMSIG(status));
    }
    /* Da testare su *nix BSD
    #ifdef WCOREDUMP
            WCOREDUMP(status) ? " (file core generato)" : "");
    #else
                "");
    #endif
    */
    else if (WIFSTOPPED(status)) {
        printf("Figlio fermato, numero segnale: %d\n", WSTOPSIG(status));
    }
}
