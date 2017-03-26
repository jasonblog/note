#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>

#include "slnipc.h"

int main(int argc, const char* argv[])
{
    int     rc;
    int     wr_fd, rd_fd;
    char    sendbuf[128];
    char    recvbuf[128];

    rc = mkfifo(SLN_IPC_2SER_PATH, O_CREAT | O_EXCL);

    if ((rc < 0) && (errno != EEXIST)) {
        fprintf(stderr, "mkfifo: %s\n", strerror(errno));
        return -1;
    }

    rc = mkfifo(SLN_IPC_2CLT_PATH, O_CREAT | O_EXCL);

    if ((rc < 0) && (errno != EEXIST)) {
        fprintf(stderr, "mkfifo: %s\n", strerror(errno));
        return -1;
    }

    wr_fd =  open(SLN_IPC_2CLT_PATH, O_RDWR, 0);

    if (wr_fd < 0) {
        fprintf(stderr, "open: %s\n", strerror(errno));
        return -1;
    }

    rd_fd =  open(SLN_IPC_2SER_PATH, O_RDWR, 0);

    if (rd_fd < 0) {
        fprintf(stderr, "open: %s\n", strerror(errno));
        return -1;
    }

    for (;;) {
        rc = read(rd_fd, recvbuf, sizeof(recvbuf));

        if (rc < 0) {
            fprintf(stderr, "read: %s\n", strerror(errno));
            continue;
        }

        printf("server recv: %s\n", recvbuf);

        snprintf(sendbuf, sizeof(sendbuf), "Hello, this is server!\n");

        rc = write(wr_fd, sendbuf, strlen(sendbuf));

        if (rc < 0) {
            fprintf(stderr, "write: %s\n", strerror(errno));
            continue;
        }
    }

    close(wr_fd);
    close(rd_fd);
    return 0;
}
