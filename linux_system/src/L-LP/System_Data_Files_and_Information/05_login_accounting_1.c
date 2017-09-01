#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <time.h>
#include <utmpx.h>

/* Necessario per utilizzare ulteriori caratteristiche */
#ifndef _GNU_SOURCE
#define _GNU_SOURCE
#endif

/*
L'accounting degli utenti e' un registro che permette di salvare una serie
di informazioni sugli utenti loggati al sistema e non solo, solitamente e
storicamente tali informazioni sono registrate nei file /var/run/utmp e
/var/log/wtmp.

Quando un utente si logga al sistema viene inserita una riga nel file utmp
contenente diverse informazioni quali l'username del login, il terminale
collegato, l'UID della login shell e l'orario di connessione (da notare che sono
le stesse informazioni che ricava il comando who); all'uscita dal sistema, la
riga viene eliminata da utmp e aggiunta in wtmp, per cui utmp serve per
verificare chi e' in real-time nel sistema, wtmp invece conserva i login
avvenuti.

POSIX STANDARD: Da notare che utmp non e' standard POSIX, pertanto, come
accennato pocanzi, la relativa implementazione puo' variare a seconda del sisema
UNIX, per risolvere questo ed altri problemi e' stata creata una nuova struttura
'utmpx' POSIX.1 standard, ed e' altamente consigliato utilizzare quest'ultima.

Le informazioni salvate nei su citati file sono una sequenza di voci dichiarate
nella struttura 'utmpx' <umtpx.h>:

struct utmpx
{
  short       int ut_type;      Type of login.
  __pid_t     ut_pid;           Process ID of login process.
  char        ut_line[__UT_LINESIZE];   Devicename.
  char        ut_id[4];         Inittab ID.
  char        ut_user[__UT_NAMESIZE];   Username.
  char        ut_host[__UT_HOSTSIZE];   Hostname for remote login.
  struct      __exit_status ut_exit;    Exit status process DEAD_PROCESS.
  __int32_t   ut_session;       Session ID, used for windowing.
  struct      ut_tv;            Time entry was made
  __int32_t   ut_addr_v6[4];        Internet address of remote host.
  char        __unused[20];     Reserved for future use.
};

HEADER    : <utmpx.h>
PROTOTYPE : struct utmpx *getutxent(void);
            struct utmpx *getutxid(const struct utmpx *ut);
        struct utmpx *getutxline(const struct utmpx *ut);
        struct utmpx *pututxline(const struct utmpx *ut);
        void setutxent(void);
        void endutxent(void);
SEMANTICS : La funzione getutxent() legge una voce dalla posizione corrente
            nel registro; la funzione getutxid() cerca la voce nel registro
        definita da 'ut', ossia ut->type ; la funzione getutxline() cerca
        nel registro la voce corrispondente a 'ut', ossia ut->line; la
        funzione pututxline() scrive la voce 'ut' nel registro; la funzione
        setutxent() apre il file contenente il registro; la funzione
        endutxent chiude il file contenente il registro.
RETURNS   : Un puntatore alla struttura *utmpx in caso di successo, -1 in caso
            di errore
--------------------------------------------------------------------------------
I possibili valori per il campo 'ut_type' della struttura 'utmp':
#define EMPTY       0    No valid user accounting information.
#define RUN_LVL     1    The system's runlevel.
#define BOOT_TIME   2    Time of system boot.
#define NEW_TIME    3    Time after system clock changed.
#define OLD_TIME    4    Time when system clock changed.
#define INIT_PROCESS    5    Process spawned by the init process.
#define LOGIN_PROCESS   6    Session leader of a logged in user.
#define USER_PROCESS    7    Normal process.
#define DEAD_PROCESS    8    Terminated process.
*/

int main(int argc, char* argv[])
{
    struct utmpx* ut;

    setutxent();

    while ((ut = getutxent()) != NULL) {
        printf("%-10s ", ut->ut_user);
        printf("%-10s ", (ut->ut_type == EMPTY) ?          "EMPTY"    :
               (ut->ut_type == RUN_LVL) ?       "RUN_LVL"  :
               (ut->ut_type == BOOT_TIME) ?     "BOOT_TIME" :
               (ut->ut_type == NEW_TIME) ?      "NEW_TIME" :
               (ut->ut_type == OLD_TIME) ?      "OLD_TIME" :
               (ut->ut_type == INIT_PROCESS) ?  "INIT_PR"  :
               (ut->ut_type == LOGIN_PROCESS) ? "LOGIN_PR" :
               (ut->ut_type == USER_PROCESS) ?  "USER_PR"  :
               (ut->ut_type == DEAD_PROCESS) ?  "DEAD_PR"  : "?");
        printf("%-7ld %-6s %-5s %-10s ",
               (long) ut->ut_pid, ut->ut_line, ut->ut_id, ut->ut_host);
        printf("%s", ctime((time_t*) & (ut->ut_tv.tv_sec)));
    }

    endutxent();

    exit(EXIT_SUCCESS);
}
