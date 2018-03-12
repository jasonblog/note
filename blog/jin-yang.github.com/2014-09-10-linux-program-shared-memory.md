---
title: Linux 共享内存
layout: post
comments: true
language: chinese
category: [linux,misc]
keywords: linux,内存,shared memory,共享内存
description: 共享内存应该是进程间通信最有效的方式，同一块物理内存被映射到两个不同进程 A、B 各自的地址空间；进程 A 可以立即看到进程 B 对共享内存中数据的更新，反之亦然。接下来简单介绍下与共享内存相关的内容。
---

共享内存应该是进程间通信最有效的方式，同一块物理内存被映射到两个不同进程 A、B 各自的地址空间；进程 A 可以立即看到进程 B 对共享内存中数据的更新，反之亦然。

接下来简单介绍下与共享内存相关的内容。

<!-- more -->

## 简介

采用共享内存通信的一个显而易见的好处是效率高，因为进程可以直接读写内存，而不需要任何数据的拷贝。对于像管道和消息队列等通信方式，则需要在内核和用户空间进行四次的数据拷贝，而共享内存则只拷贝两次数据：一次从输入文件到共享内存区，另一次从共享内存区到输出文件。

<!--
实际上，进程之间在共享内存时，并不总是读写少量数据后就 解除映射，有新的通信时，再重新建立共享内存区域。而是保持共享区域，直到通信完毕为止，这样，数据内容一直保存在共享内存中，并没有写回文件。共享内存 中的内容往往是在解除映射时才写回文件的。因此，采用共享内存的通信方式效率是非常高的。
-->

**注意** 共享内存并未提供同步机制，也就是说，在第一个进程结束对共享内存的写操作之前，并无自动机制可以阻止第二个进程开始对它进行读取。所以我们通常需要用其他的机制来同步对共享内存的访问，例如信号量、互斥锁。

<!--

1、shmget函数
该函数用来创建共享内存，它的原型为：
    int shmget(key_t key, size_t size, int shmflg);
第一个参数，与信号量的semget函数一样，程序需要提供一个参数key（非0整数），它有效地为共享内存段命名，shmget函数成功时返回一个与key相关的共享内存标识符（非负整数），用于后续的共享内存函数。调用失败返回-1.

不相关的进程可以通过该函数的返回值访问同一共享内存，它代表程序可能要使用的某个资源，程序对所有共享内存的访问都是间接的，程序先通过调用shmget函数并提供一个键，再由系统生成一个相应的共享内存标识符（shmget函数的返回值），只有shmget函数才直接使用信号量键，所有其他的信号量函数使用由semget函数返回的信号量标识符。

第二个参数，size以字节为单位指定需要共享的内存容量

第三个参数，shmflg是权限标志，它的作用与open函数的mode参数一样，如果要想在key标识的共享内存不存在时，创建它的话，可以与IPC_CREAT做或操作。共享内存的权限标志与文件的读写权限一样，举例来说，0644,它表示允许一个进程创建的共享内存被内存创建者所拥有的进程向共享内存读取和写入数据，同时其他用户创建的进程只能读取共享内存。

2、shmat函数
第一次创建完共享内存时，它还不能被任何进程访问，shmat函数的作用就是用来启动对该共享内存的访问，并把共享内存连接到当前进程的地址空间。它的原型如下：
[cpp] view plain copy
print?在CODE上查看代码片派生到我的代码片

    void *shmat(int shm_id, const void *shm_addr, int shmflg);

第一个参数，shm_id是由shmget函数返回的共享内存标识。
第二个参数，shm_addr指定共享内存连接到当前进程中的地址位置，通常为空，表示让系统来选择共享内存的地址。
第三个参数，shm_flg是一组标志位，通常为0。

调用成功时返回一个指向共享内存第一个字节的指针，如果调用失败返回-1.

3、shmdt函数
该函数用于将共享内存从当前进程中分离。注意，将共享内存分离并不是删除它，只是使该共享内存对当前进程不再可用。它的原型如下：
[cpp] view plain copy
print?在CODE上查看代码片派生到我的代码片

    int shmdt(const void *shmaddr);

参数shmaddr是shmat函数返回的地址指针，调用成功时返回0，失败时返回-1.

4、shmctl函数
与信号量的semctl函数一样，用来控制共享内存，它的原型如下：
[cpp] view plain copy
print?在CODE上查看代码片派生到我的代码片

    int shmctl(int shm_id, int command, struct shmid_ds *buf);

第一个参数，shm_id是shmget函数返回的共享内存标识符。

第二个参数，command是要采取的操作，它可以取下面的三个值 ：
    IPC_STAT：把shmid_ds结构中的数据设置为共享内存的当前关联值，即用共享内存的当前关联值覆盖shmid_ds的值。
    IPC_SET：如果进程有足够的权限，就把共享内存的当前关联值设置为shmid_ds结构中给出的值
    IPC_RMID：删除共享内存段

第三个参数，buf是一个结构指针，它指向共享内存模式和访问权限的结构。
shmid_ds结构至少包括以下成员：
[cpp] view plain copy
print?在CODE上查看代码片派生到我的代码片

    struct shmid_ds
    {
        uid_t shm_perm.uid;
        uid_t shm_perm.gid;
        mode_t shm_perm.mode;
    };


三、使用共享内存进行进程间通信
说了这么多，又到了实战的时候了。下面就以两个不相关的进程来说明进程间如何通过共享内存来进行通信。其中一个文件shmread.c创建共享内存，并读取其中的信息，另一个文件shmwrite.c向共享内存中写入数据。为了方便操作和数据结构的统一，为这两个文件定义了相同的数据结构，定义在文件shmdata.c中。结构shared_use_st中的written作为一个可读或可写的标志，非0：表示可读，0表示可写，text则是内存中的文件。

shmdata.h的源代码如下：
[cpp] view plain copy
print?在CODE上查看代码片派生到我的代码片

    #ifndef _SHMDATA_H_HEADER
    #define _SHMDATA_H_HEADER

    #define TEXT_SZ 2048

    struct shared_use_st
    {
        int written;//作为一个标志，非0：表示可读，0表示可写
        char text[TEXT_SZ];//记录写入和读取的文本
    };

    #endif

源文件shmread.c的源代码如下：
[cpp] view plain copy
print?在CODE上查看代码片派生到我的代码片

    #include <unistd.h>
    #include <stdlib.h>
    #include <stdio.h>
    #include <sys/shm.h>
    #include "shmdata.h"

    int main()
    {
        int running = 1;//程序是否继续运行的标志
        void *shm = NULL;//分配的共享内存的原始首地址
        struct shared_use_st *shared;//指向shm
        int shmid;//共享内存标识符
        //创建共享内存
        shmid = shmget((key_t)1234, sizeof(struct shared_use_st), 0666|IPC_CREAT);
        if(shmid == -1)
        {
            fprintf(stderr, "shmget failed\n");
            exit(EXIT_FAILURE);
        }
        //将共享内存连接到当前进程的地址空间
        shm = shmat(shmid, 0, 0);
        if(shm == (void*)-1)
        {
            fprintf(stderr, "shmat failed\n");
            exit(EXIT_FAILURE);
        }
        printf("\nMemory attached at %X\n", (int)shm);
        //设置共享内存
        shared = (struct shared_use_st*)shm;
        shared->written = 0;
        while(running)//读取共享内存中的数据
        {
            //没有进程向共享内存定数据有数据可读取
            if(shared->written != 0)
            {
                printf("You wrote: %s", shared->text);
                sleep(rand() % 3);
                //读取完数据，设置written使共享内存段可写
                shared->written = 0;
                //输入了end，退出循环（程序）
                if(strncmp(shared->text, "end", 3) == 0)
                    running = 0;
            }
            else//有其他进程在写数据，不能读取数据
                sleep(1);
        }
        //把共享内存从当前进程中分离
        if(shmdt(shm) == -1)
        {
            fprintf(stderr, "shmdt failed\n");
            exit(EXIT_FAILURE);
        }
        //删除共享内存
        if(shmctl(shmid, IPC_RMID, 0) == -1)
        {
            fprintf(stderr, "shmctl(IPC_RMID) failed\n");
            exit(EXIT_FAILURE);
        }
        exit(EXIT_SUCCESS);
    }

源文件shmwrite.c的源代码如下：
[cpp] view plain copy
print?在CODE上查看代码片派生到我的代码片

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
        char buffer[BUFSIZ + 1];//用于保存输入的文本
        int shmid;
        //创建共享内存
        shmid = shmget((key_t)1234, sizeof(struct shared_use_st), 0666|IPC_CREAT);
        if(shmid == -1)
        {
            fprintf(stderr, "shmget failed\n");
            exit(EXIT_FAILURE);
        }
        //将共享内存连接到当前进程的地址空间
        shm = shmat(shmid, (void*)0, 0);
        if(shm == (void*)-1)
        {
            fprintf(stderr, "shmat failed\n");
            exit(EXIT_FAILURE);
        }
        printf("Memory attached at %X\n", (int)shm);
        //设置共享内存
        shared = (struct shared_use_st*)shm;
        while(running)//向共享内存中写数据
        {
            //数据还没有被读取，则等待数据被读取,不能向共享内存中写入文本
            while(shared->written == 1)
            {
                sleep(1);
                printf("Waiting...\n");
            }
            //向共享内存中写入数据
            printf("Enter some text: ");
            fgets(buffer, BUFSIZ, stdin);
            strncpy(shared->text, buffer, TEXT_SZ);
            //写完数据，设置written使共享内存段可读
            shared->written = 1;
            //输入了end，退出循环（程序）
            if(strncmp(buffer, "end", 3) == 0)
                running = 0;
        }
        //把共享内存从当前进程中分离
        if(shmdt(shm) == -1)
        {
            fprintf(stderr, "shmdt failed\n");
            exit(EXIT_FAILURE);
        }
        sleep(2);
        exit(EXIT_SUCCESS);
    }

再来看看运行的结果：



分析：
1、程序shmread创建共享内存，然后将它连接到自己的地址空间。在共享内存的开始处使用了一个结构struct_use_st。该结构中有个标志written，当共享内存中有其他进程向它写入数据时，共享内存中的written被设置为0，程序等待。当它不为0时，表示没有进程对共享内存写入数据，程序就从共享内存中读取数据并输出，然后重置设置共享内存中的written为0，即让其可被shmwrite进程写入数据。

2、程序shmwrite取得共享内存并连接到自己的地址空间中。检查共享内存中的written，是否为0，若不是，表示共享内存中的数据还没有被完，则等待其他进程读取完成，并提示用户等待。若共享内存的written为0，表示没有其他进程对共享内存进行读取，则提示用户输入文本，并再次设置共享内存中的written为1，表示写完成，其他进程可对共享内存进行读操作。

四、关于前面的例子的安全性讨论
这个程序是不安全的，当有多个程序同时向共享内存中读写数据时，问题就会出现。可能你会认为，可以改变一下written的使用方式，例如，只有当written为0时进程才可以向共享内存写入数据，而当一个进程只有在written不为0时才能对其进行读取，同时把written进行加1操作，读取完后进行减1操作。这就有点像文件锁中的读写锁的功能。咋看之下，它似乎能行得通。但是这都不是原子操作，所以这种做法是行不能的。试想当written为0时，如果有两个进程同时访问共享内存，它们就会发现written为0，于是两个进程都对其进行写操作，显然不行。当written为1时，有两个进程同时对共享内存进行读操作时也是如些，当这两个进程都读取完是，written就变成了-1.

要想让程序安全地执行，就要有一种进程同步的进制，保证在进入临界区的操作是原子操作。例如，可以使用前面所讲的信号量来进行进程的同步。因为信号量的操作都是原子性的。

五、使用共享内存的优缺点
1、优点：我们可以看到使用共享内存进行进程间的通信真的是非常方便，而且函数的接口也简单，数据的共享还使进程间的数据不用传送，而是直接访问内存，也加快了程序的效率。同时，它也不像匿名管道那样要求通信的进程有一定的父子关系。

2、缺点：共享内存没有提供同步的机制，这使得我们在使用共享内存进行进程间通信时，往往要借助其他的手段来进行进程间的同步工作。
-->

## 使用示例

共享内存主要用于进程间通信，Linux 有几种方式的共享内存 (Shared Memory) 机制：

1. ```System V shared memory(shmget/shmat/shmdt)```，用于不相关进程的通讯；
2. ```POSIX shared memory(shm_open/shm_unlink)```，同样不同进程通讯，相比接口更简单；
3. ```mmap(), shared mappings``` 包括了 A) 匿名映射 (通过fork关联)，B) 文件映射，通常用于不相关的进程通讯；

SYSV 和 POSIX 两者提供功能基本类似，也就是 ```semaphores``` ```shared memory``` ```message queues```，不过由于 SYSV 的历史原因使用更广泛，不过两者的实现基本相同。

{% highlight text %}
The POSIX shared memory object implementation on Linux 2.4 makes use of
a dedicated filesystem, which is normally mounted under /dev/shm.
{% endhighlight %}

也就是说，POSIX 共享内存是基于 tmpfs 来实现的，而 SYSV 共享内存在内核也是基于 tmpfs 实现的；从内核文档 [tmpfs.txt]({{ site.kernel_docs_url }}/Documentation/filesystems/tmpfs.txt) 可以看到如下内容：

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

在上述的内容中，tmpfs 与共享内存相关的主要有两个作用：

* SYSV 还有匿名内存映射，这部分由内核管理，用户不可见；
* POSIX 由用户负责 mount，而且一般 mount 到 ```/dev/shm```，依赖 ```CONFIG_TMPFS``` 。

### SYSV

可以通过示例 [github shmv.c]({{ site.example_repository }}/linux/memory/sharedmemory/shmv.c) 。

{% highlight text %}
----- 查看以及调整支持的最大内存，shmmax shmall shmmni
$ cat /proc/sys/kernel/shmmax
33554432

----- 修改为32M
# echo 33554432 > /proc/sys/kernel/shmmax

----- 尝试创建65M的system V共享内存时失败
$ ipcmk -M 68157440
ipcmk: create share memory failed: Invalid argument

----- 创建10M共享内存
$ ipcmk -M 10485760
Shared memory id: 19431492

----- 查看刚创建的共享内存
$ ipcs -m -i 19431492
Shared memory Segment shmid=19431492
uid=0   gid=0   cuid=0  cgid=0
mode=0644       access_perms=0644
bytes=10485760  lpid=0  cpid=28064      nattch=0
att_time=Not set
det_time=Not set
change_time=Sun May 28 10:15:50 2015

----- 删除刚创建的资源
# ipcrm -m 19431492
{% endhighlight %}

注意，这里看到的 system v 共享内存的大小并不受 ```/dev/shm``` 的影响。

<!--
System V共享内存把所有共享数据放在共享内存区，任何想要访问该数据的进程都必须在本进程的地址空间新增一块内存区域，用来映射存放共享数据的物理内存页面。System V共享内存通过shmget函数获得或创建一个IPC共享内存区域，并返回相应的标识符，内核在保证shmget获得或创建一个共享内存区，初始化该共享内存区相应的shmid_kernel结构，同时还将在特殊文件系统shm中创建并打开一个同名文件，并在内存中建立起该文件的相应的dentry及inode结构，新打开的文件不属于任何一个进程，所有这一切都是系统调用shmget函数完成的。
-->

#### 配置项

与共享内存相关的内容可以参考 ```/etc/sysctl.conf``` 文件中的配置，简介如下：

{% highlight text %}
kernel.shmmax = 4398046511104
kernel.shmall = 1073741824     系统可用共享内存的总页数
kernel.shmmni = 4096
{% endhighlight %}

<!-- http://www.361way.com/ipcs-shared-memory/5144.html -->

### POSIX

大多数的 Linux 发行版本中，内存盘默认使用的是 `/dev/shm` 路径，文件系统类型为 tmpfs，默认大小是内存实际的大小，其大小可以通过 `df -h` 查看。

{% highlight text %}
----- 查看当前大小
$ df -h /dev/shm
Filesystem      Size  Used Avail Use% Mounted on
tmpfs           3.9G   17M  3.9G   1% /dev/shm

----- 修改挂载点的大小，然后重新挂载
$ cat /etc/fstab
tmpfs /dev/shm tmpfs defaults,size=4096M 0 0
# mount -o remount /dev/shm

----- 不用重启重新挂载
# mount -o remount,size=256M /dev/shm
# mount -o size=1500M -o nr_inodes=1000000 -o noatime,nodiratime -o remount /dev/shm
{% endhighlight %}

可以通过示例 [github posix.c]({{ site.example_repository }}/linux/memory/sharedmemory/posix.c) ，将会申请 65M 左右的共享内存。

{% highlight text %}
$ df -h /dev/shm
Filesystem      Size  Used Avail Use% Mounted on
tmpfs           256M   82M  175M  32% /dev/shm
$ ls /dev/shm/shm1 -alh
-rw-r--r-- 1 andy andy 65M 5月  28 10:57 /dev/shm/shm1
$ stat /dev/shm/shm1
{% endhighlight %}

注意，如果申请的内存超过了限制，那么会报 ```Bus error``` 的错误。

### 总结

虽然 System V 与 POSIX 共享内存都是通过 tmpfs 实现，但是两个不同实例，对于 ```/proc/sys/kernel/shmmax``` 只会影响 SYS V 共享内存，```/dev/shm``` 只会影响 Posix 共享内存。

## tmpfs

tmpfs 是基于内存/交换分区的文件系统，ramdisk 是作为块设备，基于 ext 的文件系统，所以不会绕过 page cache 的内存复制；而 tmpfs 直接操作内存做为文件系统的，而不是基于块设备的。

其源码实现在 `mm/shmem.c` 中，根据 `CONFIG_SHMEM` 是否配置，略微有所区别。

{% highlight c %}
static struct file_system_type shmem_fs_type = {
    .owner      = THIS_MODULE,
    .name       = "tmpfs",
    .mount      = shmem_mount,
    .kill_sb    = kill_litter_super,
    .fs_flags   = FS_USERNS_MOUNT,
};
{% endhighlight %}

在函数 `init_tmpfs()` 里，实际会通过 `register_filesystem()` 函数将 tmpfs 注册到文件系统中，在 `shmem_file_setup()` 中，更改了 `file->f_op = &shmem_file_operations;` 下面来看具体的结构体。

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

也就是说在操作在 tmpfs 文件时候，并没有使用常用的 ext 文件系统中的函数 `do_sync_read()`，而是调用了 tmpfs 自己封装的函数 `shmem_file_read()`，当然在 `shmem_file_read()` 并没有对 page cache 进行操作，虽然里面还是使用了 page cache 中 maping、file、inode 等结构体和算法。


<!--
3. 函数shmem_file_read主要是调用do_shmem_file_read函数，在do_shmem_file_read函数中核心是shmem_getpage，通过索引和inode快速找到page.
-->

## 参考

<!--
[浅析Linux的共享内存与tmpfs文件系统](http://hustcat.github.io/shared-memory-tmpfs/)
-->

{% highlight text %}
{% endhighlight %}
