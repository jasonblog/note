# 查看線程信息
## 例子
	#include <stdio.h>
	#include <pthread.h>
	void *thread_func(void *p_arg)
	{
	        while (1)
	        {
	                printf("%s\n", (char*)p_arg);
	                sleep(10);
	        }
	}
	int main(void)
	{
	        pthread_t t1, t2;
	
	        pthread_create(&t1, NULL, thread_func, "Thread 1");
	        pthread_create(&t2, NULL, thread_func, "Thread 2");
	
	        sleep(1000);
	        return;
	}

## 技巧
用gdb調試多線程程序，可以用“i threads”命令（i是info命令縮寫）查看所有線程的信息，以上面程序為例（運行平臺為Linux，CPU為X86_64）:  

	  (gdb) i threads
	  Id   Target Id         Frame
	  3    Thread 0x7ffff6e2b700 (LWP 31773) 0x00007ffff7915911 in clone () from /lib64/libc.so.6
	  2    Thread 0x7ffff782c700 (LWP 31744) 0x00007ffff78d9bcd in nanosleep () from /lib64/libc.so.6
	* 1    Thread 0x7ffff7fe9700 (LWP 31738) main () at a.c:18

第一項（Id）：是gdb標示每個線程的唯一ID：1，2等等。  
第二項（Target Id）：是具體系統平臺用來標示每個線程的ID，不同平臺信息可能會不同。 像當前Linux平臺顯示的就是： Thread 0x7ffff6e2b700 (LWP 31773)。  
第三項（Frame）：顯示的是線程執行到哪個函數。  
前面帶“*”表示的是“current thread”，可以理解為gdb調試多線程程序時，選擇的一個“默認線程”。

再以Solaris平臺（CPU為X86_64）為例，可以看到顯示信息會略有不同：

    (gdb) i threads
	[New Thread 2 (LWP 2)]
	[New Thread 3 (LWP 3)]
	  Id   Target Id         Frame
	  6    Thread 3 (LWP 3)  0xfeec870d in _thr_setup () from /usr/lib/libc.so.1
	  5    Thread 2 (LWP 2)  0xfefc9661 in elf_find_sym () from /usr/lib/ld.so.1
	  4    LWP    3          0xfeec870d in _thr_setup () from /usr/lib/libc.so.1
	  3    LWP    2          0xfefc9661 in elf_find_sym () from /usr/lib/ld.so.1
	* 2    Thread 1 (LWP 1)  main () at a.c:18
	  1    LWP    1          main () at a.c:18


也可以用“i threads [Id...]”指定打印某些線程的信息，例如：

	  (gdb) i threads 1 2
	  Id   Target Id         Frame
	  2    Thread 0x7ffff782c700 (LWP 12248) 0x00007ffff78d9bcd in nanosleep () from /lib64/libc.so.6
	* 1    Thread 0x7ffff7fe9700 (LWP 12244) main () at a.c:18


參見[gdb手冊](https://sourceware.org/gdb/onlinedocs/gdb/Threads.html).

## 貢獻者

nanxiao
