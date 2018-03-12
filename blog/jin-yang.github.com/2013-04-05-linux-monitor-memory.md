---
title: Linux 监控之 Memory
layout: post
comments: true
language: chinese
category: [linux]
keywords: linux,monitor,监控,内存,memory
description:
---


<!-- more -->

<!--

# 简介

在 Linux 的内存分配机制中，优先使用物理内存，当物理内存还有空闲时（还够用），不会释放其占用内存，就算占用内存的程序已经被关闭了，该程序所占用的内存用来做缓存使用，这样对于开启过的程序、或是读取刚存取过得数据会比较快，可以提高整体 I/O 效率。

为了提高磁盘存取效率，Linux 做了一些精心的设计，除了对 dentry 进行缓存（用于 VFS，加速文件路径名到 inode 的转换），还采取了两种主要 Cache 方式：Buffer Cache 和 Page Cache。前者针对磁盘块的读写，后者针对文件 inode 的读写。这些 Cache 有效缩短了 I/O 系统调用（比如 read，write，getdents）的时间。

在 Linux 缓存机制中存在两种，分别为 buffer 和 cache，主要区别为：A buffer is something that has yet to be "written" to disk. A cache is something that has been "read" from the disk and stored for later use。
具体的操作如下：<ul><li>
    当从磁盘读取到内存的数据在被相关应用程序读取后，如果有剩余内存，则这部分数据会存入cache，以备第2次读取时，避免重新读取磁盘。</li><br><li>
    当一个应用程序在内存中修改过数据后，因为写入磁盘速度相对较低，在有空闲内存的情况下，这些数据先存入buffer，在以后某个时间再写入磁盘，从而应用程序可以继续后面的操作，而不必等待这些数据写入磁盘的操作完成。</li><br><li>
    如果在某个时刻，系统需要更多的内存，则会把cache部分擦除，并把buffer中的内容写入磁盘，从而把这两部分内存释放给系统使用，这样再次读取cache中的内容时，就需要重新从磁盘读取了。</li></ul>
</p>
-->

## free 命令

内存使用情况可以通过 free 命令查看，在 CentOS 中，是 procps-ng 包中的一个程序，该命令实际是读取 /proc/meminfo 文件，对应内核源码中的 fs/proc/meminfo.c。

{% highlight text %}
$ free
         total       used       free     shared     buffers      cached
Mem:    386024     377116       8908          0       21280      155468
-/+ buffers/cache: 200368     185656
Swap:    393552        0      393552
{% endhighlight %}

最新版本的 free 会将 buffer/cache 展示在一起，可以通过 free -w 分开查看 buffers 以及 cache；默认的单位是 KB 。

{% highlight text %}
Mem 表示物理内存统计
    total  : 物理内存总大小；
    used   : 已经分配的内存总计（含buffers 与cache ），但其中部分可能尚未使用；
    free   : 未被分配的内存；
    shared : 多个进程共享的内存总额；
    buffers: 系统分配但未被使用的 buffers 数量，磁盘buffers数量；
    cached: 系统分配但未被使用的 cache 数量，磁盘cache数量；

-/+ buffers/cached 表示物理内存的缓存统计
    used: 实际使用的 buffers 与 cache 总量(used-buffers-cached)，也是实际使用的内存总量；
    free: 未被使用的 buffers 与 cache 和未被分配的内存之和(buffers+cached+free)，这就是系统当前实际可用内存；

Swap 表示硬盘上交换分区的使用情况
    只有 mem 被当前进程实际占用完，即没有了 buffers 和 cache 时，才会使用到 swap 。
{% endhighlight %}

其实第一行可以从操作系统的角度去看，即 OS 认为自己总共有 total 的内存，分配了 used 的内存，还有 free 的内存；其中分配的内存中分别有 buffers 和 cached 的内存还没有使用，也就是说 OS 认为还有空闲的内存，又懒得收回之前分配，但已经分配的不能再使用了。

第二行可以认为从进程的角度去看，可用的内存包括了空闲的+buffers+cached，total-空闲的就是时用的内存数。

可以通过如下方式计算：

{% highlight text %}
----- 实际可用内存大小
Free(-/+ buffers/cache) = Free(Mem) + buffers(Mem) + Cached(Mem)
                 185656 =      8908 +        21280 +      155468

----- 物理内存总大小
total(Mem) = used(-/+ buffers/cache) + free(-/+ buffers/cache)
    386024 =                  200368 +                  185656
           = used(Mem) + free(Mem)
           =    377116 +      8908

----- 已经分配的内存大小
Used(Mem) = Used(-/+ buffers/cache) + buffers(Mem) + Cached(Mem)
   377116 =                  200368 +        21280 +     155468
{% endhighlight %}

关于内存的其它信息可以参考如下文件。

{% highlight text %}
/proc/meminfo                  // 机器的内存使用信息
/proc/pid/maps                 // 显示进程 PID 所占用的虚拟地址
/proc/pid/statm                // 进程所占用的内存
/proc/self/statm               // 当前进程的信息
{% endhighlight %}

#### 新版

如上所述，新版的命令行输入如下。

{% highlight text %}
$ free -w
              total        used        free      shared     buffers       cache   available
Mem:        8070604     2928928      495012     1162676      104156     4542508     3624840
Swap:       8127484      232388     7895096

total     (MemTotal     @ /proc/meminfo)
free      (MemFree      @ /proc/meminfo)
shared    (Shmem        @ /proc/meminfo)  主要是tmpfs使用
buffers   (Buffers      @ /proc/meminfo)
cache     (Cached+Slab  @ /proc/meminfo)
available (MemAvailable @ /proc/meminfo)
used = total - free - buffers - cache     (注意，没有去除shared)
{% endhighlight %}

可以通过 ```free -k -w && cat /proc/meminfo``` 命令进行测试。


### Swap对性能的影响

当内存不足的时候把磁盘的部分空间当作虚拟内存来使用，而实际上并非是在内存不足的时候才使用，有个参数叫做 swappiness，是用来控制 swap 分区使用的，可以直接查看 ```/proc/sys/vm/swappiness``` 文件。

默认值是 60，范围是 [0, 100]；为 0 表示会最大限度使用物理内存，然后才是 swap；为 100 时，表示尽量使用 swap 分区，把内存上的数据及时的搬运到 swap 空间里面。

分配太多的 Swap 空间会浪费磁盘空间，而 Swap 空间太少，则系统有可能会发生错误。当系统的物理内存用光了，系统就会跑得很慢，但仍能运行；但是如果 Swap 空间也用光了，那么系统就会发生错误。

通常情况下，Swap 空间是物理内存的 2~2.5 倍，最小为 64M；这也根据不同的应用，有不同的配置：如果是桌面系统，则只需要较小的 Swap 空间；而大的服务器则视情况不同需要不同大小的 Swap 空间，一般来说对于 4G 以下的物理内存，配置 2 倍的 swap ，4G 以上配置 1 倍。

另外， Swap 分区的数量对性能也有很大的影响。因为 Swap 交换的操作是磁盘 IO 的操作，如果有多个 Swap 交换区， Swap 空间的分配会以轮流的方式操作于所有的 Swap ，这样会大大均衡 IO 的负载，加快 Swap 交换的速度。

实际上，当空闲物理内存不多时，不一定表示系统运行状态很差，因为内存的 cache 及 buffer 部分可以随时被重用。**只有 swap 被频繁调用，才是内存资源是否紧张的依据，可以通过 vmstat 查看**。

#### 状态查看

可以使用 [getswapused.sh]({{ site.example_repository }}/linux/memory/getswapused.sh) 查看 swap 的使用情况，如果不使用 root 用户则只会显示当前用户的 swap，其它用户显示为 0。

<!--
<br><br><h2>测试</h2><p>
通过执行 dd 查看缓存的变化。
<pre style="font-size:0.8em; face:arial;">
# free -w && dd if=/dev/zero of=foobar bs=4k count=40960 && free -w          // 写入168M
</pre>
可以看到 cache 增长的数目与文件大小基本相符，而且 buffers 同样由所增长；然后，通过 linux-ftools 中的工具查看。
<pre style="font-size:0.8em; face:arial;">
# linux-fincore foobar                    // cached_perc为100%
</pre>
该命令的执行过程大致如下。
<pre style="font-size:0.8em; face:arial;">
main()
  |-getpagesize()                         // 获取系统中页的大小
  |-open(path, O_RDONLY)                  // 只读方式打开
  |-fstat(fd, &file_stat)                 // 查看文件属性，主要是大小
    ....
    file_mmap = mmap((void *)0, file_stat.st_size, PROT_NONE, MAP_SHARED, fd, 0 );
    size_t calloc_size = (file_stat.st_size+page_size-1)/page_size;
    mincore_vec = calloc(1, calloc_size);
    if ( mincore(file_mmap, file_stat.st_size, mincore_vec) != 0 )
    ....
    if (mincore_vec[page_index]&1) {

            ++cached;
</pre>
页面的大小是4K ，有了长度，我们就知道了，我们需要多少个int来存放结果。mmap建立映射关系，mincore获取文件页面的驻留情况，从起始地址开始，长度是filesize，结果保存在mincore_vec数组里。如果mincore[page_index]&1 == 1,那么表示该页面驻留在内存中，否则没有对应缓存。
-->

## 常用命令

介绍下与内存相关的命令。

### pmap

可以根据进程查看进程相关信息占用的内存情况，可以通过 ```-x```、```-X```、```-XX``` 查看详细信息。

{% highlight text %}
$ pmap -x $(pidof mysqld)
Address           Kbytes     RSS   Dirty Mode  Mapping
0000000000400000   37116    9120       0 r-x-- mysqld
0000000002a3e000     872     192      12 r---- mysqld
0000000002b18000     700     300     196 rw--- mysqld
0000000002bc7000     772     628     628 rw---   [ anon ]
00007f0df23ff000       4       0       0 -----   [ anon ]
00007f0df2400000   53248    8472    8472 rw---   [ anon ]
00007f0df5bfc000       4       0       0 -----   [ anon ]
... ...
00007fff995ca000     132      76      76 rw---   [ stack ]
00007fff995f2000       8       4       0 r-x--   [ anon ]
ffffffffff600000       4       0       0 r-x--   [ anon ]
---------------- ------- ------- -------
total kB          705216  141876  129408

输出值: 显示扩展格式
  Address : 虚拟内存开始地址；
  Kbytes  : 占用内存的字节数，单位是KB；
  RSS     : (Resident Set Size)驻留内存的字节数(KB)；
  Dirty   : 脏页的字节数（包括共享和私有的）(KB)；
  Mode    : 内存页的权限(r=read, w=write, x=execute, s=shared, p=private)；
  Mapping : 占用内存的文件、[anon] (分配的内存)、[stack] (堆栈)；

$ pmap -d $(pidof mysqld)
Address           Kbytes Mode  Offset           Device    Mapping
0000000000400000   23288 r-x-- 0000000000000000 0ca:00001 mysqld
0000000001cbe000     940 r---- 00000000016be000 0ca:00001 mysqld
0000000001da9000     680 rw--- 00000000017a9000 0ca:00001 mysqld
0000000001e53000     888 rw--- 0000000000000000 000:00000   [ anon ]
0000000001f31000   11680 rw--- 0000000000000000 000:00000   [ anon ]
00007fd784000000    4248 rw--- 0000000000000000 000:00000   [ anon ]
00007fd784426000   61288 ----- 0000000000000000 000:00000   [ anon ]
00007fd78c000000     132 rw--- 0000000000000000 000:00000   [ anon ]
00007fd78c021000   65404 ----- 0000000000000000 000:00000   [ anon ]
00007fd7d2c67000      12 rw-s- 0000000000000000 000:0000a [aio] (deleted)
00007fd7d2c6a000      12 rw-s- 0000000000000000 000:0000a [aio] (deleted)
00007fd7d2c6d000       4 rw-s- 0000000000000000 000:0000a [aio] (deleted)
00007fd7d2c6e000       4 rw--- 0000000000000000 000:00000   [ anon ]
00007fd7d2c6f000       4 r---- 000000000001f000 0ca:00001 ld-2.17.so
00007fd7d2c70000       4 rw--- 0000000000020000 0ca:00001 ld-2.17.so
00007fd7d2c71000       4 rw--- 0000000000000000 000:00000   [ anon ]
00007ffc8fef3000     132 rw--- 0000000000000000 000:00000   [ stack ]
mapped: 1248492K    writeable/private: 545308K    shared: 128K

输出值: 显示设备格式
  Offset: 文件偏移
  Device: 设备名 (major:minor)
最后一行输出:
  mapped            : 该进程映射的虚拟地址空间大小，也就是该进程预先分配的虚拟内存大小，即ps出的vsz；
  writeable/private : 进程所占用的私有地址空间大小，也就是该进程实际使用的内存大小，不含shared libraries；
  shared            : 进程和其他进程共享的内存大小；
假设上述结构导出到了/tmp/1文件中，可以通过如下命令计算：
  mapped: 所有行Kbytes字段的累加
    awk 'BEGIN{sum=0};{sum+=$2};END{print sum}' /tmp/1
  writeable/private: 模式为w+p，且不是s的内存
    awk 'BEGIN{sum=0};{if($3~/w/ && $3!~/s/) {sum+=$2}};END{print sum}' /tmp/1
  shared: 共享内存数
    awk 'BEGIN{sum=0};{if($3~/s/) {sum+=$2}};END{print sum}' /tmp/1
{% endhighlight %}

<!--

共享内存的库实际包括了两部分，分别是代码 ```r----``` 以及数据 ```rw---``` 。

如果加载的插件比较多，那么writeable/private非常大126M，而 top %MEM 只有 0.1M ？？？？

man 1 top
%MEM -- Memory usage (RES)
   A task's currently used share of available physical memory.

RES  --  Resident Memory Size (KiB)
   The non-swapped physical memory a task is using.

writeable/private 真正含义是什么，为什么大于 RSS ，而 -x 统计的 RSS 基本相同
-->


### vmstat

vmstat 不会将自己作为一个有效的进程。

{% highlight text %}
$ vmstat [options] [delay [count]]

常用选项：
  delay: 每多少秒显示一次。
  count: 显示多少次，默认为一次。
  -a: 显示活跃和非活跃内存。
  -f: 显示从系统启动之后 forks 的数量；包括了 fork, vfork, clone 的系统调用，等价于创建的全部任务数。
  -m,--slabs: 显示 slabinfo 。
  -n,--one-header: 只显示一次，而不是周期性的显示。
  -s,--stats: 显示内存统计以及各种活动信息。
  -d,--disk: 显示磁盘的相关统计。
  -D,--disk-sum: 显示硬盘统计信息的摘要。
  -p,--partition device: 显示某个磁盘的统计信息。
  -S,--unit char: 显示输出时的单位，1000(k),1024(K),1000*1000(m),1024*1024(M)，默认是 K，不会改变 swap 和 block 。

----- 每秒刷新一次
$ vmstat -S M 1
procs -----------memory---------- ---swap-- -----io---- --system-- ----cpu----
r  b   swpd   free   buff  cache   si   so    bi    bo   in    cs us sy id wa
3  0      0   1963    607   2359    0    0     0     0    0     1 32  0 68  0

各个段的含义为：
  procs 进程
    r: 等待运行的进程数，如果运行队列过大，表示CPU很忙，一般会造成CPU使用率很高。
    b: 不可中断睡眠(uninterruptible sleep)的进程数，也就是被阻塞的进程，通常为IO。
  memory 内存
    swpd: 虚拟内存(Virtual Memory)的使用量，应该是交换区。</li><li>
    free: 空闲内存数，同free命令中的free(Mem)。</li><li>
    buff: 同free命令，已分配未使用的内存大小。</li><li>
    cache: 同free命令，已分配未使用的内存大小。</li><li>
    inact: 非活跃(inactive)内存的大小，使用 -a 选项????。</li><li>
    active: 活跃(active)内存的大小，使用 -a 选项????。
  swap 交换区
    si: 每秒从交换区(磁盘)写到内存的大小。
    so: 每秒写入交换区(磁盘)的大小。
        如果这两个值长期大于0，系统性能会受到影响，即我们平常所说的Thrashing(颠簸)。
  IO
    bi: 每秒读取的块数(blocks/s)。
    bo: 每秒写入的块数(blocks/s)。
  system 系统
    in: 每秒的中断数，包括时钟中断。
    cs: 每秒上线文的交换数。
  CPU
    us: 用户进程执行时间。
    sy: 系统进程执行时间。
    id: 空闲进程执行时间，2.5.41 之后含有 IO-wait 的时间。
    wa: 等待 IO 的时间，2.5.41 之后含有空闲任务。
{% endhighlight %}

### vmtouch

通过 [vmtouch offical site](http://hoytech.com/vmtouch/) 可以查看文件在内存中的占比，可以下载 [本地源码](/reference/linux/monitor/vmtouch.c) 。

使用参数可以直接通过 ```vmtouch -h``` 查看，如下是简单的示例：

{% highlight text %}
----- 查看/etc目录下有多少在缓存中
$ vmtouch /etc/
           Files: 2844
     Directories: 790
  Resident Pages: 274/9971  1M/38M  2.75%
         Elapsed: 0.15243 seconds

----- 查看一个大文件在缓存中的比例
$ vmtouch -v big-dataset.txt
big-dataset.txt
[                                                            ] 0/169321

           Files: 1
     Directories: 0
  Resident Pages: 0/169321  0/661M  0%
         Elapsed: 0.011822 seconds

----- 缓存部分内容到内存中，分别对应了文本和二进制文件
$ tail -n 10000 big-dataset.txt > /dev/null
$ hexdump -n 102400 big-dataset.txt > /dev/null

----- 再次查看缓存的文件，也就是62页在内存中
$ vmtouch -v big-dataset.txt
big-dataset.txt
[o                                                           ] 62/169321

           Files: 1
     Directories: 0
  Resident Pages: 62/169321  248K/661M  0.0366%
         Elapsed: 0.003463 seconds

----- 将文件加载到内存中
$ vmtouch -vt big-dataset.txt
big-dataset.txt
[OOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOO] 169321/169321
           Files: 1
     Directories: 0
   Touched Pages: 169321 (661M)
         Elapsed: 1.7372 seconds

----- 手动从内存中删除
$ vmtouch -ve big-dataset.txt
Evicting big-dataset.txt
           Files: 1
     Directories: 0
   Evicted Pages: 42116 (164M)
         Elapsed: 0.076824 seconds

----- 将目录下的所有文件锁定到内存中
$ vmtouch -dl /var/www/htdocs/critical/
{% endhighlight %}

在 Linux 中，主要是通过 posix_fadvise() 来清除缓存；通过 mincore() 判断页是否存在于缓存中。


<!---
Linux 内核模式使用 Buffer IO，以充分使用操作系统的 Page Cache；读取时先检查页缓存里面是否有需要的数据，没有就从设备读取，返回给用户的同时，加到缓存一份；写时，写到缓存后返回系统调用，再由后台的进程定期涮到磁盘去。

但是如果你的IO非常密集，就会出现问题。首先由于pagesize是4K，内存的利用效率比较低。其次缓存的淘汰算法很简单，由操作系统自主进行，用户不大好参与。当你的写很多，超过系统内存的某个上限的时候，后台的进程(swapd)要出来回收页面，而且一旦回收的速度小于写入的速度，就会出现不可预期的行为。

这里面最大的问题是：当你使用的内存包括缓存，没超过操作系统规定的上限的时候，操作系统选择不作为，让用户充分使用缓存，从它的角度来看这样效率最高。但是正是由于这种策略在实践中会导致问题。

比如说MYSQL服务器，我们可以把数据直接走direct IO,但是它的日志是走bufferio的。因为走directio需要对写入文件的偏移和大小都要扇区对全，这对日志系统来讲太麻烦了。由于MYSQL是基于事务的，会涉及到大量的日志动作，频繁的写入，然后fsync. 日志一旦写入磁盘，buffer page就没用了，但是一直会在内存呆着，直到达到内存上限，引起操作系统突然大量回收
页面，出现IO柱塞或者内存交换等负面问题。

那么我们知道了困境在哪里，我们可以主动避免这个现象的发生。有二种方法：
1. 日志也走direct io,需要规模的修改MYSQL代码，如percona就这么做了，提供相应的patch。
2. 日志还是走buffer io, 但是定期清除无用page cache.

第一张方法不是我们要讨论的，我们重点讨论第二种如何做：

我们在程序里知道文件的句柄，是不是就可以很轻松的用：

    int posix_fadvise(int fd, off_t offset, off_t len, int advice);
    POSIX_FADV_DONTNEED The specified data will not be accessed in the near future.

来解决问题呢？
比如写类似 posix_fadvise(fd, 0, len_of_file, POSIX_FADV_DONTNEED)；这样的代码来清掉文件所属的缓存。

前面介绍的vmtouch就有这样的功能，清某个文件的缓存。
vmtouch -ve logfile 就可以试验，但是你会发现内存根本就没下来，原因呢？
-->

### smem

Linux 使用的是虚拟内存 (virtual memory)，因此要准确的计算一个进程实际使用的物理内存就比较麻烦，如下是在计算内存使用率时比较重要的指标：

* RSS (Resident Set Size) 表示进程占用的物理内存大小，可以使用 top 命令查询；不过将各进程的 RSS 值相加，通常会超出整个系统的内存消耗，这是因为 RSS 中包含了各进程间共享的内存。
* PSS (Proportional Set Size) 它将共享内存的大小进行平均后，再分摊到各进程上去，相比来说更准确。
* USS (Unique Set Size) 也就是 PSS 中只属于本进程的部分，只计算了进程独自占用的内存大小，不包含任何共享的部分。

可以通过 [smem](https://www.selenic.com/smem/) 查看，这是一个 Python 写的程序，该程序也可以通过 matplotlib 输出图片；另外，同时提供了一个 smemcap.c 程序，可以打包嵌入式的相关数据，然后通过 ```-S/--source``` 指定打包文件。

在 CentOS 中，可以通过如下命令安装。

{% highlight text %}
# yum --enablerepo=epel install smem python-matplotlib
{% endhighlight %}

<!--
{% highlight text %}
Show basic process information  smem
Show library-oriented view  smem -m
Show user-oriented view     smem -u
Show system view    smem -R 4G -K /path/to/vmlinux -w
Show totals and percentages     smem -t -p
Show different columns  smem -c "name user pss"
Sort by reverse RSS     smem -s rss -r
Show processes filtered by mapping  smem -M libxml
Show mappings filtered by process   smem -m -P [e]volution
Read data from capture tarball  smem --source capture.tar.gz
Show a bar chart labeled by pid     smem --bar pid -c "pss uss"
Show a pie chart of RSS labeled by name     smem --pie name -s rss
{% endhighlight %}
-->


### 进程内存使用量

简单介绍下如何查看各个进程的内存使用量。

#### top

可以直接使用 top 命令后，查看 ```%MEM``` 的内容，表示该进程内容使用比例，可以通过 -u 指定用户，也就是只监控特定的用户。

常用的命令有。

{% highlight text %}
P: 按%CPU使用率排行
T: 按TIME+排行
M: 按%MEM排行
{% endhighlight %}

#### ps

如下例所示：

{% highlight text %}
$ ps -e -o 'pid,comm,args,pcpu,rsz,vsz,stime,user,uid' | sort -nrk5
{% endhighlight %}

其中 rsz 为实际内存，上例实现按内存排序，由大到小。


## 小结

可以通过 free 查看整个系统内存的使用情况，free(Mem) 过小不会对系统的性能产生影响，实际剩余的内存数为 free(-/+ buffers/cache)。

通过 vmstat 1 3 输出的 swap(si/so) 可以查看交换区的切入/出情况，如果长期不为 0，则可能是因为内存不足，此时应该查看那个进程占用内存较多。


## 参考

关于内存的介绍可以参考 [What every programmer should know about memory](https://www.akkadia.org/drepper/cpumemory.pdf)，包括了从硬件到软件的详细介绍，内容非常详细。

关于 Linux 内存的介绍可以参考 [Linux ate my ram!](http://www.linuxatemyram.com/) 。

<!--
memtester 内存测试工具。

Linux ate my ram
http://www.linuxatemyram.com/
不错介绍内存的文章
http://linuxperf.com/?cat=7
Linux内存中的Cache真的能被回收么？
http://liwei.life/2016/04/26/linux%e5%86%85%e5%ad%98%e4%b8%ad%e7%9a%84cache%e7%9c%9f%e7%9a%84%e8%83%bd%e8%a2%ab%e5%9b%9e%e6%94%b6%e4%b9%88%ef%bc%9f/


举例介绍 cache/buffers，比较有意思，介绍如何强制刷新 cache，<a href="http://www.linuxatemyram.com/">http://www.linuxatemyram.com/</a>，也可以参考 <a href="reference/block/Linux_ate_my_ram.html">本地版本</a> 。<br><br>

<a href="http://www.ibm.com/developerworks/cn/linux/l-cache/">Linux 内核的文件 Cache 管理机制介绍</a>，一篇很好的关于 cache 的详解，可以参考 <a href="reference/block/Linux 内核的文件 Cache 管理机制介绍.html">本地文档</a> 。<br><br>

<a href="http://blog.csdn.net/shanshanpt/article/details/39258373">Linux文件系统(五)---三大缓冲区之buffer块缓冲区</a>，不错的文章，介绍 buffer 的。
-->



{% highlight text %}
{% endhighlight %}
