#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <errno.h>

#include "slnmq.h"


int sln_msgsnd(int msg_type, const void* sndmsg, int sndlen)
{
    int                 msgid, rcvmsg_len;
    key_t               key;
    struct sln_msgbuf   slnmsg;

    key = ftok(SLN_IPC_MQ_NAME, 0);

    if (key < 0) {
        fprintf(stderr, "ftok: %s\n", strerror(errno));
        return -1;
    }

    msgid = msgget(key, IPC_CREAT);

    if (msgid < 0) {
        fprintf(stderr, "msgget: %s\n", strerror(errno));
        return -1;
    }

    slnmsg.msgtype = msg_type;
    memcpy(&slnmsg.msg, sndmsg, sndlen);
    msgsnd(msgid, (void*)&slnmsg, sndlen, 0);
}

int main(int argc, const char* argv[])
{
    int     type;

    if (argc != 3) {
        fprintf(stderr, "Usage: %s <msg type> <msg content>\n", argv[0]);
        return -1;
    }

    type = atoi(argv[1]);

    sln_msgsnd(type, argv[2], strlen(argv[2]) + 1);
    return 0;
}
