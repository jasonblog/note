# Android debug 技巧


##一. 获取Trace
调用栈信息(Trace)是分析异常经常使用的，这里简单划分两类情况：

- 当前线程Trace: 当前执行流所在线程的调用栈信息；
- 目标进程Trace：可获取目标进程的调用栈，用于动态调试；



###1.1 当前线程Trace
1) Java层

```java
Thread.currentThread().dumpStack();   //方法1
Log.d(TAG,"Gityuan", new RuntimeException("Gityuan")); //方法2
new RuntimeException("Gityuan").printStackTrace(); //方法3
```

2） Native层


```cpp
#include <utils/CallStack.h>
android::CallStack stack(("Gityuan"));
```

### 1.2 目标进程Trace
1) Java层

```java
adb shell kill -3 [pid]     //方法1
Process.sendSignal(pid, Process.SIGNAL_QUIT)  //方法2
```

生成trace文件保存在文件data/anr/traces.txt


2） Native层

```sh
adb shell debuggerd -b [tid] //方法1
Debug.dumpNativeBacktraceToFile(pid, tracesPath) //方法2
```

前两条命令输出内容相同:

- 命令1输出到控制台
- 命令2输出到目标文件

对于debuggerd命令，若不带参数则输出tombstones文件，保存到目录/data/tombstones


3） Kernel层

```sh
adb shell cat /proc/[tid]/stack  //方法1
WatchDog.dumpKernelStackTraces() //方法2
```

其中dumpKernelStackTraces()只能用于打印当前进程的kernel线程

###1.3 小节
以下分别列举输出Java, Native, Kernel的调用栈方式：

```sh
类别	函数式	命令式
Java	Process.sendSignal(pid, Process.SIGNAL_QUIT)	kill -3 [pid]
Native	Debug.dumpNativeBacktraceToFile(pid, tracesPath)	debuggerd -b [pid]
Kernel	WD.dumpKernelStackTraces()	cat /proc/[tid]/stack
```

分析异常时往往需要关注的重要目录：

```sh
/data/anr/traces.txt
/data/tombstones/tombstone_X
/data/system/dropbox/
```


##二. 时间调试
为了定位耗时过程，有时需要在关注点添加相应的systrace，而systrace可跟踪系统cpu,io以及各个子系统运行状态等信息，对于kernel是利用Linux的ftrace功能。当然也可以直接在方法前后加时间戳，输出log的方式来分析。

###2.1 新增systrace

1) App

```java
import android.os.Trace;
void foo() {
    Trace.beginSection("app:foo");
    ...
    Trace.endSection(); 
}
```

2) Java Framework


```java
import android.os.Trace;
void foo() {
    Trace.traceBegin(Trace.TRACE_TAG_ACTIVITY_MANAGER, "fw:foo");
    ...
    Trace.traceEnd(Trace.TRACE_TAG_ACTIVITY_MANAGER); 
}
```

3） Native Framework


```cpp
#define ATRACE_TAG ATRACE_TAG_GITYUAN
#include <utils/Trace.h> // used for C++
#include <cutils/trace.h> // used for C

void foo() 
{
    ATRACE_CALL();
    ...
}
```

或者

```cpp
#define ATRACE_TAG ATRACE_TAG_GITYUAN
#include <utils/Trace.h> // used for C++
#include <cutils/trace.h> // used for C

void foo() {
    ATRACE_BEGIN();
    ...
    ATRACE_END();
}
```

### 2.2 打印时间戳

1) Java

```java
import android.util.Log;
void foo()
{
    long startTime = System.currentTimeMillis();
    ...
    long spendTime = System.currentTimeMillis() - startTime;
    Log.i(TAG,"took " + spendTime + “ ms.”);
}
```

2） C/C++

```cpp
#include <stdio.h>
#include <sys/time.h>

void foo() 
{
    struct timeval time;
    gettimeofday(&time, NULL); //精度us
    printf("took %lld ms.\n", time.tv_sec * 1000 + time.tv_usec /1000);
}
```

## 2.3 kernel log

有时候Kernel log的输出是由级别限制，可通过如下命令查看：

```sh
adb shell cat /proc/sys/kernel/printk  
4       4       1       7
```

参数解读：

- 控制台日志级别：优先级高于该值的消息将被打印至控制台。
- 缺省的消息日志级别：将用该值来打印没有优先级的消息。
- 最低的控制台日志级别：控制台日志级别可能被设置的最小值。
- 缺省的控制台日志级别：控制台日志级别的缺省值


日志级别：

<table class="table">
  <thead>
    <tr>
      <th>级别</th>
      <th>值</th>
      <th>说明</th>
    </tr>
  </thead>
  <tbody>
    <tr>
      <td>KERN_EMERG</td>
      <td>0</td>
      <td>致命错误</td>
    </tr>
    <tr>
      <td>KERN_ALERT</td>
      <td>1</td>
      <td>报告消息</td>
    </tr>
    <tr>
      <td>KERN_CRIT</td>
      <td>2</td>
      <td>严重异常</td>
    </tr>
    <tr>
      <td>KERN_ERR</td>
      <td>3</td>
      <td>出错</td>
    </tr>
    <tr>
      <td>KERN_WARNING</td>
      <td>4</td>
      <td>警告</td>
    </tr>
    <tr>
      <td>KERN_NOTICE</td>
      <td>5</td>
      <td>通知</td>
    </tr>
    <tr>
      <td>KERN_INFO</td>
      <td>6</td>
      <td>常规</td>
    </tr>
    <tr>
      <td>KERN_DEBUG</td>
      <td>7</td>
      <td>调试</td>
    </tr>
  </tbody>
</table>

Log相关命令

- dmesg 或 cat /proc/kmsg
- logcat -L 或 cat /proc/last_kmsg
- logcat -b events -b system


##三. addr2line
addr2line功能是将函数地址解析为函数名。分析过Native Crash，那么对addr2line一定不会陌生。 addr2line命令参数：


```sh
Usage: addr2line [option(s)] [addr(s)]
 The options are:
  @<file>                Read options from <file>
  -a --addresses         Show addresses
  -b --target=<bfdname>  Set the binary file format
  -e --exe=<executable>  Set the input file name (default is a.out)
  -i --inlines           Unwind inlined functions
  -j --section=<name>    Read section-relative offsets instead of addresses
  -p --pretty-print      Make the output easier to read for humans
  -s --basenames         Strip directory names
  -f --functions         Show function names
  -C --demangle[=style]  Demangle function names
  -h --help              Display this information
  -v --version           Display the program's version
```

### 3.1 Native地址转换

Step 1: 获取symbols表

先获取对应版本的symbols，即可找到对应的so库。（最好是对应版本addr2line，可确保完全匹配）

Step 2: 执行addr2line命令

```sh
// 64位
cd prebuilts/gcc/linux-x86/aarch64/aarch64-linux-android-4.9/bin
./aarch64-linux-android-addr2line -f -C -e libxxx.so  <addr1>

//32位
cd /prebuilts/gcc/linux-x86/arm/arm-linux-androideabi-4.9/bin
./arm-linux-androideabi-addr2line -f -C -e libxxx.so  <addr1>
```

另外，有兴趣可以研究下development/scripts/stack，地址批量转换工具。

###3.2 kernel地址转换
addr2line也适用于调试分析Linux Kernel的问题。例如，查询如下命令所对应的代码行号

```sh
[<0000000000000000>] binder_thread_read+0x2a0/0x324
```


Step 1: 获取符号地址

通过命令arm-eabi-nm从vmlinux找到目标方法的符号地址，其中nm和vmlinux所在目录：

- arm-eabi-nm位于目录prebuilts/gcc/linux-x86/arm/arm-eabi-4.8/bin/
- vmlinux位于目录out/target/product/xxx/obj/KERNEL_OBJ/

执行如下命令：(需要带上绝对目录)

```sh
arm-eabi-nm  vmlinux |grep binder_thread_read
```

则输出结果： `c02b2f28 T binder_thread_read`，可知binder_thread_read的符号地址为c02b2f28， 其偏移量为0x2a0，则计算后的目标符号地址= c02b2f28 + 2a0，然后再采用addr2line转换得到方法所对应的行数

Step 2: 执行addr2line命令

```sh
./aarch64-linux-android-addr2line -f -C -e vmlinux [目标地址]
```

注意:对于kernel调用栈翻译过程都是通过vmlinux来获取的


