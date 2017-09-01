#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <unistd.h>
#include <sys/stat.h>
#include <syslog.h>
#include <fcntl.h>
#include <sys/resource.h>

/* I demoni sono processi che godono di una vita media sensibilmente maggiore
rispetto agli altri processi, solitamente sono eseguiti subito dopo la fase di
boostrap del sistema e terminati nella fase di shutdown del sistema stesso; una
tra le caratteristiche tipiche di un demone e' l'assenza del terminale di
controllo, per cui e' eseguito in background e senza interazione con l'utente.

Nota: Un demone in esecuzione puo' essere riconosciuto, talvolta, dal carattere
      finale 'd', ad esempio tipici demoni sono sshd, acpid, rsyslogd, httpd, ma
      anche cron, portmap, etc. Il comando 'ps axj' puo' essere utile in tal
      senso.

I sistemi operativi UNIX utilizzano specifici demoni per la gestione di ogni
sorta di caratteristica del sistema e sono scritti seguendo regole ben
definite:

- Creazione di un processo e chiusura del padre (fork);
- Settaggio della maschera di creazione dei permessi dei file (umask);
- Creazione di una nuova sessione per il processo figlio (setsid);
- Cambiamento della working directory (chdir);
- Chiusura di tutti i file descriptor;
- Redirezione dei file descriptor verso /dev/null;
- Gestione del log (syslog);

*/

void daemonize(void);

int main(void)
{
    daemonize();

    return (EXIT_SUCCESS);
}

void daemonize(void)
{
    pid_t pid, sid;
    int fd0, fd1, fd2;

    /* E' necessario creare un nuovo processo, il padre tuttavia sara' subito
    terminato in modo tale da lasciare in esecuzione solo e soltanto il processo
    figlio che in tal modo, essendo orfano, sara' ereditato da init. */

    if ((pid = fork()) < 0) {
        exit(EXIT_FAILURE);
    }

    if (pid > 0) {
        exit(EXIT_SUCCESS);
    }

    /* Quando un utente crea un file o una directory, essi sono settati con i
    permessi di default, tali permessi tuttavia possono essere modificati
    mediante la maschera di creazione dei permessi dei file.

    Per cio' che concerne un demone e' assolutamente necessario poter leggere
    e scrivere sui file, per cui impostando il valore di umask a 0 si avra'
    l'accesso completo alle risorse da parte del demone. */

    umask(0);

    /* Il processo figlio, unico processo in esecuzione, deve ottenere un SID
    (Session ID o ID di sessione) dal kernel per poter continuare l'esecuzione
    come demone; si invoca la funzione setsid() che consente la creazione di una
    nuova sessione e il processo chiamante, in questo caso il processo figlio,
    diventera' leader della nuova sessione. */

    if ((sid = setsid()) < 0) {
        /* log */
        exit(EXIT_FAILURE);
    }

    /* Si cambia la directory di lavoro corrente */
    if (chdir("/") < 0) {
        /* log */
        exit(EXIT_FAILURE);
    }

    /* Chiusura di tutti gli standard file descriptor (stdin, stdout, stderr),
    da ricordare che un demone non dispone del terminale di controllo, per cui
    i file descriptor devono essere disabilitati. */
    close(STDIN_FILENO);
    close(STDOUT_FILENO);
    close(STDERR_FILENO);

    /* Redirezione dei file descriptor 0,1,2 verso /dev/null  */
    fd0 = open("/dev/null", O_RDWR);
    fd1 = dup(fd0); /* La funzione dup() duplica un file descriptor */
    fd2 = dup(fd1);

    /* Si inizializza il file di log; si faccia attenzione a questo passaggio,
    per avere informazioni sullo stato del demone si potra' leggere il file
    di log /var/log/syslog.  */
    openlog("demone: ", LOG_PID, LOG_DAEMON);

    if (fd0 != 0 || fd1 != 1 || fd2 != 2) {
        syslog(LOG_ERR, "fd inaspettato %d %d %d", fd0, fd1, fd2);
        exit(EXIT_FAILURE);
    }

    /* Specializzazione del demone */
    while (1) {
        /* Ogni 60 secondi apparira' sul terminale un messaggio inviato da
        syslog, il cui messaggio sara' scritto anche su /var/log/syslog.*/
        syslog(LOG_DAEMON, "Messaggio del demone");
        sleep(60);
    }

    /* Lo scheletro di un demone unix e' quasi completo, si rimanda al codice dei
    successivi sorgenti di questa sezione per la creazione di un demone piu'
    dettagliato che preveda la gestione dei segnali e una piu' accurata gestione
    del log.*/
}
