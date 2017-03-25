# pipe和FIFO


在unix系统上最早的IPC形式为管道，管道的创建使用pipe函数：

```sh
#include <unistd.h>
int pipe(int pipefd[2]);
```

该函数创建一个单向的管道，返回两个描述符 pipefd[0],和pipefd[1]，pipefd[0]用于读操作，pipefd[1]用于写操作。该函数一般应用在父子进程（有亲缘关系的进程）之间的通信，先是一个进程创建管道，再fork出一个子进程，然后父子进程可以通过管道来实现通信。

管道具有以下特点：
管道是半双工的，数据只能向一个方向流动；需要双方通信时，需要建立起两个管道；
只能用于父子进程或者兄弟进程之间（具有亲缘关系的进程）；

单独构成一种独立的文件系统：管道对于管道两端的进程而言，就是一个文件，但它不是普通的文件，它不属于某种文件系统，而是自立门户，单独构成一种文件系统，并且只存在与内存中。

数据的读出和写入：一个进程向管道中写的内容被管道另一端的进程读出。写入的内容每次都添加在管道缓冲区的末尾，并且每次都是从缓冲区的头部读出数据。


函数pipe一般使用步骤如下：

```sh
1.pipe创建管道；
2.fork创建子进程；
3.父子进程分别关闭掉读和写（或写和读）描述符；
4.读端在读描述符上开始读（或阻塞在读上等待写端完成写），写端开始写，完成父子进程通信过程。
一个简单的通信实现（来自linux man手册的修改）：
```


```c
#include <sys/wait.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>

int main(int argc, char* argv[])
{
    int pipefd[2];
    pid_t cpid;
    char buf[128];
    int readlen;


    if (argc != 2) {
        fprintf(stderr, "Usage: %s <string>\n", argv[0]);
        return -1;
    }

    if (pipe(pipefd) < 0) {
        fprintf(stderr, "pipe: %s\n", strerror(errno));
        return -1;
    }

    cpid = fork();

    if (cpid < 0) {
        fprintf(stderr, "fork: %s\n", strerror(errno));
        return -1;
    }

    if (0 == cpid) { /* 子进程 */
        close(pipefd[1]); /* 子进程关闭写端 */
        readlen = read(pipefd[0], buf,
                       128); //子进程阻塞在读上，等待父进程写

        if (readlen < 0) {
            fprintf(stderr, "read: %s\n", strerror(errno));
            return -1;
        }

        write(STDOUT_FILENO, buf, readlen);
        write(STDOUT_FILENO, "\n", 1);
        close(pipefd[0]); //读完之后关闭读描述符
        return 0;
    } else { /* 父进程 */
        close(pipefd[0]); /*父进程关闭没用的读端 */
        sleep(2);
        write(pipefd[1], argv[1], strlen(argv[1])); //父进程开始写
        close(pipefd[1]); /* 父进程关闭写描述符 */
        wait(NULL); /* 父进程等待子进程退出，回收子进程资源 */
        return 0;
    }
}
```

运行时将打印命令行输入参数，打印将在父进程睡眠2秒之后，子进程将阻塞在读，直到父进程写完数据，可见管道是有同步机制的，不需要自己添加同步机制。如果希望两个进程双向数据传输，那么需要建立两个管道来实现。

管道最大的劣势就是只能在拥有共同祖先进程的进程之间通信，在无亲缘关系的两个进程之间没有办法使用，不过有名管道FIFO解决了这个问题。FIFO类似于pipe，也是只能单向传输数据，不过和pipe不同的是他可以在无亲缘关系的进程之间通信，它提供一个路径与之关联，所以只要能访问该路径的进程都可以建立起通信，类似于前面的共享内存，都提供一个路径与之关联。

```sh
#include <sys/types.h>
#include <sys/stat.h>
int mkfifo(const char *pathname, mode_t mode);
```

pathname 为系统路径名，mode为文件权限位，类似open函数第二个参数。

打开或创建一个新的fifo是先调用mkfifo，当指定的pathname已存在fifo时，mkfifo返回EEXIST错误，此时再调用open函数。

下面来使用mkfifo实现一个无亲缘关系进程间的双向通信，此时需要建立两个fifo，分别用于读写。服务进程循环的读并等待客户进程写，之后打印客户进程传来数据并向客户进程返回数据；客户进程向服务器写数据并等待读取服务进程返回的数据。

server process：


```c
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>
#include "slnipc.h"
int main(int argc, const char* argv[])
{
    int rc;
    int wr_fd, rd_fd;
    char sendbuf[128];
    char recvbuf[128];
    rc = mkfifo(SLN_IPC_2SER_PATH, O_CREAT | O_EXCL); //建立服务进程读的fifo

    if ((rc < 0) && (errno != EEXIST)) {
        fprintf(stderr, "mkfifo: %s\n", strerror(errno));
        return -1;
    }

    rc = mkfifo(SLN_IPC_2CLT_PATH, O_CREAT | O_EXCL); //建立服务进程写的fifo

    if ((rc < 0) && (errno != EEXIST)) {
        fprintf(stderr, "mkfifo: %s\n", strerror(errno));
        return -1;
    }

    wr_fd = open(SLN_IPC_2CLT_PATH, O_RDWR, 0);

    if (wr_fd < 0) {
        fprintf(stderr, "open: %s\n", strerror(errno));
        return -1;
    }

    rd_fd = open(SLN_IPC_2SER_PATH, O_RDWR, 0);

    if (rd_fd < 0) {
        fprintf(stderr, "open: %s\n", strerror(errno));
        return -1;
    }

    for (;;) {
        rc = read(rd_fd, recvbuf,
                  sizeof(recvbuf)); //循环等待接受客户进程数据

        if (rc < 0) {
            fprintf(stderr, "read: %s\n", strerror(errno));
            continue;
        }

        printf("server recv: %s\n", recvbuf);
        snprintf(sendbuf, sizeof(sendbuf), "Hello, this is server!\n");
        rc = write(wr_fd, sendbuf, strlen(sendbuf));

        if (rc < 0) {
            fprintf(stderr, "write: %s\n", strerror(errno));
            continue;
        }
    }

    close(wr_fd);
    close(rd_fd);
    return 0;
}
```

client process：

```c
#include <stdio.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>
#include <string.h>
#include <errno.h>
#include "slnipc.h"
int main(int argc, const char *argv[])
{
    int rc;
    int rd_fd, wr_fd;
    char recvbuf[128];
    char sendbuf[128];
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <string>\n", argv[0]);
        return -1;
    }
    snprintf(sendbuf, sizeof(sendbuf), "%s", argv[1]);
    wr_fd = open(SLN_IPC_2SER_PATH, O_RDWR, 0);
    if (wr_fd < 0) {
        fprintf(stderr, "open: %s\n", strerror(errno));
        return -1;
    }
    rd_fd = open(SLN_IPC_2CLT_PATH, O_RDWR, 0);
    if (rd_fd < 0) {
        fprintf(stderr, "open: %s\n", strerror(errno));
        return -1;
    }
    rc = write(wr_fd, sendbuf, strlen(sendbuf));
    if (rc < 0) {
        fprintf(stderr, "write: %s\n", strerror(errno));
        return -1;
    }
    rc = read(rd_fd, recvbuf, sizeof(recvbuf));
    if (rc < 0) {
        fprintf(stderr, "write: %s\n", strerror(errno));
        return -1;
    }
    printf("client read: %s\n", recvbuf);
    close(wr_fd);
    close(rd_fd);
    return 0;
}
```

服务器先启动运行，之后运行客户端，运行结果

```sh
# ./server 
server recv: hi,this is fifo client 
```

```sh
# ./client "hi,this is fifo client" 
client read: Hello, this is server! 
```


这里有一些类似于socket实现进程间通信过程，只是fifo的读写描述符是两个，socket的读写使用同一个描述符。fifo的出现克服了管道的只能在有亲缘关系的进程之间的通信。和其他的进程间通信一直，fifo传送的数据也是字节流，需要自己定义协议格式来解析通信的数据，可以使用socket章节介绍的方式来实现的通信协议。

本节源码下载：
http://download.csdn.net/detail/gentleliu/8183027
