# （一）：系统调用


一般情况下进程不能访问内核所占内存空间也不能调用内核函数。为了和用户空间上运行的进程进行交互，内核提供了一组接口。透过该接口，应用程序可以访问硬件设备和其他操作系统资源。这组接口在应用程序和内核之间扮演了使者的角色，应用程序发送各种请求，而内核负责满足这些请求(或者让应用程序暂时搁置)。系统调用就是用户空间应用程序和内核提供的服务之间的一个接口。

系统调用在用户空间进程和硬件设备之间添加了一个中间层，其为用户空间提供了一种统一的硬件的抽象接口，保证了系统的稳定和安全，使用户程序具有可移植性。例如fork(),read(), write()等用户程序可以使用的函数都是系统调用。


用户空间的程序无法直接执行内核代码。它们不能直接调用内核空间中的函数，因为内核驻留在受保护的地址空间上。所以，应用程序应该以某种方式通知系统，告诉内核自己需要执行一个系统调用，希望系统切换到内核态，这样内核就可以代表应用程序来执行该系统调用了。那么应用程序应该以何种方式通知系统，系统如何切换到内核态？

其实这种改变是通过软中断来实现。首先，用户程序为系统调用设置参数。其中一个参数是系统调用编号。参数设置完成后，程序执行“系统调用”指令。x86系统上的软中断由int产生。这个指令会导致一个异常：产生一个事件，这个事件会致使处理器切换到内核态并执行0x80号异常处理程序。此时的异常处理程序实际上就是系统调用处理程序，该处理程序的名字为system_call,它与硬件体系结构紧密相关。对于x86-32系统来说，该处理程序位于arch/x86/kernel/entry_32.S文件中,代码为：


```asm
......  
 # system call handler stub  
ENTRY(system_call)  
 RING0_INT_FRAME # can't unwind into user space anyway  
 pushl %eax # save orig_eax  
 CFI_ADJUST_CFA_OFFSET 4  
 SAVE_ALL  
 GET_THREAD_INFO(%ebp)  
     # system call tracing in operation / emulation  
 testl $_TIF_WORK_SYSCALL_ENTRY,TI_flags(%ebp)  
 jnz syscall_trace_entry  
 cmpl $(nr_syscalls), %eax  
 jae syscall_badsys  
syscall_call:  
 call *sys_call_table(,%eax,4) //此处执行相应的系统调用  
 movl %eax,PT_EAX(%esp) # store the return value  
syscall_exit:  
 LOCKDEP_SYS_EXIT  
 DISABLE_INTERRUPTS(CLBR_ANY) # make sure we don't miss an interrupt  
     # setting need_resched or sigpending  
     # between sampling and the iret  
 TRACE_IRQS_OFF  
 movl TI_flags(%ebp), %ecx  
 testl $_TIF_ALLWORK_MASK, %ecx # current->work  
 jne syscall_exit_work  
 ......  
```


在Linux中，每个系统调用被赋予一个系统调用号。这样，通过这个独一无二的号就可以关联系统调用。当用户空间的进程执行一个系统调用的时候，这个系统调用号就被用来指明到底是要执行哪个系统调用。进程不会提及系统调用的名称。系统调用号定义文件以及形式如下：


```c
$ cat ./arch/x86/include/asm/unistd.h  
#ifdef __KERNEL__                                                                                                                        
# ifdef CONFIG_X86_32  
#  include "unistd_32.h"  
# else  
#  include "unistd_64.h"  
# endif  
#else  
# ifdef __i386__  
#  include "unistd_32.h"  
# else  
#  include "unistd_64.h"  
# endif  
#endif  
```

```c
# cat arch/x86/include/asm/unistd_32.h  
#ifndef _ASM_X86_UNISTD_32_H  
#define _ASM_X86_UNISTD_32_H  
  
/*  
 * This file contains the system call numbers.  
 */  
  
#define __NR_restart_syscall      0  
#define __NR_exit                 1  
#define __NR_fork                 2  
#define __NR_read                 3  
#define __NR_write                4  
#define __NR_open                 5  
#define __NR_close                6  
#define __NR_waitpid              7  
#define __NR_creat                8  
#define __NR_link                 9  
#define __NR_unlink              10  
#define __NR_execve              11  
#define __NR_chdir               12  
#define __NR_time                13  
#define __NR_mknod               14  
#define __NR_chmod               15  
#define __NR_lchown              16  
#define __NR_break               17  
#define __NR_oldstat             18  
#define __NR_lseek               19  
#define __NR_getpid              20  
#define __NR_mount               21  
......  
```

系统调用号相当关键，一旦分配就不能再有任何变更，否则编译好的应用程序就会崩溃。Linux有一个“未实现”系统调用sys_ni_syscall()，它除了返回一ENOSYS外不做任何其他工作，这个错误号就是专门针对无效的系统调用而设的。

因为所有的系统调用陷入内核的方式都一样，所以仅仅是陷入内核空间是不够的。因此必须把系统调用号一并传给内核。在x86上，系统调用号是通过eax寄存器传递给内核的。在陷人内核之前，用户空间就把相应系统调用所对应的号放入eax中了。这样系统调用处理程序一旦运行，就可以从eax中得到数据。其他体系结构上的实现也都类似。

内核记录了系统调用表中的所有已注册过的系统调用的列表，存储在sys_call_table中。它与体系结构有关，32位x86一般定义在arch/x86/kernel/syscall_table_32.s文件中。这个表中为每一个有效的系统调用指定了惟一的系统调用号。sys_call_table是一张由指向实现各种系统调用的内核函数的函数指针组成的表。syscall_table_32.s文件如下：


```c
ENTRY(sys_call_table)  
 .long sys_restart_syscall /* 0 - old "setup()" system call, used for restarting */  
 .long sys_exit  
 .long ptregs_fork  
 .long sys_read  
 .long sys_write  
 .long sys_open /* 5 */  
 .long sys_close  
 .long sys_waitpid  
 .long sys_creat  
 .long sys_link  
 .long sys_unlink /* 10 */  
 .long ptregs_execve  
 ......  
 .long sys_timerfd_settime /* 325 */  
 .long sys_timerfd_gettime  
 .long sys_signalfd4  
 .long sys_eventfd2  
 .long sys_epoll_create1  
 .long sys_dup3 /* 330 */  
 .long sys_pipe2  
 .long sys_inotify_init1  
 .long sys_preadv  
 .long sys_pwritev  
 .long sys_rt_tgsigqueueinfo /* 335 */  
 .long sys_perf_event_open  
 ```
 
system_call()函数通过将给定的系统调用号与NR_syscalls做比较来检查其有效性。如果它大于或者等于NR syscalls,该函数就返回一ENOSYS。否则，就执行相应的系统调用。

call *sys_call_table(，%eax, 4)

由于系统调用表中的表项是以32位(4字节)类型存放的，所以内核需要将给定的系统调用号乘以4，然后用所得的结果在该表中查询其位置。

除了系统调用号以外，大部分系统调用都还需要一些外部的参数输入。所以，在发生异常的时候，应该把这些参数从用户空间传给内核。最简单的办法就是像传递系统调用号一样把这些参数也存放在寄存器里。在x86系统上，ebx, ecx, edx, esi和edi按照顺序存放前五个参数。需要六个或六个以上参数的情况不多见，此时，应该用一个单独的寄存器存放指向所有这些参数在用户空间地址的指针。给用户空间的返回值也通过寄存器传递。在x86系统上，它存放在eax寄存器中。

下面我们看看用中断的方式如何完成系统调用功能：


```c
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
int main(int argc, const char* argv[])
{
    pid_t   pid;
    asm volatile(
        "mov $0, %%ebx\n\t"
        "mov $20, %%eax\n\t"    //把系统调用号20放入eax寄存器中，20对应于SYS_getpid()系统调用
        "int $0x80\n\t"    //0x80中断
        "mov %%eax, %0\n\t"    //将执行结果存放在pid变量中
        :"=m"(pid)
    );
    printf("int PID: %d\n", pid);
    printf("api PID: %d\n", getpid());
    return 0;
}
```


此处没有传递参数，因为getpid不需要参数。关于传参数的示例可以参考文章中hello
world示例代码：http://blog.csdn.net/shallnet/article/details/45544271
本实例执行结果为：

```sh
$ ./target_bin
int PID: 4911
api PID: 4911
```

一般情况下，应用程序通过在用户空间实现的应用编程接口（API）而不是系统调用来编程。API是一个函数定义，说明了如何获得一个给定的服务，比如read()、malloc()、free（）、abs()等。它有可能和系统调用形式上一致，比如read()接口就和read系统调用对应，但这种对应并非一一对应，往往会出现几种不同的API内部用到统一个系统调用，比如malloc()、free（）内部利用brk( )系统调用来扩大或缩小进程的堆；或一个API利用了好几个系统调用组合完成服务。更有些API甚至不需要任何系统调用——因为它不必需要内核服务，如计算整数绝对值的abs（）接口。

Linux的用户编程接口遵循了在Unix世界中最流行的应用编程界面标准——POSIX标准，这套标准定义了一系列API。在Linux中（Unix也如此）这些API主要是通过C库（libc）实现的，它除了定义的一些标准的C函数外，一个很重要的任务就是提供了一套封装例程将系统调用在用户空间包装后供用户编程使用。不过封装并非必须的，如果你愿意直接调用，内核也提供了一个syscall()函数来实现调用。如下示例为使用c库调用和直接调用分别来获取当前进程ID：


```c
#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/syscall.h>
int main(int argc, const char* argv[])
{
    pid_t     pid, pidt;
    pid = getpid();
    pidt = syscall(SYS_getpid);
    printf("getpid: %d\n", pid);
    printf("SYS_getpid: %d\n", pidt);
    return 0;
}
```

系统调用在内核有一个实现函数，以getpid为例，其在内核实现为：

```c
/** 
 * sys_getpid - return the thread group id of the current process 
 * 
 * Note, despite the name, this returns the tgid not the pid.  The tgid and 
 * the pid are identical unless CLONE_THREAD was specified on clone() in 
 * which case the tgid is the same in all threads of the same group. 
 * 
 * This is SMP safe as current->tgid does not change. 
 */  
SYSCALL_DEFINE0(getpid)  
{  
    return task_tgid_vnr(current);  
}
 ```
 
其中SYSCALL_DEFINE0为一个宏，它定义一个无参数（尾部数字代表参数个数）的系统调用，展开后代码如下：


```c
asmlinkage long sys_getpid(void)  
{  
    return current->tpid;  
}  
```

其中asmlinkage 是一个编译指令，通知编译器仅从栈中提取该函数参数，所有系统调用都需要这个限定词。系统调用getpid()在内核中被定义成sys_getpid()，这是linux所有系统调用都应该遵守的命名规则。

下面总结一下系统调用的实现过程：

Linux中实现系统调用利用了0x86体系结构中的软件中断，也就是调用int $0x80汇编指令，这条汇编指令将产生向量为128的编程异常，此时处理器切换到内核态并执行0x80号异常处理程序。此时的异常处理程序实际上就是系统调用处理程序，该处理程序的名字为system_call()，对于x86-32系统来说，该处理程序位于arch/x86/kernel/entry_32.S文件中，使用汇编语言编写。那么所有的系统调用都会转到这里。在执行int0x80前，系统调用号被装入eax寄存器（相应参数也会传递到其它寄存器中），这个系统调用号被用来指明到底是要执行哪个系统调用，这样系统调用处理程序一旦运行，就从eax中得到系统调用号，然后根据系统调用号在系统调用表中寻找相应服务例程（例如sys_getpid()函数）。当服务例程结束时，system_call( ) 从eax获得系统调用的返回值，并把这个返回值存放在曾保存用户态 eax寄存器栈单元的那个位置上，最后该函数再负责切换到用户空间，使用户进程继续执行。


http://download.csdn.net/detail/gentleliu/8941353