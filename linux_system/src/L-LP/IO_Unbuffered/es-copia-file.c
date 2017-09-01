#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>

#define BUF_SIZE    4096
#define PERMS_MODE  0755

/*
 Copia un file in un altro file, e' una sorta di cp minimale
*/
int main(int argc, char* argv[])
{
    int fd_in, fd_out, rd_len, wr_len;
    char buffer[BUF_SIZE];

    if (argc < 3) {
        fprintf(stderr, "Uso: %s <from filename> <to filename>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    if ((fd_in = open(argv[1], O_RDONLY)) < 0) {
        fprintf(stderr, "Err: (%d) - %s\n", errno, strerror(errno));
        exit(errno);
    }

    if ((fd_out = open(argv[2], O_RDWR | O_CREAT, PERMS_MODE)) < 0) {
        fprintf(stderr, "Err: (%d) - %s\n", errno, strerror(errno));
        exit(errno);
    }

    while ((rd_len = read(fd_in, buffer, BUF_SIZE)) > 0) {
        if ((wr_len = write(fd_out, buffer, rd_len)) < 0) {
            fprintf(stderr, "Err: (%d) - %s\n", errno, strerror(errno));
            exit(EXIT_FAILURE);
        }
    }

    close(fd_in);
    close(fd_out);

    return (EXIT_SUCCESS);
}
