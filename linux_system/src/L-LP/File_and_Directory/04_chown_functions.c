#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <unistd.h>
#include <sys/stat.h>
#include <fcntl.h>

/*
HEADER    : <unistd.h>

PROTOTYPE : int chown(const char *pathname, uid_t owner, gid_t group);
            int fchown(int fd, uid_t owner, gid_t group);
            int lchown(const char *pathname, uid_t owner, gid_t group);

SEMANTICS : Ciascuna di esse cambia i permessi utente (user-ID) e gruppo
            (group-ID)  del file indicato da 'pathname', con i valori indicati
        in 'owner' per l'utente  e 'group' per il gruppo; le sottili
        differenze sono che la funzione fchown() lavora su un file
        descriptor 'fd', mentre chown() e lchown() su un puntatore a
        caratteri 'pathname', inoltre la lchown(), allorquando 'pathname'
        dovesse essere un synbolic link, cambiera' i permessi del symbolic
        link stesso e non del file a cui punta.

RETURNS   : 0 In caso di successo, -1 in caso di errore
--------------------------------------------------------------------------------
In pratica, sia esso un file sia esso un symbolic link, le funzioni modificano
il proprietario del file, l'uso di queste funzioni peraltro andra' a buon fine
solo se l'effective UserID del processo corrisponde all'UserID del proprietario
del file o dell'amministratore del sistema.

Nota: Qualora l'esecuzione del programma dovesse ritornare errori relativi alla
non possibilita' di poter modificare UserID e GroupID, sarebbe opportuno
provarlo con permessi di superutente (o con sudo).
*/
int main(int argc, char* argv[])
{
    struct stat buf;
    int fd;

    if (argc < 3) {
        fprintf(stderr, "Uso: %s <filename> <filename>\n"
                "Il primo argomento dev'essere un link simbolico\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    if (lstat(argv[1], &buf) < 0) {
        fprintf(stderr, "Err stat: (%d) - %s\n", errno, strerror(errno));
        exit(errno);
    }

    /*
     Verifica che si stia lavorando su un symbolic link, dopdiche' imposta i
     permessi di UserID e GroupID
    */
    if (S_ISLNK(buf.st_mode)) {
        if (lchown(argv[1], 1001, 1001) < 0) {
            fprintf(stderr, "Err. cannot set User-ID Group-ID on '%s'\n", argv[1]);
            exit(EXIT_FAILURE);
        }
    } else {
        fprintf(stderr, "Err. '%s' is not a symbolic link\n", argv[1]);
        exit(EXIT_FAILURE);
    }

    if ((fd = open(argv[2], O_RDONLY)) < 0) {
        fprintf(stderr, "Err: (%d) - %s: '%s'\n", errno, strerror(errno), argv[2]);
        exit(errno);
    }

    /*
     fchown() lavora con il file descriptor 'fd', il parametro -1 indica di
     lasciare i permessi immutati
    */
    if (fchown(fd, -1, 1001) < 0) {
        fprintf(stderr, "Err. cannot set User-ID Group-ID on '%s'\n", argv[2]);
        exit(EXIT_FAILURE);
    }

    return (EXIT_SUCCESS);
}
