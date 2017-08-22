#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <unistd.h>

int main(int argc, char* argv[])
{
    char* buf;
    size_t n;

    n = confstr(_CS_GNU_LIBC_VERSION, NULL, (size_t) 0);

    buf = malloc(n);

    confstr(_CS_GNU_LIBC_VERSION, buf, n);
    printf("%s\n", buf);

    return (EXIT_SUCCESS);
}
