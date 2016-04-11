# libevent 專案分析


我理解libevent是一个轻量级的，跨平台+高效的（C语言实现）事件驱动库，类似于ACE项目中的ACE_Reactor，它实现了网络通讯套接口I/O事件，定时器事件，信号事件的监听和事件处理函数回调机制。从项目主页可以了解到libevent已经支持   /dev/poll ,  kqueue(2) ,  event ports ,  POSIX  select(2) ,  Windows  select() ,  poll(2) , and  epoll(4) 方式的事件监测和驱动机制 

项目主页： http://libevent.org/ <br>
维基百科： http://zh.wikipedia.org/wiki/Libevent <br>
参考资料： http://blog.csdn.net/sparkliang/article/details/4957667 <br>

PS：在分析开源项目代码之前，需首先了解该项目的特性，应用场景和价值，这些信息一方面可以从项目的主页获取，另一方面可以通过搜索引擎从技术论坛，博客等方面获取。最好选择和自己工作/兴趣比较相关的项目，这样有利于分析的深入和坚持，并及时体现收益。 


###下载源代码 
从项目主页可以很方便的下载当前版本的源码，我下载的版本是 libevent-2.0.17-stable.tar.gz 


###代码量分析 
通过Wine运行SourceCounter工具对该项目进行代码量统计，可以看到该项目代码量大概5W多行，且代码工程结构简单，比较适合像我这样对开源项目代码分析经验不足的人 
PS：在开始分析项目源码之前，分析该项目的代码量可以大致评估该项目的难度和分析计划，分析工程结构可以大致评估该项目的重点部分，以免一开始就满腔热血地栽在一个深坑里（比较复杂的开源项目），而后面又不了了之 


###编译和安装 
在将源码包在本地解压后即可以编译和安装。这里和其他开源项目差不多，没有什么特别的，只是为了方便后面通过调试的手段来分析源码，编译的时候最好编译成debug模式，如下 


```sh
#./configure --enable-debug-mode --enable-static --enable-thread-support 
#make 
#make install 
```

安装完成后，libevent库的头文件会安装在/usr/local/include目录下，而库文件会安装在/usr/local/lib目录下，故需确保/usr/local/lib在LD_LIBRARY_PATH变量包含的路径中 
PS：卸载的方法 

```sh
#make uninstall 
#make clean 
```

编写测试应用代码 
该项目源码包中的sample目录中其实已经有些例子，但我还是情愿参考样例自己写一个，好久没Coding了 ：） 

mytimer.c : 实现一个定时器事件处理函数，并通过libevent的事件驱动机制定时调用 

```c
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>

#include <event2/event-config.h>
#include <event2/event.h>
#include <event2/event_struct.h>
#include <event2/util.h>


static void handle_timer(evutil_socket_t fd, short event, void* arg)
{
    printf("handle_timer function is called \n");
    fflush(stdout);
}

int main(int argc, char** argv)
{
    /* Initalize the event library */
    struct event_base* base = event_base_new();

    if (NULL == base) {
        return -1;
    }

    /* Initalize one timeout event */
    struct event timeout = {0};
    event_assign(&timeout, base, -1, EV_PERSIST, handle_timer, (void*)&timeout);

    /* Register the event */
    struct timeval tv;
    evutil_timerclear(&tv);
    tv.tv_sec = 2;
    event_add(&timeout, &tv);

    /*event dispatch*/
    event_base_dispatch(base);

    event_base_free(base);

    return 0;
}
```

编译 ：

```sh
gcc -g -I/usr/local/include -o mytimer mytimer.c -L/usr/local/lib -levent 
```


运行 ： $ ./mytimer  

```sh
handle_timer function is called  
handle_timer function is called  
handle_timer function is called  
^C 
```


通过例程调试libevent 

通过gdb去调试mytimer时发现其链接的是libevent的动态库，且无法在libevent库的函数上设置断点 ：（ 
安装glibc的静态库：# yum install glibc-static libstdc++-static 
静态编译命令：

```sh
gcc -g -I/usr/local/include -o mytimer mytimer.c -L/usr/local/lib -static -levent -lc -lrt
```
这样就可以通过gdb调试例程时，在libevent库的函数上设置断点 

###初始化 
libevent库的使用是从对event_base结构初始化开始的，前面例程中使用的方法是不带任何参数的 event_base_new 函数，类似的还有 event_init 函数。前者创建并初始化了一个默认的event_config结构，然后调用event_base_new_with_config函数；而后者更加简单，用了一个NULL做为参数调用event_base_new_with_config函数。所以可以理解libevent库的初始化核心是 event_base_new_with_config 函数，对该函数的分析留给后面，这次不打算分析到具体函数的实现 ：） 



###事件定义 
libevent库实现的就是事件监听和驱动机制，该库中对事件的定义是event结构。该结构不是很复杂，但比较重要，这里列出几个比较重要的成员变量 


1，双向事件链表 

```c
TAILQ_ENTRY(event) ev_active_next; 
TAILQ_ENTRY(event) ev_next; 
```

其中

TAILQ_ENTRY宏在event2/event_struct.h文件中定义如下 

```c
/* Fix so that people don't have to run with <sys/queue.h> */ 
#ifndef TAILQ_ENTRY
#define _EVENT_DEFINED_TQENTRY
#define TAILQ_ENTRY(type) \
    struct { \
        struct type *tqe_next; /* next element */ \
        struct type **tqe_prev; /* address of previous next element */ \
    }
#endif /* !TAILQ_ENTRY */
```

2，事件对应的句柄 

SOCKET事件句柄   
```c
evutil_socket_t ev_fd; 
```

IO事件句柄 
```c
/* used for io events */
struct {
    TAILQ_ENTRY(event) ev_io_next;
    struct timeval ev_timeout;
} ev_io;
````

信号事件句柄 
```c
/* used by signal events */
struct {
    TAILQ_ENTRY(event) ev_signal_next;
    short ev_ncalls;
    /* Allows deletes in callback */
    short* ev_pncalls;
} ev_signal;
```

PS：<br>

1，IO事件句柄与信号事件句柄通过union方式定义；<br>
2，定时器事件是靠时间差比较来监听，所以这里没有定时器事件的句柄 <br>
3，event_base指针： 通过该成员变量，event事件可以知道自己注册在哪个event_base 
```c
struct event_base *ev_base; 
```
4，event属性 ： 
```c
short ev_events;
/**
 * @name event flags
 *
 * Flags to pass to event_new(), event_assign(), event_pending(), and
 * anything else with an argument of the form "short events"
 */
/**@{*/
/** Indicates that a timeout has occurred.  It's not necessary to pass
 * this flag to event_for new()/event_assign() to get a timeout. */
#define EV_TIMEOUT 0x01
/** Wait for a socket or FD to become readable */
#define EV_READ 0x02
/** Wait for a socket or FD to become writeable */
#define EV_WRITE 0x04
/** Wait for a POSIX signal to be raised*/
#define EV_SIGNAL 0x08
/**
 * Persistent event: won't get removed automatically when activated.
 *
 * When a persistent event with a timeout becomes activated, its timeout
 * is reset to 0.
 */
#define EV_PERSIST 0x10
/** Select edge-triggered behavior, if supported by the backend. */
#define EV_ET       0x20
/**@}*/
```

5，event标识： 
```c
short ev_flags; 
#define EVLIST_TIMEOUT 0x01 
#define EVLIST_INSERTED 0x02 
#define EVLIST_SIGNAL 0x04 
#define EVLIST_ACTIVE 0x08 
#define EVLIST_INTERNAL 0x10 
#define EVLIST_INIT 0x80 
```
6，事件优先级：数值越小，优先级越高，默认为适中 

```c
ev_uint8_t ev_pri; 
```


7，事件对应的回调函数指针及参数

```c
/* allows us to adopt for different types of events */ 
void (*ev_callback)(evutil_socket_t, short, void *arg); 
void *ev_arg; 
```

###事件初始化 

和event相关的函数有下面几个： 

```c
event创建： event_new 
event设置： event_set ， event_assign 
event释放: event_free 
其中最核心的是event_assign函数，这里分析 其参数的意义，说明事件关注的方面 

int event_assign(struct event *ev, struct event_base *base, evutil_socket_t fd, short events, void (*callback)(evutil_socket_t, short, void *), void *arg) 

struct event *ev  ：event事件指针 
struct event_base *base  ： event_base指针 
evutil_socket_t fd  ： 套接口句柄，其他类型事件可以赋值-1 
short events  ： 事件的属性 
void (*callback)(evutil_socket_t, short, void *)  ： 事件处理回调函数指针 
void *arg  ： 事件处理回调函数参数 
```

###注册事件 

例程中通过event_add函数注册一个定时事件，实际上注册事件是由event_add_internal函数实现，event_add函数会为调用event_add_internal函数增加锁保护。event_add_internal函数通过事件的ev_events属性判断事件类型，从而调用不同函数实现事件注册 
对于IO事件的注册由evmap_io_add函数实现 

```c
int evmap_io_add(struct event_base *base, evutil_socket_t fd, struct event *ev) 
```

PS ：相关的其他几个和IO事件操作函数evmap_io_init，evmap_io_del，evmap_io_active 


对于Signal事件的注册由evmap_signal_add函数实现 

```c
int evmap_signal_add(struct event_base *base, int sig, struct event *ev) 
```

PS ： 相关的其他几个和Signal事件操作函数 evmap_signal_init， evmap_signal_del， evmap_signal_active 



对于定时器事件的注册由 event_add_internal参数中的const struct timeval *tv参数决定？？？ 


###事件驱动

例程通过调用event_base_dispatch函数驱动事件，实际上事件驱动机制是由event_base_loop函数封装实现的。这里需注意event_base结构的成员变量const struct eventop *evsel，这个指针指向 eventops数组中的一个元素，根据宏定义选择不同的事件监听机制（例如poll/select/epoll等），这里先不细说。 

事件回调 
在源代码中一时还没发现是在哪里回调了事件处理函数，换一种途径，通过gdb在例程的时间事件处理函数设置断点跟踪，可以发现事件处理函数是由 event_process_active_single_queue 函数调用的，关系如下： 

```sh
(gdb) where 
#0  handle_timer (fd=-1, event=1, arg=0xbffff054) at mytimer.c:19 
#1  0x0011e248 in event_process_active_single_queue (base=0x804a008, flags=0) 
    at event.c:1340 
#2   event_process_active (base=0x804a008, flags=0) at event.c:1407 
#3  event_base_loop (base=0x804a008, flags=0) at event.c:1604 
#4  0x0011f255 in event_base_dispatch (event_base=0x804a008) at event.c:1435 
#5  0x080486c4 in main (argc=1, argv=0xbffff164) at mytimer.c:43 
```

