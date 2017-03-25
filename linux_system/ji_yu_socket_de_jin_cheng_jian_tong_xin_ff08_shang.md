# 基於socket的進程間通信（上）


 在一個較大的工程當中，一般都會有多個進程構成，各個功能是一個獨立的進程在運行。既然多個進程構成一個工程，那麼多個進程之間肯定會存在一些信息交換或共享數據，這就涉及到進程間通信。進程間通道有很多種，比如有最熟悉網絡編程中的socket、還有共享內存、消息隊列、信號、管道等很多方式，每一種方式都有自己的適用情況，在本系列文章中筆者將會對多種進程間通信方式進行詳解，一是對自己工作多年在這方面的經驗做一個積累，二是將其分享給各位民工，或許還能從大家的拍磚當中得到意外收穫。
 
 socket網絡編程可能使用得最多，經常用在網絡上不同主機之間的通信。其實在同一主機內通信也可以使用socket來完成，socket進程通信與網絡通信使用的是統一套接口，只是地址結構與某些參數不同。在使用socket創建套接字時通過指定參數domain是af_inet（ipv4因特網域）或af_inet6（ipv6因特網域）或af_unix（unix域）來實現。
在筆者這一篇文章中曾有詳細介紹創建socket通信流程及基礎知識。


```c
int ser_afinet_listen(int port)
{
    int                 listenfd, on;
    struct sockaddr_in  seraddr;

    listenfd = socket(af_inet, sock_stream, 0);

    if (listenfd < 0) {
        fprintf(stderr, "socket: %s\n", strerror(errno));
        return -1;
    }

    on = 1;
    setsockopt(listenfd, sol_socket, so_reuseaddr, &on, sizeof(on));

    seraddr.sin_family = af_inet;
    seraddr.sin_port = port;
    seraddr.sin_addr.s_addr = htonl(inaddr_any);

    if (bind(listenfd, (struct sockaddr*)&seraddr,
             sizeof(struct sockaddr_in)) < 0) {
        fprintf(stderr, "bind: %s\n", strerror(errno));
        return -1;
    }

    if (listen(listenfd, sock_ipc_max_conn) < 0) {
        fprintf(stderr, "listen: %s\n", strerror(errno));
        return -1;
    }

    return listenfd;
}
```

服務進程處理連接請求如下：


```c
int ser_accept(int listenfd)
{
    int                 connfd;
    struct sockaddr_un  cltaddr;
    ssize_t             recvlen, sendlen;
    char                buf[sock_ipc_max_buf];
    socklen_t           addrlen;

    addrlen = sizeof(cltaddr);

    for (;;) {
        connfd = accept(listenfd, (struct sockaddr*)&cltaddr, &addrlen);

        if (connfd < 0) {
            fprintf(stderr, "accept: %s\n", strerror(errno));
            return -1;
        }

        if (recvlen = ipc_recv(connfd, buf, sizeof(buf)) < 0) {
            continue;
        }

        printf("recv: %s\n", buf);
        snprintf(buf, sizeof(buf), "hello, ipc client!");

        if (ipc_send(connfd, buf, strlen(buf)) < 0) {
            continue;
        }

        close(connfd);
    }
}
```

客戶進程初始化如下：
指定要連接的服務器地址為本地換回地址，這樣發送的連接就會回到本地服務進程。

```c
int clt_afinet_conn_init(int port)
{
    int fd;

    fd = socket(af_inet, sock_stream, 0);

    if (fd < 0) {
        fprintf(stderr, "socket: %s\n", strerror(errno));
        return -1;
    }

    seraddr.sin_family = af_inet;
    seraddr.sin_port = port;

    if (inet_pton(af_inet, "127.0.0.1", &seraddr.sin_addr) < 0) {//環回地址
        fprintf(stderr, "inet_pton: %s\n", strerror(errno));
        return -1;
    }

    return fd;
}
```

客戶進程向服務進程發送接收請求如下：

```c

if (connect(fd, (struct sockaddr*)&seraddr, sizeof(struct sockaddr_in)) < 0)
{
    fprintf(stderr,  "connect: %s\n", strerror(errno));
    return -1;
}

if ((sendlen = ipc_send(fd, buf, strlen(buf))) < 0)
{
    return -1;
}

if ((recvlen = ipc_recv(fd, buf, sizeof(buf))) < 0)
{
    return -1;
}
```

服務進程先運行，客戶進程執行：

```sh
# ./client
recv: hello, ipc client!
# ./server
recv: hello ipc server!
```

通信過程完成。
創建類型為af_unix（或af_local）的socket，表示用於進程通信。
socket進程通信與網絡通信使用的是統一套接口：


```sh
int socket(int domain, int type, int protocol);  
```

其中，domain 參數指定協議族，對於本地套接字來說，其值被置為 af_unix 枚舉值，隨便說一下，af_unix和af_local是同一個值，看下面linux/socket.h頭文件部分如下，兩個宏的值都一樣為1。


```c
/* supported address families. */  
#define af_unspec       0  
#define af_unix         1       /* unix domain sockets          */  
#define af_local        1       /* posix name for af_unix       */  
#define af_inet         2       /* internet ip protocol         */  
#define af_ax25         3       /* amateur radio ax.25          */  
……  
```

以af_xx開頭和pf_xx開頭的域都是一樣的，繼續看頭文件部分內容就一切都明白了：

```c
……  
#define pf_unspec       af_unspec  
#define pf_unix         af_unix  
#define pf_local        af_local  
#define pf_inet         af_inet  
#define pf_ax25         af_ax25  
……  
```

所以我們在指定socket的類型時這四個域可以隨便用啦，筆者這裡統一使用af_unix了。
type 參數可被設置為 sock_stream（流式套接字）或 sock_dgram（數據報式套接字），對於本地套接字來說，流式套接字（sock_stream）是一個有順序的、可靠的雙向字節流，相當於在本地進程之間建立起一條數據通道；數據報式套接字（sock_dgram）相當於單純的發送消息，在進程通信過程中，理論上可能會有信息丟失、複製或者不按先後次序到達的情況，但由於其在本地通信，不通過外界網絡，這些情況出現的概率很小。
本地套接字的通信雙方均需要具有本地地址，地址類型為 struct sockaddr_un結構體（位於linux/un.h）：


```c
#ifndef _linux_un_h  
#define _linux_un_h  
   
#define unix_path_max   108  
   
struct sockaddr_un {  
        sa_family_t sun_family; /* af_unix */  
        char sun_path[unix_path_max];   /* pathname */  
};  
#endif /* _linux_un_h */ 
```

創建監聽套接字：

```c
int ser_afunix_listen(const char* pathname)
{
    int                 listenfd, on;
    struct sockaddr_un  seraddr;

    listenfd = socket(af_unix, sock_stream, 0);

    if (listenfd < 0) {
        fprintf(stderr, "socket: %s\n", strerror(errno));
        return -1;
    }

    unlink(pathname);
    seraddr.sun_family = af_unix;
    snprintf(seraddr.sun_path, sizeof(seraddr.sun_path), "%s", pathname);

    if (bind(listenfd, (struct sockaddr*)&seraddr,
             sizeof(struct sockaddr_un)) < 0) {
        fprintf(stderr, "bind: %s\n", strerror(errno));
        return -1;
    }

    if (listen(listenfd, sock_ipc_max_conn) < 0) {
        fprintf(stderr, "listen: %s\n", strerror(errno));
        return -1;
    }

    return listenfd;
}
```
 
服務進程處理連接請求類似上面網絡通信。
 
客戶進程初始化套接字過程為：

```c
int clt_afunix_conn_init(const char* pathname)
{
    int                 fd;
    struct sockaddr_un  localaddr;

    fd = socket(af_unix, sock_stream, 0);

    if (fd < 0) {
        fprintf(stderr, "socket: %s\n", strerror(errno));
        return -1;
    }

    localaddr.sun_family = af_unix;
    snprintf(localaddr.sun_path, sizeof(localaddr.sun_path), "%s-cltid%d", pathname,
             getpid());

    if (bind(fd, (struct sockaddr*)&localaddr, sizeof(struct sockaddr_un)) < 0) {
        fprintf(stderr, "bind: %s\n", strerror(errno));
        return -1;
    }

    seraddr.sun_family = af_unix;
    snprintf(seraddr.sun_path, sizeof(seraddr.sun_path), "%s", pathname);

    return fd;
}
```

客戶進程向服務進程發送接收請求類似上面網絡通信。
運行結果同網絡通信部分。
本節僅僅給出了一個很簡單的通信程序，僅僅簡單實現了客戶進程和服務進程之間的通信，在下一節筆者將會在本節示例的基礎上做修改，寫一個在實際應用中可以使用的代碼。
本節示例代碼下載鏈接：

http://download.csdn.net/detail/gentleliu/8140459
 
 
 
 