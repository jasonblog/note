#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <unistd.h>

/*
Si consiglia di leggere il sorgente in cui si introduce il controllo di
accesso[1] prima di proseguire con le funzioni per la modifica dell'identita'.

HEADER    : <unistd.h>
PROTOTYPE : int seteuid(uid_t uid);
            int setegid(uid_t gid);
SEMANTICS : La funzione seteuid() setta l'effective user-ID a 'uid';
            la funzione setegid() setta l'effective group-ID a 'gid'.
RETURNS   : 0 in caso di successo, -1 in caso di errore
--------------------------------------------------------------------------------
Le funzioni sopra citate sono simili alle funzioni setuid() e setgid(), l'unica
e sostanziale differenza e' che tali funzioni vanno a modificare solo
l'effective user-ID e l'effective group-ID.
*/

int main(int argc, char* argv[])
{
    printf("Permessi originari:\n");
    printf("EUID: %d\n", geteuid());
    printf("EGID: %d\n", getegid());

    /*
    Si testi il programma dopo aver apportato le seguenti modifiche al file
    binario:
    $ sudo chown root.root a.out && sudo chmod +s a.out

    Se si esegue il programma con i normali privilegi, le funzioni, ovviamente,
    non apporteranno nessuna modifica, impostando invece il bit suid ed
    aggiornando i privilegi sul file, si otterra' il risultato desiderato.
    */

    printf("Permessi modificati:\n");
    seteuid(500);
    setegid(1000);
    printf("EUID: %d\n", geteuid());
    printf("EGID: %d\n", getegid());

    return (EXIT_SUCCESS);
}
/*
[1] ../Process-Control/01_process_identifiers.c
*/
