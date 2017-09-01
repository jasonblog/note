#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <dirent.h>

int main(void)
{
    DIR* dir;
    struct dirent* ent;

    /* "." e' la directory corrente */
    if (!(dir = opendir("."))) {
        fprintf(stderr, "Err.(%s) opendir() failed\n", strerror(errno));
        exit(EXIT_FAILURE);
    }

    errno = 0;

    while ((ent = readdir(dir))) {
        puts(ent->d_name);
        errno = 0;
    }

    if (errno) {
        fprintf(stderr, "Err.(%s) readdir() failed\n", strerror(errno));
        exit(EXIT_FAILURE);
    }

    closedir(dir);

    return (EXIT_SUCCESS);
}
