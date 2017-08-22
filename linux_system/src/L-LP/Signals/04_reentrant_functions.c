#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <pwd.h>

/*
Quando un segnale viene catturato, la normale sequenza di istruzioni viene
interrotta, il processo tuttavia continua la propria esecuzione eseguendo
le istruzioni del signal handler; quando un signal handler ritorna, invece
di chiamare la funzione exit() o altre, il processo prosegue la normale sequenza
di istruzioni che stava eseguendo prima di essere "interrotto" dal segnale.

Cosa succede se nel corso della esecuzione di una system call viene catturato un
segnale? Solitamente l'eventuale azione associata viene eseguita solo dopo la
terminazione della system call, tuttavia in alcune system call lente "slow",
le prime  versioni di Unix potevano interrompere la system call stessa, che
ritornava ­1 come errore ed 'errno' settata a EINTR.

Reentrant functions, funzioni rientranti
----------------------------------------
SUS definisce una serie di funzioni che sono garantite essere rientranti.

Si dice rientrante una funzione che puo' essere interrotta in qualunque punto
della sua esecuzione ed essere invocata una seconda volta da un altro thread di
esecuzione senza che questo comporti nessun problema nell'esecuzione della
stessa; affinche' una funzione possa essere definita rientrante deve soddisfare
taluni specifici requisiti:

1 - Nessuna porzione del codice possa essere alterata durante l'esecuzione;

2 - Il codice non deve richiamare nessuna routine che non sia a sua volta
    rientrante.

    Ad esempio, talune implementazioni delle funzioni malloc() e free() in
    ANSI-C non lo sono e pertanto non dovrebbero essere utilizzate; nel caso
    in cui tuttavia dovesse essere necessario allocare memoria, si potrebbero
    adoperare le API di sistema rientranti, che offrono sicuramente maggiori
    garanzie.

3 - Il codice deve usare, se necessarie, solo variabili temporanee allocate
    sullo stack;

4 - Il codice non deve modificare ne' variabili globali, ne' aree di memoria
    condivise, ne' impiegare variabili locali statiche.

accept, fchmod, lseek, sendto, stat, access, fchown, lstat, setgid, symlink,
aio_error, fcntl, mkdir, setpgid, sysconf, aio_return, fdatasync, mkfifo,
setsid, tcdrain, aio_suspend, fork, open, setsockopt, tcflow, alarm, fpathconf,
pathconf, setuid, tcflush, bind, fstat, pause, shutdown, tcgetattr, cfgetispeed,
fsync, pipe, sigaction, tcgetpgrp, cfgetospeed, ftruncate, poll, sigaddset,
tcsendbreak, cfsetispeed, getegid, posix_trace_event, sigdelset, tcsetattr,
cfsetospeed, geteuid, pselect, sigemptyset, tcsetpgrp, chdir, getgid, raise,
sigfillset, time, chmod, getgroups, read, sigismember, timer_getoverrun, chown,
getpeername, readlink, signal, timer_gettime, clock_gettime, getpgrp, recv,
sigpause, timer_settime, close, getpid, recvfrom, sigpending, times, connect,
getppid, recvmsg, sigprocmask, umask, creat, getsockname, rename, sigqueue,
uname, dup, getsockopt, rmdir, sigset, unlink, dup2, getuid, select, sigsuspend,
utime, execle, kill, sem_post, sleep, wait, execve, link, send, socket, waitpid,
_Exit, &, _exit, listen, sendmsg, socketpair, write.

Se una funzione non e' presente nella lista:
- E' perche' utilizza strutture dati statiche;
- E' una malloc() o una free();
- E' perche' fa parte della libreria I/O standard.
*/

static void new_alarm(int sig_num);

/* Il programma invoca la funzione non rientrante getpwnam() dal signal handler,
ogni secondo; il programma dovrebbe crashare con un segnale SIGSEGV.  */

int main(int argc, char* argv[])
{
    struct passwd* ptr;
    signal(SIGALRM, new_alarm);
    alarm(1);

    for (;;) {
        if ((ptr = getpwnam("b3h3m0th")) == NULL) {
            fprintf(stderr, "Err.(%s) getpwnam() failed\n", strerror(errno));
            exit(EXIT_FAILURE);
        }

        if (strcmp(ptr->pw_name, "b3h3m0th") != 0) {
            printf("valore di ritorno corrotto, pw_name = %s\n", ptr->pw_name);
        }
    }
}

static void new_alarm(int sig_num)
{
    struct passwd* rootptr;

    printf("Nel signal handler\n");

    /* Taluni puntatori interni della funzione getpwnam() sono stati corrotti
    allorquando il signal handler ha invocato la medesima funzione. */
    if ((rootptr = getpwnam("root")) == NULL) {
        fprintf(stderr, "Err.(%s) getpwnam() sh failed\n", strerror(errno));
        exit(EXIT_FAILURE);
    }

    alarm(1);
}
