# （一）：添加一个系统调用


应用不能访问内核的内存空间，为了应用和内核交互信息，内核提供一组接口，通过这组接口，应用可以发送请求，内核可以响应处理该请求，这组接口就是系统调用。
关于内核系统调用的实现查看本博客中这篇文章:
http://blog.csdn.net/shallnet/article/details/47113753
本文章以x86-32为例：


```sh
$ uname -a  
Linux localhost 2.6.32 #1 SMP Sat Jun 13 23:55:06 CST 2015 i686 i686 i386 GNU/Linux  
```

第一步，在系统调用表中加入一个表项，表中为每一个有效的系统调用指定了惟一的系统调用号。系统调用表位于arch/x86/kernel/syscall_table_32.S文件中,在该文件中最后一行添加自己的系统调用表项，如下：


```sh
......  
.long sys_preadv  
.long sys_pwritev  
.long sys_rt_tgsigqueueinfo     /* 335 */  
.long sys_perf_event_open  
.long sys_shallnet  
```

虽然没有明确指定编号，但该系统调用已经按次序分配了337这个系统调用号，接下来就应该添加系统调用号。
第二步，添加系统调用号。在Linux中，每个系统调用被赋予一个系统调用号。这样，通过这个独一无二的号就可以关联系统调用。当用户空间的进程执行一个系统调用的时候，这个系统调用号就被用来指明到底是要执行哪个系统调用。在文件arch/sh/include/asm/unistd_32.h该列表中加入一行#define __NR_shallnet   337 ：


```sh
......  
#define __NR_inotify_init1 332  
#define __NR_preadv 333  
#define __NR_pwritev 334  
#define __NR_rt_tgsigqueueinfo 335  
#define __NR_perf_event_open 336  
  
#define __NR_shallnet   337  
  
//#define NR_syscalls 337  
#define NR_syscalls 338  
......  
```

在内核源文件中该行为#define NR_syscalls 337，在系统调用执行的过程中，system_call()函数会根据该值来对用户态进程的有效性进行检查。如果这个号大于或等于NR_syscalls，系统调用处理程序终止。所以应该将原来的#define NR_syscalls 337修改为#define NR_syscalls 338。

第三步，实现shallnet系统调用。在文件kernel/sys.c最后添加如下函数：


```sh
SYSCALL_DEFINE0(shallnet)SYSCALL_DEFINE1(shallnet, int, arg)  
{  
    printk(KERN_ALERT"My blog address: \"http://blog.csdn.net/shallnet\"");  
    return arg + arg;  
}  
```

第四步，重新编译内核。依次执行：

```sh
make oldconfig  
make bzImage  
make modules  
make modules_install  
make install  
```

第五步，重新启动系统然后进入刚新编译的系统，编写测试代码如下：


```c
#include <stdio.h>
#include <unistd.h>
#include <sys/syscall.h>

#define __NR_shallnet 337

int main(int argc, const char* argv[])
{
    int     ret;

    ret = syscall(337, 99);

    printf("shallnet() return: %d\n", ret);
    return 0;
}
```
编译执行该程序如下：


```sh
$ ./target_bin  
shallnet() return: 198  
  
$ demsg  
......  
My blog address: "http://blog.csdn.net/shallnet"  
```


可以看到我们新加的系统调用执行成功了。

可见建立一个新的系统调用还是很容易的，但是不提倡这么做，系统调用需要一个系统调用号，需要修改内核代码，修改之后需要重新编译内核。linux系统应当尽量避免每出现一个新的抽象就加入一个新的系统调用，通常有其他的方法可以代替系统调用，比如说实现一个设备节点等。

本节源码下载：
http://download.csdn.net/detail/gentleliu/9035717




