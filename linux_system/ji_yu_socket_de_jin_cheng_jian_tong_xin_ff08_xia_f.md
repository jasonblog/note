# 基於socket的進程間通信（下）


 在兩個進程通信當中，存在兩個進程相互交換信息的過程，有的都比較複雜，不像上一節那樣簡單。一般情況下，存在一個服務進程一直在等待客戶進程連接，客戶進程和服務進程存在如下三種交換數據方式：
客戶進程發獲取服務進程某一全局數據的請求，服務進程返回該數據（簡稱get請求）；
客戶進程發設置服務進程全局數據的請求（簡稱set請求）；
客戶進程發設置服務進程全局數據的請求，服務進程設置完成後返回某一數據，（set與get並存）；
 
這一節我們就來完成這樣一個可以在實際應用中使用的socket進程間通信。
上面所描述的get與set請求可能有很多種，每一種請求所使用到的數據都不一樣，所以我們需要為每一請求定義一個類型，服務進程通過類型可以知道客戶進程需要處理什麼數據；每一個請求服務進程都應當返回其處理的結果返回值；既然是兩個進程通信，那麼就一定存在數據交換，所以一定存在數據的收發，以及需要知道對端收發數據的大小。所以我們根據這種需要首先定義兩個進程之間交換數據的數據結構（類似於tcp/ip協議數據包格式）：


```c
typedef struct _ipc_sock_msg_t {
    int     msg_type;//請求類型
    int     msg_rc;//服務進程處理結果的返回值
    int     msg_buflen;//交換數據的大小
    char    msg_buf[SOCK_IPC_MAX_BUF];//交換數據的內容
} ipc_sock_msg_t;
```



服務進程收到客戶進程請求之後首先判斷請求類型，根據請求類型來進行處理。我們首先定義一個函數數組，在服務進程接收請求之前將要處理的所有請求註冊到該函數數組當中來，收到請求之後根據請求類型索引找到處理函數。
函數數組如下：

```c
static int
(*sln_ipc_ser_func[SLN_IPC_MAX_TYPE])(
    void* recvbuf, int recv_size,
    void* sendbuf, int* send_size);
```

服務進程接收處理之前先將需要處理的函數註冊到函數數組中，如下：

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

之後服務進程開始監聽，等待連接：
監聽代碼類似上節示例：

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


服務進程接收客戶進程發送的數據，交給函數sln_ser_handle來處理：

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

其中處理函數sln_ser_handle實現為：

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

在函數sln_ser_handle中調用初始化時註冊的服務進程處理函數來完成客戶進程的請求。初始化註冊的兩函數為：

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

處理函數帶4個參數，分別為：接收數據buffer、接收到的數據大小、發送數據buffer、發送數據大小。其中前兩個參數為輸入參數，後兩個參數為返回輸出的參數，服務進程需要根據客戶進程發送過來的數據來處理客戶請求，然後返回服務進程需要返回的數據以及處理結果（完成或失敗），處理結果在該處理函數的返回值中返回。
下面看看客戶進程的實現：

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

 客戶進程調用的接口的實現起來比較簡單，只需要告知服務進程我請求的處理類型（msg_type），以及需要傳輸的數據（sendbuf）及大小（sendlen），服務進程會返回處理結果（ret_code）以及返回數據（recvbuf）和大小（recvlen）。
本節示例源碼：
http://download.csdn.net/detail/gentleliu/8140479


