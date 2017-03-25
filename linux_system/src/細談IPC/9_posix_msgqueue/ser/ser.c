#include <stdio.h>
#include <string.h>
#include <fcntl.h>           /* For O_* constants */
#include <sys/stat.h>        /* For mode constants */
#include <mqueue.h>
#include <errno.h>
#include <stdlib.h>

#include "slnmq.h"

int sln_ipc_mq_loop(void)
{
    mqd_t           mqd;
    struct mq_attr  setattr, attr;
    char*            recvbuf = NULL;
    unsigned int    prio;
    int             recvlen;

    memset(&setattr, 0, sizeof(setattr));
    setattr.mq_maxmsg = SLN_IPC_MQ_MAXMSG;
    setattr.mq_msgsize = SLN_IPC_MQ_MSGSIZE;

    mqd = mq_open(SLN_IPC_MQ_NAME, O_RDWR | O_CREAT | O_EXCL, 0644, &setattr);

    //mqd = mq_open(SLN_IPC_MQ_NAME, O_RDWR | O_CREAT | O_EXCL, 0644, NULL);
    if ((mqd < 0) && (errno != EEXIST)) {
        fprintf(stderr, "mq_open: %s\n", strerror(errno));
        return -1;
    }

    if ((mqd < 0) && (errno == EEXIST)) { // name is exist
        mqd = mq_open(SLN_IPC_MQ_NAME, O_RDWR);

        if (mqd < 0) {
            fprintf(stderr, "mq_open: %s\n", strerror(errno));
            return -1;
        }
    }

    if (mq_getattr(mqd, &attr) < 0) {
        fprintf(stderr, "mq_getattr: %s\n", strerror(errno));
        return -1;
    }

    printf("flags: %ld, maxmsg: %ld, msgsize: %ld, curmsgs: %ld\n",
           attr.mq_flags, attr.mq_maxmsg, attr.mq_msgsize, attr.mq_curmsgs);

    recvbuf = malloc(attr.mq_msgsize);

    if (NULL == recvbuf) {
        return -1;
    }


    sleep(10);

    for (;;) {
        if (mq_getattr(mqd, &attr) < 0) {
            fprintf(stderr, "mq_getattr: %s\n", strerror(errno));
            return -1;
        }

        printf("msgsize: %ld, curmsgs: %ld\n", attr.mq_msgsize, attr.mq_curmsgs);


        recvlen = mq_receive(mqd, recvbuf, attr.mq_msgsize, &prio);

        if (recvlen < 0) {
            fprintf(stderr, "mq_receive: %s\n", strerror(errno));
            continue;
        }

        printf("recvive-> prio: %d, recvbuf: %s\n", prio, recvbuf);
        sleep(1);
    }

    mq_close(mqd);
    return 0;
}

int main(int argc, const char* argv[])
{
    if (sln_ipc_mq_loop() < 0) {
        return -1;
    }

    return 0;
}
