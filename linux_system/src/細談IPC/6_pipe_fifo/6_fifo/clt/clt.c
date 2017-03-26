#include <stdio.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>
#include <string.h>
#include <errno.h>

#include "slnipc.h"

int main(int argc, const char* argv[])
{
    int     rc;
    int     rd_fd, wr_fd;
    char    recvbuf[128];
    char    sendbuf[128];

    if (argc != 2) {
        fprintf(stderr, "Usage: %s <string>\n", argv[0]);
        return -1;
    }

    snprintf(sendbuf, sizeof(sendbuf), "%s", argv[1]);

    wr_fd =  open(SLN_IPC_2SER_PATH, O_RDWR, 0);

    if (wr_fd < 0) {
        fprintf(stderr, "open: %s\n", strerror(errno));
        return -1;
    }

    rd_fd =  open(SLN_IPC_2CLT_PATH, O_RDWR, 0);

    if (rd_fd < 0) {
        fprintf(stderr, "open: %s\n", strerror(errno));
        return -1;
    }

    rc = write(wr_fd, sendbuf, strlen(sendbuf));

    if (rc < 0) {
        fprintf(stderr, "write: %s\n", strerror(errno));
        return -1;
    }

    rc = read(rd_fd, recvbuf, sizeof(recvbuf));

    if (rc < 0) {
        fprintf(stderr, "write: %s\n", strerror(errno));
        return -1;
    }

    printf("client read: %s\n", recvbuf);

    close(wr_fd);
    close(rd_fd);
    return 0;
}
