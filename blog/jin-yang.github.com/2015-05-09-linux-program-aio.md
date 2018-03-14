---
title: Linux AIO
layout: post
comments: true
language: chinese
category: [linux,program]
keywords: linux,aio
description: AIO 是 Linux 下的異步讀寫模型，它是 2.6 內核提供的一個標準增強特性。對於文件的讀寫，即使以 O_NONBLOCK 方式來打開一個文件，也會處於 "阻塞" 狀態，因為文件時時刻刻處於可讀狀態，而從磁盤到內存所等待的時間是驚人的。為了充份發揮把數據從磁盤複製到內存的時間，引入了 AIO 模型，其基本原理是允許進程發起很多 I/O 操作，而不用阻塞或等待任何操作完成。稍後或在接收到 I/O 操作完成的通知時，進程就可以檢索 I/O 操作的結果。
---

AIO 是 Linux 下的異步讀寫模型，它是 2.6 內核提供的一個標準增強特性。對於文件的讀寫，即使以 O_NONBLOCK 方式來打開一個文件，也會處於 "阻塞" 狀態，因為文件時時刻刻處於可讀狀態，而從磁盤到內存所等待的時間是驚人的。

為了充份發揮把數據從磁盤複製到內存的時間，引入了 AIO 模型，其基本原理是允許進程發起很多 I/O 操作，而不用阻塞或等待任何操作完成。稍後或在接收到 I/O 操作完成的通知時，進程就可以檢索 I/O 操作的結果。

<!-- more -->

## IO 模型

下圖給出了同步和異步，以及阻塞和非阻塞的模型。簡單來說，一個系統調用 (read, select) 立即返回表示非阻塞；在一個時間點只能去處理一個請求表示同步。

![io brief]({{ site.url }}/images/linux/aio-brief.gif "io brier"){: .pull-center }

每個 I/O 模型都有自己的使用模式，它們對於特定的應用程序都有自己的優點。

### 同步阻塞IO

最常用的模型，當用戶空間的應用執行一個系統調用後，會導致應用程序阻塞，直到系統調用完成為止（數據傳輸完成或發生錯誤）。應用程序只能處於一種不再消費 CPU 而只是簡單等待響應的狀態，因此從處理的角度來看，這是非常有效的。

![io block sync]({{ site.url }}/images/linux/aio-block-sync.gif "io block sync"){: .pull-center }

在調用 read 系統調用時，應用程序會阻塞並對內核進行上下文切換；然後會觸發讀磁盤操作，當從讀取的設備中返回後，數據就被移動到用戶空間的緩衝區中；然後應用程序就會解除阻塞（read 調用返回）。

### 同步非阻塞IO

備以非阻塞的形式打開，這意味著 I/O 操作不會立即完成，read 操作可能會返回一個錯誤代碼，說明這個命令不能立即滿足（EAGAIN 或 EWOULDBLOCK）。

該模型中可能需要應用程序調用多次來等待操作完成，這樣的效率仍然不高，因為很多情況下，當內核執行這個命令時，應用程序必須要進行忙碌等待，直到數據可用為止，或者試圖執行其他工作。

![io nonblock sync]({{ site.url }}/images/linux/aio-nonblock-sync.gif "io nonblock sync"){: .pull-center }

正如上圖所示，這個方法會引入 I/O 操作的延時，因為數據在內核中變為可用到用戶調用 read 返回數據之間存在一定的間隔，這會導致整體數據吞吐量的降低。

### 異步阻塞IO

阻塞通知的非阻塞 I/O，配置的是非阻塞 I/O，然後使用阻塞 select 系統調用來確定一個 I/O 描述符何時有操作。select 可以為多個描述符提供通知，而不是僅僅為一個描述符提供通知；通知的事件包括寫數據、有讀數據可用以及是否發生錯誤。

![io block async]({{ site.url }}/images/linux/aio-block-async.gif "io block async"){: .pull-center }

select 調用的主要問題是它的效率不是非常高，儘管這是異步通知使用的一種方便模型，但是對於高性能的 I/O 操作來說不建議使用。

### 異步非阻塞IO（AIO）

一種處理與 I/O 重疊進行的模型，讀請求會立即返回，說明 read 請求已經成功發起了；然後應用程序會執行其他處理操作；當 read 的響應到達時，就會產生一個信號或執行一個基於線程的回調函數來完成這次 I/O 處理過程。

![io nonblock async]({{ site.url }}/images/linux/aio-nonblock-async.gif "io nonblock async"){: .pull-center }

該模型可以重疊執行多個 I/O 請求以及 CPU 操作，利用了處理速度與 I/O 速度之間的差異。當一個或多個 I/O 請求掛起時，此時 CPU 可以執行其他任務；或者更為常見的是，在發起其他 I/O 的同時對已經完成的 I/O 進行操作。

## Linux AIO簡介

Linux 中有兩套異步 IO，一套是由 glibc 實現的 `aio_*` 系列，通過線程+阻塞調用在用戶空間模擬 AIO 的功能，不需要內核的支持，類似的還有 `libeio`；另一套是採用原生的 Linux AIO，並由 `libaio` 來封裝調用接口，相比來說更底層。

glibc 的實現性能比較差，在此先介紹 `libaio` 的使用。`libaio` 的使用並不複雜，過程為：A) `libaio` 的初始化； B) IO 請求的下發和回收，C) `libaio` 銷燬。提供了下面五個主要 API 函數以及宏定義：

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

後五個宏定義是為了操作 `struct iocb` 結構體，該結構體是 `libaio` 中很重要的一個結構體，用於表示 IO，但是其結構略顯複雜，為了保持封裝性不建議直接操作其元素而用上面五個宏定義操作。

#### 初始化

用來初始化類型為 `io_context_t` 的變量，這個變量為 `libaio` 的工作空間，可以採用 `io_setup()` 或者 `io_queue_init()`，兩者功能實際相同。

#### 自定義字段

這一階段是可選的，在 `struct iocb` 中保留了供用戶自定義的元素，也就是 `void *data`，可以通過 `io_set_callback()` 設置回調函數，或者通過 `iocbp->data=XXX` 自定義。

需要注意的是，兩者均使用 data 變量，因此不能同時使用。

#### 讀寫請求下發

讀寫均通過 `io_submit()` 下發，之前需要通過 `io_prep_pwrite()` 和 `io_prep_pread()` 生成 `struct iocb` 做為該函數的參數。在這個結構體中指定了讀寫類型、起始扇區、長度和設備標誌符等信息。

#### 讀寫請求回收

使用 `io_getevents()` 等待 IO 的結束信號，該函數返回 `events[]` 數組，`nr` 為數組的最大長度，`min_nr` 為最少返回的 `events` 數，`timeout` 可填 `NULL` 表示一直等待。

{% highlight c %}
struct io_event {
    PADDEDptr(void *data, __pad1);
    PADDEDptr(struct iocb *obj,  __pad2);
    PADDEDul(res,  __pad3);
    PADDEDul(res2, __pad4);
};
{% endhighlight %}

其中，res 為實際完成的字節數；res2 為讀寫成功狀態，0 表示成功；obj 為之前下發的 struct iocb 結構體。

#### 銷燬

直接通過 `io_destory()` 銷燬即可。

### 示例

下面是一個簡單的示例，通過 AIO 寫入到 foobar.txt 文件中。

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

然後，可以通過 ```gcc foobar.c -o foobar -laio -Wall``` 進行編譯。

## POSIX AIO

也就是 glibc 中包含的版本，主要包含如下接口：

{% highlight c %}
#include <aio.h>

// 提交一個異步讀/寫，通過結構體告知系統讀取的文件、起始位置、讀取字節數、讀取後的寫入buffer
int aio_read(struct aiocb *aiocbp);
int aio_write(struct aiocb *aiocbp);

// 檢查當前AIO的狀態，可用於查看請求是否成功，返回0(成功)EINPROGRESS(正在讀取)
int aio_error(const struct aiocb *aiocbp);

// 查看一個異步請求的返回值，如果成功則返回讀取字節數，否則返回-1，此時跟同步讀寫定義的一樣
ssize_t aio_return(struct aiocb *aiocbp);         
{% endhighlight %}

<!--
int aio_cancel(int fildes, struct aiocb *aiocbp); /* 取消一個異步請求（或基於一個fd的所有異步請求，aiocbp==NULL） */
int aio_suspend(const struct aiocb * const list[], int nent, const struct timespec *timeout); /* 阻塞等待請求完成 */

其中，struct aiocb主要包含以下字段：
int               aio_fildes;        /* 要被讀寫的fd */
void *            aio_buf;           /* 讀寫操作對應的內存buffer */
__off64_t         aio_offset;        /* 讀寫操作對應的文件偏移 */
size_t            aio_nbytes;        /* 需要讀寫的字節長度 */
int               aio_reqprio;       /* 請求的優先級 */
struct sigevent   aio_sigevent;      /* 異步事件，定義異步操作完成時的通知信號或回調函數 */

異步 IO 的實現邏輯大致如下：

1. 異步請求提交到請求隊列中；
request_queue中；
2、request_queue實際上是一個表結構，"行"是fd、"列"是具體的請求。也就是說，同一個fd的請求會被組織在一起；
3、異步請求有優先級概念，屬於同一個fd的請求會按優先級排序，並且最終被按優先級順序處理；
4、隨著異步請求的提交，一些異步處理線程被動態創建。這些線程要做的事情就是從request_queue中取出請求，然後處理之；
5、為避免異步處理線程之間的競爭，同一個fd所對應的請求只由一個線程來處理；
6、異步處理線程同步地處理每一個請求，處理完成後在對應的aiocb中填充結果，然後觸發可能的信號通知或回調函數（回調函數是需要創建新線程來調用的）；
7、異步處理線程在完成某個fd的所有請求後，進入閒置狀態；
8、異步處理線程在閒置狀態時，如果request_queue中有新的fd加入，則重新投入工作，去處理這個新fd的請求（新fd和它上一次處理的fd可以不是同一個）；
9、異步處理線程處於閒置狀態一段時間後（沒有新的請求），則會自動退出。等到再有新的請求時，再去動態創建；
看起來，換作是我們，要在用戶態實現一個異步IO，似乎大概也會設計成類似的樣子……
-->

### 示例程序

如下是一個測試示例。

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


## Linux direct-io簡介

在 Linux 2.6 使用 direct io 需要按照如下幾點來做：

1. 在源文件的最頂端加上 #define _GNU_SOURCE  宏定義，或在編譯時在命令行上指定，否則編譯報錯。
2. 在 open() 時加上 O_DIRECT 標誌。
3. 存放文件數據的緩存起始位置以及每次讀寫數據長度必須是磁盤邏輯塊大小的整數倍，一般是 512 字節，否則將導致讀寫失敗，返回 -EINVAL。

對於第 3 點，要滿足緩存區起始位置對齊，可以在進行緩存區空間申請時使用 posix_memalign 這樣的函數指定字節對齊。

{% highlight text %}
ret = posix_memalign((void **)&buf, 512, BUF_SIZE);
real_buf = malloc(BUF_SIZE + 512);
aligned_buf = ((((unsigned int)real_buf + 512 - 1) / 512) * 512);
{% endhighlight %}

由於要滿足每一次讀寫數據長度必須是磁盤邏輯塊大小的整數倍，所以最後一次文件操作可能無法滿足，此時只能重新以cached io模式打開文件後，fseek到對應位置進行剩餘數據的讀寫。

## 參考

<a href="http://www.ibm.com/developerworks/cn/linux/l-async/">使用異步 I/O 大大提高應用程序的性能</a> 介紹各種 IO 模型不錯的參考；在內核文檔 <a href="http://www.fsl.cs.sunysb.edu/~vass/linux-aio.txt">linux-io.txt</a> 中介紹了 Linux AIO 機制以及部分 libaio、librt 的內容。

{% highlight text %}
{% endhighlight %}
