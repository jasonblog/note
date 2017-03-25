# posix消息隊列


消息隊列可以看作一系列消息組織成的鏈表，一個程序可以往這個鏈表添加消息，另外的程序可以從這個消息鏈表讀走消息。

mq_open()函數打開或創建一個posix消息隊列。

```c
#include <fcntl.h>           /* For O_* constants */
#include <sys/stat.h>        /* For mode constants */
#include <mqueue.h>
mqd_t mq_open(const char *name, int oflag);
mqd_t mq_open(const char *name, int oflag, mode_t mode,
             struct mq_attr *attr);
Link with -lrt.
```

參數name為posix IPC名字， 即將要被打開或創建的消息隊列對象，為了便於移植，需要指定為“/name”的格式。

參數oflag必須要有O_RDONLY（只讀）、標誌O_RDWR(讀寫)， O_WRONLY（只寫）之一，除此之外還可以指定O_CREAT（沒有該對象則創建）、O_EXCL（如果O_CREAT指定，但name不存在，就返回錯誤），O_NONBLOCK（以非阻塞方式打開消息隊列，在正常情況下mq_receive和mq_send函數會阻塞的地方，使用該標誌打開的消息隊列會返回EAGAIN錯誤）。

當操作一個新隊列時，使用O_CREAT標識，此時後面兩個參數需要被指定，參數mode為指定權限位，attr指定新創建隊列的屬性。

- mq_close()函數關閉消息隊列。

```c
#include <mqueue.h>
int mq_close(mqd_t mqdes);
Link with -lrt.
```

- mq_unlink()函數從系統中刪除某個消息隊列。

```c
 #include <mqueue.h>
int mq_unlink(const char *name);
Link with -lrt.
```

刪除會馬上發生，即使該隊列的描述符引用計數仍然大於0。參數為mq_open()函數第一個參數。

mq_setattr()函數和mq_getattr()函數分別設置和和獲取消息隊列屬性。

```c
#include <mqueue.h>
int mq_getattr(mqd_t mqdes, struct mq_attr *attr);
int mq_setattr(mqd_t mqdes, struct mq_attr *newattr,  struct mq_attr *oldattr); 
Link with -lrt.
```

參數mqdes為mq_open()函數返回的消息隊列描述符。
參數attr、newattr、oldattr為消息隊列屬性結構體指針；

```c
struct mq_attr {
   long mq_flags;       /* Flags: 0 or O_NONBLOCK */
   long mq_maxmsg;      /* Max. # of messages on queue */
   long mq_msgsize;     /* Max. message size (bytes) */
   long mq_curmsgs;     /* # of messages currently in queue */
};
```

參數mq_flags在mq_open時被初始化（oflag參數），其值為0 或者 O_NONBLOCK。<br>
參數mq_maxmsg和mq_msgsize在mq_open時在參數attr中初始化設置，mq_maxmsg是指隊列的消息個數最大值；<br>
mq_msgsize為隊列每個消息的最大值。<br>
參數mq_curmsgs為當前隊列消息。<br>


mq_getattr()函數把隊列當前屬性填入attr所指向的結構體。<br>
mq_setattr()函數只能設置mq_flags屬性，另外的域會被自動忽略，mq_maxmsg和mq_msgsize的設置需要在mq_open當中來完成， 參數oldattr會和函數mq_getattr函數中參數attr相同的值。<br>


- mq_send() 函數 和mq_receive()函數分別用於向消息隊列放置和取走消息。

```c
#include <mqueue.h>
int mq_send(mqd_t mqdes, const char *msg_ptr,   size_t msg_len, unsigned msg_prio); 
ssize_t mq_receive(mqd_t mqdes, char *msg_ptr,  size_t msg_len, unsigned *msg_prio); 
Link with -lrt.
```

參數msg_ptr為指向消息的指針。

msg_len為消息長度，該值不能大於屬性值中mq_msgsize的值。<br>
msg_prio為優先級，消息在隊列中將按照優先級大小順序來排列消息。<br>
如果消息隊列已滿，mq_send()函數將阻塞，直到隊列有可用空間再次允許放置消息或該調用被信號打斷；如果O_NONBLOCK被指定，mq_send()那麼將不會阻塞，而是返回EAGAIN錯誤。如果隊列空，mq_receive()函數將阻塞，直到消息隊列中有新的消息；如果O_NONBLOCK被指定，mq_receive()那麼將不會阻塞，而是返回EAGAIN錯誤。<br>

示例：

服務進程：

```c
int sln_ipc_mq_loop(void)
{
    mqd_t           mqd;
    struct mq_attr  setattr, attr;
    char*            recvbuf = NULL;
    unsigned int    prio;
    int             recvlen;

    setattr.mq_maxmsg = SLN_IPC_MQ_MAXMSG;
    setattr.mq_msgsize = SLN_IPC_MQ_MSGSIZE;

    mqd = mq_open(SLN_IPC_MQ_NAME, O_RDWR | O_CREAT | O_EXCL, 0644,
                  &setattr); //創建消息隊列並設置消息隊列屬性

    if ((mqd < 0) && (errno != EEXIST)) {
        fprintf(stderr, "mq_open: %s\n", strerror(errno));
        return -1;
    }

    if ((mqd < 0) && (errno == EEXIST)) { // 消息隊列存在則打開
        mqd = mq_open(SLN_IPC_MQ_NAME, O_RDWR);

        if (mqd < 0) {
            fprintf(stderr, "mq_open: %s\n", strerror(errno));
            return -1;
        }
    }

    if (mq_getattr(mqd, &attr) < 0) { //獲取消息隊列屬性
        fprintf(stderr, "mq_getattr: %s\n", strerror(errno));
        return -1;
    }

    printf("flags: %ld, maxmsg: %ld, msgsize: %ld, curmsgs: %ld\n",
           attr.mq_flags, attr.mq_maxmsg, attr.mq_msgsize, attr.mq_curmsgs);

    recvbuf = malloc(
                  attr.mq_msgsize); //為讀取消息隊列分配當前系統允許的每條消息的最大大小的內存空間

    if (NULL == recvbuf) {
        return -1;
    }

    for (;;) {
        recvlen = mq_receive(mqd, recvbuf, attr.mq_msgsize,
                             &prio); //從消息隊列中讀取消息

        if (recvlen < 0) {
            fprintf(stderr, "mq_receive: %s\n", strerror(errno));
            continue;
        }

        printf("recvive length: %d, prio: %d, recvbuf: %s\n", recvlen, prio, recvbuf);
    }

    return 0;
}
```

客戶進程：

```c
int sln_ipc_mq_send(const char *sendbuf, int sendlen, int prio)
{
    mqd_t           mqd;

    mqd = mq_open(SLN_IPC_MQ_NAME, O_WRONLY); //客戶進程打開消息隊列
    if (mqd < 0) {
        fprintf(stderr, "mq_open: %s\n", strerror(errno));
        return -1;
    }

    if (mq_send(mqd, sendbuf, sendlen, prio) < 0) { //客戶進程網消息隊列中添加一條消息
        fprintf(stderr, "mq_send: %s\n", strerror(errno));
        return -1;
    }

    return 0;
}
```

程序運行時，服務進程阻塞於mq_receive，客戶進程每發一條消息隊列，服務進程都會從mq_receive處返回，但不一定接收到的消息就是客戶進程最近發送的那一條消息，因為客戶進程往消息隊列中添加消息時會按照優先級來排序，如果客戶進程同時向消息隊列添加多條消息，服務進程還未來得及讀取，那麼當服務進程開始讀取的消息一定是優先級最高的那條消息，而不是客戶進程最先發送的那一條消息。

我們將服務進程稍作修改來試一下：

```c
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

    sleep(10); //此處等待10秒，此時客戶進程一次性向消息隊列加入多條消息

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
        sleep(1); //每秒處理一個消息
    }

    mq_close(mqd);
    return 0;
}
```

服務進程先運行，然後客戶進程立即向消息隊列加入12消息，每條消息優先級從1到12，，之後服務進程運行，程序運行如下：

```sh
# ./server
flags: 0, maxmsg: 10, msgsize: 1024, curmsgs: 0
msgsize: 1024, curmsgs: 10
recvive-> prio: 10, recvbuf: asdf
msgsize: 1024, curmsgs: 10
recvive-> prio: 11, recvbuf: 1234
msgsize: 1024, curmsgs: 10
recvive-> prio: 12, recvbuf: asdf
msgsize: 1024, curmsgs: 9
recvive-> prio: 9, recvbuf: 1234
msgsize: 1024, curmsgs: 8
recvive-> prio: 8, recvbuf: asdf
msgsize: 1024, curmsgs: 7
recvive-> prio: 7, recvbuf: 1234
msgsize: 1024, curmsgs: 6
recvive-> prio: 6, recvbuf: asdf
msgsize: 1024, curmsgs: 5
recvive-> prio: 5, recvbuf: 1234
msgsize: 1024, curmsgs: 4
recvive-> prio: 4, recvbuf: asdf
msgsize: 1024, curmsgs: 3
recvive-> prio: 3, recvbuf: 1234
msgsize: 1024, curmsgs: 2
recvive-> prio: 2, recvbuf: asdf
msgsize: 1024, curmsgs: 1
recvive-> prio: 1, recvbuf: 1234
msgsize: 1024, curmsgs: 0
```

可以看到，系統允許最大消息數量是10條，當客戶進程一次性加入12條消息時，客戶進程在加入最後兩條會阻塞在那裡，直到服務進程取出消息之後，最後兩天消息才能依次加入到消息隊列。並且服務進程取出消息時從優先級從高到低取出：10->11->12->9->8->... ->1


http://download.csdn.net/detail/gentleliu/8228425
本節源碼下載：
