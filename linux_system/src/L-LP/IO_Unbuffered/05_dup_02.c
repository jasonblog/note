#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>

#define MAX_STR 64

/*
Il programma redirige lo standard input sullo standard output.
*/

void print_line(int n);

int main(int argc, char* argv[])
{
    int fd, i;
    char* filename = "testdup2.txt";

    /* stampa la linea sullo standard output */
    for (i = 1; i <= 3; i++) {
        print_line(i);
    }

    if ((fd = open(filename, O_WRONLY | O_CREAT, 0755)) < 0) {
        fprintf(stderr, "Err.(%s) open() failed\n", strerror(errno));
        exit(EXIT_FAILURE);
    }

    /* redirezione dello standard output verso il file descriptor 'fd' */
    if (dup2(fd, STDOUT_FILENO) < 0) {
        fprintf(stderr, "Err.(%s) dup2() failed\n", strerror(errno));
        exit(EXIT_FAILURE);
    }

    /* Dopo la redirezione stampa la linea nel file  */
    for (; i <= 6; i++) {
        print_line(i);
    }

    close(STDOUT_FILENO);
    close(fd);

    return (EXIT_SUCCESS);
}

void print_line(int n)
{
    char buf[MAX_STR];
    snprintf(buf, sizeof(buf), "Redirezione con dup2() - Linea '%d'\n", n);

    /* il contenuto di 'buf' su STDOUT_FILENO o standard output o 1 */
    if (write(STDOUT_FILENO, buf, strlen(buf)) < 0) {
        fprintf(stderr, "Err.(%s) write() failed\n", strerror(errno));
        exit(EXIT_FAILURE);
    }
}
