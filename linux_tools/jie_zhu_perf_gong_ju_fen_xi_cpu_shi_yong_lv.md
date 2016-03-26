# 藉助PERF工具分析CPU使用率


如果CPU的使用率突然暴漲，如何迅速定位是哪個進程、哪段代碼引起的呢？我們需要一個profiling工具，對CPU上執行的代碼進行採樣、統計，告訴我們CPU到底在忙些什麼。

perf 就是這樣的工具。我們舉個例子看看 perf 是怎樣工作的。

首先我們用以下命令模擬出CPU利用率暴漲的現象：

```sh
$ cat /dev/zero > /dev/null
```

然後我們看到 CPU 1 的 %system 飆升到95%：


```sh
# sar -P ALL -u 2 2
 
08:21:16 PM     CPU     %user     %nice   %system   %iowait    %steal     %idle
08:21:18 PM     all      2.25      0.00     48.25      0.00      0.00     49.50
08:21:18 PM       0      0.50      0.00      1.00      0.00      0.00     98.51
08:21:18 PM       1      4.02      0.00     95.98      0.00      0.00      0.00

```

現在我們用 perf 工具採樣：

```sh
# perf record -a -e cycles -o cycle.perf -g sleep 10
[ perf record: Woken up 18 times to write data ]
[ perf record: Captured and wrote 4.953 MB cycle.perf (~216405 samples) ]
```

注：”-a”表示對所有CPU採樣，如果只需針對特定的CPU，可以使用”-C”選項。

把採樣的數據生成報告：

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

我們很清楚地看到，CPU利用率有75%來自 cat 進程 的 sys_read 系統調用，perf 甚至精確地告訴了我們是消耗在 read_zero 這個 kernel routine 上。

