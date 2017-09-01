#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <setjmp.h>

#define MAX_LINE 1024

static void handler(int sig_num);
static jmp_buf env;

/* Il programma imposta un limite di lettura e scrittura di 10 secondi, la
peculiarita' di tale programma e' l'utilizzo dei salti non locali "nonlocal
jumps", mediante i quali e' possibile, in questo caso, eseguire una system call
o rieseguirla nel caso si ricevesse un segnale. Da notare che longjmp() e'
impostato all'interno del signal handler.*/
int main(int argc, char* argv[])
{
    int n;
    char line[MAX_LINE];

    /* Si installa il signal handler relativo al segnale SIGALRM prima della
    chiamata ad alarm(), altrimenti il processo sarebbe terminato. */
    if (signal(SIGALRM, handler) == SIG_ERR) {
        fprintf(stderr, "Err.(%s) signal() failed\n", strerror(errno));
        exit(EXIT_FAILURE);
    }

    /* Si fissa il punto esatto dal quale riprendere l'esecuzione dopo la
    chiamata longjmp(), si ricorda che setjmp() alla prima chiamata ritorna
    zero mentre alla successiva, ossia dopo l'esecuzione di longjmp(), ritorna
    un valore diverso da 0. */
    if (setjmp(env) != 0) {
        fprintf(stderr, "Err.(%s) setjmp() failed\n", strerror(errno));
        exit(EXIT_FAILURE);
    }

    /* In questo modo, anche se vi dovesse essere un segnale SIGALARM, la
    system call sarebbe eseguita di nuovo. */
    alarm(10);

    if ((n = read(STDIN_FILENO, line, MAX_LINE)) < 0) {
        fprintf(stderr, "Err.(%s) read() failed\n", strerror(errno));
        exit(EXIT_FAILURE);
    }

    alarm(0);

    write(STDOUT_FILENO, line, n);

    return (EXIT_SUCCESS);
}

static void handler(int sig_num)
{
    /* Trasferisce il controllo alla chiamata originaria, la funzione setjmp
    restituira' un valore diverso da zero, 1 in questo caso. */
    longjmp(env, 1);
}
