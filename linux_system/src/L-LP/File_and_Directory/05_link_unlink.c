#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/stat.h>

/*
HEADERS   : <unistd.h>

PROTOTYPE : int link(const char *existingpath, const char *newpath);
            int unlink(const char *pathname);

SEMANTICS : link() crea la nuova voce di directory 'newpath', collegamento
            diretto al pathname 'existingpath', gia' esistente nel filesystem;
        unlink() elimina la voce di directory 'pathname'.

RETURNS   : 0 in caso di successo, -1 in caso di errore.
--------------------------------------------------------------------------------
link() crea una nuova voce di directory (collegamento diretto), cio'  comporta
l'incremento di 1 del link count riferito al pathname esistente, inoltre e'
d'obbligo che entrambi i pathname risiedano nello sesso file system. Il nuovo
pathname puntera' ad un file esistente, ma non ne e' una copia; da notare che
uno stesso pathname potra' avere diversi collegamenti diretti, il totale dei
collegamenti ad un pathname e' gestito da st_nlink.

unlink(), come accennato nell'header, non fa altro che cancellare la nuova voce
di directory inserita, ma in realta' il file non sara' eliminato, per lo meno
fino a quando il processo non terminera' la sua esecuzione, dopodiche' il kernel
chiudera' tutti i file aperti relativi al processo stesso e quindi cancellera'
il file. Da notare inoltre che ritornera' un errore qualora lo si dovesse
utilizzare con una directory su sistemi UNIX con kernel Linux - in tal caso, su
una directory vuota, si dovrebbe utilizzare rmdir().
*/

int main(int argc, char* argv[])
{
    struct stat statbuf_old, statbuf_new;

    if (argc < 2) {
        fprintf(stderr, "Uso: %s <pathname esistentae> <nuovo pathname>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    /* Crea il link argv[2] verso argv[1] */
    if (link(argv[1], argv[2]) < 0) {
        fprintf(stderr, "Err.(%d) - %s\n", errno, strerror(errno));
        exit(EXIT_FAILURE);
    }

    if (stat(argv[1], &statbuf_old) < 0) {
        fprintf(stderr, "Err. (%d) - %s\n", errno, strerror(errno));
        exit(EXIT_FAILURE);
    }

    /*
     Se la funzione link() non dovesse ritornare errori, si verifica con la
     stat() se l'inode del nuovo pathname argv[2] corrisponde al medesimo
     inode del pathname dal quale e' stato creato.
    */
    if (stat(argv[2], &statbuf_new) < 0) {
        fprintf(stderr, "Err. (%d) - %s\n", errno, strerror(errno));
        exit(EXIT_FAILURE);
    }

    printf("          i-node del link: %d '%s'\n", (int)statbuf_new.st_ino,
           argv[2]);
    printf("i-node pathname esistente: %d '%s'\n", (int)statbuf_new.st_ino,
           argv[1]);

    /*
     Si stampa il valore del link count, per verificare il numero di
     collegamenti verso il pathname di origine; il campo della struttura stat a
     cui si fa riferimento e' st_nlink.
    */
    printf("          tot. link count: '%s' = %d\n", argv[1], statbuf_old.st_nlink);

    /*
     Elimina la nuova voce di directory appena creata 'argv[2]'; da notare che
     questa operazione consente di decrementare il link count del pathname a cui
     si riferiva, nel caso specifico argv[1].
    */
    if (unlink(argv[2]) < 0) {
        fprintf(stderr, "Err. (%d) - %s\n", errno, strerror(errno));
        exit(EXIT_FAILURE);
    }

    return (EXIT_SUCCESS);
}
