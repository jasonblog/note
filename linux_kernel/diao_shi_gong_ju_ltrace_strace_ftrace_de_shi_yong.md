# 调试工具ltrace strace ftrace的使用


最近学习了一些调试工具，包括ltrace strace ftrace。这些都属于不同层级的调试工具。 下面是我画的简易的层次关系图。



```sh
  App
  |
  |  <--------ltrace
  |
libc ld  < -------strace
  |
  |     <----------systemtap
 kernel  <---------ftrace
```

systemtap是当下功能强大的内核函数追踪工具，我们编写特定的script就可以调试内核函数，由于这个篇幅有限，我将在其他文章中进行介绍。

先从最简单的说起ltrace起。 拿最简单的hello world程序来说，printf调用的lic里面的库函数说白了就是put（）,put()函数返回值就是打印字符的个数，包括转移字符\n。

```sh
[root@localhost day3]# ltrace -f ./hello
[pid 15439] __libc_start_main(0x40052c, 1, 0x7fff226b6888, 0x400560
[pid 15439] puts("Hello world"Hello world
)                                                    = 12
[pid 15439] puts("Hello world"Hello world
)                                                    = 12
[pid 15439] +++ exited (status 0) +++
[root@localhost day3]#
```

下面我来说一下strace的功能，追踪system call 与 signal。所谓系统调用，就是内核提供的、功能十分强大的一系列的函数。这些系统调用是在内核中实现的，比如linux中的POSIX标准就是指的这一些。再通过一定的方式把系统调用给用户，一般都通过门(gate)陷入(trap)实现。系统调用是用户程序和内核交互的接口。

```sh
[root@localhost day3]# strace -f ./hello
execve("./hello", ["./hello"], [/* 51 vars */]) = 0
brk(0)                                  = 0x184e000
mmap(NULL, 4096, PROT_READ|PROT_WRITE, MAP_PRIVATE|MAP_ANONYMOUS, -1, 0) = 0x7ff8cecb5000
access("/etc/ld.so.preload", R_OK)      = -1 ENOENT (No such file or directory)
open("/etc/ld.so.cache", O_RDONLY|O_CLOEXEC) = 3
fstat(3, {st_mode=S_IFREG|0644, st_size=114694, ...}) = 0
mmap(NULL, 114694, PROT_READ, MAP_PRIVATE, 3, 0) = 0x7ff8cec98000
close(3)                                = 0
open("/lib64/libc.so.6", O_RDONLY|O_CLOEXEC) = 3
read(3, "\177ELF\2\1\1\3\0\0\0\0\0\0\0\0\3\0>\0\1\0\0\0\20\33\242\361<\0\0\0"..., 832) = 832 
fstat(3, {st_mode=S_IFREG|0755, st_size=2071376, ...}) = 0 
mmap(0x3cf1a00000, 3896312, PROT_READ|PROT_EXEC, MAP_PRIVATE|MAP_DENYWRITE, 3, 0) = 0x3cf1a00000 
mprotect(0x3cf1bad000, 2097152, PROT_NONE) = 0 
mmap(0x3cf1dad000, 24576, PROT_READ|PROT_WRITE, MAP_PRIVATE|MAP_FIXED|MAP_DENYWRITE, 3, 0x1ad000) = 0x3cf1dad000 mmap(0x3cf1db3000, 17400, PROT_READ|PROT_WRITE, MAP_PRIVATE|MAP_FIXED|MAP_ANONYMOUS, -1, 0) = 0x3cf1db3000 
close(3)                                = 0 
mmap(NULL, 4096, PROT_READ|PROT_WRITE, MAP_PRIVATE|MAP_ANONYMOUS, -1, 0) = 0x7ff8cec97000 
mmap(NULL, 8192, PROT_READ|PROT_WRITE, MAP_PRIVATE|MAP_ANONYMOUS, -1, 0) = 0x7ff8cec95000 
arch_prctl(ARCH_SET_FS, 0x7ff8cec95740) = 0 
mprotect(0x3cf1dad000, 16384, PROT_READ) = 0 
mprotect(0x600000, 4096, PROT_READ)     = 0 
mprotect(0x3cf1820000, 4096, PROT_READ) = 0 
munmap(0x7ff8cec98000, 114694)          = 0 
fstat(1, {st_mode=S_IFCHR|0620, st_rdev=makedev(136, 0), ...}) = 0 
mmap(NULL, 4096, PROT_READ|PROT_WRITE, MAP_PRIVATE|MAP_ANONYMOUS, -1, 0) = 0x7ff8cecb4000 
write(1, "Hello world\n", 12Hello world )           = 12 
write(1, "Hello world\n", 12Hello world )           = 12 
exit_group(0)                           = ? +++ 
exited with 0 +++ 
[root@localhost day3]# 
```
通过查看上面的system call，我们就会对elf文件载入流程有一个清晰的认识。

流程：

```sh
1.调用execve()函数执行载入

2.brk() allocate new space to load the infomation of programmer

3.mmap()把elf头载入virtual address

4.先链接ld.so与ld.so.cache中是否存在之前调用过库函数的绝对地址

5.查看file 状态的fstat()，包括r w x 等

6.读取ELF头，并映射到虚拟地址，进行内存保护mprotect（）

7.载入libc.so库函数

8.arch_prctl()设置运行环境的体系结构

9.write（）就是内核中写函数，包括发消息给其他的用户，写入设备等。 

10.完成调用，退出。
```

Ftrace 是一个内核中的追踪器，用于帮助系统开发者或设计者查看内核运行情况，它可以被用来调试或者分析延迟/性能问题。最早 ftrace 是一个 function tracer，仅能够记录内核的函数调用流程。如今 ftrace 已经成为一个 framework，采用 plugin 的方式支持开发人员添加更多种类的 trace 功能。

Ftrace需要kernel支持 
```sh
CONFIG_FUNCTION_TRACER 
CONFIG_FUNCTION_GRAPH_TRACER 
CONFIG_CONTEXT_SWITCH_TRACER 
CONFIG_NOP_TRACER 
CONFIG_SCHED_TRACER Debugfs 
```
勾选，这样才可以使用ftrace中的一些特定功能。

编译内核完成后，重新开机载入新内核。 ftrace不同于其他的调试工具，他需要debugfs的辅助。debugfs是一种特殊的文件系统，本身无法进行编辑，任何写入信息都要靠echo载入。另外由于是kernel debug，所以需要最高的root权限。 我们要先挂载这个文件系统到特殊的文件目录。这个/mnt/与/sys/kernel/debug/tracing是等同的。

```sh
[root@localhost /]# mount -t debugfs debugfs /mnt/ 
[root@localhost /]# cd /mnt/ 
[root@localhost mnt]# ls 
acpi         cleancache     dri            hid        iwlwifi             mei     sched_features  usb             x86 bdi          cxgb4          dynamic_debug  ideapad    kernel_page_tables  mmc0    sleep_time      vgaswitcheroo   xen bluetooth    debug_objects  extfrag        ieee80211  kprobes             rcu     suspend_stats   virtio-ports boot_params  dma_buf        frontswap      ips        mce                 regmap  tracing         wakeup_sources
[root@localhost mnt]# cd tracing/ 
[root@localhost tracing]# ls 
available_events            enabled_functions         max_graph_depth  set_ftrace_filter   stack_trace_filter  tracing_cpumask available_filter_functions  events                    options          set_ftrace_notrace  trace               tracing_max_latency available_tracers           free_buffer               per_cpu          set_ftrace_pid      trace_clock         tracing_on buffer_size_kb              function_profile_enabled  printk_formats   set_graph_function  trace_marker        tracing_thresh buffer_total_size_kb        instances                 README           snapshot            trace_options       uprobe_events current_tracer              kprobe_events             saved_cmdlines   stack_max_size      trace_pipe          uprobe_profile dyn_ftrace_total_info       kprobe_profile            set_event        stack_trace         trace_stat 
```


============= Ftrace 的普通使用步骤如下：===============================

挂载Debugfs: Ftrace 通过 debugfs 向用户态提供访问接口。配置内核时激活 debugfs 后会创建目录 /sys/kernel/debug ，debugfs 文件系统就是挂载 到该目录。 

###1.1 运行时挂载： Officially mount method :

```sh
# mount -t debugfs nodev /sys/kernel/debug
OR:
# mkdir /debug
# mount -t debugfs nodev /debug
# cd /debug/tracing

OR: # mount -t debugfs nodev /sys/kernel/debug
# ln -s /sys/kernel/debug /debug
```

###1.2 系统启动自动挂载： 要在系统启动自动挂载debugfs，需要将如下内容添加到

```sh
/etc/fstab 文件： debugfs /sys/kernel/debug debugfs defaults 0 0
```

- 选择一种 tracer: # cat current_tracer nop // no option # cat available_tracers blk function_graph function nop # echo function_graph > current_tracer

- 打开关闭追踪（在老的内核上有tracing_enabled，需要给tracing_enabled和tracing_on同时赋1 才能打开追踪，而在新的内核上去掉tracing_enabled只需要控制tracing_on 即可打开关闭追踪） # echo 1 > tracing_on; run_test; echo 0 > tracing_on

注：其实ftrace_enabled并不是去掉了，而是从 tracing目录中去掉，我们还是可以在 /proc/sys/kernel/ftrace_enabled 目录下看到他的身影，而且默认已经被设置为1，所以现在我们只需要echo 1到tracing_on 中即可打开追踪。 $ cat /proc/sys/kernel/ftrace_enabled 1

##查看追踪结果：

ftrace 的输出信息主要保存在 3 个文件中。

- Trace，该文件保存 ftrace 的输出信息，其内容可以直接阅读。
- latency_trace，保存与 trace 相同的信息，不过组织方式略有不同。主要为了用户能方便地分析系统中有关延迟的信息。
- trace_pipe 是一个管道文件，主要为了方便应用程序读取 trace 内容。算是扩展接口吧。


所以可以直接查看 trace 追踪文件，也可以在追踪之前使用trace_pipe 将追踪结果直接导向其他的文件。 比如： # cat trace_pipe > /tmp/log & # cat /tmp/log OR # cat trace

============= Ftrace 的进阶使用：================

使用 echo pid > set_ftrace_pid 来追踪特定的进程！

##2. 追踪事件：

```sh
    2.1 首先查看事件文件夹下面有哪些选项：
        # ls events/
    block       ext4    header_event  jbd2    napi    raw_syscalls  …… enable
        # ls events/sched/
    enable          sched_kthread_stop_ret  sched_process_exit  sched_process_wait  ……
 
    2.2 追踪一个/若干事件：
        # echo 1 > events/sched/sched_wakeup/enable
        # cat trace | head -10
        # tracer: nop
        #TASK-PID  CPU#  TIMESTAMP    FUNCTION
        #   |       |       |           |
        bash-2613 [001] 425.078164: sched_wakeup: task bash:2613 [120] success=0 [001]
        bash-2613 [001] 425.078184: sched_wakeup: task bash:2613 [120] success=0 [001]
        ...
 
    2.3 追踪一类事件：
        # echo 1 > events/sched/enable
        # cat trace | head -10
        # tracer: nop
        #TASK-PID        CPU#  TIMESTAMP    FUNCTION
        #   |             |       |           |
        events/0-9      [000] 638.042792: sched_switch: task events/0:9 [120] (S) ==> kondemand/0:1305 [120]
        ondemand/0-1305 [000] 638.042796: sched_stat_wait: task: restorecond:1395 wait: 15023 [ns]
        ...
 
    2.4 追踪所有事件：
        # echo 1 > events/enable
        # cat trace | head -10
        # tracer: nop
        #TASK-PID    CPU#  TIMESTAMP    FUNCTION
        #   |         |       |           |
        cpid-1470   [001]   794.947181:   kfree: call_site=ffffffff810c996d ptr=(null)
        acpid-1470  [001]   794.947182: sys_read -> 0x1
        acpid-1470  [001]   794.947183: sys_exit: NR 0 = 1
        ...
```

##3. stack_trace

```sh
    # echo 1 > /proc/sys/kernel/stack_tracer_enabled
OR  # kernel command line “stacktrace”
查看： # cat stack_trace
``` 
##4. 将要跟踪的函数写入文件 
set_ftrace_filter ，将不希望跟踪的函数写入文件 set_ftrace_notrace。通常直接操作文件 set_ftrace_filter 就可以了.



============= Ftrace 提供的函数使用=============

内核头文件 include/linux/kernel.h 中描述了 ftrace 提供的工具函数的原型，这些函数包括 trace_printk、tracing_on/tracing_off 等。

使用 `trace_printk` 打印跟踪信息
ftrace 提供了一个用于向 ftrace 跟踪缓冲区输出跟踪信息的工具函数，叫做 trace_printk()，它的使用方式与 printk() 类似。可以通过 trace 文件读取该函数的输出。从头文件 include/linux/kernel.h 中可以看到，在激活配置 CONFIG_TRACING 后，trace_printk() 定义为宏： #define trace_printk(fmt, args…) \ … 所以在使用时：(例子是在一个内核模块中添加打印信息)

```c
#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
MODULE_LICENSE("GPL");

static int ftrace_demo_init(void)
{
    trace_printk("Can not see this in trace unless loaded for the second time\n");
    return 0;
}

static void ftrace_demo_exit(void)
{
    trace_printk("Module unloading\n");
}

module_init(ftrace_demo_init);
module_exit(ftrace_demo_exit);
```

使用 tracing_on/tracing_off 控制跟踪信息的记录
在跟踪过程中，有时候在检测到某些事件发生时，想要停止跟踪信息的记录，这样，跟踪缓冲区中较新的数据是与该事件有关的。在用户态，可以通过向文件 tracing_on 写入 0 来停止记录跟踪信息，写入 1 会继续记录跟踪信息。而在内核代码中，可以通过函数 tracing_on() 和 tracing_off() 来做到这一点，它们的行为类似于对 /sys/kernel/debug/tracing 下的文件 tracing_on 分别执行写 1 和 写 0 的操作。

使用这两个函数，会对跟踪信息的记录控制地更准确一些，这是因为在用户态写文件 tracing_on 到实际暂停跟踪，中间由于上下文切换、系统调度控制等可能已经经过较长的时间，这样会积累大量的跟踪信息，而感兴趣的那部分可能会被覆盖掉了。

实际代码中，可以通过特定条件（比如检测到某种异常状况，等等）来控制跟踪信息的记录，函数的使用方式类似如下的形式：


```c
if (condition)
tracing_on() or tracing_off()
```


跟踪模块运行状况时，使用 ftrace 命令操作序列在用户态进行必要的设置，而在代码中则可以通过 traceing_on() 控制在进入特定代码区域时开启跟踪信息，并在遇到某些条件时通过 tracing_off() 暂停；读者可以在查看完感兴趣的信息后，将 1 写入 tracing_on 文件以继续记录跟踪信息。实践中，可以通过宏来控制是否将对这些函数的调用编译进内核模块，这样可以在调试时将其开启，在最终发布时将其关闭。 用户态的应用程序可以通过直接读写文件 tracing_on 来控制记录跟踪信息的暂停状态，以便了解应用程序运行期间内核中发生的活动。

如果我们要开启追踪功能。echo 1 > tracing_on echo function_graph >current_tracer 另外我们也可以设置要追踪的pid值 event buffer等


```sh
# tracer: function_graph
#
# CPU  DURATION                  FUNCTION CALLS
# |     |   |                     |   |   |   |
 2)               |     update_curr() {
 2)   0.147 us    |        update_min_vruntime();
 2)               |            cpuacct_charge() {
 2)   0.097 us    |            debug_lockdep_rcu_enabled();
 2)   0.082 us    |            rcu_is_cpu_idle();
 2)   0.120 us    |            debug_lockdep_rcu_enabled();
 2)   0.098 us    |            debug_lockdep_rcu_enabled();
 2)   0.094 us    |            rcu_is_cpu_idle();
 2)   0.099 us    |            rcu_lockdep_current_cpu_online();
 2)   0.072 us    |            debug_lockdep_rcu_enabled();
 2)   0.071 us    |            rcu_is_cpu_idle();
 2)   6.935 us    |                    }
 2)   8.757 us    |      }
 2)   0.269 us    |            __compute_runnable_contrib();
 2)   0.093 us    |            __update_entity_load_avg_contrib();
 2)   0.188 us    |            update_cfs_rq_blocked_load();
 2)   0.113 us    |            update_cfs_shares();
 2)               |            sched_slice.isra.38() {
 2)   0.206 us    |            calc_delta_mine();
 2)   0.962 us    |    }
```

ftrace 不仅可以追踪内核中的函数，也可以追踪用户态下的函数是如何trap in kernel 然后ret的。 比如我们写一个fork的demo


```c

```

 