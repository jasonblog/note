#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <unistd.h>
#include <termios.h>
#include <sys/wait.h>
#include <fcntl.h>

/*
Il "Job Control" e' una funzionalita' aggiunta da BSD negli anni ottanta, essa
consente di poter gestire job multipli su un singolo terminale, differenziandoli
tra job con accesso al terminale (gruppo di processi eseguiti in foreground) e
job senza tale accesso (gruppo di processi eseguiti in background).

Nota: Job e' sinonimo di gruppo o collezione di processi, ma anche una pipeline
      di processi.

Ad esempio:
$ ls -l         - job in foreground
$ ls - l &      - job in background
$ ls -l | wc -l &   - 2 job in background

Le funzionalita' del job control richiedono anzitutto una shell che lo supporti,
inoltre il kernel deve comprendere  sia i driver dei terminali sia il supporto
ai relativi segnali.

Quando si esegue un job in background, la shell gli assegna un identificatore
e stampa i relativi PID, ad esempio:
$ ls -l | wc -l &   - job in bacgkround
[1] 4648        - job numero [1], PID 4648
RETURN          - con il comando return se il job e' concluso la shell
[1]+ stopped          stampera' il numero relativo al job.

Un segnale inviato dalla tastiera, come potrebbe essere la combinazione di tasti
Control-C, generera' il segnale SIGINT, che sara' inviato a tutti i processi
del gruppo di foreground. La domanda sorge spontanea, quando c'è un job in
foreground e uno o piu' job in background, chi dei due potra' ricevere
l'input fornito dal terminale? Ovviamente solo il job in foreground, tuttavia
non e' considerato un errore qualora il job in background tentasse di leggere
l'input del terminale, riceverebbe peraltro un segnale di SIGTTIN che
fermerebbe il job in background; da notare però che grazie all'uso della shell
un job in background potrebbe essere portato in foreground, in modo tale che
possa leggere l'input del terminale, e viceversa.

Shell: Il comando per portare un job in background e' bg; il comando per portare
       un job in foreground e' fg.

Nota: Un processo fa parte di un "process group" e un "process group" fa parte
      di una sessione. Una sessione puo' avere o meno un terminale di controllo.
      Se una sessione ha un terminale di controllo, allora il terminal device
      e' a conoscenza del process group ID del processo di foreground, il valore
      del quale puo' essere settato mediante la funzione tcsetpgrp() nei driver
      del terminale. L'ID del gruppo di processi di foreground e' un attributo
      del terminale non del processo, il comando 'ps' lo stampa grazie al valore
      di TPGID, ma non tutte le shell lo supportano.

      $ ps -o pid,ppid,pgid,pgrp,sess,tpgid,comm
      PID  PPID  PGID  PGRP  SESS TPGID COMMAND
      6726  6725  6726  6726  6726  6862 bash
      6862  6726  6862  6862  6726  6862 ps

*/

void get_info(char* str_proc, pid_t pid);

/* ____ SOSTIUIRE QUESTO PROGRAMMA __ */
int main(int argc, char* argv[])
{
    pid_t pid;
    int fd;
    get_info("CALLING PROCESS", 0);

    fd = open("/dev/tty", O_RDWR);
    printf("PGID leader: %ld\n", (long)tcgetpgrp(fd));
    printf("PGID foreground: %ld\n", (long)tcgetsid(0));

    switch (pid = fork()) {

    case -1:
        fprintf(stderr, "Err.(%s) fork() failed\n", strerror(errno));
        exit(EXIT_FAILURE);

    case 0:
        /* Il processo figlio crea una nuova sessione, pertanto PGID e
        SID sono settati al PID del figlio medesimo */
        setsid();
        get_info("CHILD PROCESS", pid);

        exit(EXIT_SUCCESS);

    default:
        waitpid(pid, NULL, 0);
    }

    return (EXIT_SUCCESS);
}

void get_info(char* str_proc, pid_t pid)
{
    printf("%s\n", str_proc);
    printf("PPID: %ld\n", (long)getppid());
    printf(" PID: %ld\n", (long)getpid());
    printf(" SID: %ld\n", (long)getsid(pid));
    printf("PGID: %ld\n", (long)getpgid(pid));
}
