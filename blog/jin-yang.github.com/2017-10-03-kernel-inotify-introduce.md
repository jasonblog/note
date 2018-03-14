---
title: Inotify 機制詳解
layout: post
comments: true
language: chinese
category: [linux,misc]
keywords: inotify
description: 日常工作中，經常會需要知道某些文件上的變化，一般是通過輪詢機制檢查文件的變化，不過這樣非常低效。Inotify 是一種文件變化通知機制，Linux 內核從 2.6.13 開始引入，而 BSD 和 Mac OS 系統中比較有名的是 kqueue，可以高效地實時跟蹤文件系統的變化。這裡簡單介紹其使用方法。
---

日常工作中，經常會需要知道某些文件上的變化，一般是通過輪詢機制檢查文件的變化，不過這樣非常低效。

Inotify 是一種文件變化通知機制，Linux 內核從 2.6.13 開始引入，而 BSD 和 Mac OS 系統中比較有名的是 kqueue，可以高效地實時跟蹤文件系統的變化。

這裡簡單介紹其使用方法。

<!-- more -->

## 簡介

可以通過如下方式查看是否支持 Inotify 機制。

{% highlight text %}
$ grep INOTIFY_USER /boot/config-$(uname -r)
CONFIG_INOTIFY_USER=y
{% endhighlight %}

如果輸出 `CONFIG_INOTIFY_USER=y` 那麼就表示系統支持 inotify 機制。





## 源碼實現

在用戶態主要通過三個系統調用，首先是要創建 inotify 實例。

{% highlight text %}
#include <sys/inotify.h>
int inotify_init(void);
int inotify_init1(int flags);
{% endhighlight %}

每個 inotify 實例對應一個獨立的排序的隊列，文件系統的變化事件被稱做 watches 的一個對象管理，每一個 watch 是一個二元組：

1. 目標，可以是文件或目錄；
2. 事件掩碼，應用希望關注的 inotify 事件，每位對應一個 inotify 事件。

Watch 對象通過 watch 描述符引用，其中，目錄 watches 將返回在該目錄下的所有文件上面發生的事件。

下面函數用於添加一個 watch 。

{% highlight text %}
#include <sys/inotify.h>
int inotify_add_watch(int fd, const char *pathname, uint32_t mask);

參數：
  fd    如上的 inotify_init() 返回的文件描述符；
  path  被監視的目標的路徑名，可以是文件名或目錄名；
  mask  事件掩碼，在頭文件 `linux/inotify.h` 中定義了每一位代表的事件。
{% endhighlight %}

可以使用同樣的方式來修改事件掩碼，即改變希望被通知的 inotify 事件。

下面的函數用於刪除一個 watch 。

{% highlight text %}
#include <sys/inotify.h>
int inotify_rm_watch(int fd, int wd);

參數：
  fd   同樣是 inotify_init() 返回的文件描述符；
  wd   通過 inotify_add_watch() 返回的 watch 描述符。
{% endhighlight %}

### 獲取事件

文件事件通過 `struct inotify_event` 結構表示，通過由 `inotify_init()` 返回的文件描述符，並調用 `read()` 系統調用來獲得。

{% highlight c %}
struct inotify_event {
	__s32           wd;             /* watch descriptor */
	__u32           mask;           /* watch mask */
	__u32           cookie;         /* cookie to synchronize two events */
	__u32           len;            /* length (including nulls) of name */
	char            name[0];        /* stub for possible name */
};
{% endhighlight %}

該結構的 name 字段為一個樁，它只是為了用戶方面引用文件名，文件名是變長的，它實際緊跟在該結構的後面，文件名將被 0 填充以使下一個事件結構能夠 4 字節對齊。

通過 read 調用可以一次獲得多個事件，只要提供的 buf 足夠大。

可以在函數 `inotify_init()` 返回的文件描述符 fd 上使用 `select()` 或 `poll()`，也可以在 fd 上使用 `ioctl` 命令 `FIONREAD` 來得到當前隊列的長度。

`close(fd)` 將刪除所有添加到 fd 中的 watch 並做必要的清理。

## 內核實現

在內核中，每一個 inotify 實例對應一個 `struct inotify_device` 結構。

{% highlight c %}
struct inotify_device {
	wait_queue_head_t       wq;             /* wait queue for i/o */
	struct idr              idr;            /* idr mapping wd -> watch */
	struct semaphore        sem;            /* protects this bad boy */
	struct list_head        events;         /* list of queued events */
	struct list_head        watches;        /* list of watches */
	atomic_t                count;          /* reference count */
	struct user_struct      *user;          /* user who opened this dev */
	unsigned int            queue_size;     /* size of the queue (bytes) */
	unsigned int            event_count;    /* number of pending events */
	unsigned int            max_events;     /* maximum number of events */
	u32                     last_wd;        /* the last wd allocated */
};
{% endhighlight %}

<!--
d_list 指向所有 `struct inotify_device` 組成的列表的，i_list 指向所有被監視 inode 組成的列表，count 是引用計數，dev 指向該 watch 所在的 inotify 實例對應的 inotify_device 結構，inode 指向該 watch 要監視的 inode，wd 是分配給該 watch 的描述符，mask 是該 watch 的事件掩碼，表示它對哪些文件系統事件感興趣。
-->

`struct inotify_device` 在調用 `inotify_init()` 時創建，當關閉對應的文件描述符時將被釋放，而結構體 `struct inotify_watch` 在用戶調用 `inotify_add_watch()` 時創建，在調用 `inotify_rm_watch()` 或 `close(fd)` 時被釋放。


<!--
無論是目錄還是文件，在內核中都對應一個 inode 結構，inotify 系統在 inode 結構中增加了兩個字段：

struct inotify_watch {
        struct list_head        d_list; /* entry in inotify_device's list */
        struct list_head        i_list; /* entry in inode's list */
        atomic_t                count;  /* reference count */
        struct inotify_device   *dev;   /* associated device */
        struct inode            *inode; /* associated inode */
        s32                     wd;     /* watch descriptor */
        u32                     mask;   /* event mask for this watch */
};


    d_list 指向所有 inotify_device 組成的列表的，i_list 指向所有被監視 inode 組成的列表，count 是引用計數，dev 指向該 watch 所在的 inotify 實例對應的 inotify_device 結構，inode 指向該 watch 要監視的 inode，wd 是分配給該 watch 的描述符，mask 是該 watch 的事件掩碼，表示它對哪些文件系統事件感興趣。

    結構 inotify_device 在用戶態調用 inotify_init（） 時創建，當關閉 inotify_init（）返回的文件描述符時將被釋放。結構 inotify_watch 在用戶態調用 inotify_add_watch（）時創建，在用戶態調用 inotify_rm_watch（） 或 close（fd） 時被釋放。

    無論是目錄還是文件，在內核中都對應一個 inode 結構，inotify 系統在 inode 結構中增加了兩個字段：


#ifdef CONFIG_INOTIFY
	struct list_head	inotify_watches; /* watches on this inode */
	struct semaphore	inotify_sem;	/* protects the watches list */
#endif


    inotify_watches 是在被監視目標上的 watch 列表，每當用戶調用 inotify_add_watch（）時，內核就為添加的 watch 創建一個 inotify_watch 結構，並把它插入到被監視目標對應的 inode 的 inotify_watches 列表。inotify_sem 用於同步對 inotify_watches 列表的訪問。當文件系統發生第一部分提到的事件之一時，相應的文件系統代碼將顯示調用fsnotify_* 來把相應的事件報告給 inotify 系統，其中*號就是相應的事件名，目前實現包括：

    fsnotify_move，文件從一個目錄移動到另一個目錄fsnotify_nameremove，文件從目錄中刪除fsnotify_inoderemove，自刪除fsnotify_create，創建新文件fsnotify_mkdir，創建新目錄fsnotify_access，文件被讀fsnotify_modify，文件被寫fsnotify_open，文件被打開fsnotify_close，文件被關閉fsnotify_xattr，文件的擴展屬性被修改fsnotify_change，文件被修改或原數據被修改有一個例外情況，就是 inotify_unmount_inodes，它會在文件系統被 umount 時調用來通知 umount 事件給 inotify 系統。

    以上提到的通知函數最後都調用 inotify_inode_queue_event（inotify_unmount_inodes直接調用 inotify_dev_queue_event ），該函數首先判斷對應的inode是否被監視，這通過查看 inotify_watches 列表是否為空來實現，如果發現 inode 沒有被監視，什麼也不做，立刻返回，反之，遍歷 inotify_watches 列表，看是否當前的文件操作事件被某個 watch 監視，如果是，調用 inotify_dev_queue_event，否則，返回。函數inotify_dev_queue_event 首先判斷該事件是否是上一個事件的重複，如果是就丟棄該事件並返回，否則，它判斷是否 inotify 實例即 inotify_device 的事件隊列是否溢出，如果溢出，產生一個溢出事件，否則產生一個當前的文件操作事件，這些事件通過kernel_event 構建，kernel_event 將創建一個 inotify_kernel_event 結構，然後把該結構插入到對應的 inotify_device 的 events 事件列表，然後喚醒等待在inotify_device 結構中的 wq 指向的等待隊列。想監視文件系統事件的用戶態進程在inotify 實例（即 inotify_init（） 返回的文件描述符）上調用 read 時但沒有事件時就掛在等待隊列 wq 上。
-->



## 參考

[inotail](https://github.com/tklauser/inotail) 通過 inotify 機制實現的 tail 功能，不過實際上 tail 最新版本也已經支持了 inotify 機制。


<!--
Inotify: 高效、實時的Linux文件系統事件監控框架
http://www.infoq.com/cn/articles/inotify-linux-file-system-event-monitoring
https://www.infoq.com/articles/inotify-linux-file-system-event-monitoring
-->


{% highlight text %}
{% endhighlight %}
