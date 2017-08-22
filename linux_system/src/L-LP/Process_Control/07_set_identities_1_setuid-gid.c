#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>

/*
Si consiglia di leggere il sorgente in cui si introduce il controllo di
accesso[1] prima di proseguire con le funzioni per la modifica dell'identita'.

HEADER    : <unistd.h>
PROTOTYPE : int setuid(uid_t uid);
            int setgid(gid_t gid);
SEMANTICS : La funzione setuid() setta il real user-ID e l'effective user-ID a
            'uid';
            la funzione setgid() setta il real group-ID e l'effective group-ID
        a 'gid'.
RETURNS   : 0 in caso di successo, -1 in caso di errore
--------------------------------------------------------------------------------
Quando un programma necessita di privilegi addizionali oppure ottenere l'accesso
a specifiche risorse per le quali non si hanno i dovuti privilegi, si devono
modificare l'user-ID (UID) e il group-ID (GID); naturalmente questa e' la strada
da seguire anche qualora i privilegi dovessero essere ridotti.

Attenzione; si assume che la costante _POSIX_SAVED_IDS sia settata, altrimenti
i riferimenti a saved UID non saranno disponibili, per verificarlo e' possibile
invocare la funzione sysconf() con l'argomento _SC_SAVED_IDS a run-time oppure a
compile-time con _POSIX_SAVED_IDS.

Le applicazioni dovrebbero essere sviluppate seguendo il modello del
"least privilege", ossia il principio del privilegio minimo; in informatica e
in altri campi, il principio del privilegio minimo richiede che in un
particolare livello di astrazione di un ambiente di calcolo ogni modulo
computazionale (un processo, un programma o un utente a seconda del livello di
astrazione considerato) abbia visibilità delle sole risorse necessarie al suo
funzionamento. Lo scopo dell'applicazione del principio e' quello di concedere
solo il minimo insieme di privilegi possibile in ogni istante, in modo da
migliorare la protezione del sistema.

La funzione setuid() consente di modificare il real-UID e l'effective-UID, vi
sono regole ben precise a riguardo:

1 Se il processo gode dei privilegi di super-user, la funzione setuid() setta
  il real UID, l'effective UID e il saved UID a 'uid';[1]

2 Se il processo non gode dei privilegi di super-user, e 'uid' e' uguale o al
  real-UID o al saved-UID, la funzione setuid() imposta solo l'effective-UID
  a 'uid';

3 Se nessuna delle condizioni sopra citate e' vera, 'errno' e' impostata
  a EPERM, e la funzione setuid() ritorna -1.

Le regole per User-ID sono le medesime anche per Group-ID
*/

/* Il programma mostra com'e' possibile abbassare i privilegi dl superuser
temporaneamente e poi in modo definitivo */

int main(int argc, char* argv[])
{
    printf("Permessi originari:\n");
    printf("RUID: %d\n", getuid());
    printf("EUID: %d\n", geteuid());
    printf("RGID: %d\n", getgid());
    printf("EGID: %d\n", getegid());

    /*
    Si testi il programma dopo aver apportato le seguenti modifiche al file
    binario:
    $ sudo chown root.root a.out && sudo chmod +s a.out

    Se si esegue il programma con i normali privilegi, le funzioni, ovviamente,
    non apporteranno nessuna modifica, impostando invece il bit suid ed
    aggiornando i privilegi sul file, si otterra' il risultato desiderato.
    */

    printf("\nAbbassamento permessi: setuid(1000) setgid(1000)\n");
    setuid(1000);
    setgid(1000);
    printf("RUID: %d - invariato\n", getuid());
    printf("EUID: %d\n", geteuid());
    printf("RGID: %d - invariato\n", getgid());
    printf("EGID: %d\n", getegid());

    printf("\nRipristino permessi: setuid(0) setgid(0)\n");
    setuid(0);
    setgid(0);
    printf("RUID: %d - invariato\n", getuid());
    printf("EUID: %d - fallito\n", geteuid());
    printf("RGID: %d - invariato \n", getgid());
    printf("EGID: %d\n", getegid());

    return (EXIT_SUCCESS);
}
/*
[1] ../Process-Control/01_process_identifiers.c
*/
