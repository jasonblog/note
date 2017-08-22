#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>

#define MAX_BUF 512

int main(void)
{
    char buf[MAX_BUF];

    if (setvbuf(stdout, buf, _IOFBF, sizeof buf)) {
        fprintf(stderr, "Err(%s) cambiamento buffer stodut\n", strerror(errno));
        return EXIT_FAILURE;
    }

    printf("The art ");
    fflush(stdout);

    if (setvbuf(stdout, NULL, _IONBF, 0)) {
        fprintf(stderr, "Err(%s) cambiamento buffer stodut\n", strerror(errno));
        return EXIT_FAILURE;
    }

    printf("of *nix ");

    if (setvbuf(stdout, buf, _IOLBF, sizeof buf)) {
        fprintf(stderr, "Err(%s) cambiamento buffer stodut\n", strerror(errno));
        return EXIT_FAILURE;

    }

    printf("programming");
    putchar('\n');

    return EXIT_SUCCESS;
}
