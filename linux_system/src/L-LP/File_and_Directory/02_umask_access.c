#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

/*
La combinazione delle costanti simboliche e' definita mediante un OR binario
*/
#define RWRWRW (S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH | S_IWOTH)

/*
HEADER    : <sys/stat.h>

PROTOTYPE : mode_t umask(mode_t mask);

SEMANTICS : imposta la maschera dei permessi dei bit al valore mask.

RETURNS   : La funzione non ha valore di ritorno.
--------------------------------------------------------------------------------
Quali sono i permessi di default allorquando si crea un nuovo file?  umask() si
occupa proprio di questo, consente infatti di specificare i permessi da non
assegnare ad un nuovo file, solitamente tale valore e' impostato in fase di
login a 0022, ovvero i nuovi file non dovranno contere i permessi di scrittura
per gruppo e others.

'mask' e' un valore espresso in ottale, impostabile mediante specifiche
costanti simboliche:
- 0400 = user read;
- 0200 = user write;
- 0100 = user execute;
- 0040 = group read;
- 0020 = group write;
- 0010 = group execute;
- 0004 = other read;
- 0002 = other write;
- 0001 = other execute;
*/

int main(int argc, char* argv[])
{
    int fd1, fd2, verify_fd1;
    char* file_1 = "test1.txt";
    char* file_2 = "test2.txt";

    umask(0);

    if ((fd1 = open(file_1, O_CREAT, RWRWRW)) < 0) {
        fprintf(stderr, "Err. create file\n");
        exit(EXIT_FAILURE);
    }

    umask(022);

    if ((fd2 = open(file_2, O_CREAT, RWRWRW)) < 0) {
        fprintf(stderr, "Err. create file\n");
        exit(EXIT_FAILURE);
    }

    /*
    HEADER: <unistd.h>
    PROTOTIPO: int access(const char *pathname, int mode);
    SEMANTICA: verifica i permessi di accesso 'mode' rispetto a 'pathname'.
    RITORNA: 0 in caso di successo, -1 in caso di errore.

    access() permette di verificare l'accesso ad un file, controllando i permessi
    relativi al real-User-ID e al real-Group-ID; da notare che di solito, quando
    si apre un file, il kernel verifica l'accesso controllando l'effective-User-ID
    e l'effective-Group-ID.

    Le costanti simboliche utilizzabili da 'mode' sono:
    - F_OK = test di esistenza del file;
    - R_OK = test di lettura del file;
    - W_OK = test di scrittura del file;
    - X_OK = test di esecuzione del file.

    Gli ultimi 3 sono eseguiti solo se la prima e' vera.
    */
    /* Verifica del file */
    if ((verify_fd1 = access(file_1, F_OK)) < 0) {
        fprintf(stderr, "Il file '%s' non esiste \n", file_1);
        exit(EXIT_FAILURE);
    }

    /* Accesso lettura r */
    if ((verify_fd1 = access(file_1, R_OK)) < 0) {
        fprintf(stderr, "Il file '%s' non e' leggibile \n", file_1);
        exit(EXIT_FAILURE);
    } else {
        fprintf(stdout, "Il file '%s'  e' leggibile \n", file_1);
    }

    /* Accesso scrittura w */
    if ((verify_fd1 = access(file_1, W_OK)) < 0) {
        fprintf(stderr, "Il file '%s' non e' scrivibile \n", file_1);
        exit(EXIT_FAILURE);
    } else {
        fprintf(stdout, "Il file '%s'  e' scriviibile \n", file_1);
    }

    /* Accesso esecuzione  x */
    if ((verify_fd1 = access(file_1, X_OK)) < 0) {
        fprintf(stderr, "Il file '%s' non e' eseguibile \n", file_1);
        exit(EXIT_FAILURE);
    } else {
        fprintf(stdout, "Il file '%s'  e' eseguiibile \n", file_1);
    }

    close(fd1);
    close(fd2);

    return (EXIT_SUCCESS);
}
