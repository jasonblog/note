#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <string.h>

/*
HEADER    : <stdio.h>

PROTOTYPE : int remove(const char *pathname);
            int rename(const char *oldname, const char *newname):

SEMANTICS : remove() rimuove 'pathname' dal filesystem; rename() rinomina
            'oldname' in 'newname'.

RETURNS   : 0 In caso di successo, -1 in caso di errore
--------------------------------------------------------------------------------
Come accennato nel sorgente relativo alla funzione unlink(), essa non puo'
essere adoperata per rimuovere le directory, si dovrebbe utilizzare infatti
rmdir(), oppure la funzione remove(), che peraltro utilizza unlink() per
rimuovere i file e rmdir() per eliminare le directory. Lo standard ISO C
specifica che dovrebbe essere utilizzata la funzione remove() per cancellare i
file.

La funzione rename() consente di rinominare sia file sia directory, il pathname
'oldname' sara' rinominato con il pathname indicato da 'newname', vi sono
tuttavia regole ben precise da rispettare a seconda che si tratti di file o
directory:

- Se si lavora con i file, 'newname' non deve essere una directory; se 'newname'
  e' un file esistente, sara' rimosso e 'oldname' rinominato in 'newname'.

- Se 'oldname' e' una directory, allora 'newname' deve essere una directory
  vuota;

- Se o 'oldname' o 'newname' e' un symbolic link, 'oldname' sara' rinominato
  in 'newname', non sara' interessato il file a cui fa riferimento;

- Se 'oldname' e 'newname' indicano lo stesso file, tutto restera' immutato,
  senza errori di ritorno.

*/

int main(int argc, char** argv, char** envp)
{
    int fd;
    char* new_name_file = "test_remove";
    struct stat statbuf;

    if (argc < 2) {
        fprintf(stderr, "Uso: %s <filename>\nfilename da eliminare\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    if ((fd = open(argv[1], O_RDONLY)) < 0) {
        fprintf(stderr, "Err.:(%d) - %s: %s\n", errno, strerror(errno), argv[1]);
        exit(EXIT_FAILURE);
    }

    /*
     Si adopera una sleep() per avere il tempo materiale per poter verificare i
     cambiamenti dei pathname nel filesystem (e' sufficiente eseguire una serie
     di ls).
     */
    sleep(5);

    if (rename(argv[1], new_name_file) < 0) {
        fprintf(stderr, "Err.:(%d) - %s: %s\n", errno, strerror(errno), argv[1]);
        exit(EXIT_FAILURE);
    }

    sleep(5);

    if (remove(new_name_file) < 0) {
        fprintf(stderr, "Err.:(%d) - %s: %s\n", errno, strerror(errno),
                new_name_file);
        exit(EXIT_FAILURE);
    }

    close(fd);

    return (EXIT_SUCCESS);
}
