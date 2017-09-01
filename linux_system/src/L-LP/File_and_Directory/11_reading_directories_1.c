#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <dirent.h>

/*
Lo standard POSIX.1 prevede una specifica interfaccia per la lettura delle
directory, denominata "directory stream"; una directory puo' essere letta da
chiunque abbia i permessi di lettura su di essa, ma per preservare la coerenza e
prevenire l'inconsisenza del file system solo il kernel puo' scrivervi.

Il formato e il contenuto di una directory sono dipendenti dal sistema, tuttavia
le 3 funzioni basilari sono indipendenti dal sistema, esse sono opendir(),
readdir() e closedir(), mediante le quali si riesce ad ottenere numero di inode
e nome della directory.
--------------------------------------------------------------------------------
HEADER    : <dirent.h>
PROTOTYPE : DIR *opendir(const char *pathname);
SEMANTICS : Apre il directory stream 'pathname'
RETURNS   : Un puntatore al directory stream in caso di successo, NULL in caso
            di errore
--------------------------------------------------------------------------------
HEADER    : <dirent.h>
PROTOTYPE : struct dirent *readdir(DIR *dir);
SEMANTICS : Legge la voce corrente della directory 'dir', dopodiche' si
            posiziona alla voce successiva.
RETURNS   : Un puntatore al directory stream in caso di successo, NULL in caso
            di errore
--------------------------------------------------------------------------------
HEADER    : <dirent.h>
PROTOTYPE : int closedir(DIR *dir);
SEMANTICS : Chiude il directory stream puntato da 'dir'
RETURNS   : 0 in caso di successo, -1 in caso di errore
--------------------------------------------------------------------------------
La struttura DIR e' una struttura interna utilizzata dalle funzioni per la
gestione delle directory, contiene inoltre specifiche informazioni sulle
directory stesse; la funzione opendir() peraltro posiziona lo stream sulla prima
voce di directory, permettendo in tal modo alla readdir() di poter leggerla.

La struttura dirent, definita nell'header dirent.h, contiene, tra l'altro, due
elementi indipendenti dal sistema, ossia il nome - relativo al file contenuto
nella directory - ed il numero di inode, pertanto avendo come base i due
elementi appena citati, si possono scrivere applicazioni portabili:

struct dirent
{
    __ino_t     d_ino;      // inode number
    __off_t     d_off;
    unsigned short int  d_reclen;
    unsigned char   d_type;
    char        d_name[256];    // filename null-terminated '\0'
};

*/

int main(int argc, char* argv[])
{
    DIR* dfd;
    struct dirent* dp;

    if (argc < 2) {
        fprintf(stderr, "Uso: %s <dirname>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    if ((dfd = opendir(argv[1])) == NULL) {
        fprintf(stderr, "Err.:(%d) - %s: %s\n", errno, strerror(errno), argv[1]);
        exit(EXIT_SUCCESS);
    }

    /*
     Si legge il contenuto della directory fornita come argomento, eliminando
     dalla stampa "." e ".."
    */
    while ((dp = readdir(dfd)) != NULL) {
        if (strcmp(dp->d_name, ".") == 0 || strcmp(dp->d_name, "..") == 0) {
            continue;
        } else {
            printf("%s\n", dp->d_name);
        }
    }

    closedir(dfd);

    /* Oppure:
    while ((closedir(dfd) == -1) & (errno == EINTR))
       ;
    */

    exit(EXIT_SUCCESS);
}
