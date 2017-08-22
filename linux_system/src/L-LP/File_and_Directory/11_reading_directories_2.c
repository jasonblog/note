#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
#include <string.h>
#include <errno.h>
#include <sys/stat.h>

#define MAX_PATH 1024

void filesize(char* name);
void dirwalk(char* dir, void (*fsize)(char* name));

int main(int argc, char* argv[])
{

    /*
     Lavora sulla directory corrente se non si forniscono argomenti in input
    */
    if (argc == 1) {
        filesize(".");
    } else
        while (--argc > 0) {
            filesize(*++argv);
        }

    return (EXIT_SUCCESS);
}

/*
 La funzione filesize() ha il compito di visualizzare le dimensioni del file
 'name', se si imbatte in una directory invoca la funzione dirwalk() per
 verificare se al suo interno vi sono dei file.
*/
void filesize(char* name)
{
    struct stat stbuf;

    if (stat(name, &stbuf) == -1) {
        fprintf(stderr, "Err.: fsize %s\n", name);
        return;
    }

    if ((stbuf.st_mode & S_IFMT) == S_IFDIR) {
        dirwalk(name, filesize);
    }

    printf("%8ld %s\n", stbuf.st_size, name);
}

void dirwalk(char* dir, void (*fsize)(char* name))
{
    char name[MAX_PATH];
    struct dirent* dp;
    DIR* dfd;

    if ((dfd = opendir(dir)) == NULL) {
        fprintf(stderr, "Err.:(%d) - %s: '%s'\n", errno, strerror(errno), dir);
        return;
    }

    while ((dp = readdir(dfd)) != NULL) {
        if (strcmp(dp->d_name, ".") == 0 || strcmp(dp->d_name, "..") == 0) {
            continue;
        }

        if (strlen(dir) + strlen(dp->d_name) + 2 > sizeof(name)) {
            fprintf(stderr, "nome %s/%s troppo lungo\n", dir, dp->d_name);
        } else {
            sprintf(name, "%s/%s", dir, dp->d_name);
            (*fsize)(name);
        }
    }

    closedir(dfd);
}
