# 通過為ptrace調用設置catchpoint破解anti-debugging的程序
## 例子
	#include <sys/ptrace.h>
	#include <stdio.h>
	 
	int main()                                                                      
	{
	        if (ptrace(PTRACE_TRACEME, 0, 0, 0) < 0 ) {
	                printf("Gdb is debugging me, exit.\n");
	                return 1;
	        }
	        printf("No debugger, continuing\n");
	        return 0;
	}



## 技巧
有些程序不想被gdb調試，它們就會在程序中調用“`ptrace`”函數，一旦返回失敗，就證明程序正在被gdb等類似的程序追蹤，所以就直接退出。以上面程序為例：  

	(gdb) start
	Temporary breakpoint 1 at 0x400508: file a.c, line 6.
	Starting program: /data2/home/nanxiao/a
	
	Temporary breakpoint 1, main () at a.c:6
	6                       if (ptrace(PTRACE_TRACEME, 0, 0, 0) < 0 ) {
	(gdb) n
	7                               printf("Gdb is debugging me, exit.\n");
	(gdb)
	Gdb is debugging me, exit.
	8                               return 1;



破解這類程序的辦法就是為`ptrace`調用設置`catchpoint`，通過修改`ptrace`的返回值，達到目的。仍以上面程序為例：  

	(gdb) catch syscall ptrace
	Catchpoint 2 (syscall 'ptrace' [101])
	(gdb) r
	Starting program: /data2/home/nanxiao/a
	
	Catchpoint 2 (call to syscall ptrace), 0x00007ffff7b2be9c in ptrace () from /lib64/libc.so.6
	(gdb) c
	Continuing.
	
	Catchpoint 2 (returned from syscall ptrace), 0x00007ffff7b2be9c in ptrace () from /lib64/libc.so.6
	(gdb) set $rax = 0
	(gdb) c
	Continuing.
	No debugger, continuing
	[Inferior 1 (process 11491) exited normally]

可以看到，通過修改`rax`寄存器的值，達到修改返回值的目的，從而讓gdb可以繼續調試程序（打印“`No debugger, continuing`”）。  
詳細過程，可以參見這篇文章[避開 PTRACE_TRACME 反追蹤技巧](http://blog.linux.org.tw/~jserv/archives/2011_08.html).

## 貢獻者

nanxiao
