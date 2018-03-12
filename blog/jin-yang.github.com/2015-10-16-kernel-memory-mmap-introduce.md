---
title: Kernel 映射文件
layout: post
comments: true
language: chinese
category: [linux,misc]
keywords: linux,内存,memory,映射,mmap
description: mmap 是一种内存映射文件的方法，即将一个文件或者其它对象映射到进程的地址空间，实现文件磁盘地址和进程虚拟地址空间中一段虚拟地址的一一对映关系。这里简单介绍下如何使用。
---

mmap 是一种内存映射文件的方法，即将一个文件或者其它对象映射到进程的地址空间，实现文件磁盘地址和进程虚拟地址空间中一段虚拟地址的一一对映关系。

这里简单介绍下如何使用。

<!-- more -->

## 简介

通过 `mmap()` 实现映射后，进程就可以采用指针的方式读写这段内存，而系统会回写脏页面到对应的文件磁盘上，而无需再调用 `read()`、`write()` 等系统调用函数。

另外，内核空间对这段区域的修改也直接反映用户空间，从而可以实现不同进程间的文件共享。

![memory mmap introduce]({{ site.url }}/images/linux/kernel/memory-mmap-introduce.png "memory mmap introduce"){: .pull-center width="70%" }

如上图所示，进程的虚拟地址空间，由多个虚拟内存区域构成。虚拟内存区域是进程的虚拟地址空间中的一个同质区间，即具有同样特性的连续地址范围，如上图所示 代码段、初始数据段、BSS数据段、堆、栈和内存映射，都是一个独立的虚拟内存区域。

内存映射的地址空间处在堆栈之间的空余部分。

### vm_area_struct

内核中使用 `struct vm_area_struct` 来表示一个独立的虚拟内存区域，由于每个不同质的虚拟内存区域功能和内部机制都不同，因此一个进程使用多个 `vm_area_struct` 结构来分别表示不同类型的虚拟内存区域。

各个 `vm_area_struct` 结构使用链表或者树形结构链接，方便进程快速访问，如下图所示：

![memory vm area struct]({{ site.url }}/images/linux/kernel/memory-vm-area-struct.png "memory vm area struct"){: .pull-center width="70%" }

`vm_area_struct` 结构中包含区域起始和终止地址以及其他相关信息，同时也包含一个 `vm_ops` 指针，包括了所有针对这个区域可以使用的系统调用函数。这样，进程对某一虚拟内存区域的任何操作需要用要的信息，都可以从 `vm_area_struct` 中获得。

`mmap()` 就是要创建一个新的 `vm_area_struct` 结构，并将其与文件的物理磁盘地址相连。

## 使用示例

内存映射有几种应用场景，可以将文件或者设备 IO 映射到内存，从而提高 IO 效率，可以将用户内存空间映射到内核的内存空间。

内核中，通过 ```ioremap()``` 映射设备 IO 地址空间以供内核访问，kmap 映射申请的高端内存，DMA 使用比较多的是网卡驱动里 ring buffer 机制。

{% highlight text %}
#include <sys/mman.h>
void *mmap(void *addr, size_t length, int prot, int flags, int fd, off_t offset);
int munmap(void *addr, size_t length);

addr  : 用户进程中要映射的某段内存区域的起始地址，通常为NULL由内核来指定
length: 要映射的内存区域的大小
prot  : 设置页面的保护级别，注意不要与文件打开模式冲突
  PROT_EXEC  页可执行
  PROT_READ  页可读
  PROT_WRITE 页可写
  PROT_NONE  页不可以访问
flags : 映射对象的类型
  MAP_SHARED  共享内存映射，更新此内存对其它进程可见，而且会同步到文件，调用msync后写入文件；
  MAP_PRIVATE 创建COW映射，更新时对其它进程不可见，而且不会更新到文件；
fd    : 文件描述符，通常由open函数返回
offset: 映射的用户空间的内存区域在内核空间中已经分配好的的内存区域中的偏移。大小为PAGE_SIZE的整数倍
{% endhighlight %}

### 文件映射

可以通过 mmap 方式修改一个文件：A) 打开文件，获取对应的文件描述符；B) 通过 ```mmap()``` 将对应的文件映射到内存；C) 现在可以关闭文件，然后直接通过内存修改；D) 通过 ```msync()``` 将内存的内容同步到文件，然后 ```munmap()``` 。

可以参考示例程序 [filemap.c]( {{ site.example_repository }}/linux/memory/mmap/0-filemap.c)，该程序会读取 file.txt 文件并修改其中的一个字符。

### 共享内存

通过 mmap 可以实现 A) 文件映射，通常用于不相关的进程通讯；B) 匿名映射 (通过fork关联) 。

#### 文件映射

可以参考示例程序 [sharemem1.c]( {{ site.example_repository }}/linux/memory/mmap/1-sharemem1.c) 以及 [sharemem2.c]( {{ site.example_repository }}/linux/memory/mmap/1-sharemem2.c) 。

两个进程之间通过 mmap 共享内存，其中 ```sharemem1``` 会先初始化内存，然后睡眠 11 秒，此时可以通过 ```sharemem2``` 读取其中的内容；11 秒过后，将文件内容重新设置为 0 。

当 ```sharemem1``` 输出 ```initialize over``` 后，查询的信息不再是 0 ，而随后会再次设置为 0。

#### 匿名映射

使用匿名映射，常用在父子进程中通讯，可以参考示例程序 [anonymem.c]( {{ site.example_repository }}/linux/memory/mmap/2-anonymem.c) 。

### 内核模块

另外，可以将用户空间的一段内存区域映射到内核空间，然后用户对这段内存区域的修改可以直接反映到内核空间，相反，内核空间对这段区域的修改也直接反映用户空间。

首先，驱动先分配好一段内存，接着用户通过 ```mmap()``` 映射到内核空间，内核经过一系列函数调用后调用对应的驱动程序的 ```file_operation``` 中的 ```mmap()``` 回调函数，在该函数中调用 ```remap_pfn_range()``` 来建立映射关系。

内核示例可以参考 [kernel/hello.c]( {{ site.example_repository }}/linux/memory/mmap/kernel/hello.c) 以及应用层的示例程序 [kernel/app.c]( {{ site.example_repository }}/linux/memory/mmap/kernel/app.c) 。


<!--
### 实现原理

简单来说，就是内核是如何保证各个进程寻址到同一个共享内存区域的内存页面。

在 Linux 中，内存页面的所有信息通过 ```struct page``` 表示，该结构体在 ```include/linux/mm_types.h``` 中定义，其中有一个域指针 mapping 。

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

### 注意事项

1. 使用 mmap() 时，映射区域大小必须是物理页大小的整倍数，因为内存的最小粒度是页，而进程虚拟地址空间和内存的映射也是以页为单位。
2. 内核可以跟踪被内存映射的底层文件的大小，进程可以合法的访问在当前文件大小以内又在内存映射区以内的那些字节。<!--也就是说，如果文件的大小一直在扩张，只要在映射区域范围内的数据，进程都可以合法得到，这和映射建立时文件的大小无关。具体情形参见“情形三”。-->
3. 映射建立之后，即使文件关闭，映射依然存在，因为映射的是磁盘地址，不是文件本身，和文件句柄无关。

#### 情形一

文件大小是 5000 字节，mmap() 从一个文件的起始位置开始，映射 5000 字节到虚拟内存中。

因为单位物理页面的大小是 4096 字节，虽然被映射的文件只有 5000 字节，但是对应到进程虚拟地址区域的大小需要满足整页大小，因此 mmap() 执行后，实际映射到虚拟内存区域 8192 个字节，而 5000~8191 的字节部分用零填充。

映射后的对应关系如下所示：

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

1. 读写前 5000 个字节 (0~4999) 会返回操作文件内容。
2. 读字节 5000~8191 时，结果全为 0 ；写时进程不会报错，但是所写的内容不会写入原文件中 。
3. 读写 8192 以外的磁盘部分，会返回一个 SIGSECV 错误。

#### 情形二

一个文件的大小是 5000 字节，mmap() 从一个文件的起始位置开始，映射 15000 字节到虚拟内存中，即映射大小超过了原始文件的大小。

与上类似，由于文件的大小是 5000 字节，对应两个物理页，其行为与上相同。而由于程序要求映射 15000 字节，文件只占两个物理页，因此 8192~15000 字节都不能读写，操作时会返回异常。

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

1. 读写前 5000 个字节 (0~4999) 会返回操作文件内容。
2. 读字节 5000~8191 时，结果全为 0 ；写时进程不会报错，但是所写的内容不会写入原文件中 。
3. 对于 8192~14999 字节，进程不能对其进行读写，会报 SIGBUS 错误。
3. 读写 15000 以外的磁盘部分，会返回一个 SIGSECV 错误。


<!--
情形三：一个文件初始大小为0，使用mmap操作映射了1000*4K的大小，即1000个物理页大约4M字节空间，mmap返回指针ptr。

分析：如果在映射建立之初，就对文件进行读写操作，由于文件大小为0，并没有合法的物理页对应，如同情形二一样，会返回SIGBUS错误。

但是如果，每次操作ptr读写前，先增加文件的大小，那么ptr在文件大小内部的操作就是合法的。例如，文件扩充4096字节，ptr就能操作ptr ~ [ (char)ptr + 4095]的空间。只要文件扩充的范围在1000个物理页（映射范围）内，ptr都可以对应操作相同的大小。

这样，方便随时扩充文件空间，随时写入文件，不造成空间浪费。
-->


## 内核实现

对应到内核中的接口是 `sys_mmap()` 实际上最终调用的是 `sys_mmap_pgoff()` 函数，当然实际上这也是一个系统调用函数。

### 创建映射

首先，进程启动映射过程，并在虚拟地址空间中为映射创建虚拟映射区域。

{% highlight text %}
sys_mmap_pgoff()
 |-audit_mmap_fd()+fget()       如果是非匿名映射，那么先获取文件描述符
 |-vm_mmap_pgoff()
   |-security_mmap_file()       权限检查
   |-down_write()
   |-do_mmap_pgoff()
   | |-PAGE_ALIGN()             会做若干检查判断是否超过限制，包括sysctl_max_map_count
   | |-get_unmapped_area()      从用户地址空间寻找个合适的地址
   | |-calc_vm_prot_bits()      根据prot和flags计算获取vm_flags
   | |-calc_vm_flag_bits()
   | |-can_do_mlock()           判断是否需要锁定内存
   | |-mlock_future_check()     <<<<1>>>>该函数以下会判断文件映射还是匿名映射
   | |-mmap_region()            真正的映射函数
   |   |-may_expand_vm()        当前页面数+新分配的页面数不能超过进程对应的资源限制
   |   |-vma_merge()            尝试是否可以合并，可以就不需要新分配一个vm_area_struct
   |   |-kmem_cache_zalloc()    不能合并，则分配一个vm_area_struct来管理这个区域
   |   |-shmem_zero_setup()     如果是匿名映tat_accountk且是共享匿名区，那么通过该函数初始化
   |   |-vm_stat_account()      统计所有分配页保存在mm->total_vm中
   |-up_write()
   |-mm_populate()
{% endhighlight %}

内存锁定的含义是，分配的内存始终位于真实内存之中，从不被置换 (swap) 出去，在应用层可以通过 `mlock()` 函数实现。

`mlock()` 会锁定开始于地址 addr 并延续长度为 len 个地址范围的内存，调用成功返回后所有包含该地址范围的分页都保证在 RAM 内，这些分页保证一直在 RAM 内直到后来被解锁。

在调用 `mlock_future_check()` 函数后，会判断文件映射还是匿名映射。

#### 文件映射

接着看下如果是文件映射的情况。

{% highlight c %}
if (file) {
    struct inode *inode = file_inode(file);

    switch (flags & MAP_TYPE) {
    case MAP_SHARED:  /* 进程之间共享，可以同步到文件 */
        /* 如果内存区允许写，但是磁盘文件不允许修改，这就冲突了，必须返回错误 */
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

匿名映射的情况暂时不讨论，剩下的工作交给 `mmap_region()` 函数处理。

<!--
### 实现映射

调用内核空间的系统调用函数mmap（不同于用户空间函数），实现文件物理地址和进程虚拟地址的一一映射关系

5、为映射分配了新的虚拟地址区域后，通过待映射的文件指针，在文件描述符表中找到对应的文件描述符，通过文件描述符，链接到内核“已打开文件集”中该文件的文件结构体（struct file），每个文件结构体维护着和这个已打开文件相关各项信息。

6、通过该文件的文件结构体，链接到file_operations模块，调用内核函数mmap，其原型为：int mmap(struct file *filp, struct vm_area_struct *vma)，不同于用户空间库函数。

7、内核mmap函数通过虚拟文件系统inode模块定位到文件磁盘物理地址。

8、通过remap_pfn_range函数建立页表，即实现了文件地址和虚拟地址区域的映射关系。此时，这片虚拟地址并没有任何数据关联到主存中。

### 缺页异常

当进程发起对这片映射空间的访问，引发缺页异常，实现文件内容到物理内存（主存）的拷贝

注：前两个阶段仅在于创建虚拟区间并完成地址映射，但是并没有将任何文件数据的拷贝至主存。真正的文件读取是当进程发起读或写操作时。

9、进程的读或写操作访问虚拟地址空间这一段映射地址，通过查询页表，发现这一段地址并不在物理页面上。因为目前只建立了地址映射，真正的硬盘数据还没有拷贝到内存中，因此引发缺页异常。

10、缺页异常进行一系列判断，确定无非法操作后，内核发起请求调页过程。

11、调页过程先在交换缓存空间（swap cache）中寻找需要访问的内存页，如果没有则调用nopage函数把所缺的页从磁盘装入到主存中。

12、之后进程即可对这片主存进行读或者写的操作，如果写操作改变了其内容，一定时间后系统会自动回写脏页面到对应磁盘地址，也即完成了写入到文件的过程。

注：修改过的脏页面并不会立即更新回文件中，而是有一段时间的延迟，可以调用msync()来强制同步, 这样所写的内容就能立即保存到文件里了。
-->

## mmap VS. read

简单来说就是 mmap() 和常规文件系统操作 (调用 read()、write() 等函数) 的区别，常规文件系统的操作为：

1. 进程发起读文件请求。
2. 内核查找进程文件符表，定位到内核已打开文件集上的文件信息，从而找到此文件的 inode 。
3. inode 在 address_space 上查找要请求的文件页是否已经缓存在页缓存中，如果存在，则直接返回这片文件页的内容。
4. 如果不存在，则通过 inode 定位到文件磁盘地址，将数据从磁盘复制到页缓存；之后再次发起读页面过程，进而将页缓存中的数据发给用户进程。

也即是说，为了提高效率，使用了页缓存机制，那么再读文件时需要先将文件页从磁盘拷贝到页缓存中，由于页缓存处在内核空间，不能被用户进程直接寻址，所以还需要将页缓存中数据页再次拷贝到内存对应的用户空间中。

这样，通过了两次数据拷贝过程，才能完成进程对文件内容的读取；而写操作也基本类似，需要经过两次数据拷贝。

### 映射调用

使用 mmap() 操作文件时，在创建虚拟内存区域，以及建立文件磁盘地址和虚拟内存区域映射的这两步中，没有任何文件拷贝操作。

而之后访问数据时发现内存中并无数据而发起的缺页异常过程，可以通过已经建立好的映射关系，只使用一次数据拷贝，就从磁盘中将数据传入内存的用户空间中，供进程使用。

简言之，常规文件操作需要从磁盘到页缓存再到用户主存的两次数据拷贝；而 mmap() 只需要从磁盘到用户主存的一次数据拷贝过程。

<!--
 由上文讨论可知，mmap优点共有一下几点：
 1、对文件的读取操作跨过了页缓存，减少了数据的拷贝次数，用内存读写取代I/O读写，提高了文件读取效率。
 2、实现了用户空间和内核空间的高效交互方式。两空间的各自修改操作可以直接反映在映射的区域内，从而被对方空间及时捕捉。
 3、提供进程间共享内存及相互通信的方式。不管是父子进程还是无亲缘关系的进程，都可以将自身用户空间映射到同一个文件或匿名映射到同一片区域。从而通过各自对映射区域的改动，达到进程间通信和进程间共享的目的。
同时，如果进程A和进程B都映射了区域C，当A第一次读取C时通过缺页从磁盘复制文件页到内存中；但当B再读C的相同页面时，虽然也会产生缺页异常，但是不再需要从磁盘中复制文件过来，而可直接使用已经保存在内存中的文件数据。

4、可用于实现高效的大规模数据传输。内存空间不足，是制约大数据操作的一个方面，解决方案往往是借助硬盘空间协助操作，补充内存的不足。但是进一步会造成大量的文件I/O操作，极大影响效率。这个问题可以通过mmap映射很好的解决。换句话说，但凡是需要用磁盘空间代替内存的时候，mmap都可以发挥其功效。
-->




{% highlight text %}
{% endhighlight %}
