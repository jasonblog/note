# system V 消息队列


system V消息队列和posix消息队列类似，linux系统这两种消息队列都支持。先来看一下system V消息队列相关操作及其函数。


- msgget()函数创建一个消息队列或打开一个消息队列。

```c
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
int msgget(key_t key, int msgflg);
```

参数key为ftok返回值或IPC_PRIVATE；
参数msgflg为IPC_CREAT或和IPC_CREAT | IPC_EXCL，当消息队列已经存在时，而msgflg指定为IPC_CREAT | IPC_EXCL，函数返回EEXIST。

在系统中的每一个消息队列，内核维护这样一个结构体：

```c
struct msqid_ds {
    struct ipc_perm msg_perm;     /* Ownership and permissions */
    time_t          msg_stime;    /* Time of last msgsnd(2) */
    time_t          msg_rtime;    /* Time of last msgrcv(2) */
    time_t          msg_ctime;    /* Time of last change */
    unsigned long   __msg_cbytes; /* Current number of bytes in
                                                queue (nonstandard) */
    msgqnum_t       msg_qnum;     /* Current number of messages
                                                in queue */
    msglen_t        msg_qbytes;   /* Maximum number of bytes
                                                allowed in queue */
    pid_t           msg_lspid;    /* PID of last msgsnd(2) */
    pid_t           msg_lrpid;    /* PID of last msgrcv(2) */
};
```

其中ipc_perm 结构体定义如下：


```c
struct ipc_perm {
    key_t          __key;       /* Key supplied to msgget(2) */
    uid_t          uid;         /* Effective UID of owner */
    gid_t          gid;         /* Effective GID of owner */
    uid_t          cuid;        /* Effective UID of creator */
    gid_t          cgid;        /* Effective GID of creator */
    unsigned short mode;        /* Permissions */
    unsigned short __seq;       /* Sequence number */
};
```

当一个消息队列初始化完后，该消息队列对应的结构被作如下初始化：

msg_perm.cuid 和 msg_perm.uid被设置为当前进程的用户ID，msg_perm.cuid 和 msg_perm.uid被设置为当前进程的组ID；msg_perm.mode被设置为msgflg；msg_qnum, msg_lspid, msg_lrpid, msg_stime and msg_rtime  被设置为 0；msg_ctime被设置为当前时间；msg_qbytes被设置为系统限制值MSGMNB。


- msgsnd()函数向消息队列中添加消息。

```c
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
int msgsnd(int msqid, const void *msgp, size_t msgsz, int msgflg);
```

参数msgqid为消息队列标识，为msgget()返回值；
参数msgp为指向用户定义结构体的指针，结构体定义格式模板如下：

```sh
struct msgbuf {
   long mtype;       /* message type, must be > 0 */
   char mtext[1];    /* message data */
};
```

mtex域为数组，该数组大小为函数参数msgsz，mtype的值必须大于0；该结构由用户自己定义，一般根据实际应用来定义。

参数msgsz为将要发送消息的长度，可以指定为0，一般指定为msgbuf结构体长度减去long类型长度（sizeof(msgbuf) - sizeof(long)）；
参数msgflg为0，或IPC_NOWAIT，IPC_NOWAIT为非阻塞，发送时如果对列数据已满，则函数马上返回错误EAGAIN，如果msgflg为0，则阻塞直到消息队列由足够空间添加新的消息，或消息队列从系统中删除，或被信号中断。

- msgrcv()函数从消息队列中读取消息。

```c
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
ssize_t msgrcv(int msqid, void *msgp, size_t msgsz, long msgtyp,  int msgflg);
```


参数msgid同函数msgsnd；<br>
参数msgp通函数函数msgsnd，为指向接收消息结构体指针；<br>
参数msgsz为接受消息的缓存大小；<br>
参数msgtyp为读取何种消息，如果为：<br>

```sh
0 - 读取消息队列中最早的消息，以先进先出的方式读取消息。
> 0 - 读取消息队列类型为msgtyp最早的消息。
< 0 - 读取消息队列中，消息类型小于或等于msgtyp绝对值的消息中最小消息类型的最早消息。
```

参数msgflg为如下的值或者几个值得或：

IPC_NOWAIT - 如果队列中没有则立即返回；如果未指定该选项，函数将一直阻塞，直到指定的消息可读取，或消息队列从系统中删除，或被信号中断；
MSG_NOERROR - 截断消息如果接收到的消息大小大于msgsz给定的值；
MSG_EXCEPT - 使用大于0的msgtyp去读取消息类型不为msgtyp第一个消息；

- msgctl()函数对一个队列做控制操作。

```c
#include <sys/msg.h>
int msgctl(int msqid, int cmd, struct msqid_ds *buf);
```

参数cmd为对队列要做的操作，这些操作有：

IPC_STAT - 获取消息队列信息，从内核数据结构中拷贝消息队列为msqid的结构体到buf指针指向的msqid_ds 结构体中。<br>
IPC_SET - 使用buf指向的结构体来设置msgqid对应的消息队列内核结构体；<br>
IPC_RMID - 删除msgqid消息队列；<br>
IPC_INFO，MSG_INFO， MSG_STAT详见linux man函数手册。<br>

示例：
服务进程：


```c
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
        fprintf(stderr, "Usage: %s <msg type>\n",
                argv[0]);//运行时，指定服务要进程接收的消息类型
        return -1;
    }

    recv_type = atoi(argv[1]);

    if (access(SLN_IPC_MQ_NAME, F_OK) < 0) {
        fp = fopen(SLN_IPC_MQ_NAME, "w+");

        if (NULL != fp) {
            fclose(fp);
        }
    }

    signal(SIGINT,
           sigint_func); //服务进程捕获中断信号，在中断处理函数里面删除消息队列并推出程序

    sln_server_loop();

    return 0;
}
```
客户进程：

```c
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
```

服务进程首先运行，并在开始读取消息处等待10秒，客户进程在服务进程还未开始读取数据时向消息队列加入多条消息，如下：

```sh
./client 1 abcd
./client 2 efghi
./client 3 jkl
./client 4 mn
./client 5 opqrst
./client 6 uv
./client 7 w
./client 8 xyz
```

当服务进程运行时会从消息队列中取出服务进程要取出类型（运行时参数指定）的消息，下面分别是服务器运行时指定的消息类型以及取出的消息（客户端的运行都如上）：

```sh
# ./server 0（读取所有消息）
receive -  recv len: 5, msg type: 1, msg: abcd
receive -  recv len: 6, msg type: 2, msg: efghi
receive -  recv len: 4, msg type: 3, msg: jkl
receive -  recv len: 3, msg type: 4, msg: mn
receive -  recv len: 7, msg type: 5, msg: opqrst
receive -  recv len: 3, msg type: 6, msg: uv
receive -  recv len: 2, msg type: 7, msg: w
receive -  recv len: 4, msg type: 8, msg: xyz
# ./server 4 （读取消息类型为4的消息）
Open existing mq!
receive -  recv len: 3, msg type: 4, msg: mn
# ./server -5     （读取消息小于5的消息）        
Open existing mq!
receive -  recv len: 5, msg type: 1, msg: abcd
receive -  recv len: 6, msg type: 2, msg: efghi
receive -  recv len: 4, msg type: 3, msg: jkl
receive -  recv len: 3, msg type: 4, msg: mn
receive -  recv len: 7, msg type: 5, msg: opqrst
```

在服务器未退出时，可以使用系统命令ipcs查看系统中的消息队列：


```sh
# ipcs

------ Message Queues --------
key        msqid      owner      perms      used-bytes   messages    
0x001fffde 229376     root       0          0            0          
当用户按下CTRL+C时，程序会删除掉该消息队列，按下CTRL+C后，再查看：
# ipcs

------ Message Queues --------
key        msqid      owner      perms      used-bytes   messages    
```

可以看到程序中的删除是成功的。

posix消息队列和system V消息队列大致类似，不过通过这两节可以看到有一些差别：<br>
posix消息队列有优先级的概念，system V没有，只能按照消息类型的先进先出来读取消息即FIFO；
system V消息队列有消息类型的概念，并且可以按照消息类型取出消息，而posix不能区分消息类型；
本节源码下载：

http://download.csdn.net/detail/gentleliu/8236745