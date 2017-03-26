# （一）：系統調用


一般情況下進程不能訪問內核所佔內存空間也不能調用內核函數。為了和用戶空間上運行的進程進行交互，內核提供了一組接口。透過該接口，應用程序可以訪問硬件設備和其他操作系統資源。這組接口在應用程序和內核之間扮演了使者的角色，應用程序發送各種請求，而內核負責滿足這些請求(或者讓應用程序暫時擱置)。系統調用就是用戶空間應用程序和內核提供的服務之間的一個接口。

系統調用在用戶空間進程和硬件設備之間添加了一箇中間層，其為用戶空間提供了一種統一的硬件的抽象接口，保證了系統的穩定和安全，使用戶程序具有可移植性。例如fork(),read(), write()等用戶程序可以使用的函數都是系統調用。


用戶空間的程序無法直接執行內核代碼。它們不能直接調用內核空間中的函數，因為內核駐留在受保護的地址空間上。所以，應用程序應該以某種方式通知系統，告訴內核自己需要執行一個系統調用，希望系統切換到內核態，這樣內核就可以代表應用程序來執行該系統調用了。那麼應用程序應該以何種方式通知系統，系統如何切換到內核態？

其實這種改變是通過軟中斷來實現。首先，用戶程序為系統調用設置參數。其中一個參數是系統調用編號。參數設置完成後，程序執行“系統調用”指令。x86系統上的軟中斷由int產生。這個指令會導致一個異常：產生一個事件，這個事件會致使處理器切換到內核態並執行0x80號異常處理程序。此時的異常處理程序實際上就是系統調用處理程序，該處理程序的名字為system_call,它與硬件體系結構緊密相關。對於x86-32系統來說，該處理程序位於arch/x86/kernel/entry_32.S文件中,代碼為：


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
 call *sys_call_table(,%eax,4) //此處執行相應的系統調用  
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


在Linux中，每個系統調用被賦予一個系統調用號。這樣，通過這個獨一無二的號就可以關聯繫統調用。當用戶空間的進程執行一個系統調用的時候，這個系統調用號就被用來指明到底是要執行哪個系統調用。進程不會提及系統調用的名稱。系統調用號定義文件以及形式如下：


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

系統調用號相當關鍵，一旦分配就不能再有任何變更，否則編譯好的應用程序就會崩潰。Linux有一個“未實現”系統調用sys_ni_syscall()，它除了返回一ENOSYS外不做任何其他工作，這個錯誤號就是專門針對無效的系統調用而設的。

因為所有的系統調用陷入內核的方式都一樣，所以僅僅是陷入內核空間是不夠的。因此必須把系統調用號一併傳給內核。在x86上，系統調用號是通過eax寄存器傳遞給內核的。在陷人內核之前，用戶空間就把相應系統調用所對應的號放入eax中了。這樣系統調用處理程序一旦運行，就可以從eax中得到數據。其他體系結構上的實現也都類似。

內核記錄了系統調用表中的所有已註冊過的系統調用的列表，存儲在sys_call_table中。它與體系結構有關，32位x86一般定義在arch/x86/kernel/syscall_table_32.s文件中。這個表中為每一個有效的系統調用指定了惟一的系統調用號。sys_call_table是一張由指向實現各種系統調用的內核函數的函數指針組成的表。syscall_table_32.s文件如下：


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
 
system_call()函數通過將給定的系統調用號與NR_syscalls做比較來檢查其有效性。如果它大於或者等於NR syscalls,該函數就返回一ENOSYS。否則，就執行相應的系統調用。

call *sys_call_table(，%eax, 4)

由於系統調用表中的表項是以32位(4字節)類型存放的，所以內核需要將給定的系統調用號乘以4，然後用所得的結果在該表中查詢其位置。

除了系統調用號以外，大部分系統調用都還需要一些外部的參數輸入。所以，在發生異常的時候，應該把這些參數從用戶空間傳給內核。最簡單的辦法就是像傳遞系統調用號一樣把這些參數也存放在寄存器裡。在x86系統上，ebx, ecx, edx, esi和edi按照順序存放前五個參數。需要六個或六個以上參數的情況不多見，此時，應該用一個單獨的寄存器存放指向所有這些參數在用戶空間地址的指針。給用戶空間的返回值也通過寄存器傳遞。在x86系統上，它存放在eax寄存器中。

下面我們看看用中斷的方式如何完成系統調用功能：


```c
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
int main(int argc, const char* argv[])
{
    pid_t   pid;
    asm volatile(
        "mov $0, %%ebx\n\t"
        "mov $20, %%eax\n\t"    //把系統調用號20放入eax寄存器中，20對應於SYS_getpid()系統調用
        "int $0x80\n\t"    //0x80中斷
        "mov %%eax, %0\n\t"    //將執行結果存放在pid變量中
        :"=m"(pid)
    );
    printf("int PID: %d\n", pid);
    printf("api PID: %d\n", getpid());
    return 0;
}
```


此處沒有傳遞參數，因為getpid不需要參數。關於傳參數的示例可以參考文章中hello
world示例代碼：http://blog.csdn.net/shallnet/article/details/45544271
本實例執行結果為：

```sh
$ ./target_bin
int PID: 4911
api PID: 4911
```

一般情況下，應用程序通過在用戶空間實現的應用編程接口（API）而不是系統調用來編程。API是一個函數定義，說明了如何獲得一個給定的服務，比如read()、malloc()、free（）、abs()等。它有可能和系統調用形式上一致，比如read()接口就和read系統調用對應，但這種對應並非一一對應，往往會出現幾種不同的API內部用到統一個系統調用，比如malloc()、free（）內部利用brk( )系統調用來擴大或縮小進程的堆；或一個API利用了好幾個系統調用組合完成服務。更有些API甚至不需要任何系統調用——因為它不必需要內核服務，如計算整數絕對值的abs（）接口。

Linux的用戶編程接口遵循了在Unix世界中最流行的應用編程界面標準——POSIX標準，這套標準定義了一系列API。在Linux中（Unix也如此）這些API主要是通過C庫（libc）實現的，它除了定義的一些標準的C函數外，一個很重要的任務就是提供了一套封裝例程將系統調用在用戶空間包裝後供用戶編程使用。不過封裝並非必須的，如果你願意直接調用，內核也提供了一個syscall()函數來實現調用。如下示例為使用c庫調用和直接調用分別來獲取當前進程ID：


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

系統調用在內核有一個實現函數，以getpid為例，其在內核實現為：

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
 
其中SYSCALL_DEFINE0為一個宏，它定義一個無參數（尾部數字代表參數個數）的系統調用，展開後代碼如下：


```c
asmlinkage long sys_getpid(void)  
{  
    return current->tpid;  
}  
```

其中asmlinkage 是一個編譯指令，通知編譯器僅從棧中提取該函數參數，所有系統調用都需要這個限定詞。系統調用getpid()在內核中被定義成sys_getpid()，這是linux所有系統調用都應該遵守的命名規則。

下面總結一下系統調用的實現過程：

Linux中實現系統調用利用了0x86體系結構中的軟件中斷，也就是調用int $0x80彙編指令，這條彙編指令將產生向量為128的編程異常，此時處理器切換到內核態並執行0x80號異常處理程序。此時的異常處理程序實際上就是系統調用處理程序，該處理程序的名字為system_call()，對於x86-32系統來說，該處理程序位於arch/x86/kernel/entry_32.S文件中，使用匯編語言編寫。那麼所有的系統調用都會轉到這裡。在執行int0x80前，系統調用號被裝入eax寄存器（相應參數也會傳遞到其它寄存器中），這個系統調用號被用來指明到底是要執行哪個系統調用，這樣系統調用處理程序一旦運行，就從eax中得到系統調用號，然後根據系統調用號在系統調用表中尋找相應服務例程（例如sys_getpid()函數）。當服務例程結束時，system_call( ) 從eax獲得系統調用的返回值，並把這個返回值存放在曾保存用戶態 eax寄存器棧單元的那個位置上，最後該函數再負責切換到用戶空間，使用戶進程繼續執行。


http://download.csdn.net/detail/gentleliu/8941353