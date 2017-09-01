#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>

/*
Un intero gruppo di processi puo' essere reso orfano, ossia puo' perdere il
padre dal quale e' stato generato.
*/

static void signal_handler_sighup(int sig_num);
static void print_process_info(char* process_name);

int main(int argc, char* argv[])
{
    char c;
    pid_t pid;

    print_process_info("Padre");

    if ((pid = fork()) < 0) {
        fprintf(stderr, "Err.(%s) fork() failed\n", strerror(errno));
        exit(EXIT_FAILURE);
    } else if (pid > 0) {
        /* Processo padre
        si mette in attesa per 5 secondi dando modo al figlio di fermarsi,
        dopodiche' esce. */
        sleep(5);
        exit(0);
    } else {
        /* Processo figlio */

        print_process_info("Figlio");

        /* cattura il segnale SIGHUP mediante il gestore dei segnali, dopodiche'
        la funzione kill() invia al figlio stesso il segnale SIGTSTP, che
        serve per arrestare un processo in foreground, come se fosse stata
        utilizzata la combinazione di tasti Control-Z.*/
        signal(SIGHUP, signal_handler_sighup);
        kill(getpid(), SIGTSTP);


        /* Il processo padre a questo punto e' terminato, per cui il figlio
        e' orfano, la stampa del ppid dovrebbe essere 1, ossia adottato da
        init.

        A questo punto il figlio e' membro del gruppo dei processi orfani.*/
        print_process_info("child");

        /* Il processo figlio prova a leggere lo standard input, ma quando un
        gruppo di processi in background prova a leggere dal proprio terminale
        di controllo, viene generato il segnale SIGTTIN a tutti i processi
        del gruppo di processi in background. POSIX.1 specifica che in questi
        casi la funzione read() deve ritornare un errore e la variabile errno
        dev'essere impostata ad EIO, ossia 5 */
        if (read(STDIN_FILENO, &c, 1) != 1) {
            printf("Err.(%d) read() TTY failed - %s\n", errno, strerror(errno));
        }

        exit(0);
    }

    return (EXIT_SUCCESS);
}

static void signal_handler_sighup(int sig_num)
{
    printf("Ricevuto il segnale (SIGHUP) - pid: %d\n", getpid());
}

static void print_process_info(char* process_name)
{
    printf("%s - pid: %d, ppid: %d, pgrp: %d, tpgrp: %d\n", \
           process_name, getpid(), getppid(), getpgrp(), \
           tcgetpgrp(STDIN_FILENO));

    fflush(stdout);
}

