# 如何在 Android 各 level ( 包含 user space 與 kernel space ) 使用dump call stack的方法


瞭解 Android 各 level ( UI, framework 與 HAL) 與 kernel 間, 如何印出 call stack, 方便追 code 與 debug

- kernel call stack
- Android Java layer
- Android framework ( written by c++)
- Android HAL ( written by c )
- Call Stack 沒有出現 function name 

##kernel call stack
如果想知道call stack,也就是說, 我們想知道是誰call到func_foo(). 此時,我們可以利用 dump_stack(),放在你想dump back trace的地方就OK囉.
  
```c  
void func_foo(void){
  
   int a=3;
   ...
   
   dump_stack();

   ...

}
```


## Java layer call stack
在Java檔案, 可以使用下述方法得到dump call stack

```c
Slog.d(Thread.currentThread().getStackTrace()[2].getClassName(), "YAO ["+ Thread.currentThread().getStackTrace()[2].getMethodName() +" | "+Thread.currentThre      ad().getStackTrace()[2].getFileName()+":"+Thread.currentThread().getStackTrace()[2].getLineNumber()+"]");
```



```c
public void foo(boolean state, int flags) {
  ... 
  Log.d(TAG,"xxxx", new Throwable());
  ...
 }
```
## C++ layer call stack

在C/C++ 檔案, Android 已經有寫了frameworks/native/libs/utils/CallStack.cpp 供我們使用

```c
#include <utils/CallStack.h>
...
void foo(void) {
...
    android::CallStack stack;
    stack.update();
    stack.dump("XXX");

...
}
```

如果你所使用是Android 4.4 之後
請改用

```c
#include <utils/CallStack.h>
...
void foo(void) {
...
    android::CallStack stack;
    stack.update( );
    stack.log("XXX");

...
}
```


##在Android.mk 記得要加
```c
LOCAL_SHARED_LIBRARIES += libutils
```


##C layer call stack
由於C去call C++需要做一些宣告, 所以將它獨立出來方便使用(dump_stack.cpp與 dump_stack.h)

- dump_stack.h

```c

#ifdef __cplusplus
extern "C" {
#endif

  void dump_stack_android(void);
  
#ifdef __cplusplus
}
#endif
```

- dump_stack.cpp

```c
#include "dump_stack.h"
#include <utils/CallStack.h>

using namespace android;
extern "C"{
  void dump_stack_android(void)
  {
 CallStack stack;
 stack.update();
 stack.dump("XXX");
  }
}
```

如果你所使用是Android 4.4 之後
請改用

```c
#include "dump_stack.h"
#include <utils/CallStack.h>

using namespace android;
extern "C"{
  void dump_stack_android(void)
  {
     CallStack stack;
     stack.update();
     stack.log("XXX");
  }
}
```

同樣地, Android.mk也需要修改

```c
LOCAL_SRC_FILES := \
         …... \
         dump_stack.cpp

LOCAL_SHARED_LIBRARIES += libutils
```
接下來在C file中要使用時只要

```c
extern void dump_stack_android();


void function_a()
{
  …
  dump_stack_android();
  …
}
```


##[ Call Stack 沒有出現 function name]
有時我們會發現在C++ 或 C 語言中使用 CallStack , 在 call dump 中並沒有出現 function name


```c
D/XXX  (  147): #00  pc 00001b90  /system/lib/hw/audio.primary.mrvl.so (dump_stack_android+19)
D/XXX  (  147): #01  pc 00004b56  /system/lib/hw/audio.primary.mrvl.so
D/XXX  (  147): #02  pc 0001f828  /system/lib/libaudioflinger.so
D/XXX  (  147): #03  pc 00019138  /system/lib/libaudioflinger.so
D/XXX  (  147): #04  pc 00023bb6  /system/lib/libaudioflinger.so
D/XXX  (  147): #05  pc 0000e9fe  /system/lib/libutils.so (android::Thread::_threadLoop(void*)+213)
D/XXX  (  147): #06  pc 0000e530  /system/lib/libutils.so
D/XXX  (  147): #07  pc 0000d208  /system/lib/libc.so (__thread_entry+72)
D/XXX  (  147): #08  pc 0000d3a4  /system/lib/libc.so (pthread_create+240)
```

我們追一下 CallStack 是如何被實作
先回顧一下 CallStack 是如何被使用 (以 Android 4.4 為例)
```c
CallStack stack;  
stack.update();  
stack.log();  
 ```
 
 先看一下 update( ) function 的定義 ( it is under system/core/include/utils/CallStack.h)
 
 ```c
// Immediately collect the stack traces for the specified thread.  
void update(int32_t ignoreDepth=1, int32_t maxDepth=MAX_DEPTH, pid_t tid=CURRENT_THREAD);  
```
   
所以透過 update( ) function, 我們可以設定想看哪一個 thread 並 dump 出多少層的 call stack, 如果都沒寫, 就是以當前的 thread 去做 call stack dump, update( ) function 會將實際可以 dump 多少的 frame 給抓出來, 其中 frame 的數量記錄在 mCount 變數, 各 frame 的資訊則記錄在 mStack[ ] 裡面, 接下來再透過 log( ) function 把 call stack 裡的 program counter 所記載的記憶體位址去把相對應的 function name 給解析出來.


```c
 log( )  
 |--> print( )  
    |--> get_backtrace_symbols( )  
```


看一下 get_backtrace_symbols( ) 在做些什麼

```c
void get_backtrace_symbols(const backtrace_frame_t* backtrace, size_t frames,
     backtrace_symbol_t* backtrace_symbols) {

   ... 
   for (size_t i = 0; i < frames; i++) {
         ...
             Dl_info info;
             if (dladdr((const void*)frame->absolute_pc, &info) && info.dli_sname) {
                symbol->relative_symbol_addr = (uintptr_t)info.dli_saddr
                     - (uintptr_t)info.dli_fbase;
                symbol->symbol_name = strdup(info.dli_sname);
                symbol->demangled_name = 
                        demangle_symbol_name(symbol->symbol_name);
             }
        ...
   }
   release_my_map_info_list(milist);
```

這是因為它是使用 dladdr() 去讀取該share lib的 dynamic symbol 而獲取 function name
(http://stackoverflow.com/questions/11731229/dladdr-doesnt-return-the-function-name )

但是如果該 function 是宣告成 static, 該 function name 就不會出現在 dynamic symbol 裡 (你可以使用 arm-linux-androideabi-nm -D xxxx.so | grep the_function_name , 如果沒有出現, 就表示該 funciton name 並不在 dynamic symbol 裡),  遇到這情況就只好使用 add2line 指令去讀 out folder 下的 symbol 了, 各位可以參考我另一篇文章 http://janbarry0914.blogspot.tw/2011/07/android-crash-tombstone.html . 感謝.



- android 中 c++ 利用 constructor & deconstructor 每個函數進出打印


```cpp
class __CallLog__
{
private:
    const char* logtag;
    const char* file;
    int32_t line;
    const char* func;
    int64_t mEnterTime;
public:
    inline __CallLog__(const char* __logtag, const char* __file, int32_t __line,
                       const char* __func):
        logtag(__logtag), file(__file), line(__line), func(__func),
        mEnterTime(android::uptimeMillis())
    {
        LOG(LOG_DEBUG, logtag, "TID:%d ...%s:%d:\tEnter %s\n",
            static_cast<int32_t>(syscall(__NR_gettid)), strrchr(file, '/'), line, func);
    }

    inline void timeDiff(int32_t diffLine)
    {
        LOG(LOG_DEBUG, logtag,
            "TID:%d ...%s:%d:\tTime diff from line %d is %lld millis\n",
            static_cast<int32_t>(syscall(__NR_gettid)), strrchr(file, '/'), line, diffLine,
            android::uptimeMillis() - mEnterTime);
    }

    inline ~__CallLog__()
    {
        LOG(LOG_DEBUG, logtag, "TID:%d ...%s:%d:\tLeave %s (takes %llu millis)\n",
            static_cast<int32_t>(syscall(__NR_gettid)), strrchr(file, '/'), line, func,
            android::uptimeMillis() - mEnterTime);
    }
};

#define GLOGENTRY(args...) __CallLog__ __call_log__(LOG_TAG, __FILE__, __LINE__, __FUNCTION__);
#define GLOGTENTRY(logtag, args...) __CallLog__ __call_log__(logtag, __FILE__, __LINE__, __FUNCTION__);


int main(int argc, char* argv[])
{
    GLOGENTRY();

    return 0;
}
```
