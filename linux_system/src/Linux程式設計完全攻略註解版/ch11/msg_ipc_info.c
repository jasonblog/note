#include<stdio.h>
#include<unistd.h>
#include<stdlib.h>
#include<sys/types.h>
#include<sys/ipc.h>
#include<string.h>
#include<sys/msg.h>
#define BUFSIZE 128
struct msg_buf {
    long type;
    char msg[BUFSIZE];
};

int main(int argc, char* argv[])
{
    key_t key;
    int msgid;
    struct msg_buf msg_snd, msg_rcv;
    struct msginfo buf;
    char* ptr = "helloworld";

    memset(&msg_snd, '\0', sizeof(struct msg_buf));
    memset(&msg_rcv, '\0', sizeof(struct msg_buf));

    msg_rcv.type = 1;

    msg_snd.type = 1;
    memcpy(msg_snd.msg, ptr, strlen(ptr));

    if ((key = ftok(".", 'A')) == -1) {
        perror("ftok");
        exit(EXIT_FAILURE);
    }

    if ((msgid = msgget(key, 0600 | IPC_CREAT)) == -1) {
        perror("msgget");
        exit(EXIT_FAILURE);
    }

    printf("msgsnd_return=%d\n", msgsnd(msgid, (void*)&msg_snd, strlen(msg_snd.msg),
                                        0));

    msgctl(msgid, MSG_INFO, &buf);
    printf("buf.msgmax=%d\n", buf.msgmax);
    printf("buf.msgmnb=%d\n", buf.msgmnb);
    printf("buf.msgpool=%d\n", buf.msgpool);
    printf("buf.semmap=%d\n", buf.msgmap);
    printf("buf.msgmni=%d\n", buf.msgmni);
    printf("buf.msgssz=%d\n", buf.msgssz);
    printf("buf.msgtql=%d\n", buf.msgtql);
    printf("buf.msgseg=%u\n", buf.msgseg);

    printf("msgrcv_return=%d\n", msgrcv(msgid, (void*)&msg_rcv, BUFSIZE,
                                        msg_rcv.type, 0));
    printf("rev msg:%s\n", msg_rcv.msg);
    printf("msgctl_return=%d\n", msgctl(msgid, IPC_RMID, 0));
}


