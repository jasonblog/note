#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
#include <sys/stat.h>

/*
 Stampa le dimensioni di pathname
*/
int main(int argc, char* argv[])
{
    struct stat stbuf;

    if (argc < 2) {
        fprintf(stderr, "Uso: %s <pathname>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    if (stat(argv[1], &stbuf) == -1) {
        fprintf(stderr, "Err.: fsize %s\n", argv[1]);
        exit(EXIT_FAILURE);
    }

    printf("%8ld %s\n", stbuf.st_size, argv[1]);

    exit(EXIT_SUCCESS);
}
