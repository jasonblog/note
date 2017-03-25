# system V 消息隊列


system V消息隊列和posix消息隊列類似，linux系統這兩種消息隊列都支持。先來看一下system V消息隊列相關操作及其函數。


- msgget()函數創建一個消息隊列或打開一個消息隊列。

```c
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
int msgget(key_t key, int msgflg);
```

參數key為ftok返回值或IPC_PRIVATE；
參數msgflg為IPC_CREAT或和IPC_CREAT | IPC_EXCL，當消息隊列已經存在時，而msgflg指定為IPC_CREAT | IPC_EXCL，函數返回EEXIST。

在系統中的每一個消息隊列，內核維護這樣一個結構體：

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

其中ipc_perm 結構體定義如下：


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

當一個消息隊列初始化完後，該消息隊列對應的結構被作如下初始化：

msg_perm.cuid 和 msg_perm.uid被設置為當前進程的用戶ID，msg_perm.cuid 和 msg_perm.uid被設置為當前進程的組ID；msg_perm.mode被設置為msgflg；msg_qnum, msg_lspid, msg_lrpid, msg_stime and msg_rtime  被設置為 0；msg_ctime被設置為當前時間；msg_qbytes被設置為系統限制值MSGMNB。


- msgsnd()函數向消息隊列中添加消息。

```c
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
int msgsnd(int msqid, const void *msgp, size_t msgsz, int msgflg);
```

參數msgqid為消息隊列標識，為msgget()返回值；
參數msgp為指向用戶定義結構體的指針，結構體定義格式模板如下：

```sh
struct msgbuf {
   long mtype;       /* message type, must be > 0 */
   char mtext[1];    /* message data */
};
```

mtex域為數組，該數組大小為函數參數msgsz，mtype的值必須大於0；該結構由用戶自己定義，一般根據實際應用來定義。

參數msgsz為將要發送消息的長度，可以指定為0，一般指定為msgbuf結構體長度減去long類型長度（sizeof(msgbuf) - sizeof(long)）；
參數msgflg為0，或IPC_NOWAIT，IPC_NOWAIT為非阻塞，發送時如果對列數據已滿，則函數馬上返回錯誤EAGAIN，如果msgflg為0，則阻塞直到消息隊列由足夠空間添加新的消息，或消息隊列從系統中刪除，或被信號中斷。

- msgrcv()函數從消息隊列中讀取消息。

```c
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
ssize_t msgrcv(int msqid, void *msgp, size_t msgsz, long msgtyp,  int msgflg);
```


參數msgid同函數msgsnd；<br>
參數msgp通函數函數msgsnd，為指向接收消息結構體指針；<br>
參數msgsz為接受消息的緩存大小；<br>
參數msgtyp為讀取何種消息，如果為：<br>

```sh
0 - 讀取消息隊列中最早的消息，以先進先出的方式讀取消息。
> 0 - 讀取消息隊列類型為msgtyp最早的消息。
< 0 - 讀取消息隊列中，消息類型小於或等於msgtyp絕對值的消息中最小消息類型的最早消息。
```

參數msgflg為如下的值或者幾個值得或：

IPC_NOWAIT - 如果隊列中沒有則立即返回；如果未指定該選項，函數將一直阻塞，直到指定的消息可讀取，或消息隊列從系統中刪除，或被信號中斷；
MSG_NOERROR - 截斷消息如果接收到的消息大小大於msgsz給定的值；
MSG_EXCEPT - 使用大於0的msgtyp去讀取消息類型不為msgtyp第一個消息；

- msgctl()函數對一個隊列做控制操作。

```c
#include <sys/msg.h>
int msgctl(int msqid, int cmd, struct msqid_ds *buf);
```

參數cmd為對隊列要做的操作，這些操作有：

IPC_STAT - 獲取消息隊列信息，從內核數據結構中拷貝消息隊列為msqid的結構體到buf指針指向的msqid_ds 結構體中。<br>
IPC_SET - 使用buf指向的結構體來設置msgqid對應的消息隊列內核結構體；<br>
IPC_RMID - 刪除msgqid消息隊列；<br>
IPC_INFO，MSG_INFO， MSG_STAT詳見linux man函數手冊。<br>

示例：
服務進程：


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
                argv[0]);//運行時，指定服務要進程接收的消息類型
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
           sigint_func); //服務進程捕獲中斷信號，在中斷處理函數裡面刪除消息隊列並推出程序

    sln_server_loop();

    return 0;
}
```
客戶進程：

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

服務進程首先運行，並在開始讀取消息處等待10秒，客戶進程在服務進程還未開始讀取數據時向消息隊列加入多條消息，如下：

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

當服務進程運行時會從消息隊列中取出服務進程要取出類型（運行時參數指定）的消息，下面分別是服務器運行時指定的消息類型以及取出的消息（客戶端的運行都如上）：

```sh
# ./server 0（讀取所有消息）
receive -  recv len: 5, msg type: 1, msg: abcd
receive -  recv len: 6, msg type: 2, msg: efghi
receive -  recv len: 4, msg type: 3, msg: jkl
receive -  recv len: 3, msg type: 4, msg: mn
receive -  recv len: 7, msg type: 5, msg: opqrst
receive -  recv len: 3, msg type: 6, msg: uv
receive -  recv len: 2, msg type: 7, msg: w
receive -  recv len: 4, msg type: 8, msg: xyz
# ./server 4 （讀取消息類型為4的消息）
Open existing mq!
receive -  recv len: 3, msg type: 4, msg: mn
# ./server -5     （讀取消息小於5的消息）        
Open existing mq!
receive -  recv len: 5, msg type: 1, msg: abcd
receive -  recv len: 6, msg type: 2, msg: efghi
receive -  recv len: 4, msg type: 3, msg: jkl
receive -  recv len: 3, msg type: 4, msg: mn
receive -  recv len: 7, msg type: 5, msg: opqrst
```

在服務器未退出時，可以使用系統命令ipcs查看系統中的消息隊列：


```sh
# ipcs

------ Message Queues --------
key        msqid      owner      perms      used-bytes   messages    
0x001fffde 229376     root       0          0            0          
當用戶按下CTRL+C時，程序會刪除掉該消息隊列，按下CTRL+C後，再查看：
# ipcs

------ Message Queues --------
key        msqid      owner      perms      used-bytes   messages    
```

可以看到程序中的刪除是成功的。

posix消息隊列和system V消息隊列大致類似，不過通過這兩節可以看到有一些差別：<br>
posix消息隊列有優先級的概念，system V沒有，只能按照消息類型的先進先出來讀取消息即FIFO；
system V消息隊列有消息類型的概念，並且可以按照消息類型取出消息，而posix不能區分消息類型；
本節源碼下載：

http://download.csdn.net/detail/gentleliu/8236745