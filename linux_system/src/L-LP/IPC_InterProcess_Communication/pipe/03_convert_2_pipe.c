#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>

#define MAX_BUF 512

/* Il programma invoca un altro programma - convers - mediante una pipe, che
utilizzera' per la conversione di caratteri */

int main(int argc, char* argv[])
{
    char buf[MAX_BUF];
    FILE* fp;

    /* Apertura della pipe e invocazione del programma convert */
    if ((fp = popen("./convers", "r")) == NULL) {
        fprintf(stderr, "Err.: %d popen() - %s\n", errno, strerror(errno));
        exit(EXIT_FAILURE);
    }

    while (1) {
        fputs("shell> ", stdout);
        fflush(stdout);

        /* Lettura dalla pipe */
        if (fgets(buf, MAX_BUF, fp) == NULL) {
            break;
        }

        if (fputs(buf, stdout) == EOF) {
            fprintf(stderr, "Err.: %d fputs() - %s\n", errno, strerror(errno));
            exit(EXIT_FAILURE);
        }
    }

    if (pclose(fp) == -1) {
        fprintf(stderr, "Err.: %d pclose() - %s\n", errno, strerror(errno));
        exit(EXIT_FAILURE);
    }

    putchar('\n');

    return (EXIT_SUCCESS);
}
