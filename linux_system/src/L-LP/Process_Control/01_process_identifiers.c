#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <unistd.h>

/*
A ciascun nuovo processo e' assegnato un intero di 16 bit con segno e
progressivo che lo identifica univocamente rispetto agli altri processi, il
"Process ID" (PID); essendo peraltro un intero a 16 bit puo' raggiungere al
massimo il valore di 32768, dopodiche' eventualmente, dopo la terminazione di un
processo puo' anche  essere riutilizzato, ma solo dopo uno specifico lasso di
tempo.

Nota: Il primo bit e' riservato al segno, ecco perche' il valore e' 32768, ossia
2^15.

PID 0, solitamente e' assegnato allo schedulatore dei processi, detto "swapper";
PID 1, e' assegnao ad "init", invocato dal kernel alla fine della procedura di
"bootstrap", per cui e' il primo processo ad essere eseguito, e si occupa di
far partire tutti gli altri processi.

Il processo "init" non muore mai e sebbene venga eseguito con privilegi da
"superuser" e' un normale processo utente e non un processo kernel.

Vi sono diverse funzioni che consentono l'identificazione dei processi:

HEADER    : <unistd.h>
PROTOTYPE : pid_t getpid(void);
            pid_t getppid(void);
        uid_t getuid(void);
        uid_t geteuid(void);
        gid_g getgid(void);
        gid_t getegid(void);
SEMANTICS : getpid() ritorna il PID del processo chiamante;
            getppid() ritorna il parent PID del processo chiamante;
        getuid() ritorna il real user ID del processo chiamante;
        geteuid() ritorna l'effective user ID del processo chiamante;
        getgid() ritorna il real group ID del processo chiamante;
        getegid() ritorno l'effective real group ID del processo chiamante.
RETURNS   : L'identificativo richiesto in caso di successo, nessun errore.
--------------------------------------------------------------------------------
Nota: Le due funzioni getpid() e getppid() servono per acquisire il PID del
current process e il PID del parent process, ossia il PPID, questo perche' tutti
i processi conservano, oltre al proprio PID anche il PID del genitore.

Controllo d'accesso:
Un sistema unix-like e' basato su fondamenti di sicurezza imprescindibili,
anzitutto vi e' una netta differenziazione tra il superuser (root o
amministratore) che gode del pieno controllo del sistema e gli utenti che
inevitabilmente hanno diverse restrizioni, vi e' inoltre il concetto di "utente"
e di "gruppo".

Il sistema associa un identificatore univoco ad ogni utente e gruppo,
lo user-ID (UID) e il group-ID (GID) rispettivamente, essi servono al kernel per
identificare uno specifico utente o un gruppo di utenti, e grazie ad essi
verificare se godono delle autorizzazioni necessarie per eseguire i compiti
richiesti.

Poiche' tutte le operazioni del sistema sono eseguite da uno o piu' processi,
risulta ovvio che per poter fornire un controllo su tali operazioni e'
necessario essere a conoscenza dell'utente che ha lanciato il programma, ragion
per cui anche un processo deve avere i propri UID e GID.

I sistemi unix-like forniscono i seguenti gruppi di identificatori:

- Real      , real user-ID (RUID) / real group-ID (RGID)
              Sono impostati al login al valore dell'utente e del gruppo con cui
          si accede al sistema. Solitamente non vengono cambiati, potrebbe
          farlo tuttavia solo un processo che gode dei privilegi di
          superuser.
          Identificano l'utente e il gruppo dei proprietari del processo.

- Effective , effective user-ID (EUID) / effective group-ID (EGID)
              Ai due si aggiunge anche l'effective GID di altri eventuali gruppi
          di appartenenza.
          Sono utilizzati nelle verifiche dei permessi del processo e per il
          controllo d'accesso ai file, in pratica identificano l'utente e
          il gruppo usati per decidere se un processo possa o meno accedere
          ad una risorsa.

              Nota: solitamente real ed effective sono identici, tranne nel caso
          in cui il programma in esecuzione avesse i bit 'suid' o 'sgid'
          impostati, in tal caso gli effective saranno impostati al nome
          dell'utente e del gruppo proprietari del file. Questo e' il caso
          in cui un programma puo' essere eseguito da un utente con
          privilegi di superuser (o altri). Puo' rappresentare un serio
          problema di sicurezza.

- Saved     , saved user-ID / saved group-ID
              Solo se _POSIX_SAVED_IDS e' impostato.
          Sono copie dell'effective User-ID ed effettive group-ID del
          processo padre - impostati da una delle funzioni exec all'avvio
          del processo - cosi che sia possibile ripristinarli.

I sistemi con kernel Linux dispongono anche dell'identificatore filesystem, una
estensione di sicurezza NFS.
*/

int main(int argc, char* argv[])
{
    printf("Processo chiamante:\n");
    printf("                  Process-ID (PID): %d\n", getpid());
    printf("          Parent Process-ID (PPID): %d - la shell\n", getppid());
    printf("           Real User-ID (real-UID): %d\n", getuid());
    printf("Effective User-ID (effective-UID): %d\n", geteuid());
    printf("          Real Group-ID (real-GID): %d\n", getgid());
    printf("Effective Group-ID (effective-GID): %d\n", getegid());

    return (EXIT_SUCCESS);
}
/* L'effettiva valenza e utilita' di ciascuna funzione sara' estremamente chiara
negli esempi successivi a questo, andando avanti nello studio dei processi */
