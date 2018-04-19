# Android下打印調試堆棧方法


打印堆棧是調試的常用方法，一般在系統異常時，我們可以將異常情況下的堆棧打印出來，這樣十分方便錯誤查找。實際上還有另外一個非常有用的功能：分析代碼的行為。android代碼太過龐大複雜了，完全的靜態分析經常是無從下手，因此通過打印堆棧的動態分析也十分必要。



Android打印堆棧的方法，簡單歸類一下 

##1. zygote的堆棧dump

實際上這個可以同時dump java線程及native線程的堆棧，對於java線程，java堆棧和native堆棧都可以得到。

使用方法很簡單，直接在adb shell或串口中輸入：


```sh
kill -3 <pid>  
```

輸出的trace會保存在 /data/anr/traces.txt文件中。這個需要注意，如果沒有 /data/anr/這個目錄或/data/anr/traces.txt這個文件，需要手工創建一下，並設置好讀寫權限。
如果需要在代碼中，更容易控制堆棧的輸出時機，可以用以下命令獲取zygote的core dump:


```sh
Process.sendSignal(pid, Process.SIGNAL_QUIT);  
```

原理和命令行是一樣的。

不過需要注意兩點：


- adb shell可能會沒有權限，需要root。
- android 4.2中關閉了native thread的堆棧打印，詳見 dalvik/vm/Thread.cpp的dumpNativeThread方法：


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


##2. debuggerd的堆棧dump

debuggerd是android的一個daemon進程，負責在進程異常出錯時，將進程的運行時信息dump出來供分析。debuggerd生成的coredump數據是以文本形式呈現，被保存在 /data/tombstone/ 目錄下(名字取的也很形象，tombstone是墓碑的意思)，共可保存10個文件，當超過10個時，會覆蓋重寫最早生成的文件。從4.2版本開始，debuggerd同時也是一個實用工具：可以在不中斷進程執行的情況下打印當前進程的native堆棧。使用方法是:


```sh
debuggerd -b <pid>  
```

這可以協助我們分析進程執行行為，但最最有用的地方是：它可以非常簡單的定位到native進程中鎖死或錯誤邏輯引起的死循環的代碼位置。

##3. java代碼中打印堆棧

Java代碼打印堆棧比較簡單， 堆棧信息獲取和輸出，都可以通過Throwable類的方法實現。目前通用的做法是在java進程出現需要注意的異常時，打印堆棧，然後再決定退出或挽救。通常的方法是使用exception的printStackTrace()方法：


```java
/**
 * 打印函數的調用棧
 *
 * @return 調用棧
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

當然也可以只打印堆棧不退出，這樣就比較方便分析代碼的動態運行情況。Java代碼中插入堆棧打印的方法如下：


```java
Log.d(TAG,Log.getStackTraceString(new Throwable()));  
```

##4. C++代碼中打印堆棧

C++也是支持異常處理的，異常處理庫中，已經包含了獲取backtrace的接口，Android也是利用這個接口來打印堆棧信息的。在Android的C++中，已經集成了一個工具類CallStack，在libutils.so中。使用方法：

```cpp
#include <utils/CallStack.h>  
...  
CallStack stack;  
stack.update();  
stack.dump();  
```

使用方式比較簡單。目前Andoid4.2版本已經將相關信息解析的很到位，符號表查找，demangle，偏移位置校正都做好了。

##5. C代碼中打印堆棧

C代碼，尤其是底層C庫，想要看到調用的堆棧信息，還是比較麻煩的。 CallStack肯定是不能用，一是因為其實C++寫的，需要重新封裝才能在C中使用，二是底層庫反調上層庫的函數，會造成鏈接器循環依賴而無法鏈接。不過也不是沒有辦法，可以通過android工具類CallStack實現中使用的unwind調用及符號解析函數來處理。

這裡需要注意的是，為解決鏈接問題，最好使用dlopen方式，查找需要用到的接口再直接調用，這樣會比較簡單。如下為相關的實現代碼，只需要在要打印的文件中插入此部分代碼，然後調用getCallStack()即可，無需包含太多的頭文件和修改Android.mk文件：


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

對sched_policy.c的堆棧調用分析如下,注意具體是否要打印，在哪裡打印，還可以通過pid、uid、property等來控制一下，這樣就不會被淹死在trace的汪洋大海中。


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

##6. native 層


可以利用 android 的 CallStack 來打印：


```cpp
CallStack stack;
stack.update();
stack.dump("SurfaceFlinger"); 
// 4.4 的接口變了 dump 是用來把 log 保存到文件裡面去的
// 單純的打印用這個: statck.log("SurfaceFlinger")
```

要調用這個類的方法先插入 CallStack 頭文件，然後在 Android.mk 中鏈接 utils 庫就可以：


```sh
#include <utils/CallStack.h>
LOCAL_SHARED_LIBRARIES := libutils
```

這個可以自己去看 CallStack.h 的頭文件（frameworks/base/native/include/utils/CallStack.h），方法的定義:


```sh
// 第一個參數沒去研究啥作用，用默認的1吧，
// 第二個參數好像是設置追蹤的最大調用堆棧深度，默認是 31
void update(int32_t ignoreDepth=1, int32_t maxDepth=MAX_DEPTH);
// 這個就是把調用堆棧信息輸出到 android 的 log 裡面，
// 那個參數是 log 前面的前綴
// Dump a stack trace to the log
void dump(const char* prefix = 0) const;
```


---


在Android裡解Bug時，有時候需要分析函數的調用情況，此時需要打印出堆棧信息來輔助瞭解函數的調用過程。

下面是常用的打堆棧的方法：

##1.JAVA代碼

在需要打印的位置添加語句new 

Exception().printStackTrace();，然後在logcat裡就可以看到調用堆棧信息了。

##2.Kernel

在需要打印函數調用棧的地方添加語句`WARN_ON(1);`，然後在串口或cat /proc/kmsg裡可查看到相應信息。

##3.C/C++代碼

在需要打印的地方添加如下代碼：

```cpp
if (need_print) {
   ALOGD(“xinu, start print function call stack==============”);
   android::CallStack stack;
   stack.update();
   stack.log(“xinu”);
   ALOGD(“xinu, print done========================”);
}
```
