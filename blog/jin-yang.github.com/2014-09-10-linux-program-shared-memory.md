---
title: Linux 共享內存
layout: post
comments: true
language: chinese
category: [linux,misc]
keywords: linux,內存,shared memory,共享內存
description: 共享內存應該是進程間通信最有效的方式，同一塊物理內存被映射到兩個不同進程 A、B 各自的地址空間；進程 A 可以立即看到進程 B 對共享內存中數據的更新，反之亦然。接下來簡單介紹下與共享內存相關的內容。
---

共享內存應該是進程間通信最有效的方式，同一塊物理內存被映射到兩個不同進程 A、B 各自的地址空間；進程 A 可以立即看到進程 B 對共享內存中數據的更新，反之亦然。

接下來簡單介紹下與共享內存相關的內容。

<!-- more -->

## 簡介

採用共享內存通信的一個顯而易見的好處是效率高，因為進程可以直接讀寫內存，而不需要任何數據的拷貝。對於像管道和消息隊列等通信方式，則需要在內核和用戶空間進行四次的數據拷貝，而共享內存則只拷貝兩次數據：一次從輸入文件到共享內存區，另一次從共享內存區到輸出文件。

<!--
實際上，進程之間在共享內存時，並不總是讀寫少量數據後就 解除映射，有新的通信時，再重新建立共享內存區域。而是保持共享區域，直到通信完畢為止，這樣，數據內容一直保存在共享內存中，並沒有寫迴文件。共享內存 中的內容往往是在解除映射時才寫迴文件的。因此，採用共享內存的通信方式效率是非常高的。
-->

**注意** 共享內存並未提供同步機制，也就是說，在第一個進程結束對共享內存的寫操作之前，並無自動機制可以阻止第二個進程開始對它進行讀取。所以我們通常需要用其他的機制來同步對共享內存的訪問，例如信號量、互斥鎖。

<!--

1、shmget函數
該函數用來創建共享內存，它的原型為：
    int shmget(key_t key, size_t size, int shmflg);
第一個參數，與信號量的semget函數一樣，程序需要提供一個參數key（非0整數），它有效地為共享內存段命名，shmget函數成功時返回一個與key相關的共享內存標識符（非負整數），用於後續的共享內存函數。調用失敗返回-1.

不相關的進程可以通過該函數的返回值訪問同一共享內存，它代表程序可能要使用的某個資源，程序對所有共享內存的訪問都是間接的，程序先通過調用shmget函數並提供一個鍵，再由系統生成一個相應的共享內存標識符（shmget函數的返回值），只有shmget函數才直接使用信號量鍵，所有其他的信號量函數使用由semget函數返回的信號量標識符。

第二個參數，size以字節為單位指定需要共享的內存容量

第三個參數，shmflg是權限標誌，它的作用與open函數的mode參數一樣，如果要想在key標識的共享內存不存在時，創建它的話，可以與IPC_CREAT做或操作。共享內存的權限標誌與文件的讀寫權限一樣，舉例來說，0644,它表示允許一個進程創建的共享內存被內存創建者所擁有的進程向共享內存讀取和寫入數據，同時其他用戶創建的進程只能讀取共享內存。

2、shmat函數
第一次創建完共享內存時，它還不能被任何進程訪問，shmat函數的作用就是用來啟動對該共享內存的訪問，並把共享內存連接到當前進程的地址空間。它的原型如下：
[cpp] view plain copy
print?在CODE上查看代碼片派生到我的代碼片

    void *shmat(int shm_id, const void *shm_addr, int shmflg);

第一個參數，shm_id是由shmget函數返回的共享內存標識。
第二個參數，shm_addr指定共享內存連接到當前進程中的地址位置，通常為空，表示讓系統來選擇共享內存的地址。
第三個參數，shm_flg是一組標誌位，通常為0。

調用成功時返回一個指向共享內存第一個字節的指針，如果調用失敗返回-1.

3、shmdt函數
該函數用於將共享內存從當前進程中分離。注意，將共享內存分離並不是刪除它，只是使該共享內存對當前進程不再可用。它的原型如下：
[cpp] view plain copy
print?在CODE上查看代碼片派生到我的代碼片

    int shmdt(const void *shmaddr);

參數shmaddr是shmat函數返回的地址指針，調用成功時返回0，失敗時返回-1.

4、shmctl函數
與信號量的semctl函數一樣，用來控制共享內存，它的原型如下：
[cpp] view plain copy
print?在CODE上查看代碼片派生到我的代碼片

    int shmctl(int shm_id, int command, struct shmid_ds *buf);

第一個參數，shm_id是shmget函數返回的共享內存標識符。

第二個參數，command是要採取的操作，它可以取下面的三個值 ：
    IPC_STAT：把shmid_ds結構中的數據設置為共享內存的當前關聯值，即用共享內存的當前關聯值覆蓋shmid_ds的值。
    IPC_SET：如果進程有足夠的權限，就把共享內存的當前關聯值設置為shmid_ds結構中給出的值
    IPC_RMID：刪除共享內存段

第三個參數，buf是一個結構指針，它指向共享內存模式和訪問權限的結構。
shmid_ds結構至少包括以下成員：
[cpp] view plain copy
print?在CODE上查看代碼片派生到我的代碼片

    struct shmid_ds
    {
        uid_t shm_perm.uid;
        uid_t shm_perm.gid;
        mode_t shm_perm.mode;
    };


三、使用共享內存進行進程間通信
說了這麼多，又到了實戰的時候了。下面就以兩個不相關的進程來說明進程間如何通過共享內存來進行通信。其中一個文件shmread.c創建共享內存，並讀取其中的信息，另一個文件shmwrite.c向共享內存中寫入數據。為了方便操作和數據結構的統一，為這兩個文件定義了相同的數據結構，定義在文件shmdata.c中。結構shared_use_st中的written作為一個可讀或可寫的標誌，非0：表示可讀，0表示可寫，text則是內存中的文件。

shmdata.h的源代碼如下：
[cpp] view plain copy
print?在CODE上查看代碼片派生到我的代碼片

    #ifndef _SHMDATA_H_HEADER
    #define _SHMDATA_H_HEADER

    #define TEXT_SZ 2048

    struct shared_use_st
    {
        int written;//作為一個標誌，非0：表示可讀，0表示可寫
        char text[TEXT_SZ];//記錄寫入和讀取的文本
    };

    #endif

源文件shmread.c的源代碼如下：
[cpp] view plain copy
print?在CODE上查看代碼片派生到我的代碼片

    #include <unistd.h>
    #include <stdlib.h>
    #include <stdio.h>
    #include <sys/shm.h>
    #include "shmdata.h"

    int main()
    {
        int running = 1;//程序是否繼續運行的標誌
        void *shm = NULL;//分配的共享內存的原始首地址
        struct shared_use_st *shared;//指向shm
        int shmid;//共享內存標識符
        //創建共享內存
        shmid = shmget((key_t)1234, sizeof(struct shared_use_st), 0666|IPC_CREAT);
        if(shmid == -1)
        {
            fprintf(stderr, "shmget failed\n");
            exit(EXIT_FAILURE);
        }
        //將共享內存連接到當前進程的地址空間
        shm = shmat(shmid, 0, 0);
        if(shm == (void*)-1)
        {
            fprintf(stderr, "shmat failed\n");
            exit(EXIT_FAILURE);
        }
        printf("\nMemory attached at %X\n", (int)shm);
        //設置共享內存
        shared = (struct shared_use_st*)shm;
        shared->written = 0;
        while(running)//讀取共享內存中的數據
        {
            //沒有進程向共享內存定數據有數據可讀取
            if(shared->written != 0)
            {
                printf("You wrote: %s", shared->text);
                sleep(rand() % 3);
                //讀取完數據，設置written使共享內存段可寫
                shared->written = 0;
                //輸入了end，退出循環（程序）
                if(strncmp(shared->text, "end", 3) == 0)
                    running = 0;
            }
            else//有其他進程在寫數據，不能讀取數據
                sleep(1);
        }
        //把共享內存從當前進程中分離
        if(shmdt(shm) == -1)
        {
            fprintf(stderr, "shmdt failed\n");
            exit(EXIT_FAILURE);
        }
        //刪除共享內存
        if(shmctl(shmid, IPC_RMID, 0) == -1)
        {
            fprintf(stderr, "shmctl(IPC_RMID) failed\n");
            exit(EXIT_FAILURE);
        }
        exit(EXIT_SUCCESS);
    }

源文件shmwrite.c的源代碼如下：
[cpp] view plain copy
print?在CODE上查看代碼片派生到我的代碼片

    #include <unistd.h>
    #include <stdlib.h>
    #include <stdio.h>
    #include <string.h>
    #include <sys/shm.h>
    #include "shmdata.h"

    int main()
    {
        int running = 1;
        void *shm = NULL;
        struct shared_use_st *shared = NULL;
        char buffer[BUFSIZ + 1];//用於保存輸入的文本
        int shmid;
        //創建共享內存
        shmid = shmget((key_t)1234, sizeof(struct shared_use_st), 0666|IPC_CREAT);
        if(shmid == -1)
        {
            fprintf(stderr, "shmget failed\n");
            exit(EXIT_FAILURE);
        }
        //將共享內存連接到當前進程的地址空間
        shm = shmat(shmid, (void*)0, 0);
        if(shm == (void*)-1)
        {
            fprintf(stderr, "shmat failed\n");
            exit(EXIT_FAILURE);
        }
        printf("Memory attached at %X\n", (int)shm);
        //設置共享內存
        shared = (struct shared_use_st*)shm;
        while(running)//向共享內存中寫數據
        {
            //數據還沒有被讀取，則等待數據被讀取,不能向共享內存中寫入文本
            while(shared->written == 1)
            {
                sleep(1);
                printf("Waiting...\n");
            }
            //向共享內存中寫入數據
            printf("Enter some text: ");
            fgets(buffer, BUFSIZ, stdin);
            strncpy(shared->text, buffer, TEXT_SZ);
            //寫完數據，設置written使共享內存段可讀
            shared->written = 1;
            //輸入了end，退出循環（程序）
            if(strncmp(buffer, "end", 3) == 0)
                running = 0;
        }
        //把共享內存從當前進程中分離
        if(shmdt(shm) == -1)
        {
            fprintf(stderr, "shmdt failed\n");
            exit(EXIT_FAILURE);
        }
        sleep(2);
        exit(EXIT_SUCCESS);
    }

再來看看運行的結果：



分析：
1、程序shmread創建共享內存，然後將它連接到自己的地址空間。在共享內存的開始處使用了一個結構struct_use_st。該結構中有個標誌written，當共享內存中有其他進程向它寫入數據時，共享內存中的written被設置為0，程序等待。當它不為0時，表示沒有進程對共享內存寫入數據，程序就從共享內存中讀取數據並輸出，然後重置設置共享內存中的written為0，即讓其可被shmwrite進程寫入數據。

2、程序shmwrite取得共享內存並連接到自己的地址空間中。檢查共享內存中的written，是否為0，若不是，表示共享內存中的數據還沒有被完，則等待其他進程讀取完成，並提示用戶等待。若共享內存的written為0，表示沒有其他進程對共享內存進行讀取，則提示用戶輸入文本，並再次設置共享內存中的written為1，表示寫完成，其他進程可對共享內存進行讀操作。

四、關於前面的例子的安全性討論
這個程序是不安全的，當有多個程序同時向共享內存中讀寫數據時，問題就會出現。可能你會認為，可以改變一下written的使用方式，例如，只有當written為0時進程才可以向共享內存寫入數據，而當一個進程只有在written不為0時才能對其進行讀取，同時把written進行加1操作，讀取完後進行減1操作。這就有點像文件鎖中的讀寫鎖的功能。咋看之下，它似乎能行得通。但是這都不是原子操作，所以這種做法是行不能的。試想當written為0時，如果有兩個進程同時訪問共享內存，它們就會發現written為0，於是兩個進程都對其進行寫操作，顯然不行。當written為1時，有兩個進程同時對共享內存進行讀操作時也是如些，當這兩個進程都讀取完是，written就變成了-1.

要想讓程序安全地執行，就要有一種進程同步的進制，保證在進入臨界區的操作是原子操作。例如，可以使用前面所講的信號量來進行進程的同步。因為信號量的操作都是原子性的。

五、使用共享內存的優缺點
1、優點：我們可以看到使用共享內存進行進程間的通信真的是非常方便，而且函數的接口也簡單，數據的共享還使進程間的數據不用傳送，而是直接訪問內存，也加快了程序的效率。同時，它也不像匿名管道那樣要求通信的進程有一定的父子關係。

2、缺點：共享內存沒有提供同步的機制，這使得我們在使用共享內存進行進程間通信時，往往要藉助其他的手段來進行進程間的同步工作。
-->

## 使用示例

共享內存主要用於進程間通信，Linux 有幾種方式的共享內存 (Shared Memory) 機制：

1. ```System V shared memory(shmget/shmat/shmdt)```，用於不相關進程的通訊；
2. ```POSIX shared memory(shm_open/shm_unlink)```，同樣不同進程通訊，相比接口更簡單；
3. ```mmap(), shared mappings``` 包括了 A) 匿名映射 (通過fork關聯)，B) 文件映射，通常用於不相關的進程通訊；

SYSV 和 POSIX 兩者提供功能基本類似，也就是 ```semaphores``` ```shared memory``` ```message queues```，不過由於 SYSV 的歷史原因使用更廣泛，不過兩者的實現基本相同。

{% highlight text %}
The POSIX shared memory object implementation on Linux 2.4 makes use of
a dedicated filesystem, which is normally mounted under /dev/shm.
{% endhighlight %}

也就是說，POSIX 共享內存是基於 tmpfs 來實現的，而 SYSV 共享內存在內核也是基於 tmpfs 實現的；從內核文檔 [tmpfs.txt]({{ site.kernel_docs_url }}/Documentation/filesystems/tmpfs.txt) 可以看到如下內容：

{% highlight text %}
1) There is always a kernel internal mount which you will not see at
   all. This is used for shared anonymous mappings and SYSV shared
   memory.

   This mount does not depend on CONFIG_TMPFS. If CONFIG_TMPFS is not
   set, the user visible part of tmpfs is not build. But the internal
   mechanisms are always present.

2) glibc 2.2 and above expects tmpfs to be mounted at /dev/shm for
   POSIX shared memory (shm_open, shm_unlink). Adding the following
   line to /etc/fstab should take care of this:

    tmpfs   /dev/shm    tmpfs   defaults    0 0

   Remember to create the directory that you intend to mount tmpfs on
   if necessary.

   This mount is _not_ needed for SYSV shared memory. The internal
   mount is used for that. (In the 2.3 kernel versions it was
   necessary to mount the predecessor of tmpfs (shm fs) to use SYSV
   shared memory)

3) Some people (including me) find it very convenient to mount it
   e.g. on /tmp and /var/tmp and have a big swap partition. And now
   loop mounts of tmpfs files do work, so mkinitrd shipped by most
   distributions should succeed with a tmpfs /tmp.
{% endhighlight %}

在上述的內容中，tmpfs 與共享內存相關的主要有兩個作用：

* SYSV 還有匿名內存映射，這部分由內核管理，用戶不可見；
* POSIX 由用戶負責 mount，而且一般 mount 到 ```/dev/shm```，依賴 ```CONFIG_TMPFS``` 。

### SYSV

可以通過示例 [github shmv.c]({{ site.example_repository }}/linux/memory/sharedmemory/shmv.c) 。

{% highlight text %}
----- 查看以及調整支持的最大內存，shmmax shmall shmmni
$ cat /proc/sys/kernel/shmmax
33554432

----- 修改為32M
# echo 33554432 > /proc/sys/kernel/shmmax

----- 嘗試創建65M的system V共享內存時失敗
$ ipcmk -M 68157440
ipcmk: create share memory failed: Invalid argument

----- 創建10M共享內存
$ ipcmk -M 10485760
Shared memory id: 19431492

----- 查看剛創建的共享內存
$ ipcs -m -i 19431492
Shared memory Segment shmid=19431492
uid=0   gid=0   cuid=0  cgid=0
mode=0644       access_perms=0644
bytes=10485760  lpid=0  cpid=28064      nattch=0
att_time=Not set
det_time=Not set
change_time=Sun May 28 10:15:50 2015

----- 刪除剛創建的資源
# ipcrm -m 19431492
{% endhighlight %}

注意，這裡看到的 system v 共享內存的大小並不受 ```/dev/shm``` 的影響。

<!--
System V共享內存把所有共享數據放在共享內存區，任何想要訪問該數據的進程都必須在本進程的地址空間新增一塊內存區域，用來映射存放共享數據的物理內存頁面。System V共享內存通過shmget函數獲得或創建一個IPC共享內存區域，並返回相應的標識符，內核在保證shmget獲得或創建一個共享內存區，初始化該共享內存區相應的shmid_kernel結構，同時還將在特殊文件系統shm中創建並打開一個同名文件，並在內存中建立起該文件的相應的dentry及inode結構，新打開的文件不屬於任何一個進程，所有這一切都是系統調用shmget函數完成的。
-->

#### 配置項

與共享內存相關的內容可以參考 ```/etc/sysctl.conf``` 文件中的配置，簡介如下：

{% highlight text %}
kernel.shmmax = 4398046511104
kernel.shmall = 1073741824     系統可用共享內存的總頁數
kernel.shmmni = 4096
{% endhighlight %}

<!-- http://www.361way.com/ipcs-shared-memory/5144.html -->

### POSIX

大多數的 Linux 發行版本中，內存盤默認使用的是 `/dev/shm` 路徑，文件系統類型為 tmpfs，默認大小是內存實際的大小，其大小可以通過 `df -h` 查看。

{% highlight text %}
----- 查看當前大小
$ df -h /dev/shm
Filesystem      Size  Used Avail Use% Mounted on
tmpfs           3.9G   17M  3.9G   1% /dev/shm

----- 修改掛載點的大小，然後重新掛載
$ cat /etc/fstab
tmpfs /dev/shm tmpfs defaults,size=4096M 0 0
# mount -o remount /dev/shm

----- 不用重啟重新掛載
# mount -o remount,size=256M /dev/shm
# mount -o size=1500M -o nr_inodes=1000000 -o noatime,nodiratime -o remount /dev/shm
{% endhighlight %}

可以通過示例 [github posix.c]({{ site.example_repository }}/linux/memory/sharedmemory/posix.c) ，將會申請 65M 左右的共享內存。

{% highlight text %}
$ df -h /dev/shm
Filesystem      Size  Used Avail Use% Mounted on
tmpfs           256M   82M  175M  32% /dev/shm
$ ls /dev/shm/shm1 -alh
-rw-r--r-- 1 andy andy 65M 5月  28 10:57 /dev/shm/shm1
$ stat /dev/shm/shm1
{% endhighlight %}

注意，如果申請的內存超過了限制，那麼會報 ```Bus error``` 的錯誤。

### 總結

雖然 System V 與 POSIX 共享內存都是通過 tmpfs 實現，但是兩個不同實例，對於 ```/proc/sys/kernel/shmmax``` 只會影響 SYS V 共享內存，```/dev/shm``` 只會影響 Posix 共享內存。

## tmpfs

tmpfs 是基於內存/交換分區的文件系統，ramdisk 是作為塊設備，基於 ext 的文件系統，所以不會繞過 page cache 的內存複製；而 tmpfs 直接操作內存做為文件系統的，而不是基於塊設備的。

其源碼實現在 `mm/shmem.c` 中，根據 `CONFIG_SHMEM` 是否配置，略微有所區別。

{% highlight c %}
static struct file_system_type shmem_fs_type = {
    .owner      = THIS_MODULE,
    .name       = "tmpfs",
    .mount      = shmem_mount,
    .kill_sb    = kill_litter_super,
    .fs_flags   = FS_USERNS_MOUNT,
};
{% endhighlight %}

在函數 `init_tmpfs()` 裡，實際會通過 `register_filesystem()` 函數將 tmpfs 註冊到文件系統中，在 `shmem_file_setup()` 中，更改了 `file->f_op = &shmem_file_operations;` 下面來看具體的結構體。

{% highlight c %}
static struct file_operations shmem_file_operations = {
    .mmap       = shmem_mmap,
#ifdef CONFIG_TMPFS
    .llseek     = generic_file_llseek,
    .read       = shmem_file_read,
    .write      = shmem_file_write,
    .fsync      = simple_sync_file,
    .sendfile   = shmem_file_sendfile,
#endif
};
{% endhighlight %}

也就是說在操作在 tmpfs 文件時候，並沒有使用常用的 ext 文件系統中的函數 `do_sync_read()`，而是調用了 tmpfs 自己封裝的函數 `shmem_file_read()`，當然在 `shmem_file_read()` 並沒有對 page cache 進行操作，雖然裡面還是使用了 page cache 中 maping、file、inode 等結構體和算法。


<!--
3. 函數shmem_file_read主要是調用do_shmem_file_read函數，在do_shmem_file_read函數中核心是shmem_getpage，通過索引和inode快速找到page.
-->

## 參考

<!--
[淺析Linux的共享內存與tmpfs文件系統](http://hustcat.github.io/shared-memory-tmpfs/)
-->

{% highlight text %}
{% endhighlight %}
