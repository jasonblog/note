# 設置觀察點
## 例子
	#include <stdio.h>
	#include <pthread.h>
	#include <unistd.h>
	int a = 0;
	
	void *thread1_func(void *p_arg)
	{
	        while (1)
	        {
	                a++;
	                sleep(10);
	        }
	}
	
	int main(int argc, char* argv[])
	{
	        pthread_t t1;
	
	        pthread_create(&t1, NULL, thread1_func, "Thread 1");
			
	        sleep(1000);
	        return 0;
	}

## 技巧
gdb可以使用“`watch`”命令設置觀察點，也就是當一個變量值發生變化時，程序會停下來。以上面程序為例:  

	(gdb) start
	Temporary breakpoint 1 at 0x4005a8: file a.c, line 19.
	Starting program: /data2/home/nanxiao/a
	[Thread debugging using libthread_db enabled]
	Using host libthread_db library "/lib64/libthread_db.so.1".
	
	Temporary breakpoint 1, main () at a.c:19
	19              pthread_create(&t1, NULL, thread1_func, "Thread 1");
	(gdb) watch a
	Hardware watchpoint 2: a
	(gdb) r
	Starting program: /data2/home/nanxiao/a
	[Thread debugging using libthread_db enabled]
	Using host libthread_db library "/lib64/libthread_db.so.1".
	[New Thread 0x7ffff782c700 (LWP 8813)]
	[Switching to Thread 0x7ffff782c700 (LWP 8813)]
	Hardware watchpoint 2: a
	
	Old value = 0
	New value = 1
	thread1_func (p_arg=0x4006d8) at a.c:11
	11                      sleep(10);
	(gdb) c
	Continuing.
	Hardware watchpoint 2: a
	
	Old value = 1
	New value = 2
	thread1_func (p_arg=0x4006d8) at a.c:11
	11                      sleep(10);

可以看到，使用“`watch a`”命令以後，當`a`的值變化：由`0`變成`1`，由`1`變成`2`，程序都會停下來。  
此外也可以使用“`watch *(data type*)address`”這樣的命令，仍以上面程序為例:  

	(gdb) p &a
	$1 = (int *) 0x6009c8 <a>
	(gdb) watch *(int*)0x6009c8
	Hardware watchpoint 2: *(int*)0x6009c8
	(gdb) r
	Starting program: /data2/home/nanxiao/a
	[Thread debugging using libthread_db enabled]
	Using host libthread_db library "/lib64/libthread_db.so.1".
	[New Thread 0x7ffff782c700 (LWP 15431)]
	[Switching to Thread 0x7ffff782c700 (LWP 15431)]
	Hardware watchpoint 2: *(int*)0x6009c8
	
	Old value = 0
	New value = 1
	thread1_func (p_arg=0x4006d8) at a.c:11
	11                      sleep(10);
	(gdb) c
	Continuing.
	Hardware watchpoint 2: *(int*)0x6009c8
	
	Old value = 1
	New value = 2
	thread1_func (p_arg=0x4006d8) at a.c:11
	11                      sleep(10);

先得到`a`的地址：`0x6009c8`，接著用“`watch *(int*)0x6009c8`”設置觀察點，可以看到同“`watch a`”命令效果一樣。  
觀察點可以通過軟件或硬件的方式實現，取決於具體的系統。但是軟件實現的觀察點會導致程序運行很慢，使用時需注意。參見[gdb手冊](https://sourceware.org/gdb/onlinedocs/gdb/Set-Watchpoints.html).

如果系統支持硬件觀測的話，當設置觀測點是會打印如下信息：
	Hardware watchpoint num: expr
	
 如果不想用硬件觀測點的話可如下設置：
    set can-use-hw-watchpoints
    
## 查看斷點
列出當前所設置了的所有觀察點：    
info watchpoints    

watch 所設置的斷點也可以用控制斷點的命令來控制。如 disable、enable、delete等

## 貢獻者

nanxiao
