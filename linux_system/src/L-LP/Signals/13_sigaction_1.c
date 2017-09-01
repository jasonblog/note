#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>

/*
La funzione sigaction() consente di esaminare e/o modificare l'azione associata
ad uno specifico segnale; lo standard POSIX consiglia di utilizzare questa
funzione piuttosto che la obsoleta signal().

HEADER    : <signal.h>
PROTOTYPE : int sigaction(int signo, const struct sigaction act,
            struct sigaction old_act);
SEMANTICS : La funzione sigaction() installa una nuova azione relativa al
            segnale indicato in 'signo'.
RETURNS   : 0 in caso di successo, -1 in caso di errore
--------------------------------------------------------------------------------
- Il parametro 'signo' indica il segnale da modificare;
- se il puntatore 'act' non e' nullo, si modifica l'azione installando la
  funzione specificata;
- se il  puntatore 'old_act' non e' nullo viene restituito il valore dell'azione
  corrente.

Nota: Si e' gia' superato un limite della signal(), ovvero la restituzione
      dell'azione corrente senza dover installarne una nuova, nel caso in cui
      'act' dovesse essere un valore nullo e 'old_act' un valore non-nullo.
      In pratica il parametro 'old_act' consente di recuperare lo stato del
      gestore originale nel momento in cui si installa quello nuovo.

I puntatori del secondo e terzo parametro fanno riferimento alla struttura
'sigaction', che specifica le caratteristiche dell'azione associata al segnale:

struct sigaction {
    void (*sa_handler)(int);
    sigset_t sa_mask;
    int sa_flags;
    void (*sa_sigaction)(int, siginfo_t *, void *);
    void (*sa_restorer);
};

Nota: ll campo 'sa_restorer' non fa parte dello standard POSIX e pertanto non
deve essere considerato.

- sa_mask , indica l'insieme dei segnali bloccati durante l'elaborazione del
            gestore, ad essi viene aggiunto anche il segnale che ne ha causato
        la chiamata, se non specificato altrimenti con 'sa_flags'.

- sa_flags, consente di impostare diversi valori, flag per l'appunto, che
            possono modificare il processo di gestione dei segnali:

        - SA_INTERRUPT, le system-call interrotte da questo segnale non
                        ripartono automaticamente;

        - SA_NOCLDSTOP, se il segnale e' SIGCHLD allora non deve essere
                        notificato quando il processo figlio viene fermato
                da uno dei segnali SIGSTOP, SIGTTIN e SIGTTOU;

        - SA_NOCLDWAIT, se il segnale e' SIGCHLD previene la creazione di
                        processi zombie nel sistema allorquando il figlio
                del processo chiamante termina;

        - SA_NODEFER  , Sinonimo di SA_NOMASK
        - SA_NOMASK   , Fa in modo di evitare che il segnale corrente possa
                        essere bloccato nel corso dell'esecuzione del
                gestore;

        - SA_ONSTACK  , Stack alternativo per l'esecuzione del gestore;

        - SA_RESETHAND, Sinonimo di SA_ONESHOT
        - SA_ONESHOT  , Riporta l'azione relativa al segnale al valore
                        di default a gestore avviato. Riproduce il
                comportamento della semantica inaffidabile;

        - SA_RESTART  , Riavvia automaticamente le slow system call quando
                            vengono interrotte dal segnale. Riproduce il
                comportamento standard BSD;

        - SA_SIGINFO, se si vuole utilizzare un signal-handler in forma
                      estesa mediante 'sa_sigaction' invece di 'sa_handler'.

E' possibile utilizzare due tipi di signal-handler, uno mediante 'sa_sigaction',
l'altro mediante 'sa_handler', il primo e' molto piu' complesso poiche' consente
di ottenere diverse informazioni aggiuntive grazie alla struttura siginfo_t. La
funzione signal() utilizza sa_handler.

Nota: Normalmente il signal handler e' invocato mediante la sequente chiamata:
      void handler(int signo);
      Se invece e' settato il flag SA_SIGINFO, il signal-handler e' invocato
      mediante quest'altra chiamata:
      void handler(int signo, siginfo_t *info, void *context);

La struttura siginfo_t, come accenato pocanzi, contiene diverse informazioni
circa il segnale generato. Una struttura conforme alle specifiche POSIX.1 deve
includere come minimo i campi 'si_signo' e 'si_code', mentre invece una
struttura conforme alle specifiche XSI deve conenere i campi:

struct siginfo {
    int    si_signo;   "numero del segnale"
    int    si_errno;   "se non zero, il valore di 'errno' da <errno.h>"
    int    si_code;    "informazioni addizionali a seconda del segnale"
    pid_t  si_pid;     "PID del processo che invia il segnale"
    uid_t  si_uid;     "Real UID del processo che invia il segnale"
    void   *si_addr    "Indirizzo che causa il fault"
    int    si_status   "valore di exit o numero del segnale"
    long   si_band;    "numero band per SIGPOLL"
    ...
};

Nota: L'intera struttura e' visionabile in /usr/include/bits/siginfo.h su
      sistemi con kernel Linux. Nello specifico il kernel in uso e' il 3.2.0 su
      architettura PPC.

Valori si_code
--------------
I segnali utilizzano  'si_code' per ottenere svariate informazioni, ad esempio
taluni segnali di errore lo usano per avere specifici dettagli sull'errore
stesso, quali potrebbero essere una violazione di memoria o un errore aritmetico
e cosi' via. L'elenco dei valori disponibili, cosi' come sono stati
implementati in siginfo.h, e' il seguente

Valori positivi sono riservati per segnali generati dal kernel:
enum
{
  SI_ASYNCNL = -60,      Sent by asynch name lookup completion.
  SI_TKILL = -6,         Sent by tkill.
  SI_SIGIO,          Sent by queued SIGIO.
  SI_ASYNCIO,            Sent by AIO completion.
  SI_MESGQ,          Sent by real time mesq state change.
  SI_TIMER,          Sent by timer expiration.
  SI_QUEUE,          Sent by sigqueue.
  SI_USER,           Sent by kill, sigsend, raise.
  SI_KERNEL = 0x80       Send by kernel.
};

Valori relativi al segnale SIGILL:
enum
{
  ILL_ILLOPC = 1,        Illegal opcode.
  ILL_ILLOPN,            Illegal operand.
  ILL_ILLADR,            Illegal addressing mode.
  ILL_ILLTRP,            Illegal trap.
  ILL_PRVOPC,            Privileged opcode.
  ILL_PRVREG,            Privileged register.
  ILL_COPROC,            Coprocessor error.
  ILL_BADSTK             Internal stack error.
};

Valori relativi al segnale SIGFPE:
enum
{
  FPE_INTDIV = 1,        Integer divide by zero.
  FPE_INTOVF,            Integer overflow.
  FPE_FLTDIV,            Floating point divide by zero.
  FPE_FLTOVF,            Floating point overflow.
  FPE_FLTUND,            Floating point underflow.
  FPE_FLTRES,            Floating point inexact result.
  FPE_FLTINV,            Floating point invalid operation.
  FPE_FLTSUB             Subscript out of range.
};

Valori relativi al segnale SIGSEGV:
enum
{
  SEGV_MAPERR = 1,       Address not mapped to object.
  SEGV_ACCERR            Invalid permissions for mapped object.
};

Valori relativi al segnale SIGBUS:
enum
{
  BUS_ADRALN = 1,        Invalid address alignment.
  BUS_ADRERR,            Non-existant physical address.
  BUS_OBJERR             Object specific hardware error.
};

Valori relativi al segnale SIGTRAP:
enum
{
  TRAP_BRKPT = 1,        Process breakpoint.
  TRAP_TRACE             Process trace trap.
};

Valori relativi al segnale SIGCHLD:
enum
{
  CLD_EXITED = 1,        Child has exited.
  CLD_KILLED,            Child was killed.
  CLD_DUMPED,            Child terminated abnormally.
  CLD_TRAPPED,           Traced child has trapped.
  CLD_STOPPED,           Child has stopped.
  CLD_CONTINUED          Stopped child has continued.
};

Valori relativi al segnale SIGPOLL:
enum
{
  POLL_IN = 1,           Data input available.
  POLL_OUT,          Output buffers available.
  POLL_MSG,          Input message available.
  POLL_ERR,          IO error.
  POLL_PRI,          High priority input available.
  POLL_HUP           Device disconnected.
};

*/

static int count = 0;

/* Il gestore del segnale, si occupa di catturare il segnale SIGINT; ad ogni
ricezione di tale segnale la variabile count sara' incrementata di 1 */
static void sig_handler_sigint(int signo);

int main(int argc, char* argv[])
{
    struct sigaction sa_old;
    struct sigaction sa_new;

    sa_new.sa_handler = sig_handler_sigint;
    sigemptyset(&sa_new.sa_mask);
    sa_new.sa_flags = 0;

    sigaction(SIGINT, &sa_new, &sa_old);

    while (1) {
        sleep(1);

        if (count == 3) {
            printf("exit...\n");
            break;
        }
    }

    return (EXIT_SUCCESS);
}

static void sig_handler_sigint(int signo)
{
    ++count;
    write(1, " SIGINT captured\n", 17);
}
