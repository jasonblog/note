#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <errno.h>

#include "filelock.h"

int main(int argc, const char* argv[])
{
    int             fd, pid;

    fd = open("filelock.txt", O_RDWR | O_CREAT, 0644);

    if (fd < 0) {
        fprintf(stderr, "open: %s\n", strerror(errno));
        return -1;
    }

    pid = sln_file_wrlock_test(fd);

    if (pid > 0) {
        printf("write locked!\n");
    } else {
        printf("write unlock!\n");
    }

    pid = sln_file_rdlock_test(fd);

    if (pid > 0) {
        printf("read locked!\n");
    } else {
        printf("read unlock!\n");
    }

    close(fd);
    return 0;
}
