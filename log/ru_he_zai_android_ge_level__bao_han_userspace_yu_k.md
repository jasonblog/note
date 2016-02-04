# 如何在 Android 各 level ( 包含 user space 与 kernel space ) 使用dump call stack的方法


瞭解 Android 各 level ( UI, framework 与 HAL) 与 kernel 间, 如何印出 call stack, 方便追 code 与 debug

- kernel call stack
- Android Java layer
- Android framework ( written by c++)
- Android HAL ( written by c )
- Call Stack 没有出现 function name 

##kernel call stack
如果想知道call stack,也就是说, 我们想知道是谁call到func_foo(). 此时,我们可以利用 dump_stack(),放在你想dump back trace的地方就OK啰.
  
```c  
void func_foo(void){
  
   int a=3;
   ...
   
   dump_stack();

   ...

}
```


## Java layer call stack
在Java档案, 可以使用下述方法得到dump call stack

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

在C/C++ 档案, Android 已经有写了frameworks/native/libs/utils/CallStack.cpp 供我们使用

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

如果你所使用是Android 4.4 之后
请改用

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


##在Android.mk 记得要加
```c
LOCAL_SHARED_LIBRARIES += libutils
```


##C layer call stack
由于C去call C++需要做一些宣告, 所以将它独立出来方便使用(dump_stack.cpp与 dump_stack.h)

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

如果你所使用是Android 4.4 之后
请改用

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

同样地, Android.mk也需要修改

```c
LOCAL_SRC_FILES := \
         …... \
         dump_stack.cpp

LOCAL_SHARED_LIBRARIES += libutils
```
接下来在C file中要使用时只要

```c
extern void dump_stack_android();


void function_a()
{
  …
  dump_stack_android();
  …
}
```


##[ Call Stack 没有出现 function name]
有时我们会发现在C++ 或 C 语言中使用 CallStack , 在 call dump 中并没有出现 function name


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

我们追一下 CallStack 是如何被实作
先回顾一下 CallStack 是如何被使用 (以 Android 4.4 为例)
```c
CallStack stack;  
stack.update();  
stack.log();  
 ```
 
 先看一下 update( ) function 的定义 ( it is under system/core/include/utils/CallStack.h)
 
 ```c
// Immediately collect the stack traces for the specified thread.  
void update(int32_t ignoreDepth=1, int32_t maxDepth=MAX_DEPTH, pid_t tid=CURRENT_THREAD);  
```
   
所以透过 update( ) function, 我们可以设定想看哪一个 thread 并 dump 出多少层的 call stack, 如果都没写, 就是以当前的 thread 去做 call stack dump, update( ) function 会将实际可以 dump 多少的 frame 给抓出来, 其中 frame 的数量记录在 mCount 变数, 各 frame 的资讯则记录在 mStack[ ] 里面, 接下来再透过 log( ) function 把 call stack 里的 program counter 所记载的记忆体位址去把相对应的 function name 给解析出来.


```c
 log( )  
 |--> print( )  
    |--> get_backtrace_symbols( )  
```


看一下 get_backtrace_symbols( ) 在做些什么

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

这是因为它是使用 dladdr() 去读取该share lib的 dynamic symbol 而获取 function name
(http://stackoverflow.com/questions/11731229/dladdr-doesnt-return-the-function-name )

但是如果该 function 是宣告成 static, 该 function name 就不会出现在 dynamic symbol 里 (你可以使用 arm-linux-androideabi-nm -D xxxx.so | grep the_function_name , 如果没有出现, 就表示该 funciton name 并不在 dynamic symbol 里),  遇到这情况就只好使用 add2line 指令去读 out folder 下的 symbol 了, 各位可以参考我另一篇文章 http://janbarry0914.blogspot.tw/2011/07/android-crash-tombstone.html . 感谢.

