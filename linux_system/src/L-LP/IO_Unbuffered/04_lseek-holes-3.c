#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <fcntl.h>
#include <ctype.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>

#define FILE_PERMS (S_IRUSR | S_IWUSR |S_IRGRP | S_IWGRP |S_IROTH | S_IWOTH)

int main(int argc, char* argv[])
{
    int fd, i, j;
    size_t len;
    ssize_t num_read, num_written;
    char* buf;
    off_t offset;

    if (argc < 3 || strncmp(argv[1], "--help", 6)  == 0) {
        fprintf(stderr, "Usage: %s <filename> [r|R|w!s]\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    // Apertura del file su cui lavorare
    if ((fd = open(argv[1], O_RDWR | O_CREAT, FILE_PERMS)) == -1) {
        fprintf(stderr, "Err. open(%s): %s\n", argv[1], strerror(errno));
        exit(EXIT_FAILURE);
    }

    for (i = 2; i < argc; i++) {
        switch (argv[i][0]) {

        // Visualizza il file-offset corrente in byte, in testo
        case 'r':

        // Visualizza il file-offset corrente in byte, in esadecimale
        case 'R':
            // lunghezza del buffer
            len = atoi(argv[1]);

            // Allocazione della memoria per il buffer
            if ((buf = malloc(len)) == NULL) {
                fprintf(stderr, "Err. malloc()\n");
                exit(EXIT_FAILURE);
            }

            // Lettura del file
            if ((num_read = read(fd, buf, len)) == -1) {
                fprintf(stderr, "Err. read()\n");
                exit(EXIT_FAILURE);
            }

            if (num_read == 0) {
                printf("%s: EOF\n", argv[i]);
            } else {
                printf("%s: ", argv[i]);

                for (j = 0; j < num_read; j++) {
                    if (argv[i][0] == 'r')
                        printf("%c", isprint((unsigned char) \
                                             buf[j]) ? buf[j] : '?');
                    else {
                        printf("%02x ", (unsigned int) buf[j]);
                    }
                }

                printf("\n");
            }

            free(buf);
            break;

        case 'w':
            num_written = write(fd, &argv[i][1], strlen(&argv[i][1]));

            if (num_written == -1) {
                fprintf(stderr, "Err. write()\n");
                exit(EXIT_FAILURE);
            }

            printf("%s: wrote %ld bytes\n", argv[i], (long)num_written);
            break;

        case 's':
            offset = atoi(argv[1]);

            if (lseek(fd, offset, SEEK_SET) == -1) {
                fprintf(stderr, "Err. lseek()\n");
                exit(EXIT_FAILURE);
            }

            printf("%s: seek succeeded\n", argv[i]);
            break;

        default:
            printf("aio\n");
        }
    }

    return (EXIT_SUCCESS);
}
