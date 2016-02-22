# gdb常用指令


##1. 启动
```
 gdb 应用程序名
 gdb 应用程序名 core文件名
 gdb 应用程序名 pid
 gdb --args 应用程序名 应用程序的运行参数
```

###帮助:
```
 help 显示帮助
 info 显示程序状态
 set 修改
 show 显示gdb状态
```
###运行及运行环境设置:
 ```
 set args # 设置运行参数
 show args # 显示运行参数

 set env 变量名 = 值 # 设置环境变量
 unset env [变量名] # 取消环境变量
 show env [变量名] # 显示环境变量

 path 目录名 # 把目录添加到查找路径中
 show paths # 显示当前查找路径

 cd 目录 # 切换工作目录
 pwd # 显示当前工作目录

 tty /dev/pts/1 # 修改程序的输入输出到指定的tty
 set inferior-tty /dev/pts/1 # 修改程序的输出到指定的tty
 show inferior-tty
 show tty

 run 参数 # 运行
 start 参数 # 开始运行，但是会在main函数停止
 attach pid
 detach

 kill # 退出
 Ctrl-C # 中断(SIGINT)
 Ctrl-]
```

### 线程操作:
```sh
 info threads # 查看所有线程信息
 thread 线程id # 切换到指定线程
 thread apply [threadno | all ] 参数 # 对所有线程都应用某个命令
```

####子进程调试:
```sh
 set follow-fork-mode child|parent # fork后，需要跟踪谁
 show follow-fork-mode
 set detach-on-flow on|off # fork后，需要两个都跟踪吗

 info forks # 显示所有进程信息
 fork 进程id # 切换到某个进程
 detach-fork 进程id # 不再跟踪某个进程
 delete fork 进程id # kill某个进程并停止对它的跟踪
```
检查点: checkpoint/restart


### 查看停止原因:
```c
info program
```

###断点(breakpoint): 程序运行到某处就会中断
```sh 
 break(b) 行号|函数名|程序地址 | +/-offset | filenam:func [if 条件] # 在指定位置设置断点
 tbreak ... # 与break相似，只是设置一次断点
 hbreak ... # 与break相似，只是设置硬件断点，需要硬件支持
 thbreak ... # 与break相似，只是设置一次性硬件断点，需要硬件支持
 rbreak 正则表达式 # 给一批满足条件的函数打上断点
 info break [断点号] # 查看某个断点或所有断点信息

 set breadpoint pending auto|on|off # 查看如果断点位置没有找到时行为
 show breakpoint pending
```

###观察点(watchpoint): 表达式的值修改时会被中断

```sh
 watch 表达式 # 当表达式被写入，并且值被改变时中断
 rwatch 表达式 # 当表达式被读时中断
 awatch 表达式 # 当表达式被读或写时中断
 info watchpoints

 set can-use-hw-watchpoints 值 # 设置使用的硬件观察点的数
 show can-use-hw-watchpoints

 rwatch与awatch需要有硬件支持，另外如果是对局部变量使用watchpoint，那退出作用域时观察点会自动被删除
 另外在多线程情况下，gdb的watchpoint只对一个线程有效
```

捕获点(catchpoint): 程序发生某个事件时停止，如产生异常时

 catch 事件名
 事件包括:
 throw # 产生c++异常
 catch # 捕获到c++异常
 exec/fork/vfork # 一个exec/fork/vfork函数调用,只对HP-UX
 load/unload [库名] # 加载/卸载共享库事件，对只HP-UX
 tcatch 事件名 # 一次性catch
 info break

断点操作:
 clear [函数名|行号] # 删除断点，无参数表示删卫当前位置
 delete [断点号] # 删除断点，无参数表示删所有断点
 disable [断点号]
 enable [断点号]


 condition 断点号 条件 # 增加断点条件
 condition 断点号 # 删除断点条件

 ignore 断点号 数目 # 忽略断点n次

 commands 断点号 # 当某个断点中断时打印条件
 条件
 end
下面是一个例子,可以一直打印当前的X值：
 commands 3
 printf "X:%d\n",x
 cont
 end


断点后操作:
 continue(c) [忽略次数] # 继续执行，[忽略前面n次中断]
 fg [忽略次数] # 继续执行，[忽略前面n次中断]

 step(s) [n步] # 步进,重复n次
 next(n) [n步] # 前进,重复n次

 finish # 完成当前函数调用，一直执行到返回处，并打印返回值

 until(u) [位置] # 一直执行到当前行或指定位置，或是当前函数返回
 advance 位置 # 前面到指定位置，如果当前函数返回则停止,与until类似

 stepi(si) [n步] # 精确的只执行一个汇编指令,重复n次
 nexti(ni) [n步] # 精确的只执行一个汇编指令,碰到函数跳过,重复n次

 set step-mode on|off # on时,如果函数没有调试信息也跟进
 show step-mode

信号:
 info signals # 列出所有信号的处理方式
 info handle # 同上

 handle 信号 方式 # 改变当前处理某个信号的方式
 方式包括:
 nostop # 当信号发生时不停止，只打印信号曾经发生过
 stop # 停止并打印信号
 print # 信号发生时打印
 noprint # 信号发生时不打印
 pass/noignore # gdb充许应用程序看到这个信号
 nopass/ignore # gdb不充许应用程序看到这个信号

线程断点:
 break 行号信息 thread 线程号 [if 条件] # 只在某个线程内加断点

线程调度锁:
 set scheduler-locking on|off # off时所有线程都可以得到调度,on时只有当前
 show scheduler-locking

帧:
 frame(f) [帧号] # 不带参数时显示所有帧信息，带参数时切换到指定帧
 frame 地址 # 切换到指定地址的帧
 up [n] # 向上n帧
 down [n] # 向下n帧

 select-frame 帧号 # 切换到指定帧并且不打印被转换到的帧的信息
 up-silently [n] # 向上n帧,不显示帧信息
 down-silently [n] # 向下n帧,不显示帧信息

调用链:
 backtrace(bt) [n|-n|full] # 显示当前调用链,n限制显示的数目,-n表示显示后n个,n表示显示前n个，full的话还会显示变量信息
 使用 thread apply all bt 就可以显示所有线程的调用信息

 set backtrace past-main on|off
 show backtrace past-main

 set backtrace past-entry on|off
 show backtrace past-entry

 set backtrace limit n # 限制调用信息的显示层数
 show backtrace limit

显示帧信息:
 info frame # 显示当前帧信息
 info frame addr # 显示指定地址的帧信息
 info args # 显示帧的参数
 info locals # 显示局部变量信息
 info catch # 显示本帧异常信息


显示行号:
 list(l) [行号|函数|文件:行号] # 显示指定位置的信息,无参数为当前位置
 list - # 显示当前行之前的信息

 list first,last # 从frist显示到last行
 list ,last # 从当前行显示到last行
 list frist, # 从指定行显示
 list + # 显示上次list后显示的内容
 list - # 显示上次list前面的内容

 在上面，first和last可以是下面类型:
 行号
 +偏移
 -偏移
 文件名:行号
 函数名
 函数名:行号

 set listsize n # 修改每次显示的行数
 show listsize


编辑:
 edit [行号|函数|函数名:行号|文件名:函数名] # 编辑指定位置


查找:
 search 表示式 # 向前查找表达式
 reverse-search 表示式 # 向后查找表达式


指定源码目录:
 directory(dir) [目录名] # 指定源文件查找目录
 show directories

源码与机器码：
 info line [函数名|行号] # 显示指定位置对应的机器码地址范围
 disassemble [函数名 | 起始地址 结束地址] # 对指定范围进行反汇编
 set disassembly-flavor att|intel # 指定汇编代码形式
 show disassembly-flavor



查看数据:
 ptype 表达式 # 查看某个表达式的类型

 print [/f] [表达式] # 按格式查看表达式内容,/f是格式化

 set print address on|off # 打印时是不是显示地址信息
 show print address

 set print symbol-filename on|off # 是不是显示符号所在文件等信息
 show print symbol-filename

 set print array on | off # 是不是打印数组
 show print array

 set print array index on | off # 是不是打印下标
 show print array index

 ...

 表达式可以用下面的修饰符:
 var@n # 表示把var当成长度为n的数组
 filename::var # 表示打印某个函数内的变量,filename可以换成其它范围符如文件名
 {type} var # 表示把var当成type类型

 输出格式:
 x # 16进制
 d # 10进制
 u # 无符号
 o # 8进制
 t # 2进制
 a # 地址
 c # 字符
 f # 浮点

查看内存:
 x /nfu 地址 # 查看内存
 n 重复n次
 f 显示格式,为print使用的格式
 u 每个单元的大小,为
 b byte
 h 2 byte
 w 4 byte
 g 8 byte


自动显示:
 display [/fmt] 表达式 # 每次停止时都会显示表达式,fmt与print的格式一样,如果是内存地址，那fmt可像 x的参数一样
 undisplay 显示编号
 delete display 显示编号 # 这两个都是删附某个显示

 disable display 显示编号 # 禁止某个显示
 enable display 显示编号 # 重显示
 display # 显示当前显示内容
 info display # 查看所有display项


查看变量历史：
 show values 变量名 [n] # 显示变量的上次显示历史,显示n条
 show values 变量名 + # 继续上次显示内容

便利变量: (声明变量的别名以方便使用)
 set $foo = *object_ptr # 声明foo为object_ptr的便利变量
 init-if-undefined $var = expression # 如果var还未定义则赋值
 show convenience
内部便利变量：
 $_ 上次x查看的地址
 $__
 $_exitcode 程序垢退出码

寄存器:
 into registers # 除了浮点寄存器外所有寄存器
 info all-registers # 所有寄存器
 into registers 寄存器名 # 指定寄存器内容
 info float # 查看浮点寄存器状态
 info vector # 查看向量寄存器状态

 gdb为一些内部寄存器定义了名字，如$pc(指令),$sp(栈指针),$fp(栈帧),$ps(程序状态)
 p /x $pc # 查看pc寄存器当前值
 x /i $pc # 查看要执行的下一条指令
 set $sp += 4 # 移动栈指针

内核中信息:
 info udot # 查看内核中user struct信息
 info auxv # 显示auxv内容(auxv是协助程序启动的环境变量的)


内存区域限制:
 mem 起始地址 结构地址 属性 # 对[地始地址,结构地址)区域内存进行保护，如果结构地址为0表示地址最大值0xffffffff
 delete mem 编号 # 删除一个内存保护
 disable mem 编号 # 禁止一个内存保护
 enable mem 编号 # 打开一个内存保护
 info mem # 显示所有内存保护信息

 保护的属性包括：
 1. 内存访问模式: ro | wo |rw
 2. 内存访问大小: 8 | 16 | 32 | 64 如果不限制，表示可按任意大小访问
 3. 数据缓存: cache | nocache cache表示充许gdb缓存目标内存



内存复制到/从文件:
 dump [格式] memory 文件名 起始地址 结构地址 # 把指定内存段写到文件
 dump [格式] value 文件名 表达式 # 把指定值写到文件
 格式包括:
 binary 原始二进制格式
 ihex intel 16进制格式
 srec S-recored格式
 tekhex tektronix 16进制格式

 append [binary] memory 文件名 起始地址 结构地址 # 按2进制追加到文件
 append [binary] value 文件名 表达式 # 按2进制追加到文件

 restore 文件名 [binary] bias 起始地址 结构地址 # 恢复文件中内容到内存.如果文件内容是原始二进制，需要指定binary参数，不然会gdb自动识别文件格式


产生core dump文件
 gcore [文件名] # 产生core dump文件

字符集:
 set target-charset 字符集 # 声明目标机器的locale,如gdbserver所在机器
 set host-charset 字符集 # 声明本机的locale
 set charset 字符集 # 声明目标机和本机的locale
 show charset
 show host-charset
 show target-charset


缓存远程目标的数据:为提高性能可以使用数据缓存，不过gdb不知道volatile变量，缓存可能会显示不正确的结构
 set remotecache on | off
 show remotecache
 info dcache # 显示数据缓存的性能

C预处理宏:
 macro expand(exp) 表达式 # 显示宏展开结果
 macro expand-once(expl) 表达式 # 显示宏一次展开结果
 macro info 宏名 # 查看宏定义


追踪(tracepoint): 就是在某个点设置采样信息，每次经过这个点时只执行已经定义的采样动作但并不停止，最后再根据采样结果进行分析。

采样点定义:
 trace 位置 # 定义采样点
 info tracepoints # 查看采样点列表
 delete trace 采样点编号 # 删除采杰点
 disable trace 采样点编号 # 禁止采杰点
 enable trace 采样点编号 # 使用采杰点
 passcount 采样点编号 [n] # 当通过采样点 n次后停止,不指定n则在下一个断点停止


预定义动作：预定义动作以actions开始,后面是一系列的动作
 actions [num] # 对采样点num定义动作
 行为:
 collect 表达式 # 采样表达式信息
 一些表达式有特殊意义，如$regs(所有寄存器),$args(所有函数参数),$locals(所有局部变量)
 while-steping n # 当执行第n次时的动作,下面跟自己的collect操作

采样控制:
 tstart # 开始采样
 tstop # 停止采样
 tstatus # 显示当前采样的数据

使用收集到的数据:
 tfind start # 查找第一个记录
 tfind end | none # 停止查找
 tfind # 查找下一个记录
 tfind - # 查找上一个记录
 tfind tracepoint N # 查找 追踪编号为N 的下一个记录
 tfind pc 地址 # 查找代码在指定地址的下一个记录
 tfind range 起始,结束
 tfind outside 起始，结构
 tfind line [文件名:]行号

 tdump # 显示当前记录中追踪信息
 save-tracepoints 文件名 # 保存追踪信息到指定文件,后面使用source命令读

追踪中的便利变量:
 $trace_frame # 当前帧编号, -1表示没有, INT
 $tracepoint # 当前追踪,INT
 $trace_line # 位置 INT
 $trace_file # 追踪文件 string, 需要使用output输出，不应用printf
 $trace_func # 函数名 string

覆盖技术(overray): 用于调试过大的文件

gdb文件:
 file 文件名 # 加载文件,此文件为可执行文件，并且从这里读取符号
 core 文件名 # 加载core dump文件
 exec-file 文件名 # 加载可执行文件
 symbol-file 文件名 # 加载符号文件
 add-symbol-file 文件名 地址 # 加载其它符号文件到指定地址
 add-symbol-file-from-memory 地址 # 从指定地址中加载符号
 add-share-symbol-files 库文件 # 只适用于cygwin
 session 段 地址 # 修改段信息
 info files | target # 打开当前目标信息
 maint info sections # 查看程序段信息
 set truct-readonly-sections on | off # 加快速度
 show truct-readonly-sections

 set auto-solib-add on | off # 修改自动加载动态库的模式
 show auto-solib-add

 info share # 打印当前加载的共享库的地址信息
 share [正则表达式] # 从符合的文件中加载共享库的正则表达式

 set stop-on-solib-events on | off # 设置当加载共享库时是不是要停止
 show stop-on-solib-events

 set solib-absolute-prefix 路径 # 设置共享库的绝对路矩，当加载共享库时会以此路径下查找(类似chroot)
 show solib-absolute-prefix

 set solib-search-path 路径 # 如果solib-absolute-prefix查找失败，那将使用这个目录查找共享库
 show solib-search-path