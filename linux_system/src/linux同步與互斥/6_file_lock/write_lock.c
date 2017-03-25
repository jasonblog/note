#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <errno.h>

#include "filelock.h"

int main(int argc, const char* argv[])
{
    int             fd;

    if (argc != 2) {
        fprintf(stderr, "Usage: %s <write_str>\n", argv[0]);
        return -1;
    }

    fd = open("filelock.txt", O_RDWR | O_CREAT, 0644);

    if (fd < 0) {
        fprintf(stderr, "open: %s\n", strerror(errno));
        return -1;
    }

    if (sln_file_wait_wrlock(fd) < 0) {
        printf("lock write failed!\n");
        close(fd);
        return -1;
    }

    printf("process <%d> holding write lock ok!\n", getpid());
    write(fd, argv[1], strlen(argv[1]));
    sleep(10);
    sln_file_unlock(fd);
    printf("release lock!\n");

    close(fd);
    return 0;
}
