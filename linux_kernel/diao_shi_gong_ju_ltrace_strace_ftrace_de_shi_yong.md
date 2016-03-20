# 調試工具ltrace strace ftrace的使用


最近學習了一些調試工具，包括ltrace strace ftrace。這些都屬於不同層級的調試工具。 下面是我畫的簡易的層次關係圖。



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

systemtap是當下功能強大的內核函數追蹤工具，我們編寫特定的script就可以調試內核函數，由於這個篇幅有限，我將在其他文章中進行介紹。

先從最簡單的說起ltrace起。 拿最簡單的hello world程序來說，printf調用的lic裡面的庫函數說白了就是put（）,put()函數返回值就是打印字符的個數，包括轉移字符\n。

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

下面我來說一下strace的功能，追蹤system call 與 signal。所謂系統調用，就是內核提供的、功能十分強大的一系列的函數。這些系統調用是在內核中實現的，比如linux中的POSIX標準就是指的這一些。再通過一定的方式把系統調用給用戶，一般都通過門(gate)陷入(trap)實現。系統調用是用戶程序和內核交互的接口。

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
通過查看上面的system call，我們就會對elf文件載入流程有一個清晰的認識。

流程：

```sh
1.調用execve()函數執行載入

2.brk() allocate new space to load the infomation of programmer

3.mmap()把elf頭載入virtual address

4.先鏈接ld.so與ld.so.cache中是否存在之前調用過庫函數的絕對地址

5.查看file 狀態的fstat()，包括r w x 等

6.讀取ELF頭，並映射到虛擬地址，進行內存保護mprotect（）

7.載入libc.so庫函數

8.arch_prctl()設置運行環境的體系結構

9.write（）就是內核中寫函數，包括髮消息給其他的用戶，寫入設備等。 

10.完成調用，退出。
```

Ftrace 是一個內核中的追蹤器，用於幫助系統開發者或設計者查看內核運行情況，它可以被用來調試或者分析延遲/性能問題。最早 ftrace 是一個 function tracer，僅能夠記錄內核的函數調用流程。如今 ftrace 已經成為一個 framework，採用 plugin 的方式支持開發人員添加更多種類的 trace 功能。

Ftrace需要kernel支持 
```sh
CONFIG_FUNCTION_TRACER 
CONFIG_FUNCTION_GRAPH_TRACER 
CONFIG_CONTEXT_SWITCH_TRACER 
CONFIG_NOP_TRACER 
CONFIG_SCHED_TRACER Debugfs 
```
勾選，這樣才可以使用ftrace中的一些特定功能。

編譯內核完成後，重新開機載入新內核。 ftrace不同於其他的調試工具，他需要debugfs的輔助。debugfs是一種特殊的文件系統，本身無法進行編輯，任何寫入信息都要靠echo載入。另外由於是kernel debug，所以需要最高的root權限。 我們要先掛載這個文件系統到特殊的文件目錄。這個/mnt/與/sys/kernel/debug/tracing是等同的。

```sh
[root@localhost /]# mount -t debugfs debugfs /mnt/ 
[root@localhost /]# cd /mnt/ 
[root@localhost mnt]# ls 
acpi         cleancache     dri            hid        iwlwifi             mei     sched_features  usb             x86 bdi          cxgb4          dynamic_debug  ideapad    kernel_page_tables  mmc0    sleep_time      vgaswitcheroo   xen bluetooth    debug_objects  extfrag        ieee80211  kprobes             rcu     suspend_stats   virtio-ports boot_params  dma_buf        frontswap      ips        mce                 regmap  tracing         wakeup_sources
[root@localhost mnt]# cd tracing/ 
[root@localhost tracing]# ls 
available_events            enabled_functions         max_graph_depth  set_ftrace_filter   stack_trace_filter  tracing_cpumask available_filter_functions  events                    options          set_ftrace_notrace  trace               tracing_max_latency available_tracers           free_buffer               per_cpu          set_ftrace_pid      trace_clock         tracing_on buffer_size_kb              function_profile_enabled  printk_formats   set_graph_function  trace_marker        tracing_thresh buffer_total_size_kb        instances                 README           snapshot            trace_options       uprobe_events current_tracer              kprobe_events             saved_cmdlines   stack_max_size      trace_pipe          uprobe_profile dyn_ftrace_total_info       kprobe_profile            set_event        stack_trace         trace_stat 
```


============= Ftrace 的普通使用步驟如下：===============================

掛載Debugfs: Ftrace 通過 debugfs 向用戶態提供訪問接口。配置內核時激活 debugfs 後會創建目錄 /sys/kernel/debug ，debugfs 文件系統就是掛載 到該目錄。 

###1.1 運行時掛載： Officially mount method :

```sh
# mount -t debugfs nodev /sys/kernel/debug
OR:
# mkdir /debug
# mount -t debugfs nodev /debug
# cd /debug/tracing

OR: # mount -t debugfs nodev /sys/kernel/debug
# ln -s /sys/kernel/debug /debug
```

###1.2 系統啟動自動掛載： 要在系統啟動自動掛載debugfs，需要將如下內容添加到

```sh
/etc/fstab 文件： debugfs /sys/kernel/debug debugfs defaults 0 0
```

- 選擇一種 tracer: # cat current_tracer nop // no option # cat available_tracers blk function_graph function nop # echo function_graph > current_tracer

- 打開關閉追蹤（在老的內核上有tracing_enabled，需要給tracing_enabled和tracing_on同時賦1 才能打開追蹤，而在新的內核上去掉tracing_enabled只需要控制tracing_on 即可打開關閉追蹤） # echo 1 > tracing_on; run_test; echo 0 > tracing_on

注：其實ftrace_enabled並不是去掉了，而是從 tracing目錄中去掉，我們還是可以在 /proc/sys/kernel/ftrace_enabled 目錄下看到他的身影，而且默認已經被設置為1，所以現在我們只需要echo 1到tracing_on 中即可打開追蹤。 $ cat /proc/sys/kernel/ftrace_enabled 1

##查看追蹤結果：

ftrace 的輸出信息主要保存在 3 個文件中。

- Trace，該文件保存 ftrace 的輸出信息，其內容可以直接閱讀。
- latency_trace，保存與 trace 相同的信息，不過組織方式略有不同。主要為了用戶能方便地分析系統中有關延遲的信息。
- trace_pipe 是一個管道文件，主要為了方便應用程序讀取 trace 內容。算是擴展接口吧。


所以可以直接查看 trace 追蹤文件，也可以在追蹤之前使用trace_pipe 將追蹤結果直接導向其他的文件。 比如： # cat trace_pipe > /tmp/log & # cat /tmp/log OR # cat trace

============= Ftrace 的進階使用：================

使用 echo pid > set_ftrace_pid 來追蹤特定的進程！

##2. 追蹤事件：

```sh
    2.1 首先查看事件文件夾下面有哪些選項：
        # ls events/
    block       ext4    header_event  jbd2    napi    raw_syscalls  …… enable
        # ls events/sched/
    enable          sched_kthread_stop_ret  sched_process_exit  sched_process_wait  ……
 
    2.2 追蹤一個/若干事件：
        # echo 1 > events/sched/sched_wakeup/enable
        # cat trace | head -10
        # tracer: nop
        #TASK-PID  CPU#  TIMESTAMP    FUNCTION
        #   |       |       |           |
        bash-2613 [001] 425.078164: sched_wakeup: task bash:2613 [120] success=0 [001]
        bash-2613 [001] 425.078184: sched_wakeup: task bash:2613 [120] success=0 [001]
        ...
 
    2.3 追蹤一類事件：
        # echo 1 > events/sched/enable
        # cat trace | head -10
        # tracer: nop
        #TASK-PID        CPU#  TIMESTAMP    FUNCTION
        #   |             |       |           |
        events/0-9      [000] 638.042792: sched_switch: task events/0:9 [120] (S) ==> kondemand/0:1305 [120]
        ondemand/0-1305 [000] 638.042796: sched_stat_wait: task: restorecond:1395 wait: 15023 [ns]
        ...
 
    2.4 追蹤所有事件：
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
##4. 將要跟蹤的函數寫入文件 
set_ftrace_filter ，將不希望跟蹤的函數寫入文件 set_ftrace_notrace。通常直接操作文件 set_ftrace_filter 就可以了.



============= Ftrace 提供的函數使用=============

內核頭文件 include/linux/kernel.h 中描述了 ftrace 提供的工具函數的原型，這些函數包括 trace_printk、tracing_on/tracing_off 等。

使用 `trace_printk` 打印跟蹤信息
ftrace 提供了一個用於向 ftrace 跟蹤緩衝區輸出跟蹤信息的工具函數，叫做 trace_printk()，它的使用方式與 printk() 類似。可以通過 trace 文件讀取該函數的輸出。從頭文件 include/linux/kernel.h 中可以看到，在激活配置 CONFIG_TRACING 後，trace_printk() 定義為宏： #define trace_printk(fmt, args…) \ … 所以在使用時：(例子是在一個內核模塊中添加打印信息)

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

使用 tracing_on/tracing_off 控制跟蹤信息的記錄
在跟蹤過程中，有時候在檢測到某些事件發生時，想要停止跟蹤信息的記錄，這樣，跟蹤緩衝區中較新的數據是與該事件有關的。在用戶態，可以通過向文件 tracing_on 寫入 0 來停止記錄跟蹤信息，寫入 1 會繼續記錄跟蹤信息。而在內核代碼中，可以通過函數 tracing_on() 和 tracing_off() 來做到這一點，它們的行為類似於對 /sys/kernel/debug/tracing 下的文件 tracing_on 分別執行寫 1 和 寫 0 的操作。

使用這兩個函數，會對跟蹤信息的記錄控制地更準確一些，這是因為在用戶態寫文件 tracing_on 到實際暫停跟蹤，中間由於上下文切換、系統調度控制等可能已經經過較長的時間，這樣會積累大量的跟蹤信息，而感興趣的那部分可能會被覆蓋掉了。

實際代碼中，可以通過特定條件（比如檢測到某種異常狀況，等等）來控制跟蹤信息的記錄，函數的使用方式類似如下的形式：


```c
if (condition)
tracing_on() or tracing_off()
```


跟蹤模塊運行狀況時，使用 ftrace 命令操作序列在用戶態進行必要的設置，而在代碼中則可以通過 traceing_on() 控制在進入特定代碼區域時開啟跟蹤信息，並在遇到某些條件時通過 tracing_off() 暫停；讀者可以在查看完感興趣的信息後，將 1 寫入 tracing_on 文件以繼續記錄跟蹤信息。實踐中，可以通過宏來控制是否將對這些函數的調用編譯進內核模塊，這樣可以在調試時將其開啟，在最終發佈時將其關閉。 用戶態的應用程序可以通過直接讀寫文件 tracing_on 來控制記錄跟蹤信息的暫停狀態，以便了解應用程序運行期間內核中發生的活動。

如果我們要開啟追蹤功能。

```sh
echo 1 > tracing_on 
echo function_graph >current_tracer 
```

另外我們也可以設置要追蹤的pid值 event buffer等


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

ftrace 不僅可以追蹤內核中的函數，也可以追蹤用戶態下的函數是如何trap in kernel 然後ret的。 比如我們寫一個fork的demo


```c
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

int main(int argc, char** argv)
{
    int ret;
    int fd;
    int i = 0;
    char pidbuf[20];
    pid_t id;

    id = fork();

    if (id < 0) {
        fprintf(stderr, "Error in fork");
        exit(-1);
    } else if (id == 0) {
        scanf("%d", &i);

        ret = execv("hello", NULL);

        if (ret == -1) {
            fprintf(stderr, "Error in execv");
            exit(-1);
        }
    } else {
        sprintf(pidbuf, "%ld", (long)id);
        fd = open("/sys/kernel/debug/tracing/set_ftrace_pid", O_CREAT | O_RDWR, 0660);

        if (fd < 0) {
            fprintf(stderr, "Error in open");
            exit(-1);
        }

        write(fd, pidbuf, strlen(pidbuf));
        close(fd);
        fd = open("/sys/kernel/debug/tracing/tracing_on", O_CREAT | O_RDWR, 0660);
        write(fd, "1", 2);
        close(fd);
        printf("!!!!\n");
        sleep(5);
    }

    return 0;
}
```
然後使用ftrace進行追蹤，可以得到一個system call的完整的結果。
 