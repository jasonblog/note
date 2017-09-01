#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <signal.h>

static void sig_handler_usr1(int sig, siginfo_t* siginfo, void* context);
static void sig_handler_usr2(int sig, siginfo_t* siginfo, void* context);

/* Il programma dimostra come utilizzare la funzione sigaction() per determinare
il PID del processo che invia il segnale */
int main(int argc, char* argv[])
{
    struct sigaction usr1, usr2;

    memset(&usr1, 0, sizeof(usr1));
    memset(&usr2, 0, sizeof(usr2));

    /* Si utilizza il campo sa_sigaction, per cui il gestore interagira' con la
     * struttura siginfo */
    usr1.sa_sigaction = sig_handler_usr1;
    usr2.sa_sigaction = sig_handler_usr2;

    /* Si informa la sigaction() di utilizzare sa_sigaction e non sa_handler */
    usr1.sa_flags = SA_SIGINFO;
    usr2.sa_flags = SA_SIGINFO;

    sigaction(SIGUSR1, &usr1, NULL);
    sigaction(SIGUSR2, &usr2, NULL);

    printf("In attesa del segnale: ");

    while (1) {
        printf(".");
        fflush(stdout);
        sleep(1);
    }

    return (EXIT_SUCCESS);
}

static void sig_handler_usr1(int sig, siginfo_t* siginfo, void* context)
{
    int pid = (int)siginfo->si_pid;
    printf("SIGUSR1: %d ", pid);
}

static void sig_handler_usr2(int sig, siginfo_t* siginfo, void* context)
{
    int pid = (int)siginfo->si_pid;
    printf("SIGUSR2 %d ", pid);
}
