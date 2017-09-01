#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <errno.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>

#define RXRXRX (S_IRUSR | S_IXUSR | S_IRGRP | S_IXGRP | S_IROTH | S_IXOTH)

/*
HEADER    : <sys/stat.h>

PROTOTYPE : int chmod(const char *pathname, mode_t mode);
            int fchmod(int fd, mode_t mode);

SEMANTICS : chmod() modifica i permessi del file 'pathname' secondo la modalita'
            definita da 'mode'; fchmod() e' simile alla chmod(), fatta eccezione
        che lavora sul file descriptor 'fd'.

RETURNS   : 0 in caso di successo, -1 in caso di errore.
--------------------------------------------------------------------------------
Le funzioni hanno ragione di esistere solo se le modifiche saranno apportate
dall'User-ID proprietario del processo, o dal SysAdmin naturalmente; detto in
parole povere, solo il proprietario del file potra' intervenire sui bit di mode
del file stesso.

Le costanti simboliche utilizzabili da mode:
- S_ISUID (04000) set user-ID (suid);
- S_ISGID (02000) set group-ID (sgid);
- S_ISVTX (01000) sticky bit;

- S_IRWXU (00700) red, write and execute by user
- S_IRUSR (00400) read by owner
- S_IWUSR (00200) write by owner
- S_IXUSR (00100) execute/search by owner

- S_IRWXG (00070) red, write and execute by group
- S_IRGRP (00040) read by group
- S_IWGRP (00020) write by group
- S_IXGRP (00010) execute/search by group

- S_IRWXO (00007) red, write and execute by other
- S_IROTH (00004) read by others
- S_IWOTH (00002) write by others
- S_IXOTH (00001) execute/search by others
*/

int main(int argc, char* argv[])
{
    struct stat buf;
    int fd;

    if (argc < 3) {
        fprintf(stderr, "Uso: %s <filename 1> <filename 2>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    if (stat(argv[1], &buf) < 0) {
        fprintf(stderr, "Err: (%d) - %s\n", errno, strerror(errno));
        exit(errno);
    }

    /* fchmod(), come detto, deve lavorare su un file descriptor */
    if ((fd = open(argv[2], O_RDONLY)) < 0) {
        fprintf(stderr, "Err: (%d) %s - %s\n", errno, argv[2], strerror(errno));
        exit(errno);
    }

    /* Imposta il GID bit ai permessi del file;  visibile con la S mediante ls */
    if (chmod(argv[1], (buf.st_mode & ~S_IXGRP) | S_ISGID) < 0) {
        fprintf(stderr, "Err: (%d) - %s\n", errno, strerror(errno));
        exit(errno);
    }

    /* Imposta i permessi del file indicato dal file descriptor 'fd' a RXRXRX */
    if (fchmod(fd, RXRXRX) < 0) {
        fprintf(stderr, "Err: (%d) - %s\n", errno, strerror(errno));
        exit(errno);
    }

    close(fd);

    return (EXIT_SUCCESS);
}
