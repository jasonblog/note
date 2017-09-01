#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <unistd.h>
#include <wait.h>

/*
I segnali rappresentano la modalita' piu' elementare di comunicazione tra
processi, essi sono infatti "software interrupts" - interruzioni software - e
consentono la gestione di eventi asincroni; i segnali sono standard POSIX.1.

Tutti i segnali, definiti nell'header <signal.h>[1], hanno un nome specifico, le
prime tre lettere tuttavia sono identiche per ciascuno di essi, ovvero SIG;
ogni nome definisce una costante il cui valore indica un intero positivo, il
"signal number" o numero del segnale.

Per verificare il numero totale dei segnali disponibili e' possibile utilizzare
la macro NSIG.

Nota: Non vi e' un "signal number" 0, POSIX.1 lo definisce in questo ambito come
      "null signal" o segnale nullo. E' consigliabile per la portabilita'
      utilizzare sempre i nomi associati ai segnali e non i corrispettivi valori
      numerici, questo perche' i valori numerici possono variare, anche
      sensibilmente, rispetto alle varie implementazioni del sistema[2].

Un segnale puo' essere inviato:
- dal kernel ad un processo;
- da un processo ad un altro prcoesso.

I segnali sostanzialmente sono utilizzati affinche' si possa comunicare ad un
processo il verificarsi di un particolare evento, ma quali sono le modalita' per
le quali e' possibile generare un segnale? Alcune di esse sono:

- Un segnale puo' essere generato dall'utente allorquando invia dalla tastiera
  una particolare combinazione di tasti per terminare o sospendere il programma,
  come ad esempio Control-Z oppure Control-C;

- Un segnale puo' essere generato a causa di eccezioni hardware, come ad esempio
  una divisione per zero o un riferimento non valido alla memoria; tali
  condizioni generalmente sono rilevate dall'hardware e notificate al kernel, il
  quale provvede ad inviare un segnale al processo in esecuzione nel momento in
  cui si e' verificata l'eccezione;

- La funzione kill - man 2 kill - permette ad un processo di inviare un
  qualunque tipo di segnale ad un altro processo oppure ad un "process group",
  tuttavia per farlo e' necessario essere o il proprietario del processo al
  quale si sta inviando un segnale o essere il superuser (root, amministratore);

- Il comando kill - man 1 kill - consente di inviare un segnale ad un altro
  processo. Tale comando altro non e' che una interfaccia della funzione kill();

- Vi sono inoltre delle condizioni non causate dall'hardware ma dal software,
  come ad esempio la scadenza di un timer o di un allarme o una operazione
  di I/O non ammessa, e cosi' via.

I segnali, come accennato pocanzi, sono eventi asincroni, pertanto possono
capitare in qualsiasi momento e di certo non possono essere gestiti con la
semplice definizione di una variabile, ma piuttosto vi deve essere un meccanismo
che consenta ad un processo di comunicare con il kernel su come (re)agire al
verificarsi di un segnale.

Da notare che gli eventi inseriti pocanzi comportano sempre l'intervento del
kernel, e' il kernel stesso che genera il segnale, che al verificarsi dell'
evento puo':

1 - Ignorare il segnale.
    I segnali SIGKILL e SIGSTOP non possono essere ignorati, cosi' come quelli
    generati da eccezioni hardware;

2 - Catturare il segnale,
    Al verificarsi del segnale una funzione creata ad-hoc sara' automaticamente
    invocata. I segnali SIGKILL e SIGSTOP non possono essere catturati;

3 - Comportamento di default.
    Ciascun processo ha un comportamento di default, che nella maggior parte dei
    casi corrisponde alla terminazione del processo.

Per cio' che concerne i casi due e tre, il processo non appena riceve un segnale
risponde in modo asincrono, ovvero:
a - interrompe la propria esecuzione;
b - esegue l'azione associata, che sia quella di default o la cattura;
c - ritorna alla prossima istruzione da eseguire dopo l'interruzione.

Eredità delle impostazioni
--------------------------
Come vengono gestite le impostazioni dei segnali quando si creano nuovi processi
figli ?

- il comportamento associato alla ricezione di un segnale viene "ereditato" dai
  processi figli;

- per quanto riguarda le funzioni del tipo exec, solo le impostazioni di SIG_IGN
  e SIG_DFL vengono mantenute, mentre per ogni segnale armato con una specifica
  funzione di gestione viene automaticamente impostato il comportamento di
  default;

- Naturalmente il nuovo programma può definire al suo interno una nuova gestione
  dei segnali.


Unix System Signals
-------------------
I sistemi GNU/Linux supportano i segnali standard elencati di seguito, diversi
numeri di segnale tuttavia dipendono dall'architettura, per cui nel campo valore
e' indicato solo il valore corrispondente alle architetture i386, ppc e sh[2].

Ciascun segnale ha una disposizione attuale, che determina come si comporta il
processo quando il segnale viene recapitato, le  voci  nella colonna "Azione"
della tabella specificano per l'appunto l'azione di default del segnale:

Term, terminare il processo;
Ign , ignorare il segnale;
Core, terminare il processo e fare un dump-core;
Stop, arrestare il processo;
Cont, continuare il processo se esso è attualmente fermo.

Infine, il carattere "-" denota che un segnale è assente sulla corrispondente
architettura.

Segnali descritti nello standard POSIX.1-1990 originale.

Segnale    Valore    Azione   Commento
SIGHUP        1       Term    La linea sul terminale che ha il controllo è
                               stata agganciata o il processo che ha il
                   controllo è morto
SIGINT        2       Term    Interrupt da tastiera
SIGQUIT       3       Core    Segnale d'uscita della tastiera
SIGILL        4       Core    Istruzione illegale
SIGABRT       6       Core    Segnale d'abbandono di abort(3)
SIGFPE        8       Core    Eccezione di virgola mobile
SIGKILL       9       Term    Termina il processo
SIGSEGV      11       Core    Riferimento di memoria non valido
SIGPIPE      13       Term    Pipe rotta: scrittura su una pipe priva di
                              lettori
SIGALRM      14       Term    Segnale del timer da alarm(2)
SIGTERM      15       Term    Segnale di termine
SIGUSR1      10       Term    Segnale 1 definito dall'utente
SIGUSR2      12       Term    Segnale 2 definito dall'utente
SIGCHLD      17       Ign     Figlio fermato o terminato
SIGCONT      18       Cont    Continua se fermato
SIGSTOP      19       Stop    Ferma il processo
SIGTSTP      20       Stop    Stop digitato da tty
SIGTTIN      21       Stop    Input da tty per un processo in background
SIGTTOU      22       Stop    Output da tty per un processo in background

I segnali SIGKILL e SIGSTOP non possono essere intercettati, bloccati oppure
ignorati.

Seguono i segnali  che  non  sono  nello  standard  POSIX.1-1990  ma  sono
descritti in SUSv2 e POSIX.1-2001.

Segnale      Valore    Azione   Commento
--------------------------------------------------------------------------------
SIGBUS         7        Core    Errore sul bus (accesso erroneo alla memoria)
SIGPOLL                 Term    Evento suscettibile di polling (Sys V).
                                Sinonimo di SIGIO
SIGPROF        27       Term    Timer del profiler scaduto
SIGSYS         -        Core    Argomento sbagliato alla routine (SVr4)
SIGTRAP        5        Core    Trappola per trace/breakpoint
SIGURG         23       Ign     Condizione urgente sul socket (4.2 BSD)
SIGVTALRM      26       Term    Allarme virtuale (4.2 BSD)
SIGXCPU        24       Core    Ecceduto tempo limite di CPU (4.2 BSD)
SIGXFSZ        25       Core    Limite dimensione file superato (4.2 BSD)

Altri vari segnali.

Segnale      Valore    Azione   Commento
--------------------------------------------------------------------------------
SIGIOT         6        Core    Trappola IOT. Sinonimo di SIGABRT
SIGEMT         -        Term
SIGSTKFLT      16       Term    Errore dello stack del coprocessore
SIGIO          29       Term    I/O ora possibile (4.2 BSD)
SIGCLD         -        Ign     Sinonimo di SIGCHLD
SIGPWR         30       Term    Mancanza di corrente (System V)
SIGINFO        -                Sinonimo di SIGPWR
SIGLOST        -        Term    Perso il lock del file
SIGWINCH       28       Ign     Dimensioni finestra cambiate (4.3 BSD, Sun)
SIGUNUSED      31       Term    Segnale inutilizzato (diventerà SIGSYS)

Nota: Il comando kill -l fornisce un elenco di segnali.
*/

/* Lo scopo del programma e' di verificare la combinazione di tasti utilizzata
per interrompere un programma, nel caso specifico il parametro da osservare e'
"intr", che dovrebbe corrispondere a ^C, ovvero Control-C, che causera' il
segnale di interruzione SIGINT. Il comando stty -a tuttavia restituira' il
settaggio corrente di stty. Infine la macro NSIG restituira' il numero totale
dei segnali presenti nel sistema. */

int main(int argc, char* argv[])
{
    pid_t pid;
    const char* prog_name = "/bin/stty";
    char* arg_list[] = {"stty", "-a", (char*)0};

    switch (pid = fork()) {
    case -1:
        fprintf(stderr, "Err.(%s) fork() failed\n", strerror(errno));
        exit(EXIT_FAILURE);

    case 0:
        if (execv(prog_name, arg_list) < 0) {
            fprintf(stderr, "Err.(%s) execv() failed\n", strerror(errno));
            exit(EXIT_FAILURE);
        }

    default:
        waitpid(pid, NULL, 0);
        printf("Il sistema dispone di %d segnali\n", NSIG);
    }

    return (EXIT_SUCCESS);
}
/*
[1] In realta' le diverse implementazioni dei sistemi unix-like definiscono i
    segnali in altri header, tuttavia e' considereata buona pratica inserire
    solo l'header citato.

[2] Consultare le pagine di man per un approfondimento sulle diverse
    architetture: man 7 signal.
*/
