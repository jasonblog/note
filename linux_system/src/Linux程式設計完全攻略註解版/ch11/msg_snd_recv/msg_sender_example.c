#include<stdio.h>
#include<stdlib.h>
#include<sys/ipc.h>
#include<sys/msg.h>
#include<string.h>
struct msgbuf {
    int type;
    char ptr[0];
};
int main(int argc, char* argv[])
{
    key_t key;
    key = ftok(argv[1], 100);
    int msgid;
    msgid = msgget(key, IPC_CREAT | 0600);
    pid_t pid;
    pid = fork();

    if (pid == 0) {
        while (1) {
            printf("pls input msg to send:");
            char buf[128];
            fgets(buf, 128, stdin);
            struct msgbuf* ptr = malloc(sizeof(struct msgbuf) + strlen(buf) + 1);
            ptr->type = 1;
            memcpy(ptr->ptr, buf, strlen(buf) + 1);
            msgsnd(msgid, ptr, strlen(buf) + 1, 0);
            free(ptr);
        }
    } else {
        struct msgbuf {
            int type;
            char ptr[1024];
        };

        while (1) {
            struct msgbuf mybuf;
            memset(&mybuf, '\0', sizeof(mybuf));
            msgrcv(msgid, &mybuf, 1024, 2, 0);
            printf("recv msg:%s\n", mybuf.ptr);
        }
    }
}
