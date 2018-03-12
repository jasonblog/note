---
title: Kernel 进程相关
layout: post
comments: true
language: chinese
category: [linux,misc]
keywords: linux,process
description: 实际上一个进程，包括了代码、数据和分配给进程的资源，这是一个动态资源。这里简单介绍与进程相关的东西，例如进程创建、优先级、进程之间的关系、进程组和会话、进程状态等。
---

实际上一个进程，包括了代码、数据和分配给进程的资源，这是一个动态资源。

这里简单介绍与进程相关的东西，例如进程创建、优先级、进程之间的关系、进程组和会话、进程状态等。

<!-- more -->

## 进程

一个进程，包括了代码、数据和分配给进程的资源。

### 初始进程

在 Linux 中，进程基本都是通过复制其它进程的结构来实现的，利用 slabs 来动态分配，系统没有提供用于创建进程的接口。

唯一的一个例外是第一个 `task_struct`，这是由一个静态或者说是固化的结构表示的 (`init_task`)，该进程的 `PID=0`，可以参看 `arch/x86/kernel/init_task.c`，也可以称之为空闲进程。

当内核执行到 `sched_init()` 时，`task_struct` 的核心 `TR`、`LDT` 就被手工设置好了，这时，0 号进程就有了，而在 `sched_init()` 之前，是没有 "进程" 这个概念的，而 `init(PID=1)` 进程是 0 号进程 `fork()` 出来的。

{% highlight text %}
struct task_struct init_task = INIT_TASK(init_task);
{% endhighlight %}

查看 `INIT_TASK()` 宏定义时，会发现很多有意思的东西，如 `init_task` 的栈通过 `.stack = &init_thread_info` 指定，而该栈实际时通过如下的方式静态分配。

{% highlight text %}
union thread_union init_thread_union __init_task_data = { INIT_THREAD_INFO(init_task) };
{% endhighlight %}

定义中的 `__init_task_data` 表明该内核栈所在的区域位于内核映像的 `init data` 区，可以通过编译完内核后所产生的 System.map 来看到该变量及其对应的逻辑地址。

{% highlight text %}
$ cat System.map-`uname -r` | grep init_thread_union
ffffffff818fc000 D init_thread_union
{% endhighlight %}

而内核在无进程的情况下，将一直从初始化部分的代码执行到 `start_kernel()`，然后再到其最后一个函数调用 `rest_init()`，在该函数中将产生第一个进程 `PID=1`。

{% highlight text %}
start_kernel()
 |-rest_init()
   |-kernel_thread()        实际上调用kernel_init()创建第一个进程PID=1

kernel_init()
 |-run_init_process()
   |-do_execve()
{% endhighlight %}

最后 `init_task` 任务会变成 `idle` 进程。

在 Linux 系统中，可创建进程的最大值是由 `max_threads@kernel/fork.c` 变量确定的，也可以通过 `/proc/sys/kernel/threads-max` 查看/更改此值。

### fork

通过 `fork()` 将创建一个与原来进程几乎完全相同的进程，系统给新的进程分配资源，例如存储数据和代码的空间，然后把原来的进程的所有值都复制到新的新进程中，只有少数值与原来的进程的值不同，相当于克隆了一个自己。

{% highlight c %}
#include <stdio.h>
#include <unistd.h>

int main()
{
    pid_t pid;
    int count = 0;

    pid = fork();            // vfork(); error
    if (pid < 0) {
        printf("error in fork!");
    } else if (pid == 0) {   // child process
        count++;
        printf(" child: PID is %d, count is %d\n", getpid(), count);
    } else {                 // parent process
        count++;
        printf("parent: PID is %d, count is %d\n", getpid(), count);
    }

    return 0;
}
{% endhighlight %}

在语句 `fork()` 之前，只有一个进程在执行这段代码，但在这条语句之后，就变成两个进程在执行了，这两个进程的几乎完全相同。其中 fork() 的一个奇妙之处就是它仅仅被调用一次，却能够返回两次，它可能有三种不同的返回值：

1. 在父进程中，fork() 返回新创建子进程的进程 ID；
2. 在子进程中，fork() 返回 0；
3. 如果出现错误，fork() 返回一个负值；

创建新进程成功后，系统中出现两个基本完全相同的进程，这两个进程执行没有固定的先后顺序，哪个进程先执行要看系统的进程调度策略。

### 执行流程

1. 进程可以看做程序的一次执行过程。在 linux 下，每个进程有唯一的 PID 标识进程。 PID 是一个从 1 到 32768 的正整数，其中 1 一般是特殊进程 init ，其它进程从 2 开始依次编号。当用完 32768 后，从 2 重新开始。

2. Linux 中有一个叫进程表的结构用来存储当前正在运行的进程。可以使用 `ps aux` 命令查看所有正在运行的进程。

3. 进程在 linux 中呈树状结构， init 为根节点，其它进程均有父进程，某进程的父进程就是启动这个进程的进程，这个进程叫做父进程的子进程。

4. fork 的作用是复制一个与当前进程一样的进程。新进程的所有数据（变量、环境变量、程序计数器等）数值都和原进程一致，但是是一个全新的进程，并作为原进程的子进程。


![process fork]({{ site.url }}/images/linux/process-fork.gif "process fork"){: .pull-center }

上图表示一个含有 fork 的程序，而 fork 语句可以看成将程序切为 A、B 两个部分。然后整个程序会如下运行：

1.  设由 shell 直接执行程序，生成了进程 M 。 M 执行完 Part.A 的所有代码。

2. 当执行到 pid = fork(); 时， M 启动一个进程 S ，S 是 M 的子进程，和 M 是同一个程序的进程。S 继承 M 的所有变量、环境变量、程序计数器的当前值。

3. 在 M 进程中，fork() 将 S 的 PID 返回给变量 pid ，并继续执行 Part.B 的代码。

4. 在进程 S 中，将 0 赋给 pid ，并继续执行 Part.B 的代码。

这里有三个点非常关键:

* M 执行了所有程序，而 S 只执行了 Part.B ，即 fork() 后面的程序，(这是因为 S 继承了 M 的 PC-程序计数器)。

* S 继承了 fork() 语句执行时当前的环境，而不是程序的初始环境。

* M 中 fork() 语句启动子进程 S ，并将 S 的 PID 返回，而 S 中的 fork() 语句不启动新进程，仅将 0 返回。

#### 举例

{% highlight c %}
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>

int main()
{
    pid_t pid1;
    pid_t pid2;

    pid1 = fork();
    pid2 = fork();

    printf("pid1:%d, pid2:%d\n", pid1, pid2);
}
{% endhighlight %}

对于如上的程序，在执行之后将会生成 4 个进程，如果其中一个进程的输出结果是 `pid1:1001, pid2:1002` ，那么其他的分别为。

{% highlight text %}
pid1:1001, pid2:0
pid1:0, pid2:1003
pid1:0, pid2:0
{% endhighlight %}

实际的执行过程如下所示：

![process fork]({{ site.url }}/images/linux/process-fork-2.gif "process fork"){: .pull-center }

最后的实际执行过程为：

1. 从 shell 中执行此程序，启动了一个进程，我们设这个进程为 P0 ，设其 PID 为 XXX (解题过程不需知道其 PID)。

2. 当执行到 pid1 = fork(); 时， P0 启动一个子进程 P1 ，由题目知 P1 的 PID 为 1001 。我们暂且不管 P1 。

3. P0 中的 fork 返回 1001 给 pid1 ，继续执行到 pid2 = fork(); ，此时启动另一个新进程，设为 P2 ，由题目知 P2 的 PID 为 1002 。同样暂且不管 P2 。

4. P0 中的第二个 fork 返回 1002 给 pid2 ，继续执行完后续程序，结束。所以， P0 的结果为 “pid1:1001, pid2:1002” 。

5. 再看 P2 ， P2 生成时， P0 中 pid1=1001 ，所以 P2 中 pid1 继承 P0 的 1001 ，而作为子进程 pid2=0 。 P2 从第二个 fork 后开始执行，结束后输出 “pid1:1001, pid2:0” 。

6. 接着看 P1 ， P1 中第一条 fork 返回 0 给 pid1 ，然后接着执行后面的语句。而后面接着的语句是 pid2 = fork(); 执行到这里， P1 又产生了一个新进程，设为 P3 。先不管 P3 。

7. P1 中第二条 fork 将 P3 的 PID 返回给 pid2 ，由预备知识知 P3 的 PID 为 1003 ，所以 P1 的 pid2=1003 。 P1 继续执行后续程序，结束，输出 “pid1:0, pid2:1003” 。

8. P3 作为 P1 的子进程，继承 P1 中 pid1=0 ，并且第二条 fork 将 0 返回给 pid2 ，所以 P3 最后输出 “pid1:0, pid2:0” 。

9. 至此，整个执行过程完毕。


## 进程创建

进程和线程相关的函数在内核中，最终都会调用 `do_fork()`，只是最终传入的参数不同。

其中在用户空间中与进程相关的接口有 `fork()`、`vfork()`、`clone()`、`exec()`；线程相关的有 `pthread_create` (glibc) 和 `kernel_thread` (Kernel内部函数)，`pthread_create()` 是对 `clone()` 的封装，`kernel_thread()` 用于创建内核线程，两者最终同样会调用 `do_fork()`。

<!--
fork()
fork创建一个进程时，子进程只是完全复制父进程的资源，复制出来的子进程有自己的task_struct结构和pid,但却复制父进程其它所有的资源。例如，要是父进程打开了五个文件，那么子进程也有五个打开的文件，而且这些文件的当前读写指针也停在相同的地方。所以，这一步所做的是复制。这样得到的子进程独立于父进程， 具有良好的并发性，但是二者之间的通讯需要通过专门的通讯机制，如：pipe，共享内存等机制， 另外通过fork创建子进程，需要将上面描述的每种资源都复制一个副本。

这样看来，fork是一个开销十分大的系统调用，这些开销并不是所有的情况下都是必须的，比如某进程fork出一个子进程后，其子进程仅仅是为了调用exec执行另一个可执行文件，那么在fork过程中对于虚存空间的复制将是一个多余的过程。但由于现在Linux中是采取了copy-on-write(COW写时复制)技术，为了降低开销，fork最初并不会真的产生两个不同的拷贝，因为在那个时候，大量的数据其实完全是一样的。写时复制是在推迟真正的数据拷贝。若后来确实发生了写入，那意味着parent和child的数据不一致了，于是产生复制动作，每个进程拿到属于自己的那一份，这样就可以降低系统调用的开销。所以有了写时复制后呢，vfork其实现意义就不大了。

fork()调用执行一次返回两个值，对于父进程，fork函数返回子程序的进程号，而对于子程序，fork函数则返回零，这就是一个函数返回两次的本质。

在fork之后，子进程和父进程都会继续执行fork调用之后的指令。子进程是父进程的副本。它将获得父进程的数据空间，堆和栈的副本，这些都是副本，父子进程并不共享这部分的内存。也就是说，子进程对父进程中的同名变量进行修改并不会影响其在父进程中的值。但是父子进程又共享一些东西，简单说来就是程序的正文段。正文段存放着由cpu执行的机器指令，通常是read-only的。


vfork()
vfork系统调用不同于fork，用vfork创建的子进程与父进程共享地址空间，也就是说子进程完全运行在父进程的地址空间上，如果这时子进程修改了某个变量，这将影响到父进程。

其次，子进程在vfork()返回后直接运行在父进程的栈空间，并使用父进程的内存和数据。这意味着子进程可能破坏父进程的数据结构或栈，造成失败。为了避免这些问题，需要确保一旦调用vfork()，子进程就不从当前的栈框架中返回，并且如果子进程改变了父进程的数据结构就不能调用exit函数。子进程还必须避免改变全局数据结构或全局变量中的任何信息，因为这些改变都有可能使父进程不能继续。

但此处有一点要注意的是用vfork()创建的子进程必须显示调用exit()来结束，否则子进程将不能结束，而fork()则不存在这个情况。

Vfork也是在父进程中返回子进程的进程号，在子进程中返回0。

用 vfork创建子进程后，父进程会被阻塞直到子进程调用exec(exec，将一个新的可执行文件载入到地址空间并执行之。)或exit。vfork的好处是在子进程被创建后往往仅仅是为了调用exec执行另一个程序，因为它就不会对父进程的地址空间有任何引用，所以对地址空间的复制是多余的 ，因此通过vfork共享内存可以减少不必要的开销

再次强调：在使用vfork()时，必须在子进程中调用exit()函数调用，否则会出现：__new_exitfn: Assertion `l != ((void *)0)' failed　错误！而且，现在这个函数已经很少使用了！


clone（）
    系统调用fork()和vfork()是无参数的，而clone()则带有参数。fork()是全部复制，vfork()是共享内存，而clone()是则可以将父进程资源有选择地复制给子进程，而没有复制的数据结构则通过指针的复制让子进程共享，具体要复制哪些资源给子进程，由参数列表中的clone_flags决决定。

fork不对父子进程的执行次序进行任何限制，fork返回后，子进程和父进程都从调用fork函数的下一条语句开始行，但父子进程运行顺序是不定的，它取决于内核的调度算法；而在vfork调用中，子进程先运行，父进程挂起，直到子进程调用了exec或exit之后，父子进程的执行次序才不再有限制；clone中由标志CLONE_VFORK来决定子进程在执行时父进程是阻塞还是运行，若没有设置该标志，则父子进程同时运行，设置了该标志，则父进程挂起，直到子进程结束为止。-->

### 区别

`fork()` 会创建新的进程；`exec()` 的原进程将会被新的进程替换；而 `vfork()` 其实就是 `fork()` 的部分过程，通过简化来提高效率。

`fork()` 是进程资源的完全复制，包括进程的 `PCB(task_struct)`、线程的系统堆栈、进程的用户空间、进程打开的设备等，而在 `clone()` 中其实只有前两项是被复制了的，后两项都与父进程共享，对共享数据的保护必须有上层应用来保证。

`vfork()` 与 `fork()` 主要区别:

* `fork()` 子进程拷贝父进程的数据段，堆栈段；`vfork()` 子进程与父进程共享数据段。
* `fork()` 父子进程的执行次序不确定；`vfork()` 保证子进程先运行，在调用 `exec()` 或 `exit()` 之前与父进程数据是共享的，在它调用 `exec()` 或 `exit()` 之后父进程才可能被调度运行，否则会发生错误。
* 如果在此之前子进程有依赖于父进程的进一步动作，如调用函数，则会导致错误。

`fork()` 在执行复制时，采用 "写时复制"，开始的时候内存并没有被复制，而是共享的，直到有一个进程去写某块内存时，它才被复制。实际操作时，内核先将这些内存设为只读，当它们被写时，CPU 出现访存异常，内核捕捉异常，复制空间，并改属性为可写。

但是，"写时复制" 其实还是有复制，进程的 mm 结构、页表都还是被复制了，而 `vfork()` 会忽略所有关于内存的东西，父子进程的内存是完全共享的。

不过此时，父子进程共用着栈，如果两个进程并行执行，那么可能会导致调用栈出错。所以，`vfork()` 有个限制，当子进程生成后，父进程在 `vfork()` 中被内核挂起，直到子进程有了自己的内存空间 `(exec**)` 或退出 `(_exit)`。

并且，在此之前，子进程不能从调用 `vfork()` 的函数中返回，同时，不能修改栈上变量、不能继续调用除 `_exit()` 或 `exec()` 系列之外的函数，否则父进程的数据可能被改写。

虽然 `vfork()` 的限制很多，但是对于 shell 来说却非常适合。


### 源码解析

如下是内核中的接口，`do_fork()` 返回的是子进程的 PID。

{% highlight text %}
// kernel/fork.c
do_fork(SIGCHLD, 0, 0, NULL, NULL);                           // sys_fork
do_fork(CLONE_VFORK | CLONE_VM | SIGCHLD, 0, 0, NULL, NULL);  // sys_vfork
do_fork(clone_flags, newsp, 0, parent_tidptr, child_tidptr);  // sys_clone


do_fork() @ kernel/fork.c
  |-... ...                            // 做一些参数的检查工作，主要是针对flag
  |-copy_process()                     // 复制一个进程描述符，PID不同
  |  |-security_task_create()          // 安全模块的回调函数
  |  |-dup_task_struct()               // 新建task_struct和thread_info，并将当前进程相应的结构完全复制过去
  |  |-atomic_read()                   // 判断是否超过了进程数
  |  |-copy_creds()                    // 应该是安全相关的设置
  |  |-... ...                         // 判断创建的进程是否超了进程的总量等
  |  |-sched_fork()                    // 调度相关初始化
  |  |-copy_xxx()                      // 根据传入的flag复制相应的数据结构
  |  |-alloc_pid()                     // 为新进程获取一个有效的PID
  |  |-sched_fork()                    // 父子进程平分共享的时间片
  |
  |-wake_up_new_task()                 // 如果创建成功则执行
  |-wait_for_vfork_done()              // 如果是vfork()等待直到子进程exit()或者exec()
{% endhighlight %}

`clone_flags` 由 4 个字节组成，最低的一个字节为子进程结束时发送给父进程的信号代码，`fork/vfork` 为 `SIGCHLD`，`clone` 可以指定；剩余的三个字节则是各种 `clone()` 标志的组合，用于选择复制父进程那些资源。

内核有意选择子进程首先执行。因为一般子进程都会马上调用 exec 函数，这样可以避免写时拷贝的额外开销，如果父进程首先执行的话，有可能开始向地址空间写入。

### PID分配

pid 分配的范围是 0~32767，`struct pidmap` 用来标示 pid 是不是已经分配出去了，采用位图的方式，每个字节表示 8 个 PID，为了表示 32768 个进程号要用 32768/8=4096 个字节，也即一个 page，结构体中的 nr_free 表示剩余 PID 的数量。

PID 相关的一些操作实际上就是 bitmap 的操作，常见的如下。

* int test_and_set_bit(int offset, void *addr);<br>将 offset 在 pidmap 变量当中相应的位置为 1，并返回旧值；也就是申请到一个 pid 号之后，修改位标志，其中 addr 是 pidmap.page 变量的地址。

* void clear_bit(int offset, void *addr);<br>将 offset 在 pidmap 变量当中相应的位置为 0，也就是释放一个 pid 后，修改位标志，其中 addr 是 pidmap.page 变量的地址。

* int find_next_zero_bit(void *addr, int size, int offset);<br>从 offset 开始，找下一个是 0 (也就是可以分配) 的 pid 号，其中 addr 是 pidmap.page 变量的地址，size 是一个页的大小。

* int alloc_pidmap();<br>分配一个 pid 号。

* void free_pidmap(int pid);<br>回收一个 pid 号。

目前 Linux 通过 PID 命名空间进行隔离，其中有一个变量 last_pid 用于标示上一次分配出去的 pid 编号。

在内核中通过移位操作来实现根据 PID 查找地址，可以想象抽象出一张表，这个表有 32 列，1024行，这个刚好是一个页的大小，可以参考相应程序的示例。

### 系统进程遍历

在内核中，会通过双向链表保存任务列表，可以将 `init_task` 作为链表的开头，然后进行迭代。

可以通过内核模块 `procsview` 进行查看，通过 `printk` 将所有的进程打印出来，结果可以通过 `tail -f /var/log/messages` 查看。

可以通过宏定义 `next_task()@include/linux/sched.h` 简化任务列表的迭代，该宏会返回下一个任务的 `task_struct` 引用；`current` 标识当前正在运行的进程，这实际是一个宏，在 `arch/x86/include/asm/current.h` 中定义。

通过 Make 进行编译，用 `insmod procsview.ko` 插入模块对象，用 `rmmod procsview` 删除它。插入后，`/var/log/messages` 可显示输出，其中有一个空闲任务 (称为 swapper) 和 init 任务 (pid 1)。


## 优先级

静态优先级和 nice 值的区别。

只有 nice 值对用户可见，而静态优先级则隐藏在内核中，用户可以通过修改 nice 值间接修改静态优先级，而且只会影响静态优先级，不会影响动态优先级。对于普通进程来说，动态优先级是基于静态优先级算出来的。

静态优先级在进程描述符中为 `static_prio` 成员变量，该优先级不会随着时间而改变，内核不会修改它，只能通过系统调用 `nice` 去调整。

nice值是每个进程的一个属性，不是进程的优先级，而是一个能影响优先级的数字；取值范围为-20~19，默认为0。现在内核不再存储 nice 值，而是存储静态优先级 static_prio，两者在内核中通过两个宏进行转换。

{% highlight c %}
#define NICE_TO_PRIO(nice)  (MAX_RT_PRIO + (nice) + 20)
#define PRIO_TO_NICE(prio)  ((prio) - MAX_RT_PRIO - 20)
#define TASK_NICE(p)        PRIO_TO_NICE((p)->static_prio)
{% endhighlight %}

上述的宏存在于 `kernel/sched/sched.h` 中，静态优先级的取值范围为 `[MAX_RT_PRIO, MAX_PRIO-1]` 也即 `[100, 139]` 。

动态优先级的值影响任务的调度顺序，调度程序通过增加或减少进程静态优先级的值来奖励 IO 消耗型进程或惩罚 CPU 消耗型进程，调整后的优先级称为动态优先级，在进程描述符中用 prio 表示，通常所说的优先级指的是动态优先级。

动态优先级的取值范围为 `[0, MAX_PRIO-1]` 也即 `[0, 139]`，其中 `[0, MAX_RT_PRIO-1]` 也即 `[0, 99]` 为实时进程范围，数值越大表示优先级越小。

优先级可以通过 `top`、`ps -o pid,comm,nice -p PID` 或者 `ps -el` 查看，如果是实时进程就是静态优先级，如果是非实时进程，就是动态优先级。

{% highlight text %}
# nice -n 10 commands               # 指定启动时的优先级为10
# nice -10 commands                 # 同上
# nice --10 commands                # 指定优先级为-10
# renice 10 -p PID                  # 设置已经启动进程的优先级，只有root可以设置为负值
# renice -10 PID                    # 设置为-10
{% endhighlight %}

可以修改 `/etc/security/limits.conf` 的值，指定特定用户的优先级 `[username] [hard|soft] priority [nice value]` 。

<!--
 prio和normal_prio为动态优先级，static_prio为静态优先级。static_prio是进程创建时分配的优先级，如果不人为的更 改，那么在这个进程运行期间不会发生变化。 normal_prio是基于static_prio和调度策略计算出的优先级。prio是调度器类考虑的优先级，某些情况下需要暂时提高进程的优先级 (实时互斥量)，因此有此变量，对于优先级未动态提高的进程来说这三个值是相等的。以上三个优先级值越小，代表进程的优先级有高。一般情况下子进程的静态 优先级继承自父进程，子进程的prio继承自父进程的normal_prio。
    rt_policy表示实时进程的优先级，范围为0～99，该值与prio，normal_prio和static_prio不同，值越大代表实时进程的优先级越高。
    那么内核如何处理这些优先级之间的关系呢？其实，内核使用0～139表示内部优先级，值越低优先级越高。其中0～99为实时进程，100～139为非实时进程。
    当static_prio分配好后，prio和normal_prio计算方法实现如下：
    首先，大家都知道进程创建过程中do_fork会调用wake_up_new_task,在该函数中会调用static int effective_prio(struct task_struct *p)函数。
    void fastcall wake_up_new_task(struct task_struct *p, unsigned long clone_flags)
    {
            unsigned long flags;
               struct rq *rq;
        ...
            p->prio = effective_prio(p);
        ...
    }
    static int effective_prio(struct task_struct *p)函数的实现如下：
    static int effective_prio(struct task_struct *p)
    {
            p->normal_prio = normal_prio(p);
                /*
             * If we are RT tasks or we were boosted to RT priority,
             * keep the priority unchanged. Otherwise, update priority
             * to the normal priority:
             */
            if (!rt_prio(p->prio))
                    return p->normal_prio;
            return p->prio;
    }
    在函数中设置了normal_prio的值，返回值有设置了prio，真是一箭双雕，对于实时进程需要特殊处理，总结主要涉及非实时进进程，就对实时进程的处理方法不解释了。
    static inline int normal_prio(struct task_struct *p)的实现如下：
        static inline int normal_prio(struct task_struct *p)
    {
            int prio;

            if (task_has_rt_policy(p))
                    prio = MAX_RT_PRIO-1 - p->rt_priority;
            else
                    prio = __normal_prio(p);
            return prio;
    }
    对于普通进程会调用static inline int __normal_prio(struct task_struct *p)函数。
    static inline int __normal_prio(struct task_struct *p)函数的实现如下：
        static inline int __normal_prio(struct task_struct *p)
    {
            return p->static_prio;
    }
    这样大家应该很清楚了，对于非实时进程prio，normal_prio和static_prio是一样的，但是也有特殊情况，当使用实时互斥量时prio会暂时发生变化。



普通进程的优先级通过一个关于静态优先级和进程交互性函数关系计算得到。随实际任务的实际运行情况得到。实时优先级和它的实时优先级成线性，不随进程的运行而改变。

3、实时优先级：

实时优先级只对实时进程有意义。在进程描述符rt_priority中。取值0~MAX_RT_PRIO-1。

    prio=MAX_RT_PRIO-1 – rt_priority
    时间片：

    在完全公平调度器CFS融入内核之前，时间片是各种调度器的一个重要的概念。它指定了进程在被抢占之前所能运行的时间。调用器的一个重要目标便是有效的分配时间片，以便提供良好的用户体验。时间片分的过长会导致交互式进程响应不佳。时间片分的过长会导致进程切换带来的消耗。为了解决这个矛盾内核采用了：

    1、提高交互进程的优先级，同时分配默认的时间片

    2、不需要进程一次性用完时间片，可多次使用。

    高的优先级可保证交互进程的频繁调用，长的时间片可保证它们可长时间处于可执行状态

           实时进程优先级：

                实时优先级分为SCHED_FIFO,SCHED_RR两类，有软实时硬实时之分，FIFO/RR 都有动态优先级，没有静态优先级。内核提供的修改优先级的函数，一般是修改rt_priority的值。rt_priority的取值范围[1,99]。

                     prio  = MAX_RT_PRIO – 1 – rt_priority     其中MAX_RT_PRIO = 100

http://www.cnblogs.com/zhaoyl/archive/2012/09/04/2671156.html
-->

## 进程关系

在 `task_struct` 结构中，保存了一些字段，用来维护各个进程之间的关系。

在 Linux 中，线程是通过轻量级进程 (LWP) 实现，会为每个进程和线程分配一个 PID，同时我们希望由一个进程产生的轻量级进程具有相同的 PID，这样当我们向进程发送信号时，此信号可以影响进程及进程产生的轻量级进程。

为此，采用线程组 (可以理解为轻量级进程组) 的概念，在线程组内，每个线程都使用此线程组内第一个线程 (thread group leader) 的 pid，并将此值存入tgid，当我们使用 `getpid()` 函数得到进程 ID 时，其实操作系统返回的是 `task_struct` 的 tgid 字段，而非 pid 字段。

{% highlight text %}
struct task_struct {
    pid_t pid; pid_t tgid;            // 用于标示线程和线程组ID
    struct task_struct *real_parent;  // 实际父进程real parent process
    struct task_struct *parent;       // SIGCHLD的接受者，由wait4()报告
    struct list_head children;        // 子进程列表
    struct list_head sibling;         // 兄弟进程列表
    struct task_struct *group_leader; // 线程组的leader
};
$ ps -eo  uid,pid,ppid,pgid,sid,pidns,tty,comm

getpid()[tgid]、gettid()[pid]、getppid()[real_parent]、getsid()

getuid()、getgid()、geteuid()、getegid()、getgroups()、getresuid()、getresgid()、getpgid()、<br><br>

getpgrp()/getpgid()/setpgid()        // 获取或者设置进程组
{% endhighlight %}

`gettid()` 返回线程号，如果是单线程与 `getpid()` 相同，该值在整个 Linux 系统内是唯一的；`pthread_self()` 返回的线程号只能保证在进程中是唯一的。

![process relations]({{ site.url }}/images/linux/process-relations.jpg "process relations"){: .pull-center }

下面是内核提供的系统调用，实现有点复杂，可以通过注释查看返回的 `task_strcut` 中的值。

Linux 中的进程组是为了方便对进程进行管理，假设要完成一个任务，需要同时并发 100 个进程，当用户处于某种原因要终止这个任务时，可以将这些进程设置备为同一个进程组，然后向进程组发送信号。

进程必定属于一个进程组，也只能属于一个进程组；一个进程组中可以包含多个进程，进程组的生命周期从被创建开始，到其内所有进程终止或离开该组。

由于 Linux 是多用户多任务的分时系统，所以必须要支持多个用户同时使用一个操作系统，当一个用户登录一次系统就形成一次会话。

一个或多个进程组可以组成会话，由其中一个进程建立，该进程叫做会话的领导进程 (session leader)，会话领导进程的 PID 成为识别会话的 SID(session ID)；一个会话可包含多个进程组，但只能有一个前台进程组。

会话中的每个进程组称为一个作业 (job)，bash(Bourne-Again shell) 支持作业控制，而 sh(Bourne shell) 并不支持。

会话可以有一个进程组成为会话的前台工作 (foreground)，而其他的进程组是后台工作 (background)。每个会话可以连接一个控制终端 (control terminal)，当控制终端有输入输出时，都传递给该会话的前台进程组，由终端产生的信号，比如 `CTRL+Z`、`CTRL+\` 会传递到前台进程组。

会话的意义在于将多个工作囊括在一个终端，并取其中的一个工作作为前台，来直接接收该终端的输入输出以及终端信号，其他工作在后台运行。一般开始于用户登录，终止于用户退出，此期间所有进程都属于这个会话期。

一个工作可以通过 fg 从后台工作变为前台工作，如 `fg %1` 。

### 进程组和会话期

当通过 SSH 或者 telent 远程登录到 Linux 服务器时，如果执行一个长时间运行的任务，如果关掉窗口或者断开连接，这个任务就会被杀掉，一切都半途而废了，这主要是因为 `SIGHUP` 信号。

在 Linux/Unix 中，有这样几个概念：

* 进程组 (process group): 一个或多个进程的集合，每一个进程组有唯一一个进程组 ID ，即进程组长进程的 ID 。
* 会话期 (session): 一个或多个进程组的集合，有唯一一个会话期首进程 (session leader)，会话期 ID 为首进程的 ID 。
* 会话期可以有一个单独的控制终端（controlling terminal）。与控制终端连接的会话期首进程叫做控制进程（controlling process）。当前与终端交互的进程称为前台进程组，其余进程组称为后台进程组。

根据 POSIX.1 的定义，挂断信号 (SIGHUP) 默认的动作是终止程序；当终端接口检测到网络连接断开，将挂断信号发送给控制进程 (会话期首进程)；如果会话期首进程终止，则该信号发送到该会话期前台进程组；一个进程退出导致一个孤儿进程组中产生时，如果任意一个孤儿进程组进程处于 STOP 状态，发送 SIGHUP 和 SIGCONT 信号到该进程组中所有进程。

因此当网络断开或终端窗口关闭后，控制进程收到 SIGHUP 信号退出，会导致该会话期内其他进程退出。

打开两个 SSH 终端窗口，或者两个 gnome-terminal，在其中一个运行 top 命令。在另一个终端窗口，找到 top 的进程 ID 为 24317 ，其父进程 ID 为 24230 ，即登录 shell 。使用 pstree 命令可以更清楚地看到这个关系。

{% highlight text %}
# top

# ps -ef | grep top
UID        PID  PPID  C STIME    TTY       TIME CMD
andy     24317 24230  2 13:45 pts/16   00:00:05 top
andy     24526 24419  0 13:48 pts/17   00:00:00 grep --color=auto top

# pstree -H 24317 | grep top
|-sshd-+-sshd-+-sshd---bash---top
{% endhighlight %}

使用 `ps -xj` 命令可以看到，登录 shell (PID 24230) 和 top 在同一个会话期， shell 为会话期首进程，所在进程组 PGID 为 24230， top 所在进程组 PGID 为 24317 ，为前台进程组。

{% highlight text %}
~$ ps -xj | grep 24230
 PPID   PID  PGID   SID    TTY   TPGID STAT   UID   TIME COMMAND
24229 24230 24230 24230 pts/16   24317 Ss    1000   0:00 -bash
24230 24317 24317 24230 pts/16   24317 S+    1000   0:18 top
24419 24543 24542 24419 pts/17   24542 S+    1000   0:00 grep --color=auto 2423
{% endhighlight %}

关闭第一个 SSH 窗口，在另一个窗口中可以看到 top 也被杀掉了，如果我们可以忽略 SIGHUP 信号，关掉窗口应该就不会影响程序的运行了。

nohup 命令可以达到这个目的，如果程序的标准输出/标准错误是终端， nohup 默认将其重定向到 nohup.out 文件。值得注意的是 nohup 命令只是使得程序忽略 SIGHUP 信号，还需要使用标记 `&` 把它放在后台运行。

<!--
（1）进程必定属于一个进程组，也只能属于一个进程组。
     一个进程组中可以包含多个进程。
     进程组的生命周期从被创建开始，到其内所有进程终止或离开该组。

     获取当前进程所在进程组ID使用函数getpgrp
     创建或加入其他组使用函数setpgid

（2）假设条件：pid1进程属于pgid1进程组；pid2属于pgid2进程组，并且是pgid2进程组组长；另有进程组pgid3，

     在pid1进程中调用setpgid（pid2，pgid3）；
     a）当pid2和pgid3都>0且不相等时
        功能：将pid2进程加入到pgid3组。此时pid2进程脱离pgid2进程组，进入pgid3进程组。
     b）当pid2和pgid3都>0且相等时
        功能：pid2进程创建新进程组，成为新进程组长（pgid3=pid2）。
     c）当pid2＝＝0，pgid>0时
        功能：将调用进程pid1加入到pgid3中。此时pid1脱离pgid1，进入pgid3。
     d）当pid2>0，pgid＝＝0时
        功能：将pid2加入到调用进程所在的pgid1进程组。此时pid2脱离pgid2，进入pgid1。
     e）当pid2和pgid3都＝＝0时，返回错误。

（3）一次登录就形成一次会话，会话组长即创建会话的进程。
     只有不是进程组长的进程才能创建新会话。

（4）如果pid1进程属于pgid1进程组，且不是组长，属于会话sid1。
     在pid1进程中调用setsid（）；
     功能：pid1进程脱离pgid1进程组，创建一个新的会话sid2（sid2没有控制终端），pid1进程加入到pgid2组（pgid2＝＝pid1）。
-->


## 进程状态

Linux是一个多用户，多任务的系统，可以同时运行多个用户的多个程序，就必然会产生很多的进程，而每个进程会有不同的状态，可以参考 `task_state_array[]@fs/proc/array.c` 中的内容。

{% highlight text %}
static const char * const task_state_array[] = {
    "R (running)",      /*   0 */
    "S (sleeping)",     /*   1 */
    "D (disk sleep)",   /*   2 */
    "T (stopped)",      /*   4 */
    "t (tracing stop)", /*   8 */
    "X (dead)",         /*  16 */
    "Z (zombie)",       /*  32 */
};
{% endhighlight %}

<!--
#### R (TASK_RUNNING，可执行状态)

只有在该状态的进程才可能在 CPU 上运行。而同一时刻可能有多个进程处于可执行状态，这些进程的 task_struct 结构（进程控制块）被放入对应 CPU 的可执行队列中（一个进程最多只能出现在一个 CPU 的可执行队列中）。进程调度器的任务就是从各个 CPU 的可执行队列中分别选择一个进程在该 CPU 上运行。<br><br>

    很多教科书将正在 CPU 上执行的进程定义为 RUNNING 状态、而将可执行但是尚未被调度执行的进程定义为 READY 状态，这两种状态在 Linux 下统一为 TASK_RUNNING 状态。</li><br><li>

    S (TASK_INTERRUPTIBLE，可中断的睡眠状态)<br>
    处于这个状态的进程因为等待某某事件的发生（比如等待 socket 连接、等待信号量），而被挂起。这些进程的 task_struct 结构被放入对应事件的等待队列中。当这些事件发生时（由外部中断触发、或由其他进程触发），对应的等待队列中的一个或多个进程将被唤醒。<br><br>

    通过 ps 命令我们会看到，一般情况下，进程列表中的绝大多数进程都处于 TASK_INTERRUPTIBLE 状态（除非机器的负载很高）。毕竟 CPU 就这么一两个，进程动辄几十上百个，如果不是绝大多数进程都在睡眠， CPU 又怎么响应得过来。</li><br><li>

    D (TASK_UNINTERRUPTIBLE，不可中断的睡眠状态)<br>
    与 TASK_INTERRUPTIBLE 状态类似，进程处于睡眠状态，但是此刻进程是不可中断的。不可中断，指的并不是 CPU 不响应外部硬件的中断，而是指进程不响应异步信号。<br><br>

    绝大多数情况下，进程处在睡眠状态时，总是应该能够响应异步信号的。否则你将惊奇的发现， kill -9 竟然杀不死一个正在睡眠的进程了！<br><br>

    而 TASK_UNINTERRUPTIBLE 状态存在的意义就在于，内核的某些处理流程是不能被打断的。如果响应异步信号，程序的执行流程中就会被插入一段用于处理异步信号的流程（这个插入的流程可能只存在于内核态，也可能延伸到用户态），于是原有的流程就被中断了。<br><br>

    在进程对某些硬件进行操作时（比如进程调用 read 系统调用对某个设备文件进行读操作，而 read 系统调用最终执行到对应设备驱动的代码，并与对应的物理设备进行交互），可能需要使用 TASK_UNINTERRUPTIBLE 状态对进程进行保护，以避免进程与设备交互的过程被打断，造成设备陷入不可控的状态。这种情况下的 TASK_UNINTERRUPTIBLE 状态总是非常短暂的，通过 ps 命令基本上不可能捕捉到。</li><br><li>

    T (TASK_STOPPED or TASK_TRACED，暂停状态或跟踪状态)<br>
    向进程发送一个 SIGSTOP 信号，它就会因响应该信号而进入 TASK_STOPPED 状态（除非该进程本身处于 TASK_UNINTERRUPTIBLE 状态而不响应信号）。SIGSTOP 与 SIGKILL 信号一样，是强制的，不允许用户进程通过 signal 系列的系统调用重新设置对应的信号处理函数。<br><br>

    向进程发送一个 SIGCONT 信号，可以让其从 TASK_STOPPED 状态恢复到 TASK_RUNNING 状态。<br><br>

    当进程正在被跟踪时，它处于 TASK_TRACED 这个特殊的状态。“正在被跟踪”指的是进程暂停下来，等待跟踪它的进程对它进行操作。比如在 gdb 中对被跟踪的进程下一个断点，进程在断点处停下来的时候就处于 TASK_TRACED 状态。而在其他时候，被跟踪的进程还是处于前面提到的那些状态。<br><br>

    对于进程本身来说， TASK_STOPPED 和 TASK_TRACED 状态很类似，都是表示进程暂停下来。而 TASK_TRACED 状态相当于在 TASK_STOPPED 之上多了一层保护，处于 TASK_TRACED 状态的进程不能响应 SIGCONT 信号而被唤醒。只能等到调试进程通过 ptrace 系统调用执行 PTRACE_CONT 、 PTRACE_DETACH 等操作（通过 ptrace 系统调用的参数指定操作），或调试进程退出，被调试的进程才能恢复 TASK_RUNNING 状态。
</li></ul>


有一类垃圾却并非这么容易打扫，那就是我们常见的状态为 D (Uninterruptible sleep) ，以及状态为 Z (Zombie) 的垃圾进程。这些垃圾进程要么是求而不得，像怨妇一般等待资源(D)，要么是僵而不死，像冤魂一样等待超度(Z)，它们在 CPU run_queue 里滞留不去，把 Load Average 弄的老高老高，没看过我前一篇blog的国际友人还以为这儿民怨沸腾又出了什么大事呢。怎么办？开枪！kill -9！看你们走是不走。但这两种垃圾进程偏偏是刀枪不入的，不管换哪种枪法都杀不掉它们。无奈，只好reboot，像剿灭禽流感那样不分青红皂白地一律扑杀！

贫僧还是回来说正题。怨妇 D，往往是由于 I/O 资源得不到满足，而引发等待，在内核源码 fs/proc/array.c 里，其文字定义为“ "D (disk sleep)", /* 2 */ ”（由此可知 D 原是Disk的打头字母），对应着 include/linux/sched.h 里的“ #define TASK_UNINTERRUPTIBLE 2 ”。举个例子，当 NFS 服务端关闭之时，若未事先 umount 相关目录，在 NFS 客户端执行 df 就会挂住整个登录会话，按 Ctrl+C 、Ctrl+Z 都无济于事。断开连接再登录，执行 ps axf 则看到刚才的 df 进程状态位已变成了 D ，kill -9 无法杀灭。正确的处理方式，是马上恢复 NFS 服务端，再度提供服务，刚才挂起的 df 进程发现了其苦苦等待的资源，便完成任务，自动消亡。若 NFS 服务端无法恢复服务，在 reboot 之前也应将 /etc/mtab 里的相关 NFS mount 项删除，以免 reboot 过程例行调用 netfs stop 时再次发生等待资源，导致系统重启过程挂起。

D是处于TASK_UNINTERRUPTIBLE的进程，深度睡眠，不响应信号。 一般只有等待非常关键的事件时，才把进程设为这个状态。

　　冤魂 Z 之所以杀不死，是因为它已经死了，否则怎么叫 Zombie（僵尸）呢？冤魂不散，自然是生前有结未解之故。在UNIX/Linux中，每个进程都有一个父进程，进程号叫PID（Process ID），相应地，父进程号就叫PPID（Parent PID）。当进程死亡时，它会自动关闭已打开的文件，舍弃已占用的内存、交换空间等等系统资源，然后向其父进程返回一个退出状态值，报告死讯。如果程序有 bug，就会在这最后一步出问题。儿子说我死了，老子却没听见，没有及时收棺入殓，儿子便成了僵尸。在UNIX/Linux中消灭僵尸的手段比较残忍，执行 ps axjf 找出僵尸进程的父进程号（PPID，第一列），先杀其父，然后再由进程天子 init（其PID为1，PPID为0）来一起收拾父子僵尸，超度亡魂，往生极乐。注意，子进程变成僵尸只是碍眼而已，并不碍事，如果僵尸的父进程当前有要务在身，则千万不可贸然杀之。
注意：不是所有状态为Z的进程都是无法收拾的，很可能是那个短暂的状态刚好被你发现了。
-->


### 特殊状态处理

`TASK_STOPPED`，进程终止，通常是由于向进程发送了 `SIGSTOP`、`SIGTSTP`、`SIGTTIN`、`SIGTTOU` 信号，此时可以通过 `kill -9(SIGKILL) pid` 尝试杀死进程，如果不起作用则 `kill -18 pid` ，也就是发个 `SIGCONT` 信号过去。


#### defunct

子进程是通过父进程创建的，子进程的结束和父进程的运行是一个异步过程，父进程永远无法预测子进程到底什么时候结束。当一个进程完成它的工作终止之后，它的父进程需要调用 `wait()` 或者 `waitpid()` 取得子进程的终止状态。

孤儿进程：一个父进程退出，相应的一个或多个子进程还在运行，那么那些子进程将成为孤儿进程。孤儿进程将被 init 进程所收养，并由 init 进程收集它们的完成状态。注意，孤儿进程没有危害，最终仍然回被 init 回收。

僵尸进程：一个进程使用 fork 创建子进程，如果子进程退出，而父进程并没有调用 wait 或 waitpid 获取子进程的状态信息，那么子进程的进程描述符仍然保存在系统中，仍然占用进程表，显示为 defunct 状态。可以通过重启或者杀死父进程解决。

在 Linux 中，进程退出时，内核释放该进程所有的部分资源，包括打开的文件、占用的内存等。但仍为其保留一定的信息，包括进程号 PID、退出的状态、运行时间等，直到父进程通过 `wait()` 或 `waitpid()` 来获取时才释放。

如果父进程一直存在，那么该进程的进程号就会一直被占用，而系统所能使用的进程号是有限的，如果大量的产生僵死进程，将因为没有可用的进程号而导致系统不能产生新的进程。

如下是两个示例，分别为孤儿进程和僵尸进程。

{% highlight c %}
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>

int main()
{
    pid_t pid = fork();
    if (fpid &lt; 0) {
        printf("error in fork!");
        exit(1);
    }
    if (pid == 0) { // child process.
        printf("child process create, pid: %d\tppid:%d\n",getpid(),getppid());
        sleep(5);   // sleep for 5s until father process exit.
        printf("child process exit, pid: %d\tppid:%d\n",getpid(),getppid());
    } else {
        printf("father process create\n");
        sleep(1);
        printf("father process exit\n");
    }
    return 0;
}


#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>

//static void sig_child(int signo)
//{
//     pid_t        pid;
//     int        stat;
//     while ((pid = waitpid(-1, &stat, WNOHANG)) &gt; 0)
//            printf("child %d terminated.\n", pid);
//}
int main ()
{
    pid_t fpid;
    // signal(SIGCHLD, sig_child);
    fpid = fork();
    if (fpid &lt; 0) {
        printf("error in fork!");
        exit(1);
    }
    if (fpid == 0) {
        printf("child process(%d)\n", getpid());
        exit(0);
    }
    printf("father process\n");
    sleep(2);
    system("ps -o pid,ppid,state,tty,command | grep defunct | grep -v grep");
    return 0;
}
{% endhighlight %}

第一个是孤儿进程，第二次输出时其父进程 PID 变成了 `init(PID=1)`；第二个是僵尸进程，进程退出时会产生 `SIGCHLD` 信号，父进程可以通过捕获该信号进行处理。


## 进程退出

当进程正常或异常终止时，内核就向其父进程发送 `SIGCHLD` 信号，对于 `wait()` 以及 `waitpid()` 进程可能会出现如下场景：

* 如果其所有子进程都在运行则阻塞；
* 如果某个子进程已经停止，则获取该子进程的退出状态并立即返回；
* 如果没有任何子进程，则立即出错返回。

如果进程由于接收到 `SIGCHLD` 信号而调用 wait，则一般会立即返回；但是，如果在任意时刻调用 wait 则进程可能会阻塞。

{% highlight text %}
#include <sys/wait.h>
pid_t wait(int *status);
pit_t wait(pid_t pid, int *status, int options);
{% endhighlight %}

### 获取返回值

如果上述参数中的 status 不是 NULL，那么会把子进程退出时的状态返回，该返回值保存了是否为正常退出、正常结束的返回值、被那个信号终止等。

通常使用如下的宏定义。

{% highlight text %}
WIFEXITED(status)
    用于判断子进程是否正常退出，正常退出返回0；否则返回一个非零值。

WEXITSTATUS(status)
    当WIFEXITED返回非零时，可以通过这个宏获取子进程的返回值，如果exit(5)则返回5。
    注意，如果WIFEXITED返回零时，这个返回值则无意义。
{% endhighlight %}

可以参考如下示例。

{% highlight c %}
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>

int main(void)
{
	pid_t pid;
        int status;

	pid = fork();
	if (pid < 0) {
		perror("fork");
		exit(1);
	} else if (pid == 0) { /* child */
                exit(7);
	}

	/* parent */
	if (wait(&status) < 0) {
		perror("wait");
		exit(1);
	}

	if(WIFEXITED(status)) {
		fprintf(stdout, "Normal termination, exit status %d\n",
			WEXITSTATUS(status));
        } else if(WIFSIGNALED(status)) {
		fprintf(stderr, "Abnormal termination, signal status %d\n",
			WTERMSIG(status),
			WCOREDUMP(status) ? "(core file)" : "");
        } else if(WIFSTOPPED(status)) {
		fprintf(stderr, "Child stopped, signal number %d\n",
			WSTOPSIG(status));
	}

        exit(0);
}

{% endhighlight %}

### waitpid()

当要等待一特定进程退出时，可调用 `waitpid()` 函数，其中第一个入参 `pid` 的入参含义如下：

* `pid=-1` 等待任一个子进程，与 wait 等效。
* `pid>0` 等待其进程 ID 与 pid 相等的子进程。
* `pid==0` 等待其进程组 ID 等于调用进程组 ID 的任一个子进程。
* `pid<-1` 等待其进程组 ID 等于 pid 绝对值的任一子进程。

`waitpid` 返回终止子进程的进程 ID，并将该子进程的终止状态保存在 status 中。

<!---
waitpid的返回值比wait稍微复杂一些，一共有3种情况：
    当正常返回的时候，waitpid返回收集到的子进程的进程ID；
    如果设置了选项WNOHANG，而调用中waitpid发现没有已退出的子进程可收集，则返回0；
    如果调用中出错，则返回-1，这时errno会被设置成相应的值以指示错误所在；
当pid所指示的子进程不存在，或此进程存在，但不是调用进程的子进程，waitpid就会出错返回，这时errno被设置为ECHILD；
-->




## 其它


### 指定CPU

Affinity 是进程的一个属性，这个属性指明了进程调度器能够把这个进程调度到哪些 CPU 上，可以利用 CPU affinity 把一个或多个进程绑定到一个或多个 CPU 上。

CPU Affinity 分为 soft affinity 和 hard affinity； soft affinity 仅是一个建议，如果不可避免，调度器还是会把进程调度到其它的 CPU 上；hard affinity 是调度器必须遵守的规则。

增加 CPU 缓存的命中率。CPU 之间是不共享缓存的，如果进程频繁的在各个 CPU 间进行切换，需要不断的使旧 CPU 的 cache 失效。如果进程只在某个 CPU 上执行，则不会出现失效的情况。

另外，在多个线程操作的是相同的数据的情况下，如果把这些线程调度到一个处理器上，大大的增加了 CPU 缓存的命中率。但是可能会导致并发性能的降低。如果这些线程是串行的，则没有这个影响。

适合 time-sensitive 应用，在 real-time 或 time-sensitive 应用中，我们可以把系统进程绑定到某些 CPU 上，把应用进程绑定到剩余的 CPU 上。

CPU 集可以认为是一个掩码，每个设置的位都对应一个可以合法调度的 CPU，而未设置的位则对应一个不可调度的 CPU。换而言之，线程都被绑定了，只能在那些对应位被设置了的处理器上运行。通常，掩码中的所有位都被置位了，也就是可以在所有的 CPU 中调度。

另外可以通过 [schedutils](http://sourceforge.net/projects/schedutils/) 或者 python-schedutils 进行设置，后者现在更加常见。

对于如何将进程和线程绑定到指定的 CPU 可以参考 [github affinity_process.c]({{ site.example_repository }}/linux/process/affinity_process.c)、[github affinity_thread.c]({{ site.example_repository }}/linux/process/affinity_thread.c) 。

### 杀死进程的N中方法

查看进程通常可以通过如下命令

{% highlight text %}
$ ps -ef
$ ps -aux
......
smx    1827     1   4 11:38 ?        00:27:33 /usr/lib/firefox-3.6.18/firefox-bin
......
{% endhighlight %}

此时如果我想杀了火狐的进程就在终端输入：

{% highlight text %}
$ kill -s 9 1827
{% endhighlight %}

其中 `-s 9` 制定了传递给进程的信号是 9，即强制、尽快终止进程。

无论是 `ps -ef` 还是 `ps -aux`，每次都要在一大串进程信息里面查找到要杀的进程，看的眼都花了。因此通过如下的方法进行改进。

#### 使用grep

把 ps 的查询结果通过管道给 grep 查找包含特定字符串的进程。管道符 `|` 用来隔开两个命令，管道符左边命令的输出会作为管道符右边命令的输入。

{% highlight text %}
$ ps -ef | grep firefox
smx    1827     1   4 11:38 ?        00:27:33 /usr/lib/firefox-3.6.18/firefox-bin
smx    12029  1824  0 21:54 pts/0    00:00:00 grep --color=auto firefox
$ kill -s 9 1827
{% endhighlight %}

#### 使用pgrep

pgrep 的 p 表明了这个命令是专门用于进程查询的 grep 。

{% highlight text %}
$ pgrep firefox
1827
$ kill -s 9 1827
{% endhighlight %}

#### 使用pidof

实际就是 pid of xx，字面翻译过来就是 xx 的 PID ，和 pgrep 相比稍显不足的是，pidof 必须给出进程的全名。

{% highlight text %}
$ pidof firefox-bin
1827
$kill -s 9 1827
{% endhighlight %}

无论是使用 ps 然后慢慢查找进程 PID 还是用 grep 查找包含相应字符串的进程，亦或者用 pgrep 直接查找包含相应字符串的进程 PID ，然后手动输入给 Kill 杀掉，都稍显麻烦。

#### 一步完成

{% highlight text %}
$ps -ef | grep firefox | grep -v grep | cut -c 9-15 | xargs kill -s 9
{% endhighlight %}

<!--
<ul><li>“grep firefox”的输出结果是，所有含有关键字“firefox”的进程。</li><li>
“grep -v grep”是在列出的进程中去除含有关键字“grep”的进程。</li><li>
“cut -c 9-15”是截取输入行的第9个字符到第15个字符，而这正好是进程号PID。</li><li>
“xargs kill -s 9”中的xargs命令是用来把前面命令的输出结果（PID）作为“kill -s 9”命令的参数，并执行该命令。“kill -s 9”会强行杀掉指定进程。
-->

#### 使用 pgrep/pidof

{% highlight text %}
$ pgrep firefox | xargs kill -s 9
{% endhighlight %}

#### 使用awk

{% highlight text %}
$ ps -ef | grep firefox | awk '{print $2}' | xargs kill -9
kill: No such process
{% endhighlight %}

<!--
有一个比较郁闷的地方，进程已经正确找到并且终止了，但是执行完却提示找不到进程。<br><br>
其中awk '{print $2}' 的作用就是打印（print）出第二列的内容。根据常规篇，可以知道ps输出的第二列正好是PID。就把进程相应的PID通过xargs传递给kill作参数，杀掉对应的进程。</li><br><li>
-->

#### 替换 xargs

{% highlight text %}
$ kill -s 9 `ps -aux | grep firefox | awk '{print $2}'`
{% endhighlight %}


#### 换成 pgrep

{% highlight text %}
$ kill -s 9 `pgrep firefox`
{% endhighlight %}


#### 使用 pkill

pkill＝pgrep+kill。

{% highlight text %}
$ pkill -9 firefox
{% endhighlight %}

说明："-9" 即发送的信号是9，pkill与kill在这点的差别是：pkill无须 “ｓ”，终止信号等级直接跟在 “-“ 后面。

#### 使用 killall

killall和pkill是相似的,不过如果给出的进程名不完整，killall会报错。pkill或者pgrep只要给出进程名的一部分就可以终止进程。

{% highlight text %}
$ killall -9 firefox
{% endhighlight %}


### 信号量

<center><table align="center" style = "font-size: small;" bgcolor="lightyellow" border="0" cellpadding="3" cellspacing="0" width="70%">
<tbody>
<tr align="center" bgcolor="lightblue">
	<td width="60">信号</td><td>说明</td><td>编号</td></tr>
	<tr><td align="center"><span class="text_import1">SIGHUP</span></td>
	<font style="margin: 5px 0 5px 0; padding-left:25px; color: #000088"><td>
		Hangup</td><td>1</td></font></tr>
	<tr><td align="center"><span class="text_import1">SIGINT</span></td>
	<font style="margin: 5px 0 5px 0; padding-left:25px; color: #000088"><td>
		Interrupt</td><td>2</td></font></tr>
	<tr><td align="center"><span class="text_import1">SIGQUIT</span></td>
	<font style="margin: 5px 0 5px 0; padding-left:25px; color: #000088"><td>
		Quit and dump core</td><td>3</td></font></tr>
	<tr><td align="center"><span class="text_import1">SIGILL</span></td>
	<font style="margin: 5px 0 5px 0; padding-left:25px; color: #000088"><td>
		Illegal instruction</td><td>4</td></font></tr>
	<tr><td align="center"><span class="text_import1">SIGTRAP</span></td>
	<font style="margin: 5px 0 5px 0; padding-left:25px; color: #000088"><td>
 		Trace/breakpoint trap</td><td>5</td></font></tr>
	<tr><td align="center"><span class="text_import1">SIGABRT</span></td>
	<font style="margin: 5px 0 5px 0; padding-left:25px; color: #000088"><td>
		Process aborted</td><td>6</td></font></tr>
	<tr><td align="center"><span class="text_import1">SIGBUS</span></td>
	<font style="margin: 5px 0 5px 0; padding-left:25px; color: #000088"><td>
		Bus error: "access to undefined portion of memory object"</td><td>7</td></font></tr>
	<tr><td align="center"><span class="text_import1">SIGFPE</span></td>
	<font style="margin: 5px 0 5px 0; padding-left:25px; color: #000088"><td>
		Floating point exception: "erroneous arithmetic operation"</td><td>8</td></font></tr>
	<tr><td align="center"><span class="text_import1">SIGKILL</span></td>
	<font style="margin: 5px 0 5px 0; padding-left:25px; color: #000088"><td>
		Kill (terminate immediately)</td><td>9</td></font></tr>
	<tr><td align="center"><span class="text_import1">SIGUSR1</span></td>
	<font style="margin: 5px 0 5px 0; padding-left:25px; color: #000088"><td>
		User-defined 1</td><td>10</td></font></tr>
	<tr><td align="center"><span class="text_import1">SIGSEGV</span></td>
	<font style="margin: 5px 0 5px 0; padding-left:25px; color: #000088"><td>
		Segmentation violation</td><td>11</td></font></tr>
	<tr><td align="center"><span class="text_import1">SIGUSR2</span></td>
	<font style="margin: 5px 0 5px 0; padding-left:25px; color: #000088"><td>
		User-defined 2</td><td>12</td></font></tr>
	<tr><td align="center"><span class="text_import1">SIGPIPE</span></td>
	<font style="margin: 5px 0 5px 0; padding-left:25px; color: #000088"><td>
		Write to pipe with no one reading</td><td>13</td></font></tr>
	<tr><td align="center"><span class="text_import1">SIGALRM</span></td>
	<font style="margin: 5px 0 5px 0; padding-left:25px; color: #000088"><td>
		Signal raised by alarm</td><td>14</td></font></tr>
	<tr><td align="center"><span class="text_import1">SIGTERM</span></td>
	<font style="margin: 5px 0 5px 0; padding-left:25px; color: #000088"><td>
		Termination (request to terminate)</td><td>15</td></font></tr>
	<tr><td align="center"><span class="text_import1">SIGCHLD</span></td>
	<font style="margin: 5px 0 5px 0; padding-left:25px; color: #000088"><td>
		Child process terminated, stopped (or continued*)</td><td>17</td></font></tr>
	<tr><td align="center"><span class="text_import1">SIGCONT</span></td>
	<font style="margin: 5px 0 5px 0; padding-left:25px; color: #000088"><td>
		Continue if stopped</td><td>18</td></font></tr>
	<tr><td align="center"><span class="text_import1">SIGSTOP</span></td>
	<font style="margin: 5px 0 5px 0; padding-left:25px; color: #000088"><td>
		Stop executing temporarily</td><td>19</td></font></tr>
	<tr><td align="center"><span class="text_import1">SIGTSTP</span></td>
	<font style="margin: 5px 0 5px 0; padding-left:25px; color: #000088"><td>
		Terminal stop signal</td><td>20</td></font></tr>
	<tr><td align="center"><span class="text_import1">SIGTTIN</span></td>
	<font style="margin: 5px 0 5px 0; padding-left:25px; color: #000088"><td>
		Background process attempting to read from tty ("in")</td><td>21</td></font></tr>
	<tr><td align="center"><span class="text_import1">SIGTTOU</span></td>
	<font style="margin: 5px 0 5px 0; padding-left:25px; color: #000088"><td>
		Background process attempting to write to tty ("out")</td><td>22</td></font></tr>
	<tr><td align="center"><span class="text_import1">SIGURG</span></td>
	<font style="margin: 5px 0 5px 0; padding-left:25px; color: #000088"><td>
		Urgent data available on socket</td><td>23</td></font></tr>
	<tr><td align="center"><span class="text_import1">SIGXCPU</span></td>
	<font style="margin: 5px 0 5px 0; padding-left:25px; color: #000088"><td>
		CPU time limit exceeded</td><td>24</td></font></tr>
	<tr><td align="center"><span class="text_import1">SIGXFSZ</span></td>
	<font style="margin: 5px 0 5px 0; padding-left:25px; color: #000088"><td>
		File size limit exceeded</td><td>25</td></font></tr>
	<tr><td align="center"><span class="text_import1">SIGVTALRM</span></td>
	<font style="margin: 5px 0 5px 0; padding-left:25px; color: #000088"><td>
		Signal raised by timer counting virtual time: "virtual timer expired"</td><td>26</td></font></tr>
	<tr><td align="center"><span class="text_import1">SIGPROF</span></td>
	<font style="margin: 5px 0 5px 0; padding-left:25px; color: #000088"><td>
		Profiling timer expired</td><td>27</td></font></tr>
	<tr><td align="center"><span class="text_import1">SIGPOLL</span></td>
	<font style="margin: 5px 0 5px 0; padding-left:25px; color: #000088"><td>
		Pollable event</td><td>29</td></font></tr>
	<tr><td align="center"><span class="text_import1">SIGSYS</span></td>
	<font style="margin: 5px 0 5px 0; padding-left:25px; color: #000088"><td>
		Bad syscall</td><td>31</td></font></tr>
</tbody></table></center>



## 参考

[神奇的vfork (local)](http://blog.csdn.net/ctthuangcheng/article/details/8914613)，一个针对错误使用 vfork() 时的场景分新。

[进程描述和进程创建](http://blog.csdn.net/zhoudaxia/article/details/7366832)，用于描述进程，其中包括了进程创建以及退出，一篇不错的文章。


<!--
[进程的创建 do_fork() 函数详解](http://blog.csdn.net/yunsongice/article/details/5508242)，介绍do_fork()的执行过程。

[进程切换和调度算法深入分析](http://blog.csdn.net/cxylaf/article/details/1626529)，

[Linux实时调度器](http://blog.chinaunix.net/uid-10705106-id-3541730.html) 。
-->


{% highlight text %}
{% endhighlight %}
