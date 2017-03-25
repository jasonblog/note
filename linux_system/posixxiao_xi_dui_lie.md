# posix消息队列


消息队列可以看作一系列消息组织成的链表，一个程序可以往这个链表添加消息，另外的程序可以从这个消息链表读走消息。

mq_open()函数打开或创建一个posix消息队列。

```c
#include <fcntl.h>           /* For O_* constants */
#include <sys/stat.h>        /* For mode constants */
#include <mqueue.h>
mqd_t mq_open(const char *name, int oflag);
mqd_t mq_open(const char *name, int oflag, mode_t mode,
             struct mq_attr *attr);
Link with -lrt.
```

参数name为posix IPC名字， 即将要被打开或创建的消息队列对象，为了便于移植，需要指定为“/name”的格式。

参数oflag必须要有O_RDONLY（只读）、标志O_RDWR(读写)， O_WRONLY（只写）之一，除此之外还可以指定O_CREAT（没有该对象则创建）、O_EXCL（如果O_CREAT指定，但name不存在，就返回错误），O_NONBLOCK（以非阻塞方式打开消息队列，在正常情况下mq_receive和mq_send函数会阻塞的地方，使用该标志打开的消息队列会返回EAGAIN错误）。

当操作一个新队列时，使用O_CREAT标识，此时后面两个参数需要被指定，参数mode为指定权限位，attr指定新创建队列的属性。

- mq_close()函数关闭消息队列。

```c
#include <mqueue.h>
int mq_close(mqd_t mqdes);
Link with -lrt.
```

- mq_unlink()函数从系统中删除某个消息队列。

```c
 #include <mqueue.h>
int mq_unlink(const char *name);
Link with -lrt.
```

删除会马上发生，即使该队列的描述符引用计数仍然大于0。参数为mq_open()函数第一个参数。

mq_setattr()函数和mq_getattr()函数分别设置和和获取消息队列属性。

```c
#include <mqueue.h>
int mq_getattr(mqd_t mqdes, struct mq_attr *attr);
int mq_setattr(mqd_t mqdes, struct mq_attr *newattr,  struct mq_attr *oldattr); 
Link with -lrt.
```

参数mqdes为mq_open()函数返回的消息队列描述符。
参数attr、newattr、oldattr为消息队列属性结构体指针；

```c
struct mq_attr {
   long mq_flags;       /* Flags: 0 or O_NONBLOCK */
   long mq_maxmsg;      /* Max. # of messages on queue */
   long mq_msgsize;     /* Max. message size (bytes) */
   long mq_curmsgs;     /* # of messages currently in queue */
};
```

参数mq_flags在mq_open时被初始化（oflag参数），其值为0 或者 O_NONBLOCK。<br>
参数mq_maxmsg和mq_msgsize在mq_open时在参数attr中初始化设置，mq_maxmsg是指队列的消息个数最大值；<br>
mq_msgsize为队列每个消息的最大值。<br>
参数mq_curmsgs为当前队列消息。<br>


mq_getattr()函数把队列当前属性填入attr所指向的结构体。<br>
mq_setattr()函数只能设置mq_flags属性，另外的域会被自动忽略，mq_maxmsg和mq_msgsize的设置需要在mq_open当中来完成， 参数oldattr会和函数mq_getattr函数中参数attr相同的值。<br>


- mq_send() 函数 和mq_receive()函数分别用于向消息队列放置和取走消息。

```c
#include <mqueue.h>
int mq_send(mqd_t mqdes, const char *msg_ptr,   size_t msg_len, unsigned msg_prio); 
ssize_t mq_receive(mqd_t mqdes, char *msg_ptr,  size_t msg_len, unsigned *msg_prio); 
Link with -lrt.
```

参数msg_ptr为指向消息的指针。

msg_len为消息长度，该值不能大于属性值中mq_msgsize的值。<br>
msg_prio为优先级，消息在队列中将按照优先级大小顺序来排列消息。<br>
如果消息队列已满，mq_send()函数将阻塞，直到队列有可用空间再次允许放置消息或该调用被信号打断；如果O_NONBLOCK被指定，mq_send()那么将不会阻塞，而是返回EAGAIN错误。如果队列空，mq_receive()函数将阻塞，直到消息队列中有新的消息；如果O_NONBLOCK被指定，mq_receive()那么将不会阻塞，而是返回EAGAIN错误。<br>

示例：

服务进程：

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
                  &setattr); //创建消息队列并设置消息队列属性

    if ((mqd < 0) && (errno != EEXIST)) {
        fprintf(stderr, "mq_open: %s\n", strerror(errno));
        return -1;
    }

    if ((mqd < 0) && (errno == EEXIST)) { // 消息队列存在则打开
        mqd = mq_open(SLN_IPC_MQ_NAME, O_RDWR);

        if (mqd < 0) {
            fprintf(stderr, "mq_open: %s\n", strerror(errno));
            return -1;
        }
    }

    if (mq_getattr(mqd, &attr) < 0) { //获取消息队列属性
        fprintf(stderr, "mq_getattr: %s\n", strerror(errno));
        return -1;
    }

    printf("flags: %ld, maxmsg: %ld, msgsize: %ld, curmsgs: %ld\n",
           attr.mq_flags, attr.mq_maxmsg, attr.mq_msgsize, attr.mq_curmsgs);

    recvbuf = malloc(
                  attr.mq_msgsize); //为读取消息队列分配当前系统允许的每条消息的最大大小的内存空间

    if (NULL == recvbuf) {
        return -1;
    }

    for (;;) {
        recvlen = mq_receive(mqd, recvbuf, attr.mq_msgsize,
                             &prio); //从消息队列中读取消息

        if (recvlen < 0) {
            fprintf(stderr, "mq_receive: %s\n", strerror(errno));
            continue;
        }

        printf("recvive length: %d, prio: %d, recvbuf: %s\n", recvlen, prio, recvbuf);
    }

    return 0;
}
```

客户进程：

```c
int sln_ipc_mq_send(const char *sendbuf, int sendlen, int prio)
{
    mqd_t           mqd;

    mqd = mq_open(SLN_IPC_MQ_NAME, O_WRONLY); //客户进程打开消息队列
    if (mqd < 0) {
        fprintf(stderr, "mq_open: %s\n", strerror(errno));
        return -1;
    }

    if (mq_send(mqd, sendbuf, sendlen, prio) < 0) { //客户进程网消息队列中添加一条消息
        fprintf(stderr, "mq_send: %s\n", strerror(errno));
        return -1;
    }

    return 0;
}
```

程序运行时，服务进程阻塞于mq_receive，客户进程每发一条消息队列，服务进程都会从mq_receive处返回，但不一定接收到的消息就是客户进程最近发送的那一条消息，因为客户进程往消息队列中添加消息时会按照优先级来排序，如果客户进程同时向消息队列添加多条消息，服务进程还未来得及读取，那么当服务进程开始读取的消息一定是优先级最高的那条消息，而不是客户进程最先发送的那一条消息。

我们将服务进程稍作修改来试一下：

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

    sleep(10); //此处等待10秒，此时客户进程一次性向消息队列加入多条消息

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
        sleep(1); //每秒处理一个消息
    }

    mq_close(mqd);
    return 0;
}
```

服务进程先运行，然后客户进程立即向消息队列加入12消息，每条消息优先级从1到12，，之后服务进程运行，程序运行如下：

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

可以看到，系统允许最大消息数量是10条，当客户进程一次性加入12条消息时，客户进程在加入最后两条会阻塞在那里，直到服务进程取出消息之后，最后两天消息才能依次加入到消息队列。并且服务进程取出消息时从优先级从高到低取出：10->11->12->9->8->... ->1


http://download.csdn.net/detail/gentleliu/8228425
本节源码下载：
