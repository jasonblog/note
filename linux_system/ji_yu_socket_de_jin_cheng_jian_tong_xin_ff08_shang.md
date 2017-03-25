# 基于socket的进程间通信（上）


 在一个较大的工程当中，一般都会有多个进程构成，各个功能是一个独立的进程在运行。既然多个进程构成一个工程，那么多个进程之间肯定会存在一些信息交换或共享数据，这就涉及到进程间通信。进程间通道有很多种，比如有最熟悉网络编程中的socket、还有共享内存、消息队列、信号、管道等很多方式，每一种方式都有自己的适用情况，在本系列文章中笔者将会对多种进程间通信方式进行详解，一是对自己工作多年在这方面的经验做一个积累，二是将其分享给各位民工，或许还能从大家的拍砖当中得到意外收获。
 
 socket网络编程可能使用得最多，经常用在网络上不同主机之间的通信。其实在同一主机内通信也可以使用socket来完成，socket进程通信与网络通信使用的是统一套接口，只是地址结构与某些参数不同。在使用socket创建套接字时通过指定参数domain是af_inet（ipv4因特网域）或af_inet6（ipv6因特网域）或af_unix（unix域）来实现。
在笔者这一篇文章中曾有详细介绍创建socket通信流程及基础知识。


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

服务进程处理连接请求如下：


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

客户进程初始化如下：
指定要连接的服务器地址为本地换回地址，这样发送的连接就会回到本地服务进程。

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

    if (inet_pton(af_inet, "127.0.0.1", &seraddr.sin_addr) < 0) {//环回地址
        fprintf(stderr, "inet_pton: %s\n", strerror(errno));
        return -1;
    }

    return fd;
}
```

客户进程向服务进程发送接收请求如下：

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

服务进程先运行，客户进程执行：

```sh
# ./client
recv: hello, ipc client!
# ./server
recv: hello ipc server!
```

通信过程完成。
创建类型为af_unix（或af_local）的socket，表示用于进程通信。
socket进程通信与网络通信使用的是统一套接口：


```sh
int socket(int domain, int type, int protocol);  
```

其中，domain 参数指定协议族，对于本地套接字来说，其值被置为 af_unix 枚举值，随便说一下，af_unix和af_local是同一个值，看下面linux/socket.h头文件部分如下，两个宏的值都一样为1。


```c
/* supported address families. */  
#define af_unspec       0  
#define af_unix         1       /* unix domain sockets          */  
#define af_local        1       /* posix name for af_unix       */  
#define af_inet         2       /* internet ip protocol         */  
#define af_ax25         3       /* amateur radio ax.25          */  
……  
```

以af_xx开头和pf_xx开头的域都是一样的，继续看头文件部分内容就一切都明白了：

```c
……  
#define pf_unspec       af_unspec  
#define pf_unix         af_unix  
#define pf_local        af_local  
#define pf_inet         af_inet  
#define pf_ax25         af_ax25  
……  
```

所以我们在指定socket的类型时这四个域可以随便用啦，笔者这里统一使用af_unix了。
type 参数可被设置为 sock_stream（流式套接字）或 sock_dgram（数据报式套接字），对于本地套接字来说，流式套接字（sock_stream）是一个有顺序的、可靠的双向字节流，相当于在本地进程之间建立起一条数据通道；数据报式套接字（sock_dgram）相当于单纯的发送消息，在进程通信过程中，理论上可能会有信息丢失、复制或者不按先后次序到达的情况，但由于其在本地通信，不通过外界网络，这些情况出现的概率很小。
本地套接字的通信双方均需要具有本地地址，地址类型为 struct sockaddr_un结构体（位于linux/un.h）：


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

创建监听套接字：

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
 
服务进程处理连接请求类似上面网络通信。
 
客户进程初始化套接字过程为：

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

客户进程向服务进程发送接收请求类似上面网络通信。
运行结果同网络通信部分。
本节仅仅给出了一个很简单的通信程序，仅仅简单实现了客户进程和服务进程之间的通信，在下一节笔者将会在本节示例的基础上做修改，写一个在实际应用中可以使用的代码。
本节示例代码下载链接：

http://download.csdn.net/detail/gentleliu/8140459
 
 
 
 