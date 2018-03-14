---
title: Linux IO 多路複用
layout: post
comments: true
language: chinese
category: [linux,program]
keywords: linux,io,multiplexing,多路複用
description: 通過 IO 多路複用技術，系統內核緩衝 IO 數據，當某個 IO 準備好後，系統通知應用程序該 IO 可讀或可寫，這樣應用程序可以馬上完成相應的 IO 操作，而不需要等待系統完成相應 IO 操作，從而應用程序不必因等待 IO 操作而阻塞。這裡簡單介紹下 Linux 中 IO 多路複用的使用。
---

通過 IO 多路複用技術，系統內核緩衝 IO 數據，當某個 IO 準備好後，系統通知應用程序該 IO 可讀或可寫，這樣應用程序可以馬上完成相應的 IO 操作，而不需要等待系統完成相應 IO 操作，從而應用程序不必因等待 IO 操作而阻塞。

這裡簡單介紹下 Linux 中 IO 多路複用的使用。

<!-- more -->

## 簡介

在 Linux 中，會將很多資源通過文件描述符表示，包括了文件系統、網絡、物理設備等等。

文件描述符就是一個整數，默認會選擇最小未使用的數值，其中有三個比較固定，比較特殊的值，0 是標準輸入，1 是標準輸出，2 是標準錯誤輸出。

0、1、2 是整數表示的，對應的 FILE* 結構的表示就是 stdin、stdout、stderr。

## select

select 可以用來監視多個文件句柄的狀態變化，程序會阻塞在 select 直到被監視的文件句柄有一個或多個發生了狀態改變，然後通知應用程序，應用程序 <font color='blue'>輪詢</font> 所有的 FD 集合，判斷監控的 FD 是否有事件發生，並作相應的處理。

select 相關的聲明和宏，通過宏來設置參數，相應的執行流程如下。

{% highlight text %}
int select(int maxfd, fd_set * readfds, fd_set * writefds, fd_set * exceptfds, struct timeval * timeout);
FD_CLR(inr fd, fd_set* set);                               // 清除描述詞組set中相關fd的位
FD_ISSET(int fd, fd_set *set);                             // 用來測試描述詞組set中相關fd的位是否為真
FD_SET(int fd, fd_set*set);                                // 用來設置描述詞組set中相關fd的位
FD_ZERO(fd_set *set);                                      // 用來清除描述詞組set的全部位
struct timeval {
   time_t tv_sec;
   time_t tv_usec;
};

----- 調用流程
fd_set readfso;                                            // 1.1 定義需要監視的描述符
FD_ZERO(&readfso);                                         // 1.2 清空
FD_SET(fd, &readfso);                                      // 1.3 設置需要監控的描述符，如fd=1
while(1) {
    readfs = readfso;
    ret = select(maxfd+1, &readfds, NULL, NULL, timeout);  // 2.1 監聽所關注的事件，在此為監聽讀事件
    if (ret > 0)                                           // 3.1.1 監聽事件發生
        for (i = 0; i > FD_SETSIZE; i++)                   // 3.1.2 需要遍歷所有fd
            if (FD_ISSET(fd, &readfds))
                handleEvent();
    else if (ret == 0)                                     // 3.2 超時
        handle timeout
    else if (ret < 0)                                      // 3.3 發生錯誤
        handle error
}
{% endhighlight %}

在 select 函數中，各個參數的含義如下。
* maxfd 為最大文件描述符+1，因此在程序中需要知道最大描述符的值。
* 接著的三個參數分別設置所監聽的讀、寫、異常事件對應的描述符。該值傳入需要監控事件的描述符，返回值保存了發生了的事件對應的描述符。
* 設置超時時間，NULL 表示一直等待。

返回值：成功則返回文件描述符狀態已改變的總數；如果返回 0 代表在描述詞狀態改變前已超過 timeout 時間；當有錯誤發生時則返回 -1，錯誤原因存於 errno ，此時參數 readfds, writefds, exceptfds 和 timeout 的值變成不可預測。

在有返回值時，需要通過 FD_ISSET 循環遍歷各個描述符，從而文件描述符越多，效率也就越低。而且，每次調用 select 時都需要重新設置需要監控的文件描述符。

fd_set 在 ```sys/select.h``` 中定義，大致可以簡化為如下的結構，也就是實際用數組表示，其中每一位代表一個文件描述符，最大可以表示 1024 個，也就是說 select 能監視的描述符最大為 1024 。最大可以監控的文件描述符數可以通過 FD_SETSIZE 獲得，描述符需要通過對應的宏來配置。

{% highlight c %}
typedef struct {
    long int  __fds_bits[__FD_SETSIZE(1024) / __NFDBITS(8 * sizeof(long int))];
} fd_set;
{% endhighlight %}

## poll

和 select() 不一樣，poll() 沒有使用低效的三個基於位的文件描述符 set ，而是採用了一個單獨的結構體 pollfd 數組，由 fds 指針指向這個數組，採用鏈表保存文件描述符。

{% highlight text %}
struct pollfd {
    int fd;              /* 文件描述符 */
    short events;        /* 等待的事件 */
    short revents;       /* 實際發生了的事件 */
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

fds 表示需要監控的文件描述符；nfds 表示 fds 的大小，也即需要監控的描述符數量。如果 fd 為負，則忽略 events，revents 返回 0。

返回值與 select 的返回值含義相同。


## epoll

epoll 是 Linux 內核為處理大批量句柄而作了改進的 poll，是 Linux 下多路複用 IO 接口 select/poll 的增強版本，它能顯著減少程序在大量併發連接中只有少量活躍的情況下的系統 CPU 利用率。

### 對比

select 模型的缺點：

* 最大併發數限制，因為一個進程所打開的文件描述符是有限制的，由 FD_SETSIZE 設置，默認值是 1024/2048 ，因此 select 模型的最大併發數就被相應限制了。
* 效率問題，每次 select 調用返回都需要線性掃描全部的 FD 集合，確定那個描述符發生了相應事件，這樣效率就會呈現線性下降。
* 內核/用戶空間內存拷貝問題，在通知用戶事件發生時採用內存拷貝。
* 觸發方式採用的是水平觸發，應用程序如果沒有完成對一個已經就緒的文件描述符的 IO 操作，那麼之後每次 select 調用還是會將這些文件描述符通知進程。

對於 poll 而言，2 和 3 都沒有改掉，相比來說，epoll 要好的多。
* epoll 沒有最大併發連接的限制，上限是最大可以打開文件的數目，這個數字一般遠大於 2048, 一般來說這個數目和系統內存關係很大 ，具體數目可以在 /proc/sys/fs/epoll/max_user_watches 中查看。
* 效率提升，epoll 最大的優點就在於它只管你“活躍”的連接，而跟連接總數無關，因此在實際的網絡環境中，epoll 的效率就會遠遠高於 select 和 poll。
* 內存拷貝，epoll 在這點上使用了“共享內存”，這個內存拷貝也省略了。mmap 加速內核與用戶空間的信息傳遞，epoll 是通過內核於用戶空間 mmap 同一塊內存，避免了無謂的內存拷貝。

<!--
select 可以在某一時間監視最大達到 FD_SETSIZE 數量的文件描述符， 通常是由在 libc 編譯時指定的一個比較小的數字。
poll 在同一時間能夠監視的文件描述符數量並沒有受到限制，即使除了其它因素，更加的是我們必須在每一次都掃描所有通過的描述符來檢查其是否存在己就緒通知，它的時間複雜度為 O(n) ，是緩慢的。

epoll 沒有以上所示的限制，並且不用執行線性掃描。因此， 它能有更高的執行效率且可以處理大數量的事件。

當描述符被添加到epoll實例中， 有兩種添加模式： level triggered（水平觸發） 和 edge triggered（邊沿觸發） 。 當使用 level triggered 模式並且數據就緒待讀， epoll_wait總是會返加就緒事件。如果你沒有將數據讀取完， 並且調用epoll_wait 在epoll 實例上再次監聽這個描述符， 由於還有數據是可讀的，它會再次返回。在 edge triggered 模式時， 你只會得一次就緒通知。 如果你沒有將數據讀完， 並且再次在 epoll實例上調用 epoll_wait ， 由於就緒事件已經被髮送所以它會阻塞。
-->


### 函數接口

epoll 相關的函數主要有如下三種，分別用於創建，註冊、修改、刪除，等待事件發生。

#### 創建

創建 epoll 文件描述符，```int epoll_create (int size)``` 。

需要注意的是，當創建好 epoll 句柄後，它就是會佔用一個 fd 值，在 linux 下如果查看 ```/proc/<PID>/fd/```，是能夠看到這個 fd 的，所以在使用完 epoll 後，必須調用 close() 關閉，否則可能導致 fd 被耗盡。

調用成功則返回與實例關聯的文件描述符，該文件描述符與真實的文件沒有任何關係，僅作為接下來調用的函數的句柄，實際就是申請一個內核空間，用來保存所關注的 fd 上發生的時將。size 為了與之前兼容，應該大於 0 ，現在是動態分配，而非指定支持事件的數目。正常返回值大於 0；發生錯誤時，返回 -1，errno 表明錯誤類型。

#### 修改

控制文件描述符，```int epoll_ctl(int epfd, int op, int fd, struct epoll_event *event);``` 。

類似於相對於 select 模型中的 FD_SET 和 FD_CLR 宏，用於註冊、修改、刪除；與 select 不同的是，select 在監聽事件時告訴內核要監聽什麼類型的事件，而是在這裡先註冊要監聽的事件類型。

其中<font color='blue'>參數 epfd 是 epoll_create()</font> 創建 epoll 專用的文件描述符；op 用於表示對應的操作(EPOLL_CTL_ADD、EPOLL_CTL_MOD、EPOLL_CTL_DEL)；fd 表示需要監控的描述符；event 表示需要監控的事件。

#### 等待

等待事件 ```int epoll_wait(int epfd, struct epoll_event *events, int maxevents, int timeout);```，內核通過 events 返回發生事件的集合，maxevents 告之內核最多可以返回的事件數，該函數返回需要處理的事件數目，如返回 0 表示已超時。

等待 IO 事件的發生，epfd 是由 epoll_create() 生成的 epoll 專用的文件描述符；epoll_event 用於回傳事件發生對應的數組；maxevents 表示能處理的最多的事件數；timeout 等待 IO 事件發生的超時值，-1 表示永久。

epoll_wait() 首先判斷參數的有效性，最後會調用 ep_epoll() 函數。epoll 不僅會告訴應用程序有 IO 事件到來，還會告訴應用程序相關的信息，這些信息是應用程序填充的，因此根據這些信息應用程序就能直接定位到事件，而不必遍歷整個 FD 集合。

### epoll() 的使用

如下是 epoll 使用的數據結構，執行過程。

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
int efd = epoll_create(1);                        // 1. 創建文件描述符，大於0即可，實際內核中動態分配
if (efd == -1 || error == 0)
    error

event.data.fd = fd;                               // 2.1 設置需要監聽的fd
event.events = EPOLLIN | EPOLLET;                 // 2.2 設置需要監聽的事件
ret = epoll_ctl(efd, EPOLL_CTL_ADD, 0, &event)    // 2.3 可以添加、修改、刪除
if (ret == -1)
    error

events = calloc(MAX_EVENTS, sizeof event);        // 3.1 申請內存，可返回的最大事件數
while (1) {
    n = epoll_wait(efd, events, MAX_EVENTS, -1);  // 3.2 等待事件發生
    if (n == 0)                                   // 3.3 超時
        timeout
    else if (n < 0)                               // 3.4 發生錯誤
        if(EINTT == errno)                        // 3.5 由於中斷，忽略
            n = 0; continue;
        else
            error
    else                                          // 3.6 處理髮生的事件
        for(i = 0; i &lt; n; i++)
            if (events[i].data.fd == fd)
                handleEvent
            else if (enents[i].events & EPOLLIN)   // 某些事件發生
                //
}
free(events);
close(fd);
{% endhighlight %}

結構體 epoll_event 用於註冊所感興趣的事件和回傳所發生待處理的事件。epoll_data 聯合體用來保存觸發事件相關的描述符所對應的信息，可以保存很多類型的信息，如 fd 、指針等等，有了它，應用程序就可以直接定位目標了。

對於 epoll_data ，如一個 client 連接到服務器時，服務器通過調用 accept 函數可以得到 client 對應的 socket 文件描述符，並通過 epoll_data 的 fd 字段返回。

epoll_event 結構體的 events 字段是表示感興趣的事件和被觸發的事件，可能的取值為：

{% highlight text %}
EPOLLIN ：對應的文件描述符可以讀；
EPOLLOUT：對應的文件描述符可以寫；
EPOLLPRI：對應的文件描述符有緊急的數據可讀；
EPOLLERR：對應的文件描述符發生錯誤；
EPOLLHUP：對應的文件描述符被掛斷；
EPOLLET ：對應的文件描述符有事件發生。
{% endhighlight %}

<!--
LT(level triggered)，默認的工作方式，同時支持 block 和 no-block socket。對於該模式，內核告訴你一個文件描述符是否就緒了，然後你可以對這個就緒的 fd 進行 IO 操作。如果你不作任何操作，或者還沒有處理完，內核還會繼續通知你的，所以，這種模式編程出錯誤可能性要小一點。傳統的 select/poll 都是這種模型的代表。

ET(edge-triggered)，高速工作方式，只支持 no-block socket。在這種模式下，當描述符從未就緒變為就緒時，內核通過 epoll 告訴你，然後它會假設你知道文件描述符已經就緒，並且不會再為那個文件描述符發送更多的就緒通知，直到你做了某些操作導致那個文件描述符不再為就緒狀態了，比如，你在發送、接收或者接收請求，或者發送接收的數據少於一定量時導致了一個 EWOULDBLOCK 錯誤。但是請注意，如果一直不對這個 fd 作 IO 操作（從而導致它再次變成未就緒），內核不會發送更多的通知(only once)，不過在TCP協議中，ET模式的加速效用仍需要更多的benchmark確認。

ET和LT的區別在於LT事件不會丟棄，而是隻要讀buffer裡面有數據可以讓用戶讀，則不斷的通知你。而ET則只在事件發生之時通知。可以簡單理解為LT是水平觸發，而ET則為邊緣觸發。

ET模式僅當狀態發生變化的時候才獲得通知,這裡所謂的狀態的變化並不包括緩衝區中還有未處理的數據,也就是說,如果要採用ET模式,需要一直read/write直到出錯為止,很多人反映為什麼採用ET模式只接收了一部分數據就再也得不到通知了,大多因為這樣;而LT模式是隻要有數據沒有處理就會一直通知下去的.
-->

### 內核實現

如下是與 epoll 相關的結構設計，先看下與結構體相關的結構圖。

![io epoll structure]({{ site.url }}/images/linux/kernel/io-epoll-structure.png "io epoll structure"){: .pull-center width="80%" }

其中 `struct epitem` 是 epoll 的基本單元，對於每一個事件，都會建立一個 epitem 結構體，下面分別介紹一下幾個主要的變量的含義：

{% highlight c %}
struct epitem{
    struct rb_node rbn;         // 紅黑樹節點
    struct list_head rdllink;   // 雙向鏈表節點，當epitem對應fd已經ready時，會在ep_poll_callback()函數中將該
                                //   結點鏈接到eventpoll中的rdllist循環鏈表中去，這樣就將ready的epitem都串連起來了
    struct epoll_filefd ffd;    // 事件句柄信息，包含了一個fd以及fd對應的file指針
    struct eventpoll *ep;       // 指向其所屬的eventpoll對象，用於獲取與epitem對應的eventpoll
    struct epoll_event event;   // 期待發生的事件類型，對應了epoll_ctl()中的指針，用於存儲用戶空間的epoll_event拷貝
}

int epoll_ctl(int epfd, int op, int fd, struct epoll_event *event);
{% endhighlight %}

當某一進程調用 `epoll_create()` 方法時，內核會創建一個 `eventpoll` 結構體，這個結構體中有兩個成員與 epoll 的使用方式密切相關。

{% highlight c %}
struct eventpoll{
    struct rb_root  rbr;        // 紅黑樹的根節點，這顆樹中存儲著所有添加到epoll中的需要監控的事件
                                //   它的結點都為epitem變量，通過它可以很方便的增刪改查epitem
    struct list_head rdlist;    // 雙鏈表中則存放著將要通過epoll_wait返回給用戶的滿足條件的事件
                                //   鏈表中的每個結點即為epitem中的rdllink
};
{% endhighlight %}

每一個 epoll 對象都有一個獨立的 `eventpoll` 結構體，用於存放通過 `epoll_ctl()` 方法向 epoll 對象中添加進來的事件，這些事件都會掛載在紅黑樹中。

而所有添加到 epoll 中的事件都會與設備+網卡驅動程序建立回調關係，也就是說，當相應的事件發生時會調用這個回調方法。這個回調方法在內核中就是 `ep_poll_callback()` 它會將發生的事件添加到 rdlist 雙鏈表中。

<br>

接著再看下 `struct eppoll_entry` 結構體，它主要有這樣幾個變量：

{% highlight c %}
struct eppoll_entry {
    struct epitem *base;       // 指向其對應的epitem
    wait_queue_t wait;         // 等待隊列的項，wait中有一個喚醒回調函數指針，該指針被初始化為ep_poll_callback
};
{% endhighlight %}

如上的 wait 成員會被掛在到設備的等待隊列中，等待設備的喚醒，當設備因狀態改變喚醒 wait 時，會執行 `ep_poll_callback`，而該函數會做這樣一件事 `list_add_tail(&epi->rdllink,&ep->rdllist)` 。

<!--
，其中epi即為epitem變量，通過wait偏移拿到eppoll_entry，然後可以拿到base指針，即拿到了對應的epitem，而ep即為eventpoll變量，通過epitem的ep指針即可拿到，list_add_tail將epi的rdllink鏈到ep的rdllist中


下面結合這幅圖大致講解一下epoll_create、epoll_ctl、epoll_wait都在做些什麼：

    首先，epoll_create會創建一個epoll的文件（epfd），同時創建並初始化一個struct eventpoll，其中file的private_data指針即指向了eventpoll變量，因此，知道epfd就可以拿到file，即拿到了eventpoll變量,這就是epoll_create所做的工作
    epoll_ctl又做了什麼事呢？首先大家看到了eventpoll中的rb_root紅黑樹嗎？epoll_ctl其實就是在操作這顆紅黑樹，epoll_ctl有三種操作：

    EPOLL_CTL_ADD：往紅黑樹中創建並添加一個epitem，對應處理函數為ep_insert
    在添加epitem時，也就是在ep_insert過程中，會創建一個eppoll_entry，並將其wait_queue掛載到設備的等待隊列上，其中該wait_queue的喚醒回調函數為ep_poll_callback，當設備有事件ready而喚醒wait_queue時，就會執行ep_poll_callback將當前epitem鏈接到eventpoll中的rdllist中去，另外，如果在掛載wait_queue時就發現設備有事件ready了，同樣會將epitem鏈接到rdllist中去。若每次調用poll函數,操作系統都要把current(當前進程)掛到fd對應的所有設備的等待隊列上,可以想象,fd多到上千的時候,這樣“掛”法很費事;而每次調用epoll_wait則沒有這麼羅嗦,epoll只在epoll_ctl時把current掛一遍(這第一遍是免不了的)並給每個fd一個命令“好了就調回調函數”,如果設備有事件了,通過回調函數,會把fd放入rdllist,而每次調用epoll_wait就只是收集rdllist裡的fd就可以了——epoll巧妙的利用回調函數,實現了更高效的事件驅動模型

    EPOLL_CTL_MOD：修改對應的epitem，對應處理函數為ep_modify
    在ep_modify過程中，處理會修改epitem對應的event值，同樣會先查看一下對應設備的當前狀態，如果有ready事件發生，則會將當前epitem鏈接到rdllist中去
    EPOLL_CTL_DEL：從紅黑樹中刪除對應的epitem，對應處理函數為ep_remove
    釋放鉤子、鏈接、資源空間等，如epitem所佔的空間
    其實epoll_ctl已經將絕大部分事情都做了，epoll_wait有隻需要收集結果就行了，它的目標也很單一，就看rdllist中是否有元素即可，當然，它還需要控制timeout，及結果轉移，因為對於rdllist鏈接的epitem，只能說明其對應的fd有事件ready，但是哪些事件是不知道的，因此epoll_ctl再收集結果時，會親自查看一下對應file的ready狀態來寫回events
-->

#### 初始化

epoll 的幾個接口實際都對應於 kernel 的 API ，主要位於 `fs/eventpoll.c` 文件中。在分析 epoll 時發現有 `fs_initcall()` 這樣的調用，以此為例分析一下 Linux 的初始化。

{% highlight c %}
fs_initcall(eventpoll_init);                                      // fs/eventpoll.c
#define fs_initcall(fn) __define_initcall(fn, 5)                  // include/linux/init.h
#define __define_initcall(fn, id) \                               // 同上
      static initcall_t __initcall_##fn##id __used \
      __attribute__((__section__(".initcall" #id ".init"))) = fn

// 最後展開為
static initcall_t __initcall_eventpoll_init5 __used
     __attribute__((__section__(".initcall5.init"))) = eventpoll_init;
{% endhighlight %}

也就是在 `.initcall5.init` 段中定義了一個變量 `__initcall_eventpoll_init5` 並將改變量賦值為 `eventpoll_init`，內核中對初始化的調用過程如下。

{% highlight text %}
arch/x86/kernel/head_64.S
 |-x86_64_start_kernel()                 arch/x86/kernel/head[64|32].c
   |-start_kernel()                      init/main.c
     |-rest_init()
       |-kernel_init()                   通過內核線程實現
         |-kernel_init_freeable()
           |-do_basic_setup()
             |-do_initcalls()
{% endhighlight %}

對於 epoll 來說，實際是在初始化過程中對變量進行初始化。

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

主要是進行一些初始化配置，同時創建了 2 個內核 cache 用於存放 epitem 和 epoll_entry 。

#### 創建對象

通過 `epoll_create()` 創建一個 epoll 實例，同時創建並初始化一個 `struct eventpoll`，其中返回值 epfd 所對應的 file 的 `private_data` 指針即指向了 `eventpoll` 變量，因此，知道 epfd 就可以拿到 file，即拿到了 `eventpoll` 變量。

{% highlight c %}
SYSCALL_DEFINE1(epoll_create, int, size)  // epoll_create函數帶一個整型參數
{
	if (size <= 0)
		return -EINVAL;

	return sys_epoll_create1(0);          // 實際上是調用epoll_create1
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
    error = ep_alloc(&ep);   // 分配eventpoll結構體
    if (error < 0)
    	return error;
    /*
     * Creates all the items needed to setup an eventpoll file. That is,
     * a file structure and a free file descriptor.
     */
    // 創建與eventpoll結構體相對應的file結構，ep保存在file->private_data結構中，其中
    // eventpoll_fops 為該文件所對應的操作函數
    error = anon_inode_getfd("[eventpoll]", &eventpoll_fops, ep, O_RDWR | (flags & O_CLOEXEC));
    if (error < 0)
    	ep_free(ep);        // 如果出錯則釋放該eventpoll結構體
    
    return error;
}
{% endhighlight %}

`anon_inode_getfd()` 創建與 `struct eventpoll` 對應的 file 結構，其中 ep 保存在 `file->private_data` 結構中，同時為該新文件定義操作函數。

從這幾行代碼可以看出，`epoll_create()` 主要做了兩件事：

* 創建並初始化一個 `struct eventpoll` 變量；
* 創建 epoll 的 file 結構，並指定 file 的 `private_data` 指針指向剛創建的 `eventpoll` 變量，這樣，只要根據 `epoll` 文件描述符 epfd 就可以拿到 file 進而就拿到了 `eventpoll` 變量，該 `eventpoll` 就是 `epoll_ctl()` 和 `epoll_wait()` 工作的場所。

對外看來，`epoll_create()` 就做了一件事，那就是創建一個 epoll 文件，事實上，更關鍵的是，它創建了一個 `struct eventpoll` 變量，該變量為 `epoll_ctl()` 和 `epoll_wait()` 的工作打下了基礎。

#### 添加監控

`epoll_ctl()` 主要是針對 epfd 所對應的 epoll 實例進行增、刪、改操作，一個新創建的 epoll 文件帶有一個 `struct eventpoll` ，同時該結構體上再掛一個紅黑樹，紅黑樹上的每個節點掛的是 `struct epitem`，這個紅黑樹就是每次 `epoll_ctl()` 時 fd 存放的地方。

{% highlight c %}
/*
 * epfd為該epoll套接字實例,op表示對應的操作，fd表示新加入的套接字，
 * 結構體epoll_event 用於註冊fd所感興趣的事件和回傳在fd上所發生待處理的事件
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
    // 將用戶傳入的event_poll拷貝到epds中
    if (ep_op_has_event(op) && copy_from_user(&epds, event, sizeof(struct epoll_event)))
        goto error_return;

    /* Get the "struct file *" for the eventpoll file */
    error = -EBADF;
    file = fget(epfd); // 獲取該epoll套接字實例所對應的文件描述符
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
    ep = file->private_data; // 獲取epoll實例所對應的eventpoll結構體

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
     * ep_find即從ep中的紅黑樹中根據tfile和fd來查找epitem
     */
    epi = ep_find(ep, tfile, fd);

    error = -EINVAL;
    switch (op) {
    case EPOLL_CTL_ADD: // 對應於socket上事件註冊
        if (!epi) {     // 紅黑樹中不存在這個節點
            // 確保"出錯、連接掛起"被當做事件，將出錯信息返回給應用
            epds.events |= POLLERR | POLLHUP;
            error = ep_insert(ep, &epds, tfile, fd);
        } else
            error = -EEXIST;
        break;
    case EPOLL_CTL_DEL: // 刪除
        if (epi)        // 存在則刪除這個節點，不存在則報錯
            error = ep_remove(ep, epi);
        else
            error = -ENOENT;
        break;
    case EPOLL_CTL_MOD: // 修改
        if (epi) {      // 存在則修改該fd所對應的事件，不存在則報錯
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

對於往 epoll 實例中添加新的套接字，其實現主要通過 `ep_insert()` 完成，先分析 `epoll_wait` 再回過頭來分析 `ep_insert()` 。

#### 等待事件

`epoll_wait()` 用來等待 epoll 文件上的 IO 事件發生，其代碼如下：

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
    ep = file->private_data;  // 獲取struct eventpoll結構

    /* Time to fish for events ... */
    error = ep_poll(ep, events, maxevents, timeout); // 核心代碼

error_fput:
    fput(file);
error_return:

    return error;
}
{% endhighlight %}

可以看出該函數主要時通過 epfd 獲取對應的 `struct eventpoll` 結構，然後調用 `ep_poll()` 函數，下面來看 `ep_poll()` 的實現。


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
    // 如果rdllist中還沒有epitem時，就開始等待了
    if (!ep_events_available(ep)) {
        /*
         * We don't have any available event to return to the caller.
         * We need to sleep here, and we will be wake up by
         * ep_poll_callback() when events will become available.
         */
        // 初始化等待隊列，等待隊列項對應的線程即為當前線程
        init_waitqueue_entry(&wait, current);
        // 先將當前線程掛到等待隊列上，之後在調用schedule_timeout時，就開始了超時等待了
        __add_wait_queue_exclusive(&ep->wq, &wait);

        for (;;) {
            /*
             * We don't want to sleep if the ep_poll_callback() sends us
             * a wakeup in between. That's why we set the task state
             * to TASK_INTERRUPTIBLE before doing the checks.
             */
             // 因為會被阻塞，這裡先設置線程狀態為可中斷
            set_current_state(TASK_INTERRUPTIBLE);
            // 整個循環的核心，其實就在看rdllist中是否有數據，或者等待超時
            // 應徵了前面的說明，epoll_wait只需要等著收集數據即可
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
29-43行：主要是超時時間的處理，若超時時間為0，則直接檢查有沒有準備好的I/O事件，有則立即發送給用戶空間去處理；若超時時間大於0，計算好精確的超時時間後，等待事件的發生，45-86行等待指定的時間直到有I/O事件出現；

54-58行：如果還沒有I/O事件出現，則準備休眠。先初始化等待隊列，把當前線程掛在該隊列上，同時把這個隊列掛在eventpoll結構的wq上，

60-82行：在指定的超時時間內循環檢測有沒有I/O事件發生，有事件發生、超時或者收到信號都會跳出循環。

83行：運行到此處有I/O事件發生，不用再等待，則移除該隊列
-->

調用 `epoll_wait()` 檢查是否有事件發生時，只需檢查 `eventpoll` 中 `rdlist` 鏈表是否有 `epitem` 元素即可。如果 rdlist 不為空，則把發生的事件複製到用戶態，同時將事件數量返回給用戶。

![epoll data structure]({{ site.url }}/images/linux/kernel/epoll-data-structure.jpg "epoll data structure"){: .pull-center width="60%" }

## 參考

<!--
Linux設備驅動程序——高級字符驅動程序操作(poll機制)
http://blog.chinaunix.net/uid-29339876-id-4070572.html
-->

{% highlight text %}
{% endhighlight %}
