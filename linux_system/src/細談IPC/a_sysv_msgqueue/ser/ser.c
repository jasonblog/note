#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <errno.h>
#include <unistd.h>
#include <signal.h>
#include <stdlib.h>

#include "slnmq.h"

static int recv_type, msgid;

int sln_server_loop(void)
{
    int                 rcvmsg_len;
    key_t               key;
    struct sln_msgbuf   slnmsg;

    key = ftok(SLN_IPC_MQ_NAME, 0);

    if (key < 0) {
        fprintf(stderr, "ftok: %s\n", strerror(errno));
        return -1;
    }

    msgid = msgget(key, IPC_CREAT | IPC_EXCL);

    if ((msgid < 0) && (errno != EEXIST)) {
        fprintf(stderr, "msgget: %s\n", strerror(errno));
        return -1;
    }

    if (msgid < 0) {
        printf("Open existing mq!\n");
        msgid = msgget(key, IPC_CREAT);

        if (msgid < 0) {
            fprintf(stderr, "msgget: %s\n", strerror(errno));
            return -1;
        }
    }

    sleep(10); //just for test

    for (;;) {
        rcvmsg_len = msgrcv(msgid, &slnmsg, SLN_IPC_MQ_MSGSIZE, recv_type, 0);

        if (rcvmsg_len < 0) {
            fprintf(stderr, "msgrcv: %s\n", strerror(errno));
            sleep(1);
            continue;
        }

        fprintf(stdout, "receive -  recv len: %d, msg type: %d, msg: %s\n",
                rcvmsg_len, slnmsg.msgtype, slnmsg.msg);
    }

    return 0;
}

static void sigint_func(int sig)
{
    if (msgctl(msgid, IPC_RMID, NULL) < 0) {
        fprintf(stderr, "msgctl: %s\n", strerror(errno));
    }

    exit(0);
}

int main(int argc, const char* argv[])
{
    FILE*                fp = NULL;

    if (argc != 2) {
        fprintf(stderr, "Usage: %s <msg type>\n", argv[0]);
        return -1;
    }

    recv_type = atoi(argv[1]);

    if (access(SLN_IPC_MQ_NAME, F_OK) < 0) {
        fp = fopen(SLN_IPC_MQ_NAME, "w+");

        if (NULL != fp) {
            fclose(fp);
        }
    }

    signal(SIGINT, sigint_func);

    sln_server_loop();

    return 0;
}
