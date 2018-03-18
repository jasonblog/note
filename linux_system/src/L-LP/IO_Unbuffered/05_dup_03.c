#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>

/* Si redirige tutto lo standard output verso un file */

int main(void)
{
    int fd;
    char* str;

    if ((fd = open("dupfile.txt", O_WRONLY | O_CREAT | O_TRUNC, 0666)) < 0) {
        perror("open");
        exit(EXIT_FAILURE);
    }

    if (dup2(fd, 1) < 0) {
        perror("dup2");
        exit(EXIT_FAILURE);
    }

    printf("Lo standard output va dritto su dupfile.txt\n");

    close(fd);

    printf("Anche quando si chiude il file descriptor '%d'\n", fd);

    putchar('p');
    putchar('u');
    putchar('t');
    putchar('c');
    putchar('h');
    putchar('a');
    putchar('r');
    putchar('\n');

    str = "con fwrite()\n";

    if (fwrite(str, sizeof(char), strlen(str), stdout) < 0) {
        perror("fwrite");
        exit(EXIT_FAILURE);
    }

    fflush(stdout);

    str = "e con write\n";

    if (write(1, str, strlen(str)) < 0) {
        perror("write");
        exit(EXIT_FAILURE);
    }

    return (EXIT_SUCCESS);
}
