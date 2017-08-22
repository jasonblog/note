#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>

#define MAX_BUF     512

/* Sara' eseguito mediante una pipe il comando fornito da 'cmd' e copiato sullo
standard output. */

int main(int argc, char* argv[])
{
    char* cmd = "ps ajf";
    char buf[MAX_BUF];
    FILE* fpipe;

    if (!(fpipe = popen(cmd, "r"))) {
        fprintf(stderr, "Err.: %d popen() - %s\n", errno, strerror(errno));
        return 13;
    }

    /* Legge l'output del comando fornito mediante la pipe */
    while (fgets(buf, MAX_BUF, fpipe) != 0) {
        fputs(buf, stdout);
    }

    if (pclose(fpipe)) {
        fprintf(stderr, "Err.: %d - pclose() %s\n", errno, strerror(errno));
        return (EXIT_SUCCESS);
    }

    return (EXIT_SUCCESS);
}
