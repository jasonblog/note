---
title: 内存-用户空间
layout: post
comments: true
language: chinese
category: [linux]
keywords: linux,内存,用户空间
description: 简单介绍下 Linux 中用户空间的内存管理，包括了内存的布局、内存申请等操作。
---

简单介绍下 Linux 中用户空间的内存管理，包括了内存的布局、内存申请等操作。

<!-- more -->

## 内存布局

关于 Linux 的内存分布可以查看内核文档 [x86/x86_64/mm.txt]( {{ site.kernel_docs_url }}/Documentation/x86/x86_64/mm.txt)，也就是低 128T 为用户空间。

{% highlight text %}
0000000000000000 - 00007fffffffffff (=47 bits) user space, different per mm
{% endhighlight %}

在内核中通过 ```TASK_SIZE_MAX``` 宏定义，同时还减去了一个页面的大小做为保护。

{% highlight c %}
#define TASK_SIZE_MAX   ((1UL << 47) - PAGE_SIZE)
{% endhighlight %}

而 ```0xffff,8000,0000,0000``` 以上为系统空间地址；注意：该地址的高 16bits 是 ```0xffff```，这是因为目前实际上只用了 64 位地址中的 48 位，也就是高 16 位没有使用，而从地址 ```0x0000,7fff,ffff,ffff``` 到 ```0xffff,8000,0000,0000``` 中间是一个巨大的空洞，是为以后的扩展预留的。

而真正的系统空间的起始地址，是从 ```0xffff,8800,0000,0000``` 开始的，参见：

{% highlight c %}
#define __PAGE_OFFSET     _AC(0xffff,8800,0000,0000, UL)
{% endhighlight %}

而 32 位地址时系统空间的起始地址为 ```0xC000,0000``` 。

{% highlight text %}
ffff800000000000 - ffff87ffffffffff (=43 bits) guard hole, reserved for hypervisor
ffff880000000000 - ffffc7ffffffffff (=64 TB) direct mapping of all phys. memory
ffffc80000000000 - ffffc8ffffffffff (=40 bits) hole
ffffc90000000000 - ffffe8ffffffffff (=45 bits) vmalloc/ioremap space
{% endhighlight %}

<!--
而 32 位地址空间时，当物理内存大于896M时（Linux2.4内核是896M，3.x内核是884M，是个经验值），由于地址空间的限制，内核只会将0~896M的地址进行映射，而896M以上的空间用做一些固定映射和vmalloc/ioremap。而64位地址时是将所有物理内存都进行映射。
-->

### 布局详解

内核中有两个参数影响了内存的布局，如下。

{% highlight text %}
----- 查看当前配置
$ cat /proc/sys/vm/legacy_va_layout
$ cat /proc/sys/kernel/randomize_va_space

----- 配置参数
# sysctl -w vm.legacy_va_layout=0
# sysctl -w kernel.randomize_va_space=2

----- 可以修改参数后通过如下方式测试
$ cat /proc/self/maps
00400000-0040b000 r-xp 00000000 08:07 786680                 /usr/bin/cat
0060b000-0060c000 r--p 0000b000 08:07 786680                 /usr/bin/cat
0060c000-0060d000 rw-p 0000c000 08:07 786680                 /usr/bin/cat
011ed000-0120e000 rw-p 00000000 00:00 0                      [heap]
7f924042c000-7f9246955000 r--p 00000000 08:07 793717         /usr/lib/locale/locale-archive
7f9246955000-7f9246b0c000 r-xp 00000000 08:07 793610         /usr/lib64/libc-2.17.so
7f9246b0c000-7f9246d0b000 ---p 001b7000 08:07 793610         /usr/lib64/libc-2.17.so
7f9246d0b000-7f9246d0f000 r--p 001b6000 08:07 793610         /usr/lib64/libc-2.17.so
7f9246d0f000-7f9246d11000 rw-p 001ba000 08:07 793610         /usr/lib64/libc-2.17.so
7f9246d11000-7f9246d16000 rw-p 00000000 00:00 0
7f9246d16000-7f9246d36000 r-xp 00000000 08:07 793718         /usr/lib64/ld-2.17.so
7f9246f19000-7f9246f1c000 rw-p 00000000 00:00 0
7f9246f34000-7f9246f35000 rw-p 00000000 00:00 0
7f9246f35000-7f9246f36000 r--p 0001f000 08:07 793718         /usr/lib64/ld-2.17.so
7f9246f36000-7f9246f37000 rw-p 00020000 08:07 793718         /usr/lib64/ld-2.17.so
7f9246f37000-7f9246f38000 rw-p 00000000 00:00 0
7ffd158bf000-7ffd158e0000 rw-p 00000000 00:00 0              [stack]
7ffd15945000-7ffd15947000 r-xp 00000000 00:00 0              [vdso]
ffffffffff600000-ffffffffff601000 r-xp 00000000 00:00 0      [vsyscall]
{% endhighlight %}

<!--
TEXT：代码段，映射程序的二进制代码， 该区域为私有区域；在代码段中，也会包含一些只读的常数变量，比如字符串常量等。

一般来说，IA-32体系结构中进程空间的代码段从0x08048000开始，这与最低可用地址(0x00000000)有128M的间距，按照linux内核架构一书介绍是为了捕获NULL指针(具体不详)。该值是在编译阶段就已经设定好的，其他体系结构也有类似的缺口，比如mips(), ppc() x86-64使用的是0x000000000400000。

DATA：数据段，映射程序中已经初始化的全局变量。

BSS段：存放程序中未初始化的全局变量，在ELF文件中，该区域的变量仅仅是个符号，并不占用文件空间，但程序加载后，会占用内存空间，并初始化为0。

HEAP：运行时的堆，在程序运行中使用malloc申请的内存区域。

该区域的起始地址受start_brk影响，和BSS段之间会有一个随机值的空洞；该区域的内存增长方式是由低地址向高地址增长。

MMAP：共享库及匿名文件的映射区域；该区域中会包含共享库的代码段和数据段。其中代码段为共享的，但数据段为私有的，写时拷贝。

该区域起始地址也会有一个随机值，该区域的增长方式是从高地址向低地址增长（Linux经典布局中不同，稍后讨论）

STACK：用户进程栈；

该区域起始地址也存在一个随机值，通过PF_RANDOMIZE来设置。栈的增长方向是由高地址向低地址增长，并且若设置了RLIMIT_STACK即规定了栈的大小。

最后，是命令行参数和环境变量。
-->

#### 新旧布局

不同之处在于 MMAP 区域的增长方向，新布局导致了栈空间的固定，而堆区域和 MMAP 区域公用一个空间，这在很大程度上增长了堆区域的大小。

Linux 传统内存布局如下。

![memory userspace layout]({{ site.url }}/images/linux/kernel/memory-userspace-layout-lagency.jpg "memory userspace layout"){: .pull-center width="40%" }

现在用户空间的内存空间布局如下。

![memory userspace layout]({{ site.url }}/images/linux/kernel/memory-userspace-layout.jpg "memory userspace layout"){: .pull-center width="70%" }

从上图可以看到，mmap 映射区域至顶向下扩展，mmap 映射区域和堆相对扩展，直至耗尽虚拟地址空间中的剩余区域，弥补了经典内存布局方式的不足。

为了使用此新的内存布局，执行命令 ```sysctl -w vm.legacy_va_layout=0```，然后重新编译运行程序并查看其输出及 maps 文件内容。

<!--
多线程内存空间布局
主线程与第一个线程的栈之间的距离：0xbfbaf648 – 0xb771d384 = 132M
第一个线程与第二个线程的栈之间的距离：0xb771d384 – 0xb6f1d384 = 8M
其它几个线程的栈之间距离均为8M。
也就是说，主线程的栈空间最大为132M，而普通线程的栈空间仅为8M，超这个范围就会造成栈溢出（后果很严重）。
-->

### 内核相关

利用虚拟内存，每个进程相当于占用了全部的内存空间，所有和进程相关的信息都保存在内存描述符 (memory descriptor) 中，也就是 ```struct mm_struct *mm```；在进程的进程描述符 ```struct task_struct``` 中的 mm 域记录该进程使用的内存描述符，也就是说 ```current->mm``` 代表当前进程的内存描述符。

{% highlight c %}
struct mm_struct {
    struct vm_area_struct *mmap;            // 指向内存区域对象，链表形式存放，利于高效地遍历所有元素
    struct rb_root mm_rb;                   // 与mmap表示相同，以红黑树形式存放，适合搜索指定元素

    pgd_t                 *pgd;             // 指向页目录
    atomic_t               mm_count;        // 主引用计数，为0时结构体会被撤销
    atomic_t               mm_users;        // 从计数器，代表正在使用该地址的进程数目
    int                    map_count;       // vma的数目

    struct list_head mmlist;                // 所有mm_struct都通过mmlist连接在一个双向链表中，
                                            // 该链表的首元素是init_mm内存描述符，代表init进程的地址空间
};
{% endhighlight %}

剩下字段中包含了该进程的代码段、数据段、堆栈、命令行参数、环境变量的起始地址和终止地址；内存描述符在系统中通过 ```mmlist``` 组织成了一个双向链表，这个链表的首元素是 ```init_mm.mmlist``` 。

其中 mm_count 和 mm_users 用来表示是否还有进程在使用该内存，这两个是比较重要的字段，决定了该进程空间是否仍被使用。

<!--
mm_users 记录了使用这片内存空间的所有轻量级进程的数量，包括从同一个父进程 fork() 出来的轻量级子进程在 mm_count 中算一个单元。

比如，某个进程空间由两个拥有同样父进程的轻量级进程共享，那么其 mm_users 的值为 2，而 mm_count 的值为 1，当 mm_count 的值递减为 0 时，该内存描述符 (进程空间) 被回收。
-->

一个进程的内存空间如上图所示，其中各个内存区域通过 ```vm_area_struct``` 表示，类似如下所示。

![memory_process_vma_lists]({{ site.url }}/images/linux/kernel/memory_process_vma_lists.png  "memory_process_vma_lists"){: .pull-center width="80%" }

用户进程的虚拟地址空间包含了若干区域，这些区域的分布方式是特定于体系结构的，不过所有的方式都包含下列成分：

* 可执行文件的二进制代码，也就是程序的代码段；
* 存储全局变量的数据段；
* 用于保存局部变量和实现函数调用的栈；
* 环境变量和命令行参数；
* 程序使用的动态库的代码；
* 用于映射文件内容的区域。

内核中的伙伴系统、SLAB 分配器都是尽快响应内核请求，而对于用户空间的请求略有不同。

用户空间动态申请内存时，往往只是获得一块线性地址的使用权，而并没有将这块线性地址区域与实际的物理内存对应上，只有当用户空间真正操作申请的内存时，才会触发一次缺页异常，这时内核才会分配实际的物理内存给用户空间。

#### 结构体

Linux 内核中，关于虚存管理的最基本的管理单元是虚拟内存区域 (Virtual Memory Areas, vma)，通过 ```struct vm_area_struct``` 表示，它描述了一段连续的、具有相同访问属性 (可读、可写、可执行等等) 的虚存空间，该虚存空间的大小为物理内存页面的整数倍。

{% highlight c %}
struct vm_area_struct {   // include/linux/mm_types.h
    struct mm_struct * vm_mm;                   // 反向指向该进程所属的内存描述符

    unsigned long vm_start, vm_end;             // 虚存空间的首地址，末地址后第一个字节的地址
    struct vm_area_struct *vm_next, *vm_prev;   // 每个进程的VM空间链表，按地址排序，用于遍历
    struct rb_node vm_rb;                       // 红黑树中对应的节点，用于快速定位

    pgprot_t vm_page_prot;                      // vma的访问控制权限
    unsigned long vm_flags;                     // 保护标志位和属性标志位，共享(0)还是独有(1)

    const struct vm_operations_struct *vm_ops;  // 该vma上的各种标准操作函数指针集
    unsigned long vm_pgoff;                     // 文件映射偏移，以PAGE_SIZE为单位
    struct file * vm_file;                      // 如果是文件映射，则指向文件描述符
    void * vm_private_data;                     // 设备驱动私有数据，与内存管理无关

};
{% endhighlight %}

该结构体描述了 ```[vm_start, vm_end)``` 的内存空间，以字节为单位。通常，进程所使用到的虚存空间不连续，且各部分虚存空间的访问属性也可能不同，所以一个进程的虚存空间需要多个 ```vm_area_struct``` 结构来描述。

在 ```vm_area_struct``` 较少时，各个结构体按照升序排序，通过 ```vm_next```、```vm_prev``` 以链表的形式组织数据；但当数据较多时，实现了 AVL 树，以提高 ```vm_area_struct``` 的搜索速度。

<!--
另外，假如该 vm_area_struct 描述的是一个文件映射的虚存空间，成员 vm_file 便指向被映射的文件的 file 结构，vm_pgoff 是该虚存空间起始地址在 vm_file 文件里面的文件偏移，单位为物理页面。<br><br>

一个程序可以选择 MAP_SHARED 或 MAP_PRIVATE 将一个文件的某部分数据映射到自己的虚存空间里面，这两种映射方式的区别在于：MAP_SHARED 映射后在内存中对该虚存空间的数据进行修改会影响到其他以同样方式映射该部分数据的进程，并且该修改还会被写回文件里面去，也就是这些进程实际上是在共用这些数据；而 MAP_PRIVATE 映射后对该虚存空间的数据进行修改不会影响到其他进程，也不会被写入文件中。<br><br>

来自不同进程，所有映射同一个文件的 vm_area_struct 结构都会根据其共享模式分别组织成两个链表。链表的链头分别是：vm_file->f_dentry->d_inode->i_mapping->i_mmap_shared,vm_file->f_dentry->d_inode->i_mapping->i_mmap。而vm_area_struct结构中的vm_next_share指向链表中的下一个节点；vm_pprev_share是一个指针的指针，它的值是链表中上一个节点（头节点）结构的vm_next_share（i_mmap_shared或i_mmap）的地址。<br><br>

进程建立 vm_area_struct 结构后，只是说明进程可以访问这个虚存空间，但有可能还没有分配相应的物理页面并建立好页面映射。在这种情况下，若是进程执行中有指令需要访问该虚存空间中的内存，便会产生一次缺页异常。这时候，就需要通过 vm_area_struct 结构里面的 vm_ops->nopage 所指向的函数来将产生缺页异常的地址对应的文件数据读取出来。<br><br>

vm_flags 主要保存了进程对该虚存空间的访问权限，然后还有一些其他的属性；vm_page_prot 是新映射的物理页面的页表项 pgprot 的默认值。
-->

#### 查看进程内存空间

可以通过 ```cat /proc/<pid>/maps``` 或者 ```pmap <pid>``` 查看。

<!--
每行数据格式： 开始-结束 访问权限 偏移 主设备号：次设备号 i节点 文件
    设备表示为00：00, 索引节点标示页为0，这个区域就是零页（所有数据全为零）
    数据段和bss具有可读、可写但不可执行权限；而堆栈可读、可写、甚至可执行

2.4 内存区域操作
find_vma 查看mm_struct所属于的VMA，搜索第一个vm_end大于addr的内存区域
struct vm_area_struct *find_vma(struct mm_struct *mm, usigned long addr)
    检查mmap_cache，查看缓存VMA是否包含所需地址，如果没有找到，进入2
    通过红黑树搜索；
find_vma_prev 查看mm_struct所属于的VMA，搜索第一个vm_end小于addr的内存区域

struct vm_area_struct * find_vma_prev(struct mm_struct *mm, unsigned long addr, struct vm_area_struct **pprev)

mmap
    内核使用do_mmap()创建一个新的线性地址区间，如果创建的地址区间和一个已存在的相邻地址区间有相同的访问权限，则将两个区间合并为一个。
    mmap()系统调用获取内核函数do_mmap()的功能。
    do_mummap()从特定的进程地址空间中删除指定地址区间
    mummap()与 mmap功能相反。
-->


## 内存申请

在用户空间中会通过 ```malloc()``` 动态申请内存，而实际上，在用户空间中对应了不同的实现方式，包括了 ```ptmalloc``` (glibc)、```tcmalloc``` (Google) 以及 ```jemalloc```，接下来简单介绍这三种内存分配方式。

```ptmalloc``` 的早期版本是由 Doug Lea 实现的，它有一个重要问题就是无法保证线程安全，Wolfram Gloger 改进了其实现从而支持多线程；TCMalloc (Thread-Caching Malloc) 是 google 开发的开源工具 ```google-perftools``` 之一。

<!--
http://www.360doc.com/content/13/0915/09/8363527_314549128.shtml
http://blog.jobbole.com/91887/
http://blog.chinaunix.net/uid-26772535-id-3197173.html
http://blog.csdn.net/jltxgcy/article/details/44150429
http://blog.csdn.net/jltxgcy/article/details/44133307
http://www.cnblogs.com/vinozly/p/5489138.html
-->

### 简介

从操作系统角度来看，进程分配内存有两种方式，分别由系统调用 brk() 和 mmap() 完成：

1. brk() 是将数据段 (```.data```) 的最高地址指针 ```_edata``` 往高地址推；
2. mmap() 是在进程的虚拟地址空间中，也就是堆和栈中间 (文件映射区域) 的一块空闲虚拟内存。

如上所述，这两种方式分配的都是虚拟内存，没有分配物理内存，只有在第一次访问已分配的虚拟地址空间的时候，发生缺页中断，操作系统负责分配物理内存，然后建立虚拟内存和物理内存之间的映射关系。

在标准 C 库中，提供了 ```malloc()``` ```free()``` 分配释放内存，而这两个函数的底层是由 ```brk()``` ```mmap()``` ```munmap()``` 这些系统调用实现的。

<!-- do_page_fault() -->

### brk() sbrk()

如下是两个函数的声明。

{% highlight c %}
#include <unistd.h>
int brk(void *addr);
void *sbrk(intptr_t increment);
{% endhighlight %}

这两个函数都用来改变 "program break" 的位置，如下图所示：

![memory userspace layout]({{ site.url }}/images/linux/kernel/memory-userspace-layout.jpg "memory userspace layout"){: .pull-center width="70%" }

sbrk/brk 是从堆中分配空间，实际上就是移动一个位置，也就是 ```Program Break```，这个位置定义了进程数据段的终止处，增大就是分配空间，减小就是释放空间。

sbrk 用相对的整数值确定位置，如果这个整数是正数，会从当前位置向后移若干字节，如果为负数就向前若干字节，为 0 时获取当前位置；而 brk 则使用绝对地址。

{% highlight c %}
#include <stdio.h>
#include <unistd.h>

int main(int argc, char **argv)
{
    void *p0 = sbrk(0), *p1, *p2;
    printf("P0 %p\n", p0);
    brk(p0 + 4);     // 分配4字节
    p1 = sbrk(0);
    printf("P1 %p\n", p1);
    p2 = sbrk(4);
    printf("P2=%p\n", p2);

    return 0;
}
{% endhighlight %}



## 进程加载

```execve()``` 系统调用可用于加载一个可执行文件并代替当前的进程，它在 ```libc``` 库中有几个 API 封装：```execl()```、```execve()```、```execlp()```、```execvp()```。这几个函数的功能相同，只是参数不同，在内核中统一调用 ```sys_execve()``` 。

以 ```execve()``` 函数为例，创建一个进程，参考如下。

{% highlight c %}
#include <stdio.h>
#include <unistd.h>

int main(int arg,char **args)
{
    char *argv[]={"ls", "-l", "/tmp", NULL};
    char *envp[]={0, NULL};

    execve("/bin/ls", argv, envp);
    return 0;
}
{% endhighlight %}

系统为进程运行初始化环境，无非就是完成内存分配和映射以及参数和数据段、代码段和 bss 等的载入，以及对调用 exec 的旧进程进行回收。

其中有两个可执行文件相关的数据结构，分别是 ```linux_binprm``` 和 ```linux_binfmt```，内核中为可执行程序的装入定义了一个数据结构 ```linux_binprm```，以便将运行一个可执行文件时所需的信息组织在一起。

而 ```linux_binfmt``` 用表示每一个加载模块，这个结构在系统中组成了一个链表结构。

{% highlight c %}
struct linux_binprm {
    char buf[BINPRM_BUF_SIZE];                // 保存可执行文件的头部
    struct page *page[MAX_ARG_PAGES];         // 每个参数最大使用一个物理页来存储，最大为32个页面
    struct mm_struct *mm;                     // 暂时存储新进程的可执行文件名、环境变量等
    unsigned long p;                          // 当前内存的起始地址
    int argc, envc;                           // 参数变量和环境变量的数目
    const char * filename;                    // procps查看到的名称
};

struct linux_binfmt {
    struct list_head lh;                                 // 用于形成一个列表
    struct module *module;                               // 定义该函数所属的模块
    int (*load_binary)(struct linux_binprm *);           // 加载可执行文件
    int (*load_shlib)(struct file *);                    // 加载共享库
    int (*core_dump)(struct coredump_params *cprm);      // core dump
    unsigned long min_coredump;
};
{% endhighlight %}

程序的加载主要分为两步：A) 准备阶段，将参数读如内核空间、判断可执行文件的格式、并选择相应的加载器；B) 载入阶段，完成对新进程代码段、数据段、BSS 等信息的载入。

### 进程创建

如上所述，实际调用的是内核中的 ```sys_execve()``` 函数，简单介绍如下。

{% highlight text %}
sys_execve()         @ fs/exec.c
  |-getname()                               将文件名从用户空间复制到内核空间
  | |-getname_flags()
  |  |-__getname()                          为文件名分配一个缓冲区
  |  |-strncpy_from_user()
  |   |-do_strncpy_from_user()
  |-do_execve()
    |-do_execve_common()
      |-unshare_files()
      |-do_open_exec()
      | |-do_filp_open()
      |-bprm_mm_init()                      bprm初始化，主要是bprm->mm
      | |-mm_alloc()
      | |-init_new_context()
      | |-__bprm_mm_init()
      |  |-kmem_cache_zalloc()              分配一个vma
      |  |-... ...                          设置用户空间对应的栈顶STACK_TOP_MAX
      |  |-insert_vm_struct()               将vma插入mm表示的进程空间结构
      |-count() ... ...                     计算参数个数、环境变量个数
      |-prepare_binprm()                    查看权限并加载128(BINPRM_BUF_SIZE)个字节
      | |-kernel_read()                     读取128字节
      |-search_binary_handler()             整个函数的处理核心
        |-security_bprm_check()             SELinux检查函数
        |-fmt->load_binary()                加载二进制文件，不同二进制格式对应了不同回调函数
        |-load_elf_binary()                 elf对应load_elf_binary
          |-start_thread()                  不同平台如x86会调用不同的函数
            |-start_thread_common()         主要是设置寄存器的值
{% endhighlight %}

其中有个全局变量 ```formats``` 作为链头，可以通过 ```register_binfmt()``` 注册一个可执行文件的加载模块，该模块一般在 ```fs/binfmt_xxx.c``` 文件中，每次加载可执行文件时只需要遍历 formats 变量即可。

{% highlight c %}
static LIST_HEAD(formats);

void __register_binfmt(struct linux_binfmt * fmt, int insert)
{
    BUG_ON(!fmt);
    if (WARN_ON(!fmt->load_binary))
        return;
    write_lock(&binfmt_lock);
    insert ? list_add(&fmt->lh, &formats) :
         list_add_tail(&fmt->lh, &formats);
    write_unlock(&binfmt_lock);
}

static inline void register_binfmt(struct linux_binfmt *fmt)
{
    __register_binfmt(fmt, 0);
}
{% endhighlight %}

在 Linux 中最常用的是 ELF 为例，启动时通过 ```core_initcall(init_elf_binfmt)``` 初始化。

<!--
（1） It releases all resources used by the old process.
（2） It maps the application into virtual address space. The following segments must be taken into  account (the variables specified are elements of the task structure and are             set to the correct values by binary format handler)
（3） The text segment contains the executable code of the program. start_code and end_code
      specify the area in address space where the segment resides.

（4） The pre-initialized data (variables supplied with a specific value at compilation time) are
      located between start_data and end_data and are mapped from the corresponding seg-
      ment of the executable file.
（5）The heap used for dynamic memory allocation is placed in virtual address space; start_brk
     and brk specify its boundaries.
（6）The position of the stack is defined by start_stack; the stack grows downward automati-
     cally on nearly all machines. The only exception is currently PA-Risc. The inverse direction
     of stack growth must be noted by the architecture by setting the configuration symbol
     STACK_GROWSUP.

（7）The program arguments and the environment are mapped into the virtual address space
     and are located between arg_start and arg_end and env_start and env_end, respec-
     tively.
回到do_execve_common（）中，在search_binary_handler后，做收尾工作：
-->

### 内存复制

在调用 ```fork()``` 函数时，会通过 ```copy_mm()``` 复制父进程的内存描述符，子进程通过 ```allcote_mm()``` 从高速缓存中分配 ```struct mm_struct``` 得到。通常，每个进程都有唯一的 ```struct mm_struct```，即唯一的进程地址空间。

当子进程与父进程是共享地址空间，可调用 ```clone()``` 此时不再调用 ```allcote_mm()```，而是仅仅是将 mm 域指向父进程的 mm ，即 ```task->mm = current->mm``` 。

<!--
相反地，撤销内存是exit_mm()函数，该函数会进行常规的撤销工作，更新一些统计量。
内核线程
    没有进程地址空间，即内核线程对应的进程描述符中mm=NULL
    内核线程直接使用前一个进程的内存描述符，仅仅使用地址空间中和内核内存相关的信息
-->




## 参考

{% highlight text %}
{% endhighlight %}
