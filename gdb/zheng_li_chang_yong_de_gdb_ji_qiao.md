# 整理常用的gdb技巧


##常用命令

常用的gdb命令...

##启动gdb

###直接运行
```sh 
gdb --args prog arg1 arg2
```

###运行gdb后使用run命令
```
gdb prog
run arg1 arg2
```

###attach到已运行的程序
```
gdb --pid ${PID_OF_PROG}
```

##ptype

ptype用于显示Symbol的类型，示例源码为:

```c
struct ABC {
    int val;
}

int main() {
    ABC abc;
    return 0;
}
```

运行gdb：

```sh
(gdb) b 7
(gdb) r
(gdb) ptype abc

type = struct XXX {
    int val;
}
```

ptype可以输出表达式的返回类型，具体介绍可参考Examining the Symbol Table。

##print {type} variable

print(p)可以按照某种类型输出变量的值，示例源码如下:

```c
struct ABC {
    double val;
    int val2;
}

int main() {
    ABC abc;
    abc.val = 1.5;
    abc.val2 = 10;

    void *pAbc = &abc;

    return 0;
}
```

运行gdb:

```sh
(gdb) b 13
(gdb) r

(gdb) p pAbc
$1 = (void *) 0x7fffffffe710

(gdb) p {ABC} 0x7fffffffe710
$2 = {val = 1.5, val2 = 10}

(gdb) p {ABC} pAbc
$3 = {val = 1.5, val2 = 10}

(gdb) p * (ABC*) pAbc
$4 = {val = 1.5, val2 = 10}

(gdb) p {double} pAbc
$5 = 1.5

(gdb) p * (double*) pAbc
$6 = 1.5

(gdb) p {int} (pAbc + sizeof (double))
$7 = 10

(gdb) p * (int*) (pAbc + sizeof (double))
$8 = 10
```

有时候，如果字符串太长，gdb可能只打印一部分，这时候如下设置来打印全部内容：
```sh
(gdb) set print elements 0
```

##examine

examine(x)可以按照一定的格式打印内存地址处的数据，详细文档可参考这里。

```sh
(gdb) x/{COUNT}{FMT}{SIZE} {ADDRESS}
```

- {COUNT}: 打印的数目，默认为1。
- {FMT}: 打印的格式[1]，默认为上次使用的{FMT}:
    - o(octal): 8进制整数
    - x(hex): 16进制整数
    - d(decimal): 10进制整数
    - u(unsigned decimal): 10进制非负整数
    - t(binary): 2进制整数
    - f(float): 浮点数
    - a(address): 输出相对于前面最近的符号的偏移
    - i(instruction): 输出地址处的汇编指令
    - c(char): 字符
    - s(string): c字符串(null-terminated string)
    - z(hex, zero padded on the left): 见说明
- {SIZE}: 打印的字节数目，默认为上次使用的{SIZE}:
    - b(byte): 1个字节
    - h(halfword): 2个字节
    - w(word): 4个字节
    - g(giant, 8 bytes): 8个字节
- {ADDRESS}: 目标地址

几个例子:

```sh
(gdb) x/a 0x401419
0x401419 <main()+113>:  0x55c3c900000000b8

(gdb) x/i 0x40138d
=> 0x40138d <crash(int, double)+41>:    mov    -0x10(%rbp),%eax

(gdb) x/1fg 140737488346064
0x7fffffffdbd0: 10.125
```


##设置源码目录

参考Specifying Source Directories，使用`dir /path/to/your/sources`可在调试时添加一个源码目录。

##设置字符编码

gdb默认使用utf-8编码，可以使用如下命令修改编码。

```
set charset GBK
```

也可直接在~/.gdbinit里设置。

##线程相关的命令

下面是一些调试多线程程序时常用的命令:

- info threads: 查看线程列表
- thread 2: 切换到2号线程，线程编号可由info threads得知
- thread apply all bt: 打印所有线程的堆栈


##高级技巧

一些不太广为人知的技巧...

加载独立的调试信息

gdb调试的时候可以从单独的符号文件中加载调试信息。

```sh
(gdb) exec-file test
(gdb) symbol-file test.debug
```

test是移除了调试信息的可执行文件, test.debug是被移除后单独存储的调试信息。参考stackoverflow上的一个问题，可以如下分离调试信息:

```sh
# 编译程序，带调试信息(-g)
gcc -g -o test main.c

# 拷贝调试信息到test.debug
objcopy --only-keep-debug test test.debug

# 移除test中的调试信息
strip --strip-debug --strip-unneeded test

# 然后启动gdb
gdb -s test.debug -e test

# 或这样启动gdb
gdb
(gdb) exec-file test
(gdb) symbol-file test.debug
```

分离出的调试信息test.debug还可以链接回可执行文件test中

```sh
objcopy --add-gnu-debuglink test.debug test
```

然后就可以正常用addr2line等需要读取调试信息的程序了

```sh
addr2line -e test 0x401c23
```

更多内容可阅读GDB: Debugging Information in Separate Files。

##在内存和文件系统之间拷贝数据

- 将内存数据拷贝到文件里

```sh
dump binary value file_name variable_name
dump binary memory file_name begin_addr end_addr 
```

- 改变内存数据

使用set命令

##执行gdb脚本

常用的gdb操作，比如打断点等可以放在一个gdb脚本里，然后使用时导入即可。例如:

```sh
b main.cpp:15
b test.cpp:18
```

gdb运行时，使用source命令即可导入

```sh
(gdb) source /path/to/breakpoints.txt
```

或gdb运行时导入

```sh
gdb -x /path/to/breakpoints.txt prog
```

对于每次gdb运行都要调用的脚本，比如设置字符集等，可以放在~/.gdbinit初始文件里，这样每次gdb启动时都会自动调用。

##输出到文件

可以通过set logging on将命令的输出保存到默认的gdb.txt文件中。当然也可以通过set logging file my_log.txt来设置输出文件的路径。

##执行命令并退出

有时候需要gdb执行若干条命令后就立即退出，而不是进入交互界面，这时可以使用`-batch`选项。

```sh
gdb -ex "set pagination 0" -ex "thread apply all bt" `-batch` -p $pid
```

上面的命令打印$pid进程所有线程的堆栈并退出。

##自定义命令

参考gdb/Define，可以在gdb中自定义命令，比如：

```sh
(gdb) define hello
(gdb) print "welcome"
(gdb) print "hello $arg0"
(gdb) end
```

然后如此调用

```sh
(gdb) hello world
```

即可输出

```sh
(gdb) $1 = "welcome"
(gdb) $2 = "hello world"
```

##条件断点

在条件断点里可以调用标准库的函数，比如下面这个:

```sh
# 如果strA == strB，则在断点处暂停
(gdb) b main.cpp:255 if strcmp(strA.c_str(), strB.c_str()) == 0

# 还是上面的场景，直接用string类的compare函数
(gdb) b main.cpp:255 if strA.compare(strB) != 0
```

##捕获exception

gdb遇到未处理的exception时，并不会捕获处理。但是参考Set Catchpoints，可以使用catch catch命令来捕获exception。

##阅读资料

-Copy between memory and a file
- HowTo: Writing into process memory with GDB
- Specifying Source Directories
- Examining the Symbol Table
