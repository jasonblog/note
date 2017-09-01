#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <unistd.h>
#include <termios.h>
#include <fcntl.h>

/*
I terminali, sin dagli albori dei sistemi unix-like, sono stati l'interfaccia
principale per l'interazione tra gli utenti e tra gli utenti ed il sistema; il
"job control" e' quell'insieme di funzionalita' che consentono a ciascun utente
di poter sfruttare le potenzialita' del multitasking del sistema, ossia la
capacita' di eseguire piu' programmi contemporaneamente, tipico dei sistemi unix
-like.

L'utente che si logga ad un sistema che supporta il "job control", avra' a
disposizione una shell in cui eseguire i comandi, inziando cosi' una sessione
che comprende ciascun processo eseguito all'interno dello stesso login.

Nota: I processi possono essere raggruppati in gruppi, con identificativo PGID;
uno o  piu' gruppi di  processi formano le sessioni, identificativo SID[1].

I gruppi di processi e le sessioni godono delle seguenti peculiarita':

- Una sessione puo' avere un unico terminale di controllo, usualmente il
  terminale in cui ci si logga, sul quale si effettuano le operazioni di
  lettura e scrittura e si ricevono gli eventuali segnali via tastiera. In
  generale un processo eredita dal processo padre il PGID, il SID e il terminale
  di controllo;

- Il (processo) leader della sessione che si connette al terminale di controllo
  e' detto processo di controllo;

- I gruppi di processi di una sessione possono essere suddivisi in:
  1 - Un unico gruppo di processi in foreground.
      I comandi nella shell sono inseriti uno per volta.
  2 - Piu' di un gruppo di processi in background.
      I comandi inviati alla shell inserendo il carattere "&" alla fine del
      comando stesso, ed eseguiti non collegati al terminale;

- Se una sessione ha un terminale di controllo, e' caratterizzata da ha un unico
  gruppo di processi in foreground con accesso al terminale, e da uno o piu'
  gruppi di processi in background, non collegati al terminale:

- Quando da tastiera si invia al terminale una combinazione di tasti che causano
  un segnale di interruzione (Control-C, Control-Z, etc...), tale segnale sara'
  inviato a tutti i processi del gruppo di processi in foreground;

- Quando vi e' una disconnessione (modem, network) e' inviato un segnale di
  arresto al leader della sessione (processo di controllo).

La connessione al terminale di controllo e' stabilita allorquando ci si logga,
la comunicazione con esso avviene aprendo il file /dev/tty.

I sistemi unix-like dispongono di diverse funzioni che consentono di verificare
quali processi sono eseguiti nel gruppo dei processi in foreground:

HEADER    : <unistd.h>
PROTOTYPE : pid_t tcgetpgrp(int fd);
SEMANTICS : La funzione tcgetpgrp() restituisce il "process group" di foreground
            del terminale associato al file descriptor 'fd';
RETURNS   : Il PGID del "process group" di foreground in caso di successo,
            -1 in caso di errore
--------------------------------------------------------------------------------
La funzione tcgetpgrp() ritorna il PGID del gruppp di processi in foreground
associato al terminale aperto dal file descriptor 'fd'.

HEADER    : <unistd.h>
PROTOTYPE : int tcsetpgrp(int fd, pid_t pgrpid);
SEMANTICS : La funzione tcsetpgrp() imposta il "process group" di foreground a
        'pgrpid' associato del terminale associato al file descriptor 'fd'.
RETURNS   : 0 in caso di successo, -1 in caso di errore
--------------------------------------------------------------------------------
Se un processo ha un terminale di controllo, il processo puo' invocare la
funzione tcsetpgrp() per settare il PGID di foregorund a 'pgrpid', il cui valore
deve essere il PGID del gruppo di processi della stessa sessione e 'fd' si deve
riferire al terminale di controllo della sessione stessa.

HEADER    : <termios.h>
PROTOTYPE : int tcgetsid(int fd);
SEMANTICS : La funzione tcgetsid() restituisce il leader della sessione del
            gruppo di processi
RETURNS   : Il leader della sessione del gruppo di processi in caso di successo,
            -1 in caso di errore
--------------------------------------------------------------------------------
*/

int main(int argc, char* argv[])
{
    int fd;

    printf("process ID = %ld; parent = %ld\n",
           (long)getpid(), (long)getppid());

    printf("session ID = %ld; process-group ID = %ld\n",
           (long)tcgetsid(0), (long)getpgid(0));

    if ((fd = open("/dev/tty", O_RDWR)) < 0) {
        fprintf(stderr, "Err.(%s) open() failed\n", strerror(errno));
        exit(EXIT_FAILURE);
    }

    printf("foreground Process-Group ID del terminale di controllo = %ld\n",
           (long)tcgetpgrp(fd));

    printf("Session-ID del terminale di controllo = %ld\n", (long)tcgetsid(fd));

    close(fd);

    return (EXIT_SUCCESS);
}
/*
[1] Sorgenti relativi a Process Groups e Sessioni:
../Process-Relationships/01_process_groups.c
../Process-Relationships/02_sessions.
*/
