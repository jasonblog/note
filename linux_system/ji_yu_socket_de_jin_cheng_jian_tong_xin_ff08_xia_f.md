# 基于socket的进程间通信（下）


 在两个进程通信当中，存在两个进程相互交换信息的过程，有的都比较复杂，不像上一节那样简单。一般情况下，存在一个服务进程一直在等待客户进程连接，客户进程和服务进程存在如下三种交换数据方式：
客户进程发获取服务进程某一全局数据的请求，服务进程返回该数据（简称get请求）；
客户进程发设置服务进程全局数据的请求（简称set请求）；
客户进程发设置服务进程全局数据的请求，服务进程设置完成后返回某一数据，（set与get并存）；
 
这一节我们就来完成这样一个可以在实际应用中使用的socket进程间通信。
上面所描述的get与set请求可能有很多种，每一种请求所使用到的数据都不一样，所以我们需要为每一请求定义一个类型，服务进程通过类型可以知道客户进程需要处理什么数据；每一个请求服务进程都应当返回其处理的结果返回值；既然是两个进程通信，那么就一定存在数据交换，所以一定存在数据的收发，以及需要知道对端收发数据的大小。所以我们根据这种需要首先定义两个进程之间交换数据的数据结构（类似于tcp/ip协议数据包格式）：


```c
typedef struct _ipc_sock_msg_t {
    int     msg_type;//请求类型
    int     msg_rc;//服务进程处理结果的返回值
    int     msg_buflen;//交换数据的大小
    char    msg_buf[SOCK_IPC_MAX_BUF];//交换数据的内容
} ipc_sock_msg_t;
```



服务进程收到客户进程请求之后首先判断请求类型，根据请求类型来进行处理。我们首先定义一个函数数组，在服务进程接收请求之前将要处理的所有请求注册到该函数数组当中来，收到请求之后根据请求类型索引找到处理函数。
函数数组如下：

```c
static int
(*sln_ipc_ser_func[SLN_IPC_MAX_TYPE])(
    void* recvbuf, int recv_size,
    void* sendbuf, int* send_size);
```

服务进程接收处理之前先将需要处理的函数注册到函数数组中，如下：

```c
int sln_ipc_ser_func_init(void)
{
    int     i;

    for (i = 0; i < SLN_IPC_MAX_TYPE; i++) {
        sln_ipc_ser_func[i] = NULL;
    }

    sln_ipc_ser_func[SLN_IPC_TYPE_0x1] = sln_ipc_handle_0x1;
    sln_ipc_ser_func[SLN_IPC_TYPE_0x2] = sln_ipc_handle_0x2;

    return 0;
}
```

之后服务进程开始监听，等待连接：
监听代码类似上节示例：

```c
#if USE_AF_UNIX
fd = sln_ipc_ser_afunix_listen(SOCK_IPC_NAME);

if (fd < 0)
{
    return -1;
}

#else
fd = sln_ipc_ser_afinet_listen(SOCK_IPC_SER_LISTEN_PORT);

if (fd < 0)
{
    return -1;
}
#endif
```


服务进程接收客户进程发送的数据，交给函数sln_ser_handle来处理：

```c
static int
sln_ipc_ser_accept(int listenfd)
{
    int                 connfd;
    ssize_t             recvlen;
    ipc_sock_msg_t      recv_msg;
    socklen_t           addrlen;
#if USE_AF_UNIX
    struct sockaddr_un  cltaddr;
#else
    struct sockaddr_in  cltaddr;
#endif

    addrlen = sizeof(cltaddr);

    for (;;) {
        connfd = accept(listenfd, (struct sockaddr*)&cltaddr, &addrlen);

        if (connfd < 0) {
            fprintf(stderr, "accept: %s\n", strerror(errno));
            continue;
        }

        if ((recvlen = sln_ipc_recv(connfd, &recv_msg, sizeof(ipc_sock_msg_t))) < 0) {
            continue;
        }

        sln_ser_handle(connfd, &recv_msg);

        close(connfd);
    }

    return 0;
}
```

其中处理函数sln_ser_handle实现为：

```c
static int
sln_ser_handle(int sockfd, ipc_sock_msg_t* recv_msg)
{
    ipc_sock_msg_t  send_msg;

    memset(&send_msg, 0, sizeof(ipc_sock_msg_t));

    send_msg.msg_type = recv_msg->msg_type;

    if ((recv_msg->msg_type >= SLN_IPC_MAX_TYPE)
        && (recv_msg->msg_rc < 0)) {
        send_msg.msg_rc = SLN_IPC_RC_TYPE;
    } else if (NULL == sln_ipc_ser_func[recv_msg->msg_type]) {
        send_msg.msg_rc = SLN_IPC_RC_FUNC;
    } else {
        send_msg.msg_rc
            = sln_ipc_ser_func[recv_msg->msg_type](
                  recv_msg->msg_buf,
                  recv_msg->msg_buflen,
                  send_msg.msg_buf,
                  &send_msg.msg_buflen);
    }

    if (sln_ipc_send(sockfd, &send_msg, sizeof(ipc_sock_msg_t)) < 0) {
        return -1;
    }

    return 0;
}
```

在函数sln_ser_handle中调用初始化时注册的服务进程处理函数来完成客户进程的请求。初始化注册的两函数为：

```c
static char     gbuf[256] = "hello, this is server!";

int
sln_ipc_handle_0x1(void* recvbuf, int recv_size, void* sendbuf, int* send_size)
{
    printf("=============%s->%d===========\n", __func__, __LINE__);
    memcpy(sendbuf, gbuf, strlen(gbuf));

    *send_size = strlen(gbuf);

    return SLN_IPC_RC_OK;
}

int
sln_ipc_handle_0x2(void* recvbuf, int recv_size, void* sendbuf, int* send_size)
{
    printf("=============%s->%d===========\n", __func__, __LINE__);
    memcpy(gbuf, recvbuf, recv_size);

    *send_size = 0;

    return SLN_IPC_RC_OK;
}
```

处理函数带4个参数，分别为：接收数据buffer、接收到的数据大小、发送数据buffer、发送数据大小。其中前两个参数为输入参数，后两个参数为返回输出的参数，服务进程需要根据客户进程发送过来的数据来处理客户请求，然后返回服务进程需要返回的数据以及处理结果（完成或失败），处理结果在该处理函数的返回值中返回。
下面看看客户进程的实现：

```c
int
sln_ipc_clt_conn(
    int msg_type,
    int* ret_code,
    void* sendbuf,
    int sendlen,
    void* recvbuf,
    int* recvlen)
{
    int                 connfd;
    ssize_t             ret_size;
    socklen_t           addrlen;
    ipc_sock_msg_t      send_msg, recv_msg;

#if USE_AF_UNIX

    if ((connfd = sln_ipc_clt_afunix_conn_init(SOCK_IPC_NAME)) < 0) {
        return -1;
    }

    addrlen = sizeof(struct sockaddr_un);
#else

    if ((connfd = sln_ipc_clt_afinet_conn_init(SOCK_IPC_SER_LISTEN_PORT)) < 0) {
        return -1;
    }

    addrlen = sizeof(struct sockaddr_in);
#endif

    if (connect(connfd, (struct sockaddr*)&seraddr, addrlen) < 0) {
        fprintf(stderr,  "connect: %s\n", strerror(errno));
        return -1;
    }

    memset(&send_msg, 0, sizeof(ipc_sock_msg_t));
    send_msg.msg_type = msg_type;

    if (NULL != sendbuf) {
        send_msg.msg_buflen = sendlen;
        memcpy(send_msg.msg_buf, sendbuf, sendlen);
    }

    if ((ret_size = ipc_send(connfd, &send_msg, 3 * sizeof(int) + sendlen)) < 0) {
        return -1;
    }

    if ((ret_size = ipc_recv(connfd, &recv_msg, sizeof(ipc_sock_msg_t))) < 0) {
        return -1;
    }

    if (recv_msg.msg_type != send_msg.msg_type) {
        printf("Error msg type!\n");
        return -1;
    }

    *ret_code = recv_msg.msg_rc;

    if (NULL != recvbuf) {
        *recvlen = recv_msg.msg_buflen;
        memcpy(recvbuf, recv_msg.msg_buf, recv_msg.msg_buflen);
    }

    return 0;
}
```

 客户进程调用的接口的实现起来比较简单，只需要告知服务进程我请求的处理类型（msg_type），以及需要传输的数据（sendbuf）及大小（sendlen），服务进程会返回处理结果（ret_code）以及返回数据（recvbuf）和大小（recvlen）。
本节示例源码：
http://download.csdn.net/detail/gentleliu/8140479


