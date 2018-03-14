---
title: Kernel 映射文件
layout: post
comments: true
language: chinese
category: [linux,misc]
keywords: linux,內存,memory,映射,mmap
description: mmap 是一種內存映射文件的方法，即將一個文件或者其它對象映射到進程的地址空間，實現文件磁盤地址和進程虛擬地址空間中一段虛擬地址的一一對映關係。這裡簡單介紹下如何使用。
---

mmap 是一種內存映射文件的方法，即將一個文件或者其它對象映射到進程的地址空間，實現文件磁盤地址和進程虛擬地址空間中一段虛擬地址的一一對映關係。

這裡簡單介紹下如何使用。

<!-- more -->

## 簡介

通過 `mmap()` 實現映射後，進程就可以採用指針的方式讀寫這段內存，而系統會回寫髒頁面到對應的文件磁盤上，而無需再調用 `read()`、`write()` 等系統調用函數。

另外，內核空間對這段區域的修改也直接反映用戶空間，從而可以實現不同進程間的文件共享。

![memory mmap introduce]({{ site.url }}/images/linux/kernel/memory-mmap-introduce.png "memory mmap introduce"){: .pull-center width="70%" }

如上圖所示，進程的虛擬地址空間，由多個虛擬內存區域構成。虛擬內存區域是進程的虛擬地址空間中的一個同質區間，即具有同樣特性的連續地址範圍，如上圖所示 代碼段、初始數據段、BSS數據段、堆、棧和內存映射，都是一個獨立的虛擬內存區域。

內存映射的地址空間處在堆棧之間的空餘部分。

### vm_area_struct

內核中使用 `struct vm_area_struct` 來表示一個獨立的虛擬內存區域，由於每個不同質的虛擬內存區域功能和內部機制都不同，因此一個進程使用多個 `vm_area_struct` 結構來分別表示不同類型的虛擬內存區域。

各個 `vm_area_struct` 結構使用鏈表或者樹形結構鏈接，方便進程快速訪問，如下圖所示：

![memory vm area struct]({{ site.url }}/images/linux/kernel/memory-vm-area-struct.png "memory vm area struct"){: .pull-center width="70%" }

`vm_area_struct` 結構中包含區域起始和終止地址以及其他相關信息，同時也包含一個 `vm_ops` 指針，包括了所有針對這個區域可以使用的系統調用函數。這樣，進程對某一虛擬內存區域的任何操作需要用要的信息，都可以從 `vm_area_struct` 中獲得。

`mmap()` 就是要創建一個新的 `vm_area_struct` 結構，並將其與文件的物理磁盤地址相連。

## 使用示例

內存映射有幾種應用場景，可以將文件或者設備 IO 映射到內存，從而提高 IO 效率，可以將用戶內存空間映射到內核的內存空間。

內核中，通過 ```ioremap()``` 映射設備 IO 地址空間以供內核訪問，kmap 映射申請的高端內存，DMA 使用比較多的是網卡驅動裡 ring buffer 機制。

{% highlight text %}
#include <sys/mman.h>
void *mmap(void *addr, size_t length, int prot, int flags, int fd, off_t offset);
int munmap(void *addr, size_t length);

addr  : 用戶進程中要映射的某段內存區域的起始地址，通常為NULL由內核來指定
length: 要映射的內存區域的大小
prot  : 設置頁面的保護級別，注意不要與文件打開模式衝突
  PROT_EXEC  頁可執行
  PROT_READ  頁可讀
  PROT_WRITE 頁可寫
  PROT_NONE  頁不可以訪問
flags : 映射對象的類型
  MAP_SHARED  共享內存映射，更新此內存對其它進程可見，而且會同步到文件，調用msync後寫入文件；
  MAP_PRIVATE 創建COW映射，更新時對其它進程不可見，而且不會更新到文件；
fd    : 文件描述符，通常由open函數返回
offset: 映射的用戶空間的內存區域在內核空間中已經分配好的的內存區域中的偏移。大小為PAGE_SIZE的整數倍
{% endhighlight %}

### 文件映射

可以通過 mmap 方式修改一個文件：A) 打開文件，獲取對應的文件描述符；B) 通過 ```mmap()``` 將對應的文件映射到內存；C) 現在可以關閉文件，然後直接通過內存修改；D) 通過 ```msync()``` 將內存的內容同步到文件，然後 ```munmap()``` 。

可以參考示例程序 [filemap.c]( {{ site.example_repository }}/linux/memory/mmap/0-filemap.c)，該程序會讀取 file.txt 文件並修改其中的一個字符。

### 共享內存

通過 mmap 可以實現 A) 文件映射，通常用於不相關的進程通訊；B) 匿名映射 (通過fork關聯) 。

#### 文件映射

可以參考示例程序 [sharemem1.c]( {{ site.example_repository }}/linux/memory/mmap/1-sharemem1.c) 以及 [sharemem2.c]( {{ site.example_repository }}/linux/memory/mmap/1-sharemem2.c) 。

兩個進程之間通過 mmap 共享內存，其中 ```sharemem1``` 會先初始化內存，然後睡眠 11 秒，此時可以通過 ```sharemem2``` 讀取其中的內容；11 秒過後，將文件內容重新設置為 0 。

當 ```sharemem1``` 輸出 ```initialize over``` 後，查詢的信息不再是 0 ，而隨後會再次設置為 0。

#### 匿名映射

使用匿名映射，常用在父子進程中通訊，可以參考示例程序 [anonymem.c]( {{ site.example_repository }}/linux/memory/mmap/2-anonymem.c) 。

### 內核模塊

另外，可以將用戶空間的一段內存區域映射到內核空間，然後用戶對這段內存區域的修改可以直接反映到內核空間，相反，內核空間對這段區域的修改也直接反映用戶空間。

首先，驅動先分配好一段內存，接著用戶通過 ```mmap()``` 映射到內核空間，內核經過一系列函數調用後調用對應的驅動程序的 ```file_operation``` 中的 ```mmap()``` 回調函數，在該函數中調用 ```remap_pfn_range()``` 來建立映射關係。

內核示例可以參考 [kernel/hello.c]( {{ site.example_repository }}/linux/memory/mmap/kernel/hello.c) 以及應用層的示例程序 [kernel/app.c]( {{ site.example_repository }}/linux/memory/mmap/kernel/app.c) 。


<!--
### 實現原理

簡單來說，就是內核是如何保證各個進程尋址到同一個共享內存區域的內存頁面。

在 Linux 中，內存頁面的所有信息通過 ```struct page``` 表示，該結構體在 ```include/linux/mm_types.h``` 中定義，其中有一個域指針 mapping 。

{% highlight c %}
struct page {
    unsigned long flags;
    union {
        struct address_space *mapping;
        void *s_mem;
    };
}
{% endhighlight %}
-->

### 注意事項

1. 使用 mmap() 時，映射區域大小必須是物理頁大小的整倍數，因為內存的最小粒度是頁，而進程虛擬地址空間和內存的映射也是以頁為單位。
2. 內核可以跟蹤被內存映射的底層文件的大小，進程可以合法的訪問在當前文件大小以內又在內存映射區以內的那些字節。<!--也就是說，如果文件的大小一直在擴張，只要在映射區域範圍內的數據，進程都可以合法得到，這和映射建立時文件的大小無關。具體情形參見“情形三”。-->
3. 映射建立之後，即使文件關閉，映射依然存在，因為映射的是磁盤地址，不是文件本身，和文件句柄無關。

#### 情形一

文件大小是 5000 字節，mmap() 從一個文件的起始位置開始，映射 5000 字節到虛擬內存中。

因為單位物理頁面的大小是 4096 字節，雖然被映射的文件只有 5000 字節，但是對應到進程虛擬地址區域的大小需要滿足整頁大小，因此 mmap() 執行後，實際映射到虛擬內存區域 8192 個字節，而 5000~8191 的字節部分用零填充。

映射後的對應關係如下所示：

{% highlight text %}
                    filesize(5000)
  +----------------------+
  |        file          |
  +----------------------+------------------+
  |    memory mapping    |   zero padding   |
  +----------------------+------------------+
  0                    4999                8191
  |<---------   mmap() size   ------------->|--->SIGSEGV Signal
{% endhighlight %}

1. 讀寫前 5000 個字節 (0~4999) 會返回操作文件內容。
2. 讀字節 5000~8191 時，結果全為 0 ；寫時進程不會報錯，但是所寫的內容不會寫入原文件中 。
3. 讀寫 8192 以外的磁盤部分，會返回一個 SIGSECV 錯誤。

#### 情形二

一個文件的大小是 5000 字節，mmap() 從一個文件的起始位置開始，映射 15000 字節到虛擬內存中，即映射大小超過了原始文件的大小。

與上類似，由於文件的大小是 5000 字節，對應兩個物理頁，其行為與上相同。而由於程序要求映射 15000 字節，文件只佔兩個物理頁，因此 8192~15000 字節都不能讀寫，操作時會返回異常。

{% highlight text %}
                    filesize(5000)
  +----------------------+
  |        file          |
  +----------------------+------------------+------------~~~~------------+
  |    memory mapping    |   zero padding   |     no memory mapping      |
  +----------------------+------------------+------------~~~~------------+
  0                    4999                8191
  |<---------   mmap() size   ------------->|<------ SIGBUS Signal ----->|---> SIGSEGV Signal
{% endhighlight %}

1. 讀寫前 5000 個字節 (0~4999) 會返回操作文件內容。
2. 讀字節 5000~8191 時，結果全為 0 ；寫時進程不會報錯，但是所寫的內容不會寫入原文件中 。
3. 對於 8192~14999 字節，進程不能對其進行讀寫，會報 SIGBUS 錯誤。
3. 讀寫 15000 以外的磁盤部分，會返回一個 SIGSECV 錯誤。


<!--
情形三：一個文件初始大小為0，使用mmap操作映射了1000*4K的大小，即1000個物理頁大約4M字節空間，mmap返回指針ptr。

分析：如果在映射建立之初，就對文件進行讀寫操作，由於文件大小為0，並沒有合法的物理頁對應，如同情形二一樣，會返回SIGBUS錯誤。

但是如果，每次操作ptr讀寫前，先增加文件的大小，那麼ptr在文件大小內部的操作就是合法的。例如，文件擴充4096字節，ptr就能操作ptr ~ [ (char)ptr + 4095]的空間。只要文件擴充的範圍在1000個物理頁（映射範圍）內，ptr都可以對應操作相同的大小。

這樣，方便隨時擴充文件空間，隨時寫入文件，不造成空間浪費。
-->


## 內核實現

對應到內核中的接口是 `sys_mmap()` 實際上最終調用的是 `sys_mmap_pgoff()` 函數，當然實際上這也是一個系統調用函數。

### 創建映射

首先，進程啟動映射過程，並在虛擬地址空間中為映射創建虛擬映射區域。

{% highlight text %}
sys_mmap_pgoff()
 |-audit_mmap_fd()+fget()       如果是非匿名映射，那麼先獲取文件描述符
 |-vm_mmap_pgoff()
   |-security_mmap_file()       權限檢查
   |-down_write()
   |-do_mmap_pgoff()
   | |-PAGE_ALIGN()             會做若干檢查判斷是否超過限制，包括sysctl_max_map_count
   | |-get_unmapped_area()      從用戶地址空間尋找個合適的地址
   | |-calc_vm_prot_bits()      根據prot和flags計算獲取vm_flags
   | |-calc_vm_flag_bits()
   | |-can_do_mlock()           判斷是否需要鎖定內存
   | |-mlock_future_check()     <<<<1>>>>該函數以下會判斷文件映射還是匿名映射
   | |-mmap_region()            真正的映射函數
   |   |-may_expand_vm()        當前頁面數+新分配的頁面數不能超過進程對應的資源限制
   |   |-vma_merge()            嘗試是否可以合併，可以就不需要新分配一個vm_area_struct
   |   |-kmem_cache_zalloc()    不能合併，則分配一個vm_area_struct來管理這個區域
   |   |-shmem_zero_setup()     如果是匿名映tat_accountk且是共享匿名區，那麼通過該函數初始化
   |   |-vm_stat_account()      統計所有分配頁保存在mm->total_vm中
   |-up_write()
   |-mm_populate()
{% endhighlight %}

內存鎖定的含義是，分配的內存始終位於真實內存之中，從不被置換 (swap) 出去，在應用層可以通過 `mlock()` 函數實現。

`mlock()` 會鎖定開始於地址 addr 並延續長度為 len 個地址範圍的內存，調用成功返回後所有包含該地址範圍的分頁都保證在 RAM 內，這些分頁保證一直在 RAM 內直到後來被解鎖。

在調用 `mlock_future_check()` 函數後，會判斷文件映射還是匿名映射。

#### 文件映射

接著看下如果是文件映射的情況。

{% highlight c %}
if (file) {
    struct inode *inode = file_inode(file);

    switch (flags & MAP_TYPE) {
    case MAP_SHARED:  /* 進程之間共享，可以同步到文件 */
        /* 如果內存區允許寫，但是磁盤文件不允許修改，這就衝突了，必須返回錯誤 */
        if ((prot&PROT_WRITE) && !(file->f_mode&FMODE_WRITE))
            return -EACCES;

        /* Make sure we don't allow writing to an append-only file.. */
        if (IS_APPEND(inode) && (file->f_mode & FMODE_WRITE))
            return -EACCES;

        /* Make sure there are no mandatory locks on the file. */
        if (locks_verify_locked(file))
            return -EAGAIN;

        vm_flags |= VM_SHARED | VM_MAYSHARE;
        if (!(file->f_mode & FMODE_WRITE))
            vm_flags &= ~(VM_MAYWRITE | VM_SHARED);

        /* fall through */
    case MAP_PRIVATE:
        if (!(file->f_mode & FMODE_READ))
            return -EACCES;
        if (file->f_path.mnt->mnt_flags & MNT_NOEXEC) {
            if (vm_flags & VM_EXEC)
                return -EPERM;
            vm_flags &= ~VM_MAYEXEC;
        }

        if (!file->f_op->mmap)
            return -ENODEV;
        if (vm_flags & (VM_GROWSDOWN|VM_GROWSUP))
            return -EINVAL;
        break;

    default:
        return -EINVAL;
    }
}
{% endhighlight %}

匿名映射的情況暫時不討論，剩下的工作交給 `mmap_region()` 函數處理。

<!--
### 實現映射

調用內核空間的系統調用函數mmap（不同於用戶空間函數），實現文件物理地址和進程虛擬地址的一一映射關係

5、為映射分配了新的虛擬地址區域後，通過待映射的文件指針，在文件描述符表中找到對應的文件描述符，通過文件描述符，鏈接到內核“已打開文件集”中該文件的文件結構體（struct file），每個文件結構體維護著和這個已打開文件相關各項信息。

6、通過該文件的文件結構體，鏈接到file_operations模塊，調用內核函數mmap，其原型為：int mmap(struct file *filp, struct vm_area_struct *vma)，不同於用戶空間庫函數。

7、內核mmap函數通過虛擬文件系統inode模塊定位到文件磁盤物理地址。

8、通過remap_pfn_range函數建立頁表，即實現了文件地址和虛擬地址區域的映射關係。此時，這片虛擬地址並沒有任何數據關聯到主存中。

### 缺頁異常

當進程發起對這片映射空間的訪問，引發缺頁異常，實現文件內容到物理內存（主存）的拷貝

注：前兩個階段僅在於創建虛擬區間並完成地址映射，但是並沒有將任何文件數據的拷貝至主存。真正的文件讀取是當進程發起讀或寫操作時。

9、進程的讀或寫操作訪問虛擬地址空間這一段映射地址，通過查詢頁表，發現這一段地址並不在物理頁面上。因為目前只建立了地址映射，真正的硬盤數據還沒有拷貝到內存中，因此引發缺頁異常。

10、缺頁異常進行一系列判斷，確定無非法操作後，內核發起請求調頁過程。

11、調頁過程先在交換緩存空間（swap cache）中尋找需要訪問的內存頁，如果沒有則調用nopage函數把所缺的頁從磁盤裝入到主存中。

12、之後進程即可對這片主存進行讀或者寫的操作，如果寫操作改變了其內容，一定時間後系統會自動回寫髒頁面到對應磁盤地址，也即完成了寫入到文件的過程。

注：修改過的髒頁面並不會立即更新迴文件中，而是有一段時間的延遲，可以調用msync()來強制同步, 這樣所寫的內容就能立即保存到文件裡了。
-->

## mmap VS. read

簡單來說就是 mmap() 和常規文件系統操作 (調用 read()、write() 等函數) 的區別，常規文件系統的操作為：

1. 進程發起讀文件請求。
2. 內核查找進程文件符表，定位到內核已打開文件集上的文件信息，從而找到此文件的 inode 。
3. inode 在 address_space 上查找要請求的文件頁是否已經緩存在頁緩存中，如果存在，則直接返回這片文件頁的內容。
4. 如果不存在，則通過 inode 定位到文件磁盤地址，將數據從磁盤複製到頁緩存；之後再次發起讀頁面過程，進而將頁緩存中的數據發給用戶進程。

也即是說，為了提高效率，使用了頁緩存機制，那麼再讀文件時需要先將文件頁從磁盤拷貝到頁緩存中，由於頁緩存處在內核空間，不能被用戶進程直接尋址，所以還需要將頁緩存中數據頁再次拷貝到內存對應的用戶空間中。

這樣，通過了兩次數據拷貝過程，才能完成進程對文件內容的讀取；而寫操作也基本類似，需要經過兩次數據拷貝。

### 映射調用

使用 mmap() 操作文件時，在創建虛擬內存區域，以及建立文件磁盤地址和虛擬內存區域映射的這兩步中，沒有任何文件拷貝操作。

而之後訪問數據時發現內存中並無數據而發起的缺頁異常過程，可以通過已經建立好的映射關係，只使用一次數據拷貝，就從磁盤中將數據傳入內存的用戶空間中，供進程使用。

簡言之，常規文件操作需要從磁盤到頁緩存再到用戶主存的兩次數據拷貝；而 mmap() 只需要從磁盤到用戶主存的一次數據拷貝過程。

<!--
 由上文討論可知，mmap優點共有一下幾點：
 1、對文件的讀取操作跨過了頁緩存，減少了數據的拷貝次數，用內存讀寫取代I/O讀寫，提高了文件讀取效率。
 2、實現了用戶空間和內核空間的高效交互方式。兩空間的各自修改操作可以直接反映在映射的區域內，從而被對方空間及時捕捉。
 3、提供進程間共享內存及相互通信的方式。不管是父子進程還是無親緣關係的進程，都可以將自身用戶空間映射到同一個文件或匿名映射到同一片區域。從而通過各自對映射區域的改動，達到進程間通信和進程間共享的目的。
同時，如果進程A和進程B都映射了區域C，當A第一次讀取C時通過缺頁從磁盤複製文件頁到內存中；但當B再讀C的相同頁面時，雖然也會產生缺頁異常，但是不再需要從磁盤中複製文件過來，而可直接使用已經保存在內存中的文件數據。

4、可用於實現高效的大規模數據傳輸。內存空間不足，是制約大數據操作的一個方面，解決方案往往是藉助硬盤空間協助操作，補充內存的不足。但是進一步會造成大量的文件I/O操作，極大影響效率。這個問題可以通過mmap映射很好的解決。換句話說，但凡是需要用磁盤空間代替內存的時候，mmap都可以發揮其功效。
-->




{% highlight text %}
{% endhighlight %}
