# libevent 專案分析


我理解libevent是一個輕量級的，跨平臺+高效的（C語言實現）事件驅動庫，類似於ACE項目中的ACE_Reactor，它實現了網絡通訊套接口I/O事件，定時器事件，信號事件的監聽和事件處理函數回調機制。從項目主頁可以瞭解到libevent已經支持   /dev/poll ,  kqueue(2) ,  event ports ,  POSIX  select(2) ,  Windows  select() ,  poll(2) , and  epoll(4) 方式的事件監測和驅動機制 

項目主頁： http://libevent.org/ <br>
維基百科： http://zh.wikipedia.org/wiki/Libevent <br>
參考資料： http://blog.csdn.net/sparkliang/article/details/4957667 <br>

PS：在分析開源項目代碼之前，需首先了解該項目的特性，應用場景和價值，這些信息一方面可以從項目的主頁獲取，另一方面可以通過搜索引擎從技術論壇，博客等方面獲取。最好選擇和自己工作/興趣比較相關的項目，這樣有利於分析的深入和堅持，並及時體現收益。 


###下載源代碼 
從項目主頁可以很方便的下載當前版本的源碼，我下載的版本是 libevent-2.0.17-stable.tar.gz 


###代碼量分析 
通過Wine運行SourceCounter工具對該項目進行代碼量統計，可以看到該項目代碼量大概5W多行，且代碼工程結構簡單，比較適合像我這樣對開源項目代碼分析經驗不足的人 
PS：在開始分析項目源碼之前，分析該項目的代碼量可以大致評估該項目的難度和分析計劃，分析工程結構可以大致評估該項目的重點部分，以免一開始就滿腔熱血地栽在一個深坑裡（比較複雜的開源項目），而後面又不了了之 


###編譯和安裝 
在將源碼包在本地解壓後即可以編譯和安裝。這裡和其他開源項目差不多，沒有什麼特別的，只是為了方便後面通過調試的手段來分析源碼，編譯的時候最好編譯成debug模式，如下 


```sh
#./configure --enable-debug-mode --enable-static --enable-thread-support 
#make 
#make install 
```

安裝完成後，libevent庫的頭文件會安裝在/usr/local/include目錄下，而庫文件會安裝在/usr/local/lib目錄下，故需確保/usr/local/lib在LD_LIBRARY_PATH變量包含的路徑中 
PS：卸載的方法 

```sh
#make uninstall 
#make clean 
```

編寫測試應用代碼 
該項目源碼包中的sample目錄中其實已經有些例子，但我還是情願參考樣例自己寫一個，好久沒Coding了 ：） 

mytimer.c : 實現一個定時器事件處理函數，並通過libevent的事件驅動機制定時調用 

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

編譯 ：

```sh
gcc -g -I/usr/local/include -o mytimer mytimer.c -L/usr/local/lib -levent 
```


運行 ： $ ./mytimer  

```sh
handle_timer function is called  
handle_timer function is called  
handle_timer function is called  
^C 
```


通過例程調試libevent 

通過gdb去調試mytimer時發現其鏈接的是libevent的動態庫，且無法在libevent庫的函數上設置斷點 ：（ 
安裝glibc的靜態庫：# yum install glibc-static libstdc++-static 
靜態編譯命令：

```sh
gcc -g -I/usr/local/include -o mytimer mytimer.c -L/usr/local/lib -static -levent -lc -lrt
```
這樣就可以通過gdb調試例程時，在libevent庫的函數上設置斷點 

###初始化 
libevent庫的使用是從對event_base結構初始化開始的，前面例程中使用的方法是不帶任何參數的 event_base_new 函數，類似的還有 event_init 函數。前者創建並初始化了一個默認的event_config結構，然後調用event_base_new_with_config函數；而後者更加簡單，用了一個NULL做為參數調用event_base_new_with_config函數。所以可以理解libevent庫的初始化核心是 event_base_new_with_config 函數，對該函數的分析留給後面，這次不打算分析到具體函數的實現 ：） 



###事件定義 
libevent庫實現的就是事件監聽和驅動機制，該庫中對事件的定義是event結構。該結構不是很複雜，但比較重要，這裡列出幾個比較重要的成員變量 


1，雙向事件鏈表 

```c
TAILQ_ENTRY(event) ev_active_next; 
TAILQ_ENTRY(event) ev_next; 
```

其中

TAILQ_ENTRY宏在event2/event_struct.h文件中定義如下 

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

2，事件對應的句柄 

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

信號事件句柄 
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

1，IO事件句柄與信號事件句柄通過union方式定義；<br>
2，定時器事件是靠時間差比較來監聽，所以這裡沒有定時器事件的句柄 <br>
3，event_base指針： 通過該成員變量，event事件可以知道自己註冊在哪個event_base 
```c
struct event_base *ev_base; 
```
4，event屬性 ： 
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

5，event標識： 
```c
short ev_flags; 
#define EVLIST_TIMEOUT 0x01 
#define EVLIST_INSERTED 0x02 
#define EVLIST_SIGNAL 0x04 
#define EVLIST_ACTIVE 0x08 
#define EVLIST_INTERNAL 0x10 
#define EVLIST_INIT 0x80 
```
6，事件優先級：數值越小，優先級越高，默認為適中 

```c
ev_uint8_t ev_pri; 
```


7，事件對應的回調函數指針及參數

```c
/* allows us to adopt for different types of events */ 
void (*ev_callback)(evutil_socket_t, short, void *arg); 
void *ev_arg; 
```

###事件初始化 

和event相關的函數有下面幾個： 

```c
event創建： event_new 
event設置： event_set ， event_assign 
event釋放: event_free 
其中最核心的是event_assign函數，這裡分析 其參數的意義，說明事件關注的方面 

int event_assign(struct event *ev, struct event_base *base, evutil_socket_t fd, short events, void (*callback)(evutil_socket_t, short, void *), void *arg) 

struct event *ev  ：event事件指針 
struct event_base *base  ： event_base指針 
evutil_socket_t fd  ： 套接口句柄，其他類型事件可以賦值-1 
short events  ： 事件的屬性 
void (*callback)(evutil_socket_t, short, void *)  ： 事件處理回調函數指針 
void *arg  ： 事件處理回調函數參數 
```

###註冊事件 

例程中通過event_add函數註冊一個定時事件，實際上註冊事件是由event_add_internal函數實現，event_add函數會為調用event_add_internal函數增加鎖保護。event_add_internal函數通過事件的ev_events屬性判斷事件類型，從而調用不同函數實現事件註冊 
對於IO事件的註冊由evmap_io_add函數實現 

```c
int evmap_io_add(struct event_base *base, evutil_socket_t fd, struct event *ev) 
```

PS ：相關的其他幾個和IO事件操作函數evmap_io_init，evmap_io_del，evmap_io_active 


對於Signal事件的註冊由evmap_signal_add函數實現 

```c
int evmap_signal_add(struct event_base *base, int sig, struct event *ev) 
```

PS ： 相關的其他幾個和Signal事件操作函數 evmap_signal_init， evmap_signal_del， evmap_signal_active 



對於定時器事件的註冊由 event_add_internal參數中的const struct timeval *tv參數決定？？？ 


###事件驅動

例程通過調用event_base_dispatch函數驅動事件，實際上事件驅動機制是由event_base_loop函數封裝實現的。這裡需注意event_base結構的成員變量const struct eventop *evsel，這個指針指向 eventops數組中的一個元素，根據宏定義選擇不同的事件監聽機制（例如poll/select/epoll等），這裡先不細說。 

事件回調 
在源代碼中一時還沒發現是在哪裡回調了事件處理函數，換一種途徑，通過gdb在例程的時間事件處理函數設置斷點跟蹤，可以發現事件處理函數是由 event_process_active_single_queue 函數調用的，關係如下： 

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

