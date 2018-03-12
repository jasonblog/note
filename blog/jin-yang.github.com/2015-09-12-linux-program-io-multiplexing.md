---
title: Linux IO 多路复用
layout: post
comments: true
language: chinese
category: [linux,program]
keywords: linux,io,multiplexing,多路复用
description: 通过 IO 多路复用技术，系统内核缓冲 IO 数据，当某个 IO 准备好后，系统通知应用程序该 IO 可读或可写，这样应用程序可以马上完成相应的 IO 操作，而不需要等待系统完成相应 IO 操作，从而应用程序不必因等待 IO 操作而阻塞。这里简单介绍下 Linux 中 IO 多路复用的使用。
---

通过 IO 多路复用技术，系统内核缓冲 IO 数据，当某个 IO 准备好后，系统通知应用程序该 IO 可读或可写，这样应用程序可以马上完成相应的 IO 操作，而不需要等待系统完成相应 IO 操作，从而应用程序不必因等待 IO 操作而阻塞。

这里简单介绍下 Linux 中 IO 多路复用的使用。

<!-- more -->

## 简介

在 Linux 中，会将很多资源通过文件描述符表示，包括了文件系统、网络、物理设备等等。

文件描述符就是一个整数，默认会选择最小未使用的数值，其中有三个比较固定，比较特殊的值，0 是标准输入，1 是标准输出，2 是标准错误输出。

0、1、2 是整数表示的，对应的 FILE* 结构的表示就是 stdin、stdout、stderr。

## select

select 可以用来监视多个文件句柄的状态变化，程序会阻塞在 select 直到被监视的文件句柄有一个或多个发生了状态改变，然后通知应用程序，应用程序 <font color='blue'>轮询</font> 所有的 FD 集合，判断监控的 FD 是否有事件发生，并作相应的处理。

select 相关的声明和宏，通过宏来设置参数，相应的执行流程如下。

{% highlight text %}
int select(int maxfd, fd_set * readfds, fd_set * writefds, fd_set * exceptfds, struct timeval * timeout);
FD_CLR(inr fd, fd_set* set);                               // 清除描述词组set中相关fd的位
FD_ISSET(int fd, fd_set *set);                             // 用来测试描述词组set中相关fd的位是否为真
FD_SET(int fd, fd_set*set);                                // 用来设置描述词组set中相关fd的位
FD_ZERO(fd_set *set);                                      // 用来清除描述词组set的全部位
struct timeval {
   time_t tv_sec;
   time_t tv_usec;
};

----- 调用流程
fd_set readfso;                                            // 1.1 定义需要监视的描述符
FD_ZERO(&readfso);                                         // 1.2 清空
FD_SET(fd, &readfso);                                      // 1.3 设置需要监控的描述符，如fd=1
while(1) {
    readfs = readfso;
    ret = select(maxfd+1, &readfds, NULL, NULL, timeout);  // 2.1 监听所关注的事件，在此为监听读事件
    if (ret > 0)                                           // 3.1.1 监听事件发生
        for (i = 0; i > FD_SETSIZE; i++)                   // 3.1.2 需要遍历所有fd
            if (FD_ISSET(fd, &readfds))
                handleEvent();
    else if (ret == 0)                                     // 3.2 超时
        handle timeout
    else if (ret < 0)                                      // 3.3 发生错误
        handle error
}
{% endhighlight %}

在 select 函数中，各个参数的含义如下。
* maxfd 为最大文件描述符+1，因此在程序中需要知道最大描述符的值。
* 接着的三个参数分别设置所监听的读、写、异常事件对应的描述符。该值传入需要监控事件的描述符，返回值保存了发生了的事件对应的描述符。
* 设置超时时间，NULL 表示一直等待。

返回值：成功则返回文件描述符状态已改变的总数；如果返回 0 代表在描述词状态改变前已超过 timeout 时间；当有错误发生时则返回 -1，错误原因存于 errno ，此时参数 readfds, writefds, exceptfds 和 timeout 的值变成不可预测。

在有返回值时，需要通过 FD_ISSET 循环遍历各个描述符，从而文件描述符越多，效率也就越低。而且，每次调用 select 时都需要重新设置需要监控的文件描述符。

fd_set 在 ```sys/select.h``` 中定义，大致可以简化为如下的结构，也就是实际用数组表示，其中每一位代表一个文件描述符，最大可以表示 1024 个，也就是说 select 能监视的描述符最大为 1024 。最大可以监控的文件描述符数可以通过 FD_SETSIZE 获得，描述符需要通过对应的宏来配置。

{% highlight c %}
typedef struct {
    long int  __fds_bits[__FD_SETSIZE(1024) / __NFDBITS(8 * sizeof(long int))];
} fd_set;
{% endhighlight %}

## poll

和 select() 不一样，poll() 没有使用低效的三个基于位的文件描述符 set ，而是采用了一个单独的结构体 pollfd 数组，由 fds 指针指向这个数组，采用链表保存文件描述符。

{% highlight text %}
struct pollfd {
    int fd;              /* 文件描述符 */
    short events;        /* 等待的事件 */
    short revents;       /* 实际发生了的事件 */
} ;
typedef unsigned long   nfds_t;
int poll(struct pollfd *fds, nfds_t nfds, int timeout);

struct pollfd fds[MAX_CONNECTION] = {0};
fds[0].fd = 0;
fds[0].events = POLLIN;
ret = poll(fds, sizeof(fds)/sizeof(struct pollfd), -1)
if (ret > 0)
    for (int i = 0; i < MAX_CONNECTION; i++)
        if (fds[0].revents & POLLIN)
            handleEvent(allConnection[i]);
else if (ret == 0)
    handle timeout
else if (ret > 0)
    handle error
{% endhighlight %}

fds 表示需要监控的文件描述符；nfds 表示 fds 的大小，也即需要监控的描述符数量。如果 fd 为负，则忽略 events，revents 返回 0。

返回值与 select 的返回值含义相同。


## epoll

epoll 是 Linux 内核为处理大批量句柄而作了改进的 poll，是 Linux 下多路复用 IO 接口 select/poll 的增强版本，它能显著减少程序在大量并发连接中只有少量活跃的情况下的系统 CPU 利用率。

### 对比

select 模型的缺点：

* 最大并发数限制，因为一个进程所打开的文件描述符是有限制的，由 FD_SETSIZE 设置，默认值是 1024/2048 ，因此 select 模型的最大并发数就被相应限制了。
* 效率问题，每次 select 调用返回都需要线性扫描全部的 FD 集合，确定那个描述符发生了相应事件，这样效率就会呈现线性下降。
* 内核/用户空间内存拷贝问题，在通知用户事件发生时采用内存拷贝。
* 触发方式采用的是水平触发，应用程序如果没有完成对一个已经就绪的文件描述符的 IO 操作，那么之后每次 select 调用还是会将这些文件描述符通知进程。

对于 poll 而言，2 和 3 都没有改掉，相比来说，epoll 要好的多。
* epoll 没有最大并发连接的限制，上限是最大可以打开文件的数目，这个数字一般远大于 2048, 一般来说这个数目和系统内存关系很大 ，具体数目可以在 /proc/sys/fs/epoll/max_user_watches 中查看。
* 效率提升，epoll 最大的优点就在于它只管你“活跃”的连接，而跟连接总数无关，因此在实际的网络环境中，epoll 的效率就会远远高于 select 和 poll。
* 内存拷贝，epoll 在这点上使用了“共享内存”，这个内存拷贝也省略了。mmap 加速内核与用户空间的信息传递，epoll 是通过内核于用户空间 mmap 同一块内存，避免了无谓的内存拷贝。

<!--
select 可以在某一时间监视最大达到 FD_SETSIZE 数量的文件描述符， 通常是由在 libc 编译时指定的一个比较小的数字。
poll 在同一时间能够监视的文件描述符数量并没有受到限制，即使除了其它因素，更加的是我们必须在每一次都扫描所有通过的描述符来检查其是否存在己就绪通知，它的时间复杂度为 O(n) ，是缓慢的。

epoll 没有以上所示的限制，并且不用执行线性扫描。因此， 它能有更高的执行效率且可以处理大数量的事件。

当描述符被添加到epoll实例中， 有两种添加模式： level triggered（水平触发） 和 edge triggered（边沿触发） 。 当使用 level triggered 模式并且数据就绪待读， epoll_wait总是会返加就绪事件。如果你没有将数据读取完， 并且调用epoll_wait 在epoll 实例上再次监听这个描述符， 由于还有数据是可读的，它会再次返回。在 edge triggered 模式时， 你只会得一次就绪通知。 如果你没有将数据读完， 并且再次在 epoll实例上调用 epoll_wait ， 由于就绪事件已经被发送所以它会阻塞。
-->


### 函数接口

epoll 相关的函数主要有如下三种，分别用于创建，注册、修改、删除，等待事件发生。

#### 创建

创建 epoll 文件描述符，```int epoll_create (int size)``` 。

需要注意的是，当创建好 epoll 句柄后，它就是会占用一个 fd 值，在 linux 下如果查看 ```/proc/<PID>/fd/```，是能够看到这个 fd 的，所以在使用完 epoll 后，必须调用 close() 关闭，否则可能导致 fd 被耗尽。

调用成功则返回与实例关联的文件描述符，该文件描述符与真实的文件没有任何关系，仅作为接下来调用的函数的句柄，实际就是申请一个内核空间，用来保存所关注的 fd 上发生的时将。size 为了与之前兼容，应该大于 0 ，现在是动态分配，而非指定支持事件的数目。正常返回值大于 0；发生错误时，返回 -1，errno 表明错误类型。

#### 修改

控制文件描述符，```int epoll_ctl(int epfd, int op, int fd, struct epoll_event *event);``` 。

类似于相对于 select 模型中的 FD_SET 和 FD_CLR 宏，用于注册、修改、删除；与 select 不同的是，select 在监听事件时告诉内核要监听什么类型的事件，而是在这里先注册要监听的事件类型。

其中<font color='blue'>参数 epfd 是 epoll_create()</font> 创建 epoll 专用的文件描述符；op 用于表示对应的操作(EPOLL_CTL_ADD、EPOLL_CTL_MOD、EPOLL_CTL_DEL)；fd 表示需要监控的描述符；event 表示需要监控的事件。

#### 等待

等待事件 ```int epoll_wait(int epfd, struct epoll_event *events, int maxevents, int timeout);```，内核通过 events 返回发生事件的集合，maxevents 告之内核最多可以返回的事件数，该函数返回需要处理的事件数目，如返回 0 表示已超时。

等待 IO 事件的发生，epfd 是由 epoll_create() 生成的 epoll 专用的文件描述符；epoll_event 用于回传事件发生对应的数组；maxevents 表示能处理的最多的事件数；timeout 等待 IO 事件发生的超时值，-1 表示永久。

epoll_wait() 首先判断参数的有效性，最后会调用 ep_epoll() 函数。epoll 不仅会告诉应用程序有 IO 事件到来，还会告诉应用程序相关的信息，这些信息是应用程序填充的，因此根据这些信息应用程序就能直接定位到事件，而不必遍历整个 FD 集合。

### epoll() 的使用

如下是 epoll 使用的数据结构，执行过程。

{% highlight text %}
struct epoll_event {
    __uint32_t events;      // Epoll events
    epoll_data_t data;      // User data variable
};
typedef union epoll_data {
    void *ptr;
    int fd;
    __uint32_t u32;
    __uint64_t u64;
} epoll_data_t;


struct epoll_event event;
struct epoll_event *events;
int efd = epoll_create(1);                        // 1. 创建文件描述符，大于0即可，实际内核中动态分配
if (efd == -1 || error == 0)
    error

event.data.fd = fd;                               // 2.1 设置需要监听的fd
event.events = EPOLLIN | EPOLLET;                 // 2.2 设置需要监听的事件
ret = epoll_ctl(efd, EPOLL_CTL_ADD, 0, &event)    // 2.3 可以添加、修改、删除
if (ret == -1)
    error

events = calloc(MAX_EVENTS, sizeof event);        // 3.1 申请内存，可返回的最大事件数
while (1) {
    n = epoll_wait(efd, events, MAX_EVENTS, -1);  // 3.2 等待事件发生
    if (n == 0)                                   // 3.3 超时
        timeout
    else if (n < 0)                               // 3.4 发生错误
        if(EINTT == errno)                        // 3.5 由于中断，忽略
            n = 0; continue;
        else
            error
    else                                          // 3.6 处理发生的事件
        for(i = 0; i &lt; n; i++)
            if (events[i].data.fd == fd)
                handleEvent
            else if (enents[i].events & EPOLLIN)   // 某些事件发生
                //
}
free(events);
close(fd);
{% endhighlight %}

结构体 epoll_event 用于注册所感兴趣的事件和回传所发生待处理的事件。epoll_data 联合体用来保存触发事件相关的描述符所对应的信息，可以保存很多类型的信息，如 fd 、指针等等，有了它，应用程序就可以直接定位目标了。

对于 epoll_data ，如一个 client 连接到服务器时，服务器通过调用 accept 函数可以得到 client 对应的 socket 文件描述符，并通过 epoll_data 的 fd 字段返回。

epoll_event 结构体的 events 字段是表示感兴趣的事件和被触发的事件，可能的取值为：

{% highlight text %}
EPOLLIN ：对应的文件描述符可以读；
EPOLLOUT：对应的文件描述符可以写；
EPOLLPRI：对应的文件描述符有紧急的数据可读；
EPOLLERR：对应的文件描述符发生错误；
EPOLLHUP：对应的文件描述符被挂断；
EPOLLET ：对应的文件描述符有事件发生。
{% endhighlight %}

<!--
LT(level triggered)，默认的工作方式，同时支持 block 和 no-block socket。对于该模式，内核告诉你一个文件描述符是否就绪了，然后你可以对这个就绪的 fd 进行 IO 操作。如果你不作任何操作，或者还没有处理完，内核还会继续通知你的，所以，这种模式编程出错误可能性要小一点。传统的 select/poll 都是这种模型的代表。

ET(edge-triggered)，高速工作方式，只支持 no-block socket。在这种模式下，当描述符从未就绪变为就绪时，内核通过 epoll 告诉你，然后它会假设你知道文件描述符已经就绪，并且不会再为那个文件描述符发送更多的就绪通知，直到你做了某些操作导致那个文件描述符不再为就绪状态了，比如，你在发送、接收或者接收请求，或者发送接收的数据少于一定量时导致了一个 EWOULDBLOCK 错误。但是请注意，如果一直不对这个 fd 作 IO 操作（从而导致它再次变成未就绪），内核不会发送更多的通知(only once)，不过在TCP协议中，ET模式的加速效用仍需要更多的benchmark确认。

ET和LT的区别在于LT事件不会丢弃，而是只要读buffer里面有数据可以让用户读，则不断的通知你。而ET则只在事件发生之时通知。可以简单理解为LT是水平触发，而ET则为边缘触发。

ET模式仅当状态发生变化的时候才获得通知,这里所谓的状态的变化并不包括缓冲区中还有未处理的数据,也就是说,如果要采用ET模式,需要一直read/write直到出错为止,很多人反映为什么采用ET模式只接收了一部分数据就再也得不到通知了,大多因为这样;而LT模式是只要有数据没有处理就会一直通知下去的.
-->

### 内核实现

如下是与 epoll 相关的结构设计，先看下与结构体相关的结构图。

![io epoll structure]({{ site.url }}/images/linux/kernel/io-epoll-structure.png "io epoll structure"){: .pull-center width="80%" }

其中 `struct epitem` 是 epoll 的基本单元，对于每一个事件，都会建立一个 epitem 结构体，下面分别介绍一下几个主要的变量的含义：

{% highlight c %}
struct epitem{
    struct rb_node rbn;         // 红黑树节点
    struct list_head rdllink;   // 双向链表节点，当epitem对应fd已经ready时，会在ep_poll_callback()函数中将该
                                //   结点链接到eventpoll中的rdllist循环链表中去，这样就将ready的epitem都串连起来了
    struct epoll_filefd ffd;    // 事件句柄信息，包含了一个fd以及fd对应的file指针
    struct eventpoll *ep;       // 指向其所属的eventpoll对象，用于获取与epitem对应的eventpoll
    struct epoll_event event;   // 期待发生的事件类型，对应了epoll_ctl()中的指针，用于存储用户空间的epoll_event拷贝
}

int epoll_ctl(int epfd, int op, int fd, struct epoll_event *event);
{% endhighlight %}

当某一进程调用 `epoll_create()` 方法时，内核会创建一个 `eventpoll` 结构体，这个结构体中有两个成员与 epoll 的使用方式密切相关。

{% highlight c %}
struct eventpoll{
    struct rb_root  rbr;        // 红黑树的根节点，这颗树中存储着所有添加到epoll中的需要监控的事件
                                //   它的结点都为epitem变量，通过它可以很方便的增删改查epitem
    struct list_head rdlist;    // 双链表中则存放着将要通过epoll_wait返回给用户的满足条件的事件
                                //   链表中的每个结点即为epitem中的rdllink
};
{% endhighlight %}

每一个 epoll 对象都有一个独立的 `eventpoll` 结构体，用于存放通过 `epoll_ctl()` 方法向 epoll 对象中添加进来的事件，这些事件都会挂载在红黑树中。

而所有添加到 epoll 中的事件都会与设备+网卡驱动程序建立回调关系，也就是说，当相应的事件发生时会调用这个回调方法。这个回调方法在内核中就是 `ep_poll_callback()` 它会将发生的事件添加到 rdlist 双链表中。

<br>

接着再看下 `struct eppoll_entry` 结构体，它主要有这样几个变量：

{% highlight c %}
struct eppoll_entry {
    struct epitem *base;       // 指向其对应的epitem
    wait_queue_t wait;         // 等待队列的项，wait中有一个唤醒回调函数指针，该指针被初始化为ep_poll_callback
};
{% endhighlight %}

如上的 wait 成员会被挂在到设备的等待队列中，等待设备的唤醒，当设备因状态改变唤醒 wait 时，会执行 `ep_poll_callback`，而该函数会做这样一件事 `list_add_tail(&epi->rdllink,&ep->rdllist)` 。

<!--
，其中epi即为epitem变量，通过wait偏移拿到eppoll_entry，然后可以拿到base指针，即拿到了对应的epitem，而ep即为eventpoll变量，通过epitem的ep指针即可拿到，list_add_tail将epi的rdllink链到ep的rdllist中


下面结合这幅图大致讲解一下epoll_create、epoll_ctl、epoll_wait都在做些什么：

    首先，epoll_create会创建一个epoll的文件（epfd），同时创建并初始化一个struct eventpoll，其中file的private_data指针即指向了eventpoll变量，因此，知道epfd就可以拿到file，即拿到了eventpoll变量,这就是epoll_create所做的工作
    epoll_ctl又做了什么事呢？首先大家看到了eventpoll中的rb_root红黑树吗？epoll_ctl其实就是在操作这颗红黑树，epoll_ctl有三种操作：

    EPOLL_CTL_ADD：往红黑树中创建并添加一个epitem，对应处理函数为ep_insert
    在添加epitem时，也就是在ep_insert过程中，会创建一个eppoll_entry，并将其wait_queue挂载到设备的等待队列上，其中该wait_queue的唤醒回调函数为ep_poll_callback，当设备有事件ready而唤醒wait_queue时，就会执行ep_poll_callback将当前epitem链接到eventpoll中的rdllist中去，另外，如果在挂载wait_queue时就发现设备有事件ready了，同样会将epitem链接到rdllist中去。若每次调用poll函数,操作系统都要把current(当前进程)挂到fd对应的所有设备的等待队列上,可以想象,fd多到上千的时候,这样“挂”法很费事;而每次调用epoll_wait则没有这么罗嗦,epoll只在epoll_ctl时把current挂一遍(这第一遍是免不了的)并给每个fd一个命令“好了就调回调函数”,如果设备有事件了,通过回调函数,会把fd放入rdllist,而每次调用epoll_wait就只是收集rdllist里的fd就可以了——epoll巧妙的利用回调函数,实现了更高效的事件驱动模型

    EPOLL_CTL_MOD：修改对应的epitem，对应处理函数为ep_modify
    在ep_modify过程中，处理会修改epitem对应的event值，同样会先查看一下对应设备的当前状态，如果有ready事件发生，则会将当前epitem链接到rdllist中去
    EPOLL_CTL_DEL：从红黑树中删除对应的epitem，对应处理函数为ep_remove
    释放钩子、链接、资源空间等，如epitem所占的空间
    其实epoll_ctl已经将绝大部分事情都做了，epoll_wait有只需要收集结果就行了，它的目标也很单一，就看rdllist中是否有元素即可，当然，它还需要控制timeout，及结果转移，因为对于rdllist链接的epitem，只能说明其对应的fd有事件ready，但是哪些事件是不知道的，因此epoll_ctl再收集结果时，会亲自查看一下对应file的ready状态来写回events
-->

#### 初始化

epoll 的几个接口实际都对应于 kernel 的 API ，主要位于 `fs/eventpoll.c` 文件中。在分析 epoll 时发现有 `fs_initcall()` 这样的调用，以此为例分析一下 Linux 的初始化。

{% highlight c %}
fs_initcall(eventpoll_init);                                      // fs/eventpoll.c
#define fs_initcall(fn) __define_initcall(fn, 5)                  // include/linux/init.h
#define __define_initcall(fn, id) \                               // 同上
      static initcall_t __initcall_##fn##id __used \
      __attribute__((__section__(".initcall" #id ".init"))) = fn

// 最后展开为
static initcall_t __initcall_eventpoll_init5 __used
     __attribute__((__section__(".initcall5.init"))) = eventpoll_init;
{% endhighlight %}

也就是在 `.initcall5.init` 段中定义了一个变量 `__initcall_eventpoll_init5` 并将改变量赋值为 `eventpoll_init`，内核中对初始化的调用过程如下。

{% highlight text %}
arch/x86/kernel/head_64.S
 |-x86_64_start_kernel()                 arch/x86/kernel/head[64|32].c
   |-start_kernel()                      init/main.c
     |-rest_init()
       |-kernel_init()                   通过内核线程实现
         |-kernel_init_freeable()
           |-do_basic_setup()
             |-do_initcalls()
{% endhighlight %}

对于 epoll 来说，实际是在初始化过程中对变量进行初始化。

{% highlight c %}
static int __init eventpoll_init(void)
{
    struct sysinfo si;

    si_meminfo(&si);
    /*
     * Allows top 4% of lomem to be allocated for epoll watches (per user).
     */
    max_user_watches = (((si.totalram - si.totalhigh) / 25) << PAGE_SHIFT) /
        EP_ITEM_COST;
    BUG_ON(max_user_watches < 0);

    /*
     * Initialize the structure used to perform epoll file descriptor
     * inclusion loops checks.
     */
    ep_nested_calls_init(&poll_loop_ncalls);

    /* Initialize the structure used to perform safe poll wait head wake ups */
    ep_nested_calls_init(&poll_safewake_ncalls);

    /* Initialize the structure used to perform file's f_op->poll() calls */
    ep_nested_calls_init(&poll_readywalk_ncalls);

    /*
     * We can have many thousands of epitems, so prevent this from
     * using an extra cache line on 64-bit (and smaller) CPUs
     */
    BUILD_BUG_ON(sizeof(void *) <= 8 && sizeof(struct epitem) > 128);

    /* Allocates slab cache used to allocate "struct epitem" items */
    epi_cache = kmem_cache_create("eventpoll_epi", sizeof(struct epitem),
            0, SLAB_HWCACHE_ALIGN | SLAB_PANIC, NULL);

    /* Allocates slab cache used to allocate "struct eppoll_entry" */
    pwq_cache = kmem_cache_create("eventpoll_pwq",
            sizeof(struct eppoll_entry), 0, SLAB_PANIC, NULL);

    return 0;
}
{% endhighlight %}

主要是进行一些初始化配置，同时创建了 2 个内核 cache 用于存放 epitem 和 epoll_entry 。

#### 创建对象

通过 `epoll_create()` 创建一个 epoll 实例，同时创建并初始化一个 `struct eventpoll`，其中返回值 epfd 所对应的 file 的 `private_data` 指针即指向了 `eventpoll` 变量，因此，知道 epfd 就可以拿到 file，即拿到了 `eventpoll` 变量。

{% highlight c %}
SYSCALL_DEFINE1(epoll_create, int, size)  // epoll_create函数带一个整型参数
{
	if (size <= 0)
		return -EINVAL;

	return sys_epoll_create1(0);          // 实际上是调用epoll_create1
}
/* Open an eventpoll file descriptor.  */
SYSCALL_DEFINE1(epoll_create1, int, flags)
{
    int error;
    struct eventpoll *ep = NULL;
    
    /* Check the EPOLL_* constant for consistency.  */
    BUILD_BUG_ON(EPOLL_CLOEXEC != O_CLOEXEC);
    
    if (flags & ~EPOLL_CLOEXEC)
    	return -EINVAL;
    /* Create the internal data structure ("struct eventpoll").  */
    error = ep_alloc(&ep);   // 分配eventpoll结构体
    if (error < 0)
    	return error;
    /*
     * Creates all the items needed to setup an eventpoll file. That is,
     * a file structure and a free file descriptor.
     */
    // 创建与eventpoll结构体相对应的file结构，ep保存在file->private_data结构中，其中
    // eventpoll_fops 为该文件所对应的操作函数
    error = anon_inode_getfd("[eventpoll]", &eventpoll_fops, ep, O_RDWR | (flags & O_CLOEXEC));
    if (error < 0)
    	ep_free(ep);        // 如果出错则释放该eventpoll结构体
    
    return error;
}
{% endhighlight %}

`anon_inode_getfd()` 创建与 `struct eventpoll` 对应的 file 结构，其中 ep 保存在 `file->private_data` 结构中，同时为该新文件定义操作函数。

从这几行代码可以看出，`epoll_create()` 主要做了两件事：

* 创建并初始化一个 `struct eventpoll` 变量；
* 创建 epoll 的 file 结构，并指定 file 的 `private_data` 指针指向刚创建的 `eventpoll` 变量，这样，只要根据 `epoll` 文件描述符 epfd 就可以拿到 file 进而就拿到了 `eventpoll` 变量，该 `eventpoll` 就是 `epoll_ctl()` 和 `epoll_wait()` 工作的场所。

对外看来，`epoll_create()` 就做了一件事，那就是创建一个 epoll 文件，事实上，更关键的是，它创建了一个 `struct eventpoll` 变量，该变量为 `epoll_ctl()` 和 `epoll_wait()` 的工作打下了基础。

#### 添加监控

`epoll_ctl()` 主要是针对 epfd 所对应的 epoll 实例进行增、删、改操作，一个新创建的 epoll 文件带有一个 `struct eventpoll` ，同时该结构体上再挂一个红黑树，红黑树上的每个节点挂的是 `struct epitem`，这个红黑树就是每次 `epoll_ctl()` 时 fd 存放的地方。

{% highlight c %}
/*
 * epfd为该epoll套接字实例,op表示对应的操作，fd表示新加入的套接字，
 * 结构体epoll_event 用于注册fd所感兴趣的事件和回传在fd上所发生待处理的事件
 */
SYSCALL_DEFINE4(epoll_ctl, int, epfd, int, op, int, fd,
		struct epoll_event __user *, event)
{
    int error;
    int did_lock_epmutex = 0;
    struct file *file, *tfile;
    struct eventpoll *ep;
    struct epitem *epi;
    struct epoll_event epds;
    
    error = -EFAULT;
    // 将用户传入的event_poll拷贝到epds中
    if (ep_op_has_event(op) && copy_from_user(&epds, event, sizeof(struct epoll_event)))
        goto error_return;

    /* Get the "struct file *" for the eventpoll file */
    error = -EBADF;
    file = fget(epfd); // 获取该epoll套接字实例所对应的文件描述符
    if (!file)
        goto error_return;

    /* Get the "struct file *" for the target file */
    tfile = fget(fd);
    if (!tfile)
        goto error_fput;

    /* The target file descriptor must support poll */
    error = -EPERM;
    if (!tfile->f_op || !tfile->f_op->poll)
        goto error_tgt_fput;

    /*
     * We have to check that the file structure underneath the file descriptor
     * the user passed to us _is_ an eventpoll file. And also we do not permit
     * adding an epoll file descriptor inside itself.
     */
    error = -EINVAL;
    if (file == tfile || !is_file_epoll(file))
        goto error_tgt_fput;

    /*
     * At this point it is safe to assume that the "private_data" contains
     * our own data structure.
     */
    ep = file->private_data; // 获取epoll实例所对应的eventpoll结构体

    /*
     * When we insert an epoll file descriptor, inside another epoll file
     * descriptor, there is the change of creating closed loops, which are
     * better be handled here, than in more critical paths.
     *
     * We hold epmutex across the loop check and the insert in this case, in
     * order to prevent two separate inserts from racing and each doing the
     * insert "at the same time" such that ep_loop_check passes on both
     * before either one does the insert, thereby creating a cycle.
     */
    if (unlikely(is_file_epoll(tfile) && op == EPOLL_CTL_ADD)) {
        mutex_lock(&epmutex);
        did_lock_epmutex = 1;
        error = -ELOOP;
        if (ep_loop_check(ep, tfile) != 0)
            goto error_tgt_fput;
    }

    mutex_lock(&ep->mtx);

    /*
     * Try to lookup the file inside our RB tree, Since we grabbed "mtx"
     * above, we can be sure to be able to use the item looked up by
     * ep_find() till we release the mutex.
     * ep_find即从ep中的红黑树中根据tfile和fd来查找epitem
     */
    epi = ep_find(ep, tfile, fd);

    error = -EINVAL;
    switch (op) {
    case EPOLL_CTL_ADD: // 对应于socket上事件注册
        if (!epi) {     // 红黑树中不存在这个节点
            // 确保"出错、连接挂起"被当做事件，将出错信息返回给应用
            epds.events |= POLLERR | POLLHUP;
            error = ep_insert(ep, &epds, tfile, fd);
        } else
            error = -EEXIST;
        break;
    case EPOLL_CTL_DEL: // 删除
        if (epi)        // 存在则删除这个节点，不存在则报错
            error = ep_remove(ep, epi);
        else
            error = -ENOENT;
        break;
    case EPOLL_CTL_MOD: // 修改
        if (epi) {      // 存在则修改该fd所对应的事件，不存在则报错
            epds.events |= POLLERR | POLLHUP;
            error = ep_modify(ep, epi, &epds);
        } else
            error = -ENOENT;
        break;
    }
    mutex_unlock(&ep->mtx);
    
error_tgt_fput:
    if (unlikely(did_lock_epmutex))
    mutex_unlock(&epmutex);
    fput(tfile);
error_fput:
    fput(file);
error_return:
    
    return error;
}
{% endhighlight %}

对于往 epoll 实例中添加新的套接字，其实现主要通过 `ep_insert()` 完成，先分析 `epoll_wait` 再回过头来分析 `ep_insert()` 。

#### 等待事件

`epoll_wait()` 用来等待 epoll 文件上的 IO 事件发生，其代码如下：

{% highlight c %}
/*
 * Implement the event wait interface for the eventpoll file. It is the kernel
 * part of the user space epoll_wait(2).
 */
SYSCALL_DEFINE4(epoll_wait, int, epfd, struct epoll_event __user *, events,
        int, maxevents, int, timeout)
{
    int error;
    struct file *file;
    struct eventpoll *ep;

    /* The maximum number of event must be greater than zero */
    if (maxevents <= 0 || maxevents > EP_MAX_EVENTS)
        return -EINVAL;

    /* Verify that the area passed by the user is writeable */
    if (!access_ok(VERIFY_WRITE, events, maxevents * sizeof(struct epoll_event))) {
        error = -EFAULT;
        goto error_return;
    }

    /* Get the "struct file *" for the eventpoll file */
    error = -EBADF;
    file = fget(epfd);
    if (!file)
        goto error_return;

    /*
     * We have to check that the file structure underneath the fd
     * the user passed to us _is_ an eventpoll file.
     */
    error = -EINVAL;
    if (!is_file_epoll(file))
        goto error_fput;

    /*
     * At this point it is safe to assume that the "private_data" contains
     * our own data structure.
     */
    ep = file->private_data;  // 获取struct eventpoll结构

    /* Time to fish for events ... */
    error = ep_poll(ep, events, maxevents, timeout); // 核心代码

error_fput:
    fput(file);
error_return:

    return error;
}
{% endhighlight %}

可以看出该函数主要时通过 epfd 获取对应的 `struct eventpoll` 结构，然后调用 `ep_poll()` 函数，下面来看 `ep_poll()` 的实现。


{% highlight c %}
/**
 * ep_poll - Retrieves ready events, and delivers them to the caller supplied
 *           event buffer.
 *
 * @ep: Pointer to the eventpoll context.
 * @events: Pointer to the userspace buffer where the ready events should be
 *          stored.
 * @maxevents: Size (in terms of number of events) of the caller event buffer.
 * @timeout: Maximum timeout for the ready events fetch operation, in
 *           milliseconds. If the @timeout is zero, the function will not block,
 *           while if the @timeout is less than zero, the function will block
 *           until at least one event has been retrieved (or an error
 *           occurred).
 *
 * Returns: Returns the number of ready events which have been fetched, or an
 *          error code, in case of error.
 */
static int ep_poll(struct eventpoll *ep, struct epoll_event __user *events,
           int maxevents, long timeout)
{
    int res = 0, eavail, timed_out = 0;
    unsigned long flags;
    long slack = 0;
    wait_queue_t wait;
    ktime_t expires, *to = NULL;

    /* The call waits for a maximum time of timeout milliseconds.
     * Specifying a timeout of -1 makes epoll_wait() wait indefinitely,
     * while specifying a timeout equal to zero makes epoll_wait()
     * to return immediately even if no events are available (return
     * code equal to zero).
     */
    if (timeout > 0) {
        struct timespec end_time = ep_set_mstimeout(timeout);

        slack = select_estimate_accuracy(&end_time);
        to = &expires;
        *to = timespec_to_ktime(end_time);
    } else if (timeout == 0) {
        /*
         * Avoid the unnecessary trip to the wait queue loop, if the
         * caller specified a non blocking operation.
         */
        timed_out = 1;
        spin_lock_irqsave(&ep->lock, flags);
        goto check_events;
    }

fetch_events:
    spin_lock_irqsave(&ep->lock, flags);
    // 如果rdllist中还没有epitem时，就开始等待了
    if (!ep_events_available(ep)) {
        /*
         * We don't have any available event to return to the caller.
         * We need to sleep here, and we will be wake up by
         * ep_poll_callback() when events will become available.
         */
        // 初始化等待队列，等待队列项对应的线程即为当前线程
        init_waitqueue_entry(&wait, current);
        // 先将当前线程挂到等待队列上，之后在调用schedule_timeout时，就开始了超时等待了
        __add_wait_queue_exclusive(&ep->wq, &wait);

        for (;;) {
            /*
             * We don't want to sleep if the ep_poll_callback() sends us
             * a wakeup in between. That's why we set the task state
             * to TASK_INTERRUPTIBLE before doing the checks.
             */
             // 因为会被阻塞，这里先设置线程状态为可中断
            set_current_state(TASK_INTERRUPTIBLE);
            // 整个循环的核心，其实就在看rdllist中是否有数据，或者等待超时
            // 应征了前面的说明，epoll_wait只需要等着收集数据即可
            if (ep_events_available(ep) || timed_out)
                break;
            if (signal_pending(current)) {
                res = -EINTR;
                break;
            }

            spin_unlock_irqrestore(&ep->lock, flags);
            if (!schedule_hrtimeout_range(to, slack, HRTIMER_MODE_ABS))
                timed_out = 1;

            spin_lock_irqsave(&ep->lock, flags);
        }
        __remove_wait_queue(&ep->wq, &wait);

        set_current_state(TASK_RUNNING);
    }
check_events:
    /* Is it worth to try to dig for events ? */
    eavail = ep_events_available(ep);

    spin_unlock_irqrestore(&ep->lock, flags);

    /*
     * Try to transfer events to user space. In case we get 0 events and
     * there's still timeout left over, we go trying again in search of
     * more luck.
     */
    if (!res && eavail &&
        !(res = ep_send_events(ep, events, maxevents)) && !timed_out)
        goto fetch_events;

    return res;
}
{% endhighlight %}

<!--
29-43行：主要是超时时间的处理，若超时时间为0，则直接检查有没有准备好的I/O事件，有则立即发送给用户空间去处理；若超时时间大于0，计算好精确的超时时间后，等待事件的发生，45-86行等待指定的时间直到有I/O事件出现；

54-58行：如果还没有I/O事件出现，则准备休眠。先初始化等待队列，把当前线程挂在该队列上，同时把这个队列挂在eventpoll结构的wq上，

60-82行：在指定的超时时间内循环检测有没有I/O事件发生，有事件发生、超时或者收到信号都会跳出循环。

83行：运行到此处有I/O事件发生，不用再等待，则移除该队列
-->

调用 `epoll_wait()` 检查是否有事件发生时，只需检查 `eventpoll` 中 `rdlist` 链表是否有 `epitem` 元素即可。如果 rdlist 不为空，则把发生的事件复制到用户态，同时将事件数量返回给用户。

![epoll data structure]({{ site.url }}/images/linux/kernel/epoll-data-structure.jpg "epoll data structure"){: .pull-center width="60%" }

## 参考

<!--
Linux设备驱动程序——高级字符驱动程序操作(poll机制)
http://blog.chinaunix.net/uid-29339876-id-4070572.html
-->

{% highlight text %}
{% endhighlight %}
