#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>

#define MAX_BUF 516

int main(int argc, char* argv[])
{
    FILE* fp;
    char buf[MAX_BUF];
    char* input_file = "/etc/group";

    if ((fp = fopen(input_file, "r")) == NULL) {
        fprintf(stderr, "Err(%d) %s: %s\n", errno, strerror(errno), input_file);
        exit(EXIT_FAILURE);
    }

    /* Si provvede alla lettura del file input_file linea per linea */
    while ((fgets(buf, MAX_BUF, fp)) != NULL) {
        if (fputs(buf, stdout) == EOF) {
            fprintf(stderr, "Err(%d) %s: fputs()\n", errno, strerror(errno));
            exit(EXIT_FAILURE);
        }
    }

    fclose(fp);

    return (EXIT_SUCCESS);
}
