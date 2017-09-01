#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <unistd.h>

/*
Si consiglia di leggere il sorgente in cui si introduce il controllo di
accesso[1] prima di proseguire con le funzioni per la modifica dell'identita'.

HEADER    : <unistd.h>
PROTOTYPE : int setreuid(uid_t ruid, uid_t euid);
            int setregid(uid_t rgid, gid_t egid);
SEMANTICS : La funzione setreuid() imposta il real user-ID e l'effective user-ID
            a 'ruid' e 'euid' rispettivamente;
        la funzione setregid() imposta il real group-ID e l'effective group-
        ID a 'rgid' e 'egid' rispettivamente;
RETURNS   : 0 in caso di successo, -1 in caso di errore
--------------------------------------------------------------------------------
Impostando ciascun parametro a -1, i valori non saranno modificati.
*/
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
    aggiornando i privilegi sul file, si otterra' il risultato desiderato
    */
    printf("\nPermessi invariati impostando ciascun parametro a -1:\n"
           "setreuid(-1, -1) - setregid(-1, -1)\n");
    setreuid(-1, -1);
    setregid(-1, -1);
    printf("RUID: %d\n", getuid());
    printf("EUID: %d\n", geteuid());
    printf("RGID: %d\n", getgid());
    printf("EGID: %d\n", getegid());

    printf("\nPermessi modificati:\nsetreuid(500, 1000) - setregid(0, 1000)\n");
    setreuid(0, 1000);
    setregid(0, 1000);
    printf("RUID: %d\n", getuid());
    printf("EUID: %d\n", geteuid());
    printf("RGID: %d\n", getgid());
    printf("EGID: %d\n", getegid());

    return (EXIT_SUCCESS);
}
/*
[1] ../Process-Control/01_process_identifiers.c
*/
