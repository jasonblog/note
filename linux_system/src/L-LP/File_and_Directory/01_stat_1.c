#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>

/*
HEADER    : <sys/stat.h>

PROTOTYPE : int stat(const char *restrict path, struct stat *restrict buf);

SEMANTICS : Legge le informazioni relative al file 'path', collocandole nella
            struttura stat puntata da 'buf'.

RETURNS  : 0 in caso di successo, -1 in caso di errore.
--------------------------------------------------------------------------------
stat() consente di acquisire svariate informazioni inerenti un file, ovvero
tutti quei dati che sono conservati nell'inode; in realta' la peculiarita' di
ottenere specifiche informazioni sui file e' propria della famiglia di funzioni
stat che comprende anche fstat() e lstat().

La struttura stat puo' dipendere dall'implementazione, il kernel Linux tuttavia
utilizza la seguente: (si tenga ptesente che i campi contrassegnati con un
asterisco non sono richiesti dallo standard POSIX.1):

struct stat {
    mode_t    st_mode;  "file type & mode permissions (ptotection)"
    ino_t     st_ino;   "i-node number (serial number)"
    dev_t     st_dev;   "device number (file system)"
    nlink_t   st_nlink; "number of hard links"
    uid_t     st_uid;   "user ID of owner"
    gid_t     st_gid;   "group ID of owner"
*   dev_t     st_rdev;  "device ID (if special file)"
    off_t     st_size;  "total size, in bytes"
*   blksize_t st_blksize;   "blocksize for file system I/O"
*   blkcnt_t  st_blocks;    "number of 512B blocks allocated"
    time_t    st_atime; "time of last access"
    time_t    st_mtime; "time of last modification"
    time_t    st_ctime; "time of last status change"
};

Il tipo di file e' decodificato mediante il campo 'st_mode', e' possibile
infatti determinare il tipo di file mediante specifiche macro:
- Regular file           = S_ISREG;
- Directory file         = S_ISDIR;
- Block special file     = S_ISBLK;
- Character special file = S_ISCHR;
- FIFO                   = S_ISFIFO;
- Socket file            = S_ISSOCK;
- Symbolic link file     = S_ISLNK.
*/

int main(int argc, char* argv[])
{
    struct stat buffer;
    int i;
    char* pt;

    if (argc < 2) {
        fprintf(stderr, "Uso: %s <pathname> ... <pathname N>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    /*
     Stampa il tipo di file
    */
    for (i = 0; i < argc; i++) {
        printf("%s: \n", argv[i]);

        /*
         Si adopera la funzione lstat(), invece di stat, poiche' qualora ci si
         dovesse imbattere in un symbolic link, sara' considerato quel file e
         non il file a cui esso - symbolic link - si riferisce.
        */
        if (lstat(argv[i], &buffer) < 0) {
            fprintf(stderr, "Err: stat\n");
            continue;
        }

        if (S_ISREG(buffer.st_mode)) {
            pt = "regular";
        } else if (S_ISDIR(buffer.st_mode)) {
            pt = "directory";
        } else if (S_ISCHR(buffer.st_mode)) {
            pt = "character special";
        } else if (S_ISBLK(buffer.st_mode)) {
            pt = "block special";
        } else if (S_ISFIFO(buffer.st_mode)) {
            pt = "FIFO";
        } else if (S_ISLNK(buffer.st_mode)) {
            pt = "symbolic link";
        } else if (S_ISSOCK(buffer.st_mode)) {
            pt = "socket";
        } else {
            pt = "** tipo di file sconosciuto **";
        }

        printf("%-20s UID:%-5d GID:%-5d\n\n", pt, buffer.st_uid, buffer.st_gid);
    }

    /*
     Al risulato si aggiungono le informazioni relative all'UserID e GroupID del
     file
    */

    return (EXIT_SUCCESS);
}
