#ifndef SLN_MQ_H_H_
#define SLN_MQ_H_H_

#define SLN_IPC_MQ_NAME     "/tmp/sln_ipc_mq_sysv"
#define SLN_IPC_MQ_MAXMSG   10
#define SLN_IPC_MQ_MSGSIZE  1024

struct sln_msgbuf {
    int     msgtype;
    char    msg[SLN_IPC_MQ_MSGSIZE];
};

#endif

