# Android下打印调试堆栈方法


打印堆栈是调试的常用方法，一般在系统异常时，我们可以将异常情况下的堆栈打印出来，这样十分方便错误查找。实际上还有另外一个非常有用的功能：分析代码的行为。android代码太过庞大复杂了，完全的静态分析经常是无从下手，因此通过打印堆栈的动态分析也十分必要。



Android打印堆栈的方法，简单归类一下 

##1. zygote的堆栈dump

实际上这个可以同时dump java线程及native线程的堆栈，对于java线程，java堆栈和native堆栈都可以得到。

使用方法很简单，直接在adb shell或串口中输入：


```sh
kill -3 <pid>  
```

输出的trace会保存在 /data/anr/traces.txt文件中。这个需要注意，如果没有 /data/anr/这个目录或/data/anr/traces.txt这个文件，需要手工创建一下，并设置好读写权限。
如果需要在代码中，更容易控制堆栈的输出时机，可以用以下命令获取zygote的core dump:


```sh
Process.sendSignal(pid, Process.SIGNAL_QUIT);  
```

原理和命令行是一样的。

不过需要注意两点：


- adb shell可能会没有权限，需要root。
- android 4.2中关闭了native thread的堆栈打印，详见 dalvik/vm/Thread.cpp的dumpNativeThread方法：


```sh
dvmPrintDebugMessage(target,  
    "\"%s\" sysTid=%d nice=%d sched=%d/%d cgrp=%s\n",  
    name, tid, getpriority(PRIO_PROCESS, tid),  
    schedStats.policy, schedStats.priority, schedStats.group);  
dumpSchedStat(target, tid);  
// Temporarily disabled collecting native stacks from non-Dalvik  
// threads because sometimes they misbehave.  
//dvmDumpNativeStack(target, tid); 
```


##2. debuggerd的堆栈dump

debuggerd是android的一个daemon进程，负责在进程异常出错时，将进程的运行时信息dump出来供分析。debuggerd生成的coredump数据是以文本形式呈现，被保存在 /data/tombstone/ 目录下(名字取的也很形象，tombstone是墓碑的意思)，共可保存10个文件，当超过10个时，会覆盖重写最早生成的文件。从4.2版本开始，debuggerd同时也是一个实用工具：可以在不中断进程执行的情况下打印当前进程的native堆栈。使用方法是:


```sh
debuggerd -b <pid>  
```

这可以协助我们分析进程执行行为，但最最有用的地方是：它可以非常简单的定位到native进程中锁死或错误逻辑引起的死循环的代码位置。

##3. java代码中打印堆栈

Java代码打印堆栈比较简单， 堆栈信息获取和输出，都可以通过Throwable类的方法实现。目前通用的做法是在java进程出现需要注意的异常时，打印堆栈，然后再决定退出或挽救。通常的方法是使用exception的printStackTrace()方法：


```java
/**
 * 打印函数的调用栈
 *
 * @return 调用栈
 */
public String getStackTrace() {
	StringBuilder sb = new StringBuilder("");
	Exception e = new Exception();
	StackTraceElement[] trace = e.getStackTrace();
	for (int i = 0; i < trace.length; i++) {
		sb.append(trace[i] + "\n");
	}
	return sb.toString();
}
```

```java
try {  
 ...  
} catch (RemoteException e) {  
  e.printStackTrace();  
  ...  
}  
```

当然也可以只打印堆栈不退出，这样就比较方便分析代码的动态运行情况。Java代码中插入堆栈打印的方法如下：


```java
Log.d(TAG,Log.getStackTraceString(new Throwable()));  
```

##4. C++代码中打印堆栈

C++也是支持异常处理的，异常处理库中，已经包含了获取backtrace的接口，Android也是利用这个接口来打印堆栈信息的。在Android的C++中，已经集成了一个工具类CallStack，在libutils.so中。使用方法：

```cpp
#include <utils/CallStack.h>  
...  
CallStack stack;  
stack.update();  
stack.dump();  
```

使用方式比较简单。目前Andoid4.2版本已经将相关信息解析的很到位，符号表查找，demangle，偏移位置校正都做好了。

##5. C代码中打印堆栈

C代码，尤其是底层C库，想要看到调用的堆栈信息，还是比较麻烦的。 CallStack肯定是不能用，一是因为其实C++写的，需要重新封装才能在C中使用，二是底层库反调上层库的函数，会造成链接器循环依赖而无法链接。不过也不是没有办法，可以通过android工具类CallStack实现中使用的unwind调用及符号解析函数来处理。

这里需要注意的是，为解决链接问题，最好使用dlopen方式，查找需要用到的接口再直接调用，这样会比较简单。如下为相关的实现代码，只需要在要打印的文件中插入此部分代码，然后调用getCallStack()即可，无需包含太多的头文件和修改Android.mk文件：


```cpp
#define MAX_DEPTH                       31
#define MAX_BACKTRACE_LINE_LENGTH   800
#define PATH "/system/lib/libcorkscrew.so"

typedef ssize_t (*unwindFn)(backtrace_frame_t*, size_t, size_t);
typedef void (*unwindSymbFn)(const backtrace_frame_t*, size_t,
                             backtrace_symbol_t*);
typedef void (*unwindSymbFreeFn)(backtrace_symbol_t*, size_t);

static void* gHandle = NULL;

static int getCallStack(void)
{
    ssize_t i = 0;
    ssize_t result = 0;
    ssize_t count;
    backtrace_frame_t mStack[MAX_DEPTH];
    backtrace_symbol_t symbols[MAX_DEPTH];

    unwindFn unwind_backtrace = NULL;
    unwindSymbFn get_backtrace_symbols = NULL;
    unwindSymbFreeFn free_backtrace_symbols = NULL;

    // open the so.
    if (gHandle == NULL) {
        gHandle = dlopen(PATH, RTLD_NOW);
    }

    // get the interface for unwind and symbol analyse
    if (gHandle != NULL) {
        unwind_backtrace = (unwindFn)dlsym(gHandle, "unwind_backtrace");
    }

    if (gHandle != NULL) {
        get_backtrace_symbols = (unwindSymbFn)dlsym(gHandle, "get_backtrace_symbols");
    }

    if (gHandle != NULL) {
        free_backtrace_symbols = (unwindSymbFreeFn)dlsym(gHandle,
                                 "free_backtrace_symbols");
    }

    if (!gHandle || !unwind_backtrace || !get_backtrace_symbols ||
        !free_backtrace_symbols) {
        ALOGE("Error! cannot get unwind info: handle:%p %p %p %p",
              gHandle, unwind_backtrace, get_backtrace_symbols, free_backtrace_symbols);
        return result;
    }

    count = unwind_backtrace(mStack, 1, MAX_DEPTH);
    get_backtrace_symbols(mStack, count, symbols);

    for (i = 0; i < count; i++) {
        char line[MAX_BACKTRACE_LINE_LENGTH];

        const char* mapName = symbols[i].map_name ? symbols[i].map_name : "<unknown>";
        const char* symbolName = symbols[i].demangled_name ? symbols[i].demangled_name :
                                 symbols[i].symbol_name;
        size_t fieldWidth = (MAX_BACKTRACE_LINE_LENGTH - 80) / 2;

        if (symbolName) {
            uint32_t pc_offset = symbols[i].relative_pc - symbols[i].relative_symbol_addr;

            if (pc_offset) {
                snprintf(line, MAX_BACKTRACE_LINE_LENGTH, "#%02d  pc %08x  %.*s (%.*s+%u)",
                         i, symbols[i].relative_pc, fieldWidth, mapName,
                         fieldWidth, symbolName, pc_offset);
            } else {
                snprintf(line, MAX_BACKTRACE_LINE_LENGTH, "#%02d  pc %08x  %.*s (%.*s)",
                         i, symbols[i].relative_pc, fieldWidth, mapName,
                         fieldWidth, symbolName);
            }
        } else {
            snprintf(line, MAX_BACKTRACE_LINE_LENGTH, "#%02d  pc %08x  %.*s",
                     i, symbols[i].relative_pc, fieldWidth, mapName);
        }

        ALOGD("%s", line);
    }

    free_backtrace_symbols(symbols, count);

    return result;
}

```

对sched_policy.c的堆栈调用分析如下,注意具体是否要打印，在哪里打印，还可以通过pid、uid、property等来控制一下，这样就不会被淹死在trace的汪洋大海中。


```sh
D/SchedPolicy( 1350): #00  pc 0000676c  /system/lib/libcutils.so  
D/SchedPolicy( 1350): #01  pc 00006b3a  /system/lib/libcutils.so (set_sched_policy+49)  
D/SchedPolicy( 1350): #02  pc 00010e82  /system/lib/libutils.so (androidSetThreadPriority+61)  
D/SchedPolicy( 1350): #03  pc 00068104  /system/lib/libandroid_runtime.so (android_os_Process_setThreadPriority(_JNIEnv*, _jobject*, int, int)+7)  
D/SchedPolicy( 1350): #04  pc 0001e510  /system/lib/libdvm.so (dvmPlatformInvoke+112)  
D/SchedPolicy( 1350): #05  pc 0004d6aa  /system/lib/libdvm.so (dvmCallJNIMethod(unsigned int const*, JValue*, Method const*, Thread*)+417)  
D/SchedPolicy( 1350): #06  pc 00027920  /system/lib/libdvm.so  
D/SchedPolicy( 1350): #07  pc 0002b7fc  /system/lib/libdvm.so (dvmInterpret(Thread*, Method const*, JValue*)+184)  
D/SchedPolicy( 1350): #08  pc 00060c30  /system/lib/libdvm.so (dvmCallMethodV(Thread*, Method const*, Object*, bool, JValue*, std::__va_list)+271)  
D/SchedPolicy( 1350): #09  pc 0004cd34  /system/lib/libdvm.so  
D/SchedPolicy( 1350): #10  pc 00049382  /system/lib/libandroid_runtime.so  
D/SchedPolicy( 1350): #11  pc 00065e52  /system/lib/libandroid_runtime.so  
D/SchedPolicy( 1350): #12  pc 0001435e  /system/lib/libbinder.so (android::BBinder::transact(unsigned int, android::Parcel const&, android::Parcel*, unsigned int)+57)  
D/SchedPolicy( 1350): #13  pc 00016f5a  /system/lib/libbinder.so (android::IPCThreadState::executeCommand(int)+513)  
D/SchedPolicy( 1350): #14  pc 00017380  /system/lib/libbinder.so (android::IPCThreadState::joinThreadPool(bool)+183)  
D/SchedPolicy( 1350): #15  pc 0001b160  /system/lib/libbinder.so  
D/SchedPolicy( 1350): #16  pc 00011264  /system/lib/libutils.so (android::Thread::_threadLoop(void*)+111)  
D/SchedPolicy( 1350): #17  pc 000469bc  /system/lib/libandroid_runtime.so (android::AndroidRuntime::javaThreadShell(void*)+63)  
D/SchedPolicy( 1350): #18  pc 00010dca  /system/lib/libutils.so  
D/SchedPolicy( 1350): #19  pc 0000e3d8  /system/lib/libc.so (__thread_entry+72)  
D/SchedPolicy( 1350): #20  pc 0000dac4  /system/lib/libc.so (pthread_create+160)  
D/SchedPolicy( 1350): #00  pc 0000676c  /system/lib/libcutils.so  
D/SchedPolicy( 1350): #01  pc 00006b3a  /system/lib/libcutils.so (set_sched_policy+49)  
D/SchedPolicy( 1350): #02  pc 00016f26  /system/lib/libbinder.so (android::IPCThreadState::executeCommand(int)+461)  
D/SchedPolicy( 1350): #03  pc 00017380  /system/lib/libbinder.so (android::IPCThreadState::joinThreadPool(bool)+183)  
D/SchedPolicy( 1350): #04  pc 0001b160  /system/lib/libbinder.so  
D/SchedPolicy( 1350): #05  pc 00011264  /system/lib/libutils.so (android::Thread::_threadLoop(void*)+111)  
D/SchedPolicy( 1350): #06  pc 000469bc  /system/lib/libandroid_runtime.so (android::AndroidRuntime::javaThreadShell(void*)+63)  
D/SchedPolicy( 1350): #07  pc 00010dca  /system/lib/libutils.so  
D/SchedPolicy( 1350): #08  pc 0000e3d8  /system/lib/libc.so (__thread_entry+72)  
D/SchedPolicy( 1350): #09  pc 0000dac4  /system/lib/libc.so (pthread_create+160)  
```

##6. native 层


可以利用 android 的 CallStack 来打印：


```cpp
CallStack stack;
stack.update();
stack.dump("SurfaceFlinger"); 
// 4.4 的接口变了 dump 是用来把 log 保存到文件里面去的
// 单纯的打印用这个: statck.log("SurfaceFlinger")
```

要调用这个类的方法先插入 CallStack 头文件，然后在 Android.mk 中链接 utils 库就可以：


```sh
#include <utils/CallStack.h>
LOCAL_SHARED_LIBRARIES := libutils
```

这个可以自己去看 CallStack.h 的头文件（frameworks/base/native/include/utils/CallStack.h），方法的定义:


```sh
// 第一个参数没去研究啥作用，用默认的1吧，
// 第二个参数好像是设置追踪的最大调用堆栈深度，默认是 31
void update(int32_t ignoreDepth=1, int32_t maxDepth=MAX_DEPTH);
// 这个就是把调用堆栈信息输出到 android 的 log 里面，
// 那个参数是 log 前面的前缀
// Dump a stack trace to the log
void dump(const char* prefix = 0) const;
```


---


在Android里解Bug时，有时候需要分析函数的调用情况，此时需要打印出堆栈信息来辅助了解函数的调用过程。

下面是常用的打堆栈的方法：

##1.JAVA代码

在需要打印的位置添加语句new 

Exception().printStackTrace();，然后在logcat里就可以看到调用堆栈信息了。

##2.Kernel

在需要打印函数调用栈的地方添加语句`WARN_ON(1);`，然后在串口或cat /proc/kmsg里可查看到相应信息。

##3.C/C++代码

在需要打印的地方添加如下代码：

```cpp
if (need_print) {
   ALOGD(“xinu, start print function call stack==============”);
   android::CallStack stack;
   stack.update();
   stack.log(“xinu”);
   ALOGD(“xinu, print done========================”);
}
```
