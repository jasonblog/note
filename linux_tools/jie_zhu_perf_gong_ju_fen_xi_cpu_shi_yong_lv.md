# 借助PERF工具分析CPU使用率


如果CPU的使用率突然暴涨，如何迅速定位是哪个进程、哪段代码引起的呢？我们需要一个profiling工具，对CPU上执行的代码进行采样、统计，告诉我们CPU到底在忙些什么。

perf 就是这样的工具。我们举个例子看看 perf 是怎样工作的。

首先我们用以下命令模拟出CPU利用率暴涨的现象：

```sh
$ cat /dev/zero > /dev/null
```

然后我们看到 CPU 1 的 %system 飙升到95%：


```sh
# sar -P ALL -u 2 2
 
08:21:16 PM     CPU     %user     %nice   %system   %iowait    %steal     %idle
08:21:18 PM     all      2.25      0.00     48.25      0.00      0.00     49.50
08:21:18 PM       0      0.50      0.00      1.00      0.00      0.00     98.51
08:21:18 PM       1      4.02      0.00     95.98      0.00      0.00      0.00

```

现在我们用 perf 工具采样：

```sh
# perf record -a -e cycles -o cycle.perf -g sleep 10
[ perf record: Woken up 18 times to write data ]
[ perf record: Captured and wrote 4.953 MB cycle.perf (~216405 samples) ]
```

注：”-a”表示对所有CPU采样，如果只需针对特定的CPU，可以使用”-C”选项。

把采样的数据生成报告：

```sh
# perf report -i cycle.perf | more
```

```sh
...
# Samples: 40K of event 'cycles'
# Event count (approx.): 18491174032
#
# Overhead          Command                   Shared Object              Symbol
# ........  ...............  ..............................    ................
#
    75.65%              cat  [kernel.kallsyms]                 [k] __clear_user                     
                        |
                        --- __clear_user
                           |          
                           |--99.56%-- read_zero
                           |          vfs_read
                           |          sys_read
                           |          system_call_fastpath
                           |          __GI___libc_read
                            --0.44%-- [...]
 
     2.34%              cat  [kernel.kallsyms]                 [k] system_call                      
                        |
                        --- system_call
                           |          
                           |--56.72%-- __write_nocancel
                           |          
                            --43.28%-- __GI___libc_read
...
```

我们很清楚地看到，CPU利用率有75%来自 cat 进程 的 sys_read 系统调用，perf 甚至精确地告诉了我们是消耗在 read_zero 这个 kernel routine 上。

