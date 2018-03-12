---
title: Linux AIO
layout: post
comments: true
language: chinese
category: [linux,program]
keywords: linux,aio
description: AIO 是 Linux 下的异步读写模型，它是 2.6 内核提供的一个标准增强特性。对于文件的读写，即使以 O_NONBLOCK 方式来打开一个文件，也会处于 "阻塞" 状态，因为文件时时刻刻处于可读状态，而从磁盘到内存所等待的时间是惊人的。为了充份发挥把数据从磁盘复制到内存的时间，引入了 AIO 模型，其基本原理是允许进程发起很多 I/O 操作，而不用阻塞或等待任何操作完成。稍后或在接收到 I/O 操作完成的通知时，进程就可以检索 I/O 操作的结果。
---

AIO 是 Linux 下的异步读写模型，它是 2.6 内核提供的一个标准增强特性。对于文件的读写，即使以 O_NONBLOCK 方式来打开一个文件，也会处于 "阻塞" 状态，因为文件时时刻刻处于可读状态，而从磁盘到内存所等待的时间是惊人的。

为了充份发挥把数据从磁盘复制到内存的时间，引入了 AIO 模型，其基本原理是允许进程发起很多 I/O 操作，而不用阻塞或等待任何操作完成。稍后或在接收到 I/O 操作完成的通知时，进程就可以检索 I/O 操作的结果。

<!-- more -->

## IO 模型

下图给出了同步和异步，以及阻塞和非阻塞的模型。简单来说，一个系统调用 (read, select) 立即返回表示非阻塞；在一个时间点只能去处理一个请求表示同步。

![io brief]({{ site.url }}/images/linux/aio-brief.gif "io brier"){: .pull-center }

每个 I/O 模型都有自己的使用模式，它们对于特定的应用程序都有自己的优点。

### 同步阻塞IO

最常用的模型，当用户空间的应用执行一个系统调用后，会导致应用程序阻塞，直到系统调用完成为止（数据传输完成或发生错误）。应用程序只能处于一种不再消费 CPU 而只是简单等待响应的状态，因此从处理的角度来看，这是非常有效的。

![io block sync]({{ site.url }}/images/linux/aio-block-sync.gif "io block sync"){: .pull-center }

在调用 read 系统调用时，应用程序会阻塞并对内核进行上下文切换；然后会触发读磁盘操作，当从读取的设备中返回后，数据就被移动到用户空间的缓冲区中；然后应用程序就会解除阻塞（read 调用返回）。

### 同步非阻塞IO

备以非阻塞的形式打开，这意味着 I/O 操作不会立即完成，read 操作可能会返回一个错误代码，说明这个命令不能立即满足（EAGAIN 或 EWOULDBLOCK）。

该模型中可能需要应用程序调用多次来等待操作完成，这样的效率仍然不高，因为很多情况下，当内核执行这个命令时，应用程序必须要进行忙碌等待，直到数据可用为止，或者试图执行其他工作。

![io nonblock sync]({{ site.url }}/images/linux/aio-nonblock-sync.gif "io nonblock sync"){: .pull-center }

正如上图所示，这个方法会引入 I/O 操作的延时，因为数据在内核中变为可用到用户调用 read 返回数据之间存在一定的间隔，这会导致整体数据吞吐量的降低。

### 异步阻塞IO

阻塞通知的非阻塞 I/O，配置的是非阻塞 I/O，然后使用阻塞 select 系统调用来确定一个 I/O 描述符何时有操作。select 可以为多个描述符提供通知，而不是仅仅为一个描述符提供通知；通知的事件包括写数据、有读数据可用以及是否发生错误。

![io block async]({{ site.url }}/images/linux/aio-block-async.gif "io block async"){: .pull-center }

select 调用的主要问题是它的效率不是非常高，尽管这是异步通知使用的一种方便模型，但是对于高性能的 I/O 操作来说不建议使用。

### 异步非阻塞IO（AIO）

一种处理与 I/O 重叠进行的模型，读请求会立即返回，说明 read 请求已经成功发起了；然后应用程序会执行其他处理操作；当 read 的响应到达时，就会产生一个信号或执行一个基于线程的回调函数来完成这次 I/O 处理过程。

![io nonblock async]({{ site.url }}/images/linux/aio-nonblock-async.gif "io nonblock async"){: .pull-center }

该模型可以重叠执行多个 I/O 请求以及 CPU 操作，利用了处理速度与 I/O 速度之间的差异。当一个或多个 I/O 请求挂起时，此时 CPU 可以执行其他任务；或者更为常见的是，在发起其他 I/O 的同时对已经完成的 I/O 进行操作。

## Linux AIO简介

Linux 中有两套异步 IO，一套是由 glibc 实现的 `aio_*` 系列，通过线程+阻塞调用在用户空间模拟 AIO 的功能，不需要内核的支持，类似的还有 `libeio`；另一套是采用原生的 Linux AIO，并由 `libaio` 来封装调用接口，相比来说更底层。

glibc 的实现性能比较差，在此先介绍 `libaio` 的使用。`libaio` 的使用并不复杂，过程为：A) `libaio` 的初始化； B) IO 请求的下发和回收，C) `libaio` 销毁。提供了下面五个主要 API 函数以及宏定义：

{% highlight c %}
int io_setup(int maxevents, io_context_t *ctxp);
int io_destroy(io_context_t ctx);
int io_submit(io_context_t ctx, long nr, struct iocb *ios[]);
int io_cancel(io_context_t ctx, struct iocb *iocb, struct io_event *evt);
int io_getevents(io_context_t ctx_id, long min_nr, long nr, struct io_event *events, struct timespec *timeout);

void io_set_callback(struct iocb *iocb, io_callback_t cb);
void io_prep_pwrite(struct iocb *iocb, int fd, void *buf, size_t count, long long offset);
void io_prep_pread(struct iocb *iocb, int fd, void *buf, size_t count, long long offset);
void io_prep_pwritev(struct iocb *iocb, int fd, const struct iovec *iov, int iovcnt, long long offset);
void io_prep_preadv(struct iocb *iocb, int fd, const struct iovec *iov, int iovcnt, long long offset);

struct iocb {
    PADDEDptr(void *data, __pad1);  /* Return in the io completion event */
    PADDED(unsigned key, __pad2);   /* For use in identifying io requests */

    short       aio_lio_opcode;
    short       aio_reqprio;
    int         aio_fildes;

    union {
        struct io_iocb_common    c;
        struct io_iocb_vector    v;
        struct io_iocb_poll      poll;
        struct io_iocb_sockaddr  saddr;
    } u;
};
{% endhighlight %}

后五个宏定义是为了操作 `struct iocb` 结构体，该结构体是 `libaio` 中很重要的一个结构体，用于表示 IO，但是其结构略显复杂，为了保持封装性不建议直接操作其元素而用上面五个宏定义操作。

#### 初始化

用来初始化类型为 `io_context_t` 的变量，这个变量为 `libaio` 的工作空间，可以采用 `io_setup()` 或者 `io_queue_init()`，两者功能实际相同。

#### 自定义字段

这一阶段是可选的，在 `struct iocb` 中保留了供用户自定义的元素，也就是 `void *data`，可以通过 `io_set_callback()` 设置回调函数，或者通过 `iocbp->data=XXX` 自定义。

需要注意的是，两者均使用 data 变量，因此不能同时使用。

#### 读写请求下发

读写均通过 `io_submit()` 下发，之前需要通过 `io_prep_pwrite()` 和 `io_prep_pread()` 生成 `struct iocb` 做为该函数的参数。在这个结构体中指定了读写类型、起始扇区、长度和设备标志符等信息。

#### 读写请求回收

使用 `io_getevents()` 等待 IO 的结束信号，该函数返回 `events[]` 数组，`nr` 为数组的最大长度，`min_nr` 为最少返回的 `events` 数，`timeout` 可填 `NULL` 表示一直等待。

{% highlight c %}
struct io_event {
    PADDEDptr(void *data, __pad1);
    PADDEDptr(struct iocb *obj,  __pad2);
    PADDEDul(res,  __pad3);
    PADDEDul(res2, __pad4);
};
{% endhighlight %}

其中，res 为实际完成的字节数；res2 为读写成功状态，0 表示成功；obj 为之前下发的 struct iocb 结构体。

#### 销毁

直接通过 `io_destory()` 销毁即可。

### 示例

下面是一个简单的示例，通过 AIO 写入到 foobar.txt 文件中。

{% highlight c %}
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <libaio.h>

int main(void)
{
    int               output_fd;
    struct iocb       io, *p=&io;
    struct io_event   e;
    struct timespec   timeout;
    io_context_t      ctx;
    const char        *content="hello world!";

    // 1. init the io context.
    memset(&ctx, 0, sizeof(ctx));
    if(io_setup(10, &ctx)){
        printf("io_setup error\n");
        return -1;
    }

    // 2. try to open a file.
    if((output_fd=open("foobar.txt", O_CREAT|O_WRONLY, 0644)) < 0) {
        perror("open error");
        io_destroy(ctx);
        return -1;
    }

    // 3. prepare the data.
    io_prep_pwrite(&io, output_fd, (void*)content, strlen(content), 0);
    //io.data = content;   // set or not
    if(io_submit(ctx, 1, &p) < 0){
        io_destroy(ctx);
        printf("io_submit error\n");
        return -1;
    }

    // 4. wait IO finish.
    while(1) {
        timeout.tv_sec  = 0;
        timeout.tv_nsec = 500000000; // 0.5s
        if(io_getevents(ctx, 0, 1, &e, &timeout) == 1) {
            close(output_fd);
            break;
        }
        printf("haven't done\n");
        sleep(1);
    }
    io_destroy(ctx);
    return 0;
}
{% endhighlight %}

然后，可以通过 ```gcc foobar.c -o foobar -laio -Wall``` 进行编译。

## POSIX AIO

也就是 glibc 中包含的版本，主要包含如下接口：

{% highlight c %}
#include <aio.h>

// 提交一个异步读/写，通过结构体告知系统读取的文件、起始位置、读取字节数、读取后的写入buffer
int aio_read(struct aiocb *aiocbp);
int aio_write(struct aiocb *aiocbp);

// 检查当前AIO的状态，可用于查看请求是否成功，返回0(成功)EINPROGRESS(正在读取)
int aio_error(const struct aiocb *aiocbp);

// 查看一个异步请求的返回值，如果成功则返回读取字节数，否则返回-1，此时跟同步读写定义的一样
ssize_t aio_return(struct aiocb *aiocbp);         
{% endhighlight %}

<!--
int aio_cancel(int fildes, struct aiocb *aiocbp); /* 取消一个异步请求（或基于一个fd的所有异步请求，aiocbp==NULL） */
int aio_suspend(const struct aiocb * const list[], int nent, const struct timespec *timeout); /* 阻塞等待请求完成 */

其中，struct aiocb主要包含以下字段：
int               aio_fildes;        /* 要被读写的fd */
void *            aio_buf;           /* 读写操作对应的内存buffer */
__off64_t         aio_offset;        /* 读写操作对应的文件偏移 */
size_t            aio_nbytes;        /* 需要读写的字节长度 */
int               aio_reqprio;       /* 请求的优先级 */
struct sigevent   aio_sigevent;      /* 异步事件，定义异步操作完成时的通知信号或回调函数 */

异步 IO 的实现逻辑大致如下：

1. 异步请求提交到请求队列中；
request_queue中；
2、request_queue实际上是一个表结构，"行"是fd、"列"是具体的请求。也就是说，同一个fd的请求会被组织在一起；
3、异步请求有优先级概念，属于同一个fd的请求会按优先级排序，并且最终被按优先级顺序处理；
4、随着异步请求的提交，一些异步处理线程被动态创建。这些线程要做的事情就是从request_queue中取出请求，然后处理之；
5、为避免异步处理线程之间的竞争，同一个fd所对应的请求只由一个线程来处理；
6、异步处理线程同步地处理每一个请求，处理完成后在对应的aiocb中填充结果，然后触发可能的信号通知或回调函数（回调函数是需要创建新线程来调用的）；
7、异步处理线程在完成某个fd的所有请求后，进入闲置状态；
8、异步处理线程在闲置状态时，如果request_queue中有新的fd加入，则重新投入工作，去处理这个新fd的请求（新fd和它上一次处理的fd可以不是同一个）；
9、异步处理线程处于闲置状态一段时间后（没有新的请求），则会自动退出。等到再有新的请求时，再去动态创建；
看起来，换作是我们，要在用户态实现一个异步IO，似乎大概也会设计成类似的样子……
-->

### 示例程序

如下是一个测试示例。

{% highlight c %}
#include <aio.h>
#include <errno.h>
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>

// dd if=/dev/urandom of="foobar.txt" count=10000
const int SIZE_TO_READ = 100;

int main()
{
  int file = open("foobar.txt", O_RDONLY, 0);
  if (file == -1) {
    printf("Unable to open file!\n");
    exit(EXIT_FAILURE);
  }
  char* buffer = (char *)malloc(SIZE_TO_READ);

  struct aiocb cb;
  memset(&cb, 0, sizeof(struct aiocb));
  cb.aio_nbytes = SIZE_TO_READ;
  cb.aio_fildes = file;
  cb.aio_offset = 0;
  cb.aio_buf = buffer;

  if (aio_read(&cb) == -1) {
    printf("Unable to create request!\n");
    close(file);
    exit(EXIT_FAILURE);
  }
  printf("Request enqueued!\n");

  // wait until the request has finished
  while(aio_error(&cb) == EINPROGRESS) {
    printf("Working...\n");
  }

  int numBytes = aio_return(&cb); // success?
  if (numBytes != -1)
    printf("Success!\n");
  else
    printf("Error!\n");

  free(buffer);
  close(file);

  return 0;
}
{% endhighlight %}

<!--
http://lse.sourceforge.net/io/aio.html
http://www.cnblogs.com/hustcat/archive/2013/02/05/2893488.html
http://www.wzxue.com/linux-kernel-aio%E8%BF%99%E4%B8%AA%E5%A5%87%E8%91%A9/
http://blog.yufeng.info/archives/741
http://blog.csdn.net/q1007729991/article/details/70143062
http://blog.csdn.net/jwybobo2007/article/details/6107419
http://zhaozhanxu.com/2016/07/17/Linux/2016-07-17-Linux-Kernel-Pkts_Processing6/
-->


## Linux direct-io简介

在 Linux 2.6 使用 direct io 需要按照如下几点来做：

1. 在源文件的最顶端加上 #define _GNU_SOURCE  宏定义，或在编译时在命令行上指定，否则编译报错。
2. 在 open() 时加上 O_DIRECT 标志。
3. 存放文件数据的缓存起始位置以及每次读写数据长度必须是磁盘逻辑块大小的整数倍，一般是 512 字节，否则将导致读写失败，返回 -EINVAL。

对于第 3 点，要满足缓存区起始位置对齐，可以在进行缓存区空间申请时使用 posix_memalign 这样的函数指定字节对齐。

{% highlight text %}
ret = posix_memalign((void **)&buf, 512, BUF_SIZE);
real_buf = malloc(BUF_SIZE + 512);
aligned_buf = ((((unsigned int)real_buf + 512 - 1) / 512) * 512);
{% endhighlight %}

由于要满足每一次读写数据长度必须是磁盘逻辑块大小的整数倍，所以最后一次文件操作可能无法满足，此时只能重新以cached io模式打开文件后，fseek到对应位置进行剩余数据的读写。

## 参考

<a href="http://www.ibm.com/developerworks/cn/linux/l-async/">使用异步 I/O 大大提高应用程序的性能</a> 介绍各种 IO 模型不错的参考；在内核文档 <a href="http://www.fsl.cs.sunysb.edu/~vass/linux-aio.txt">linux-io.txt</a> 中介绍了 Linux AIO 机制以及部分 libaio、librt 的内容。

{% highlight text %}
{% endhighlight %}
