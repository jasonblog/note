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
    char            buf[1024];

    fd = open("filelock.txt", O_RDONLY | O_CREAT, 0644);

    if (fd < 0) {
        fprintf(stderr, "open: %s\n", strerror(errno));
        return -1;
    }

    if (sln_file_wait_rdlock(fd) < 0) {
        printf("lock read failed!\n");
        close(fd);
        return -1;
    }

    printf("process <%d> holding read lock ok!\n", getpid());
    sleep(10);
    read(fd, buf, sizeof(buf));
    printf("read buf: %s\n", buf);
    sln_file_unlock(fd);
    printf("release lock!\n");

    close(fd);
    return 0;
}
