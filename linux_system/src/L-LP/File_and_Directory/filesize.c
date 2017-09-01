#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <sys/stat.h>

int main(int argc, char* argv[])
{
    struct stat infobuf;

    if (argc != 2) {
        fprintf(stderr, "Usage: %s <filename>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    if (stat(argv[1], &infobuf) == -1) {
        fprintf(stderr, "Err. stat(): %s\n", strerror(errno));
        exit(EXIT_FAILURE);
    } else {
        printf("The size of %s is %ld K\n", argv[1], infobuf.st_size);
    }
}
