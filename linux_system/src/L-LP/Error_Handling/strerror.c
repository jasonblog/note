#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h> /* strerror() */


int main(int argc, char** argv)
{
    FILE* password;

    if ((password = fopen("/etc/passwd", "w")) == NULL) {
        fprintf(stderr, "EACCES %d: %s\n", EACCES, strerror(EACCES));
        exit(EXIT_FAILURE);
    }

    return (EXIT_SUCCESS);
}
