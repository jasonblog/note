# 系統級性能分析工具 — Perf


從2.6.31內核開始，linux內核自帶了一個性能分析工具perf，能夠進行函數級與指令級的熱點查找。
 
##perf
 
Performance analysis tools for Linux.
Performance counters for Linux are a new kernel-based subsystem that provide a framework for all things
performance analysis. It covers hardware level (CPU/PMU, Performance Monitoring Unit) features and
software features (software counters, tracepoints) as well.


Perf是內置於Linux內核源碼樹中的性能剖析(profiling)工具。
它基於事件採樣原理，以性能事件為基礎，支持針對處理器相關性能指標與操作系統相關性能指標的性能剖析。
常用於性能瓶頸的查找與熱點代碼的定位。
 
CPU週期(cpu-cycles)是默認的性能事件，所謂的CPU週期是指CPU所能識別的最小時間單元，通常為億分之幾秒，
是CPU執行最簡單的指令時所需要的時間，例如讀取寄存器中的內容，也叫做clock tick。
 
Perf是一個包含22種子工具的工具集，以下是最常用的5種：
- perf-list
- perf-stat
- perf-top
- perf-record
- perf-report


##perf-list
 
Perf-list用來查看perf所支持的性能事件，有軟件的也有硬件的。
 
List all symbolic event types.<br>
perf list [hw | sw | cache | tracepoint | event_glob]<br>
 
###(1) 性能事件的分佈
- hw：Hardware event，9個
- sw：Software event，9個
- cache：Hardware cache event，26個
- tracepoint：Tracepoint event，775個
 
sw實際上是內核的計數器，與硬件無關。<br><br>
hw和cache是CPU架構相關的，依賴於具體硬件。<br>
tracepoint是基於內核的ftrace，主線2.6.3x以上的內核版本才支持。<br>

###(2) 指定性能事件(以它的屬性)
- -e <event> : u // userspace
- -e <event> : k // kernel
- -e <event> : h // hypervisor
- -e <event> : G // guest counting (in KVM guests)
- -e <event> : H // host counting (not in KVM guests)
 
###(3) 使用例子
顯示內核和模塊中，消耗最多CPU週期的函數：
```sh
# perf top -e cycles:k
```

顯示分配高速緩存最多的函數：
```sh
# perf top -e kmem:kmem_cache_alloc
```

##perf-top
 
對於一個指定的性能事件(默認是CPU週期)，顯示消耗最多的函數或指令。<br>
 
System profiling tool.<br>
Generates and displays a performance counter profile in real time.<br>
perf top [-e <EVENT> | --event=EVENT] [<options>]<br>
 
perf top主要用於實時分析各個函數在某個性能事件上的熱度，能夠快速的定位熱點函數，包括應用程序函數、
模塊函數與內核函數，甚至能夠定位到熱點指令。默認的性能事件為cpu cycles。<br>


###(1) 輸出格式

perf top


```sh
Samples: 1M of event 'cycles', Event count (approx.): 73891391490  
     5.44%  perf              [.] 0x0000000000023256        
     4.86%  [kernel]          [k] _spin_lock                
     2.43%  [kernel]          [k] _spin_lock_bh             
     2.29%  [kernel]          [k] _spin_lock_irqsave        
     1.77%  [kernel]          [k] __d_lookup                
     1.55%  libc-2.12.so      [.] __strcmp_sse42            
     1.43%  nginx             [.] ngx_vslprintf             
     1.37%  [kernel]          [k] tcp_poll            
```

第一列：符號引發的性能事件的比例，默認指佔用的cpu週期比例。<br>
第二列：符號所在的DSO(Dynamic Shared Object)，可以是應用程序、內核、動態鏈接庫、模塊。<br>
第三列：DSO的類型。[.]表示此符號屬於用戶態的ELF文件，包括可執行文件與動態鏈接庫)。[k]表述此符號屬於內核或模塊。<br>
第四列：符號名。有些符號不能解析為函數名，只能用地址表示。<br>

###(2) 常用交互命令
- h：顯示幫助
UP/DOWN/PGUP/PGDN/SPACE：上下和翻頁。
- a：annotate current symbol，註解當前符號。能夠給出彙編語言的註解，給出各條指令的採樣率。
- d：過濾掉所有不屬於此DSO的符號。非常方便查看同一類別的符號。
- P：將當前信息保存到perf.hist.N中。
 
###(3) 常用命令行參數
- -e <event>：指明要分析的性能事件。
- -p <pid>：Profile events on existing Process ID (comma sperated list). 僅分析目標進程及其創建的線程。
- -k <path>：Path to vmlinux. Required for annotation functionality. 帶符號表的內核映像所在的路徑。
- -K：不顯示屬於內核或模塊的符號。
- -U：不顯示屬於用戶態程序的符號。
- -d <n>：界面的刷新週期，默認為2s，因為perf top默認每2s從mmap的內存區域讀取一次性能數據。
- -G：得到函數的調用關係圖。

```sh
perf top -G [fractal]，路徑概率為相對值，加起來為100%，調用順序為從下往上。
perf top -G graph，路徑概率為絕對值，加起來為該函數的熱度。
```
 
###(4) 使用例子

```sh
# perf top // 默認配置
# perf top -G // 得到調用關係圖
# perf top -e cycles // 指定性能事件
# perf top -p 23015,32476 // 查看這兩個進程的cpu cycles使用情況
# perf top -s comm,pid,symbol // 顯示調用symbol的進程名和進程號
# perf top --comms nginx,top // 僅顯示屬於指定進程的符號
# perf top --symbols kfree // 僅顯示指定的符號
```

##perf-stat
 
用於分析指定程序的性能概況。
 
Run a command and gather performance counter statistics.

```sh
perf stat [-e <EVENT> | --event=EVENT] [-a] <command>
perf stat [-e <EVENT> | --event=EVENT] [-a] - <command> [<options>]
```
###(1) 輸出格式

```sh
# perf stat ls
```

```sh
Performance counter stats for 'ls':  
  
         0.653782 task-clock                #    0.691 CPUs utilized            
                0 context-switches          #    0.000 K/sec                    
                0 CPU-migrations            #    0.000 K/sec                    
              247 page-faults               #    0.378 M/sec                    
        1,625,426 cycles                    #    2.486 GHz                      
        1,050,293 stalled-cycles-frontend   #   64.62% frontend cycles idle     
          838,781 stalled-cycles-backend    #   51.60% backend  cycles idle     
        1,055,735 instructions              #    0.65  insns per cycle          
                                            #    0.99  stalled cycles per insn  
          210,587 branches                  #  322.106 M/sec                    
           10,809 branch-misses             #    5.13% of all branches          
  
      0.000945883 seconds time elapsed  
```

輸出包括ls的執行時間，以及10個性能事件的統計。
- task-clock：任務真正佔用的處理器時間，單位為ms。CPUs utilized = task-clock / time elapsed，CPU的佔用率。
- context-switches：上下文的切換次數。
- CPU-migrations：處理器遷移次數。Linux為了維持多個處理器的負載均衡，在特定條件下會將某個任務從一個CPU
遷移到另一個CPU。
- page-faults：缺頁異常的次數。當應用程序請求的頁面尚未建立、請求的頁面不在內存中，或者請求的頁面雖然在內
存中，但物理地址和虛擬地址的映射關係尚未建立時，都會觸發一次缺頁異常。另外TLB不命中，頁面訪問權限不匹配
等情況也會觸發缺頁異常。
- cycles：消耗的處理器週期數。如果把被ls使用的cpu cycles看成是一個處理器的，那麼它的主頻為2.486GHz。
可以用cycles / task-clock算出。
- stalled-cycles-frontend：略過。
- stalled-cycles-backend：略過。
- instructions：執行了多少條指令。IPC為平均每個cpu cycle執行了多少條指令。
- branches：遇到的分支指令數。branch-misses是預測錯誤的分支指令數。
 
###(2) 常用參數

- -p：stat events on existing process id (comma separated list). 僅分析目標進程及其創建的線程。
- -a：system-wide collection from all CPUs. 從所有CPU上收集性能數據。
- -r：repeat command and print average + stddev (max: 100). 重複執行命令求平均。
- -C：Count only on the list of CPUs provided (comma separated list), 從指定CPU上收集性能數據。
- -v：be more verbose (show counter open errors, etc), 顯示更多性能數據。
- -n：null run - don't start any counters，只顯示任務的執行時間 。
- -x SEP：指定輸出列的分隔符。
- -o file：指定輸出文件，--append指定追加模式。
- --pre <cmd>：執行目標程序前先執行的程序。
- --post <cmd>：執行目標程序後再執行的程序。
 
####(3) 使用例子
執行10次程序，給出標準偏差與期望的比值：
```sh
# perf stat -r 10 ls > /dev/null
```

顯示更詳細的信息：
```sh
# perf stat -v ls > /dev/null
```
只顯示任務執行時間，不顯示性能計數器：
```sh
# perf stat -n ls > /dev/null
```
單獨給出每個CPU上的信息：
```sh
# perf stat -a -A ls > /dev/null
```
ls命令執行了多少次系統調用：
```sh
# perf stat -e syscalls:sys_enter ls 
```


##perf-record
 
收集採樣信息，並將其記錄在數據文件中。
隨後可以通過其它工具(perf-report)對數據文件進行分析，結果類似於perf-top的。
 
Run a command and record its profile into perf.data.
This command runs a command and gathers a performance counter profile from it, into perf.data,
without displaying anything. This file can then be inspected later on, using perf report.
 
### (1) 常用參數
- -e：Select the PMU event.
- -a：System-wide collection from all CPUs.
- -p：Record events on existing process ID (comma separated list).
- -A：Append to the output file to do incremental profiling.
- -f：Overwrite existing data file.
- -o：Output file name.
- -g：Do call-graph (stack chain/backtrace) recording.
- -C：Collect samples only on the list of CPUs provided.
 
###(2) 使用例子
記錄nginx進程的性能數據：

```sh
# perf record -p `pgrep -d ',' nginx`
```

記錄執行ls時的性能數據：
```sh
# perf record ls -g
```

記錄執行ls時的系統調用，可以知道哪些系統調用最頻繁：

```sh
# perf record -e syscalls:sys_enter ls
```

##perf-report
 
讀取perf record創建的數據文件，並給出熱點分析結果。
 
Read perf.data (created by perf record) and display the profile.
This command displays the performance counter profile information recorded via perf record.
 
###(1) 常用參數
- -i：Input file name. (default: perf.data)
 
### (2) 使用例子
```sh
# perf report -i perf.data.2
```

More
 
除了以上5個常用工具外，還有一些適用於較特殊場景的工具， 比如內核鎖、slab分配器、調度器，
也支持自定義探測點。

## perf-lock

內核鎖的性能分析。
Analyze lock events.
perf lock {record | report | script | info}
 
需要編譯選項的支持：CONFIG_LOCKDEP、CONFIG_LOCK_STAT。
CONFIG_LOCKDEP defines acquired and release events.
CONFIG_LOCK_STAT defines contended and acquired lock events.
 
###(1) 常用選項
```sh
-i <file>：輸入文件
-k <value>：sorting key，默認為acquired，還可以按contended、wait_total、wait_max和wait_min來排序。
```

### (2) 使用例子
```sh
# perf lock record ls // 記錄
# perf lock report // 報告
```

### (3) 輸出格式

```sh
Name   acquired  contended total wait (ns)   max wait (ns)   min wait (ns)   
  
&mm->page_table_...        382          0               0               0               0   
&mm->page_table_...         72          0               0               0               0   
          &fs->lock         64          0               0               0               0   
        dcache_lock         62          0               0               0               0   
      vfsmount_lock         43          0               0               0               0   
&newf->file_lock...         41          0               0               0               0   
```

- Name：內核鎖的名字。
- aquired：該鎖被直接獲得的次數，因為沒有其它內核路徑佔用該鎖，此時不用等待。
- contended：該鎖等待後獲得的次數，此時被其它內核路徑佔用，需要等待。
- total wait：為了獲得該鎖，總共的等待時間。
- max wait：為了獲得該鎖，最大的等待時間。
- min wait：為了獲得該鎖，最小的等待時間。

最後還有一個Summary：

```sh
=== output for debug===  
  
bad: 10, total: 246  
bad rate: 4.065041 %  
histogram of events caused bad sequence  
    acquire: 0  
   acquired: 0  
  contended: 0  
    release: 10
```

## perf-kmem

slab分配器的性能分析。
Tool to trace/measure kernel memory(slab) properties.<br>
perf kmem {record | stat} [<options>]<br>
 
###(1) 常用選項
- --i <file>：輸入文件
- --caller：show per-callsite statistics，顯示內核中調用kmalloc和kfree的地方。
- --alloc：show per-allocation statistics，顯示分配的內存地址。
- -l <num>：print n lines only，只顯示num行。
- -s <key[,key2...]>：sort the output (default: frag,hit,bytes)


###(2) 使用例子
```sh
# perf kmem record ls // 記錄
# perf kmem stat --caller --alloc -l 20 // 報告
```
###(3) 輸出格式

```sh
------------------------------------------------------------------------------------------------------  
 Callsite                           | Total_alloc/Per | Total_req/Per   | Hit      | Ping-pong | Frag  
------------------------------------------------------------------------------------------------------  
 perf_event_mmap+ec                 |    311296/8192  |    155952/4104  |       38 |        0 | 49.902%  
 proc_reg_open+41                   |        64/64    |        40/40    |        1 |        0 | 37.500%  
 __kmalloc_node+4d                  |      1024/1024  |       664/664   |        1 |        0 | 35.156%  
 ext3_readdir+5bd                   |        64/64    |        48/48    |        1 |        0 | 25.000%  
 load_elf_binary+8ec                |       512/512   |       392/392   |        1 |        0 | 23.438%
 ```
 
- Callsite：內核代碼中調用kmalloc和kfree的地方。
- Total_alloc/Per：總共分配的內存大小，平均每次分配的內存大小。
- Total_req/Per：總共請求的內存大小，平均每次請求的內存大小。
- Hit：調用的次數。
- Ping-pong：kmalloc和kfree不被同一個CPU執行時的次數，這會導致cache效率降低。
- Frag：碎片所佔的百分比，碎片 = 分配的內存 - 請求的內存，這部分是浪費的。

有使用--alloc選項，還會看到Alloc Ptr，即所分配內存的地址。
最後還有一個Summary：

```sh
SUMMARY  
=======  
Total bytes requested: 290544  
Total bytes allocated: 447016  
Total bytes wasted on internal fragmentation: 156472  
Internal fragmentation: 35.003669%  
Cross CPU allocations: 2/509  
```

## probe-sched

調度模塊分析。
trace/measure scheduler properties (latencies)<br>
perf sched {record | latency | map | replay | script}<br>

###(1) 使用例子

```sh
# perf sched record sleep 10 // perf sched record <command>
# perf report latency --sort max
```
###(2) 輸出格式

```sh
---------------------------------------------------------------------------------------------------------------  
 Task                  |   Runtime ms  | Switches | Average delay ms | Maximum delay ms | Maximum delay at     |  
---------------------------------------------------------------------------------------------------------------  
 events/10:61          |      0.655 ms |       10 | avg:    0.045 ms | max:    0.161 ms | max at: 9804.958730 s  
 sleep:11156           |      2.263 ms |        4 | avg:    0.052 ms | max:    0.118 ms | max at: 9804.865552 s  
 edac-poller:1125      |      0.598 ms |       10 | avg:    0.042 ms | max:    0.113 ms | max at: 9804.958698 s  
 events/2:53           |      0.676 ms |       10 | avg:    0.037 ms | max:    0.102 ms | max at: 9814.751605 s  
 perf:11155            |      2.109 ms |        1 | avg:    0.068 ms | max:    0.068 ms | max at: 9814.867918 s 
```
- TASK：進程名和pid。
- Runtime：實際的運行時間。
- Switches：進程切換的次數。
- Average delay：平均的調度延遲。
- Maximum delay：最大的調度延遲。
- Maximum delay at：最大調度延遲發生的時刻。

## perf-probe

可以自定義探測點。
Define new dynamic tracepoints.
 
使用例子
###(1) Display which lines in schedule() can be probed
```sh
# perf probe --line schedule
```

前面有行號的可以探測，沒有行號的就不行了。
 
###(2) Add a probe on schedule() function 12th line.
```sh
# perf probe -a schedule:12
```

在schedule函數的12處增加一個探測點。

##Reference
 
[1]. Linux的系統級性能剖析工具系列，by 承剛<br>
[2]. http://www.ibm.com/developerworks/cn/linux/l-cn-perf1/<br>
[3]. http://www.ibm.com/developerworks/cn/linux/l-cn-perf2/<br>
[4]. https://perf.wiki.kernel.org/index.php/Tutorial<br>