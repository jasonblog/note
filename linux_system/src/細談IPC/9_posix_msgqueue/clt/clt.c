#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <mqueue.h>
#include "slnmq.h"

int sln_ipc_mq_send(const char* sendbuf, int sendlen, int prio)
{
    mqd_t           mqd;

    mqd = mq_open(SLN_IPC_MQ_NAME, O_WRONLY);

    if (mqd < 0) {
        fprintf(stderr, "mq_open: %s\n", strerror(errno));
        return -1;
    }

    if (mq_send(mqd, sendbuf, sendlen, prio) < 0) {
        fprintf(stderr, "mq_send: %s\n", strerror(errno));
        return -1;
    }

    mq_close(mqd);
    return 0;
}

int main(int argc, const char* argv[])
{
    unsigned int    prio;

    if (argc != 3) {
        fprintf(stderr, "Usage: %s <msg> <prio>", argv[0]);
        return -1;
    }


    prio = strtoul(argv[2], NULL, 10);

    if (sln_ipc_mq_send(argv[1], strlen(argv[1]), prio) < 0) {
        return -1;
    }

    return 0;
}
