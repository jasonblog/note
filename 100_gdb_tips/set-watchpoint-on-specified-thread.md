# 設置觀察點只針對特定線程生效
## 例子
	#include <stdio.h>
	#include <pthread.h>
	
	int a = 0;
	
	void *thread1_func(void *p_arg)
	{
	        while (1)
	        {
	                a++;
	                sleep(10);
	        }
	}
	
	void *thread2_func(void *p_arg)
	{
	        while (1)
	        {
	                a++;
	                sleep(10);
	        }
	}
	
	int main(void)
	{
	        pthread_t t1, t2;
	
	        pthread_create(&t1, NULL, thread1_func, "Thread 1");
			pthread_create(&t2, NULL, thread2_func, "Thread 2");
	
	        sleep(1000);
	        return;
	}

## 技巧
gdb可以使用“`watch expr thread threadnum`”命令設置觀察點只針對特定線程生效，也就是隻有編號為`threadnum`的線程改變了變量的值，程序才會停下來，其它編號線程改變變量的值不會讓程序停住。以上面程序為例:  

	(gdb) start
	Temporary breakpoint 1 at 0x4005d4: file a.c, line 28.
	Starting program: /data2/home/nanxiao/a
	[Thread debugging using libthread_db enabled]
	Using host libthread_db library "/lib64/libthread_db.so.1".
	
	Temporary breakpoint 1, main () at a.c:28
	28              pthread_create(&t1, NULL, thread1_func, "Thread 1");
	(gdb) n
	[New Thread 0x7ffff782c700 (LWP 25443)]
	29              pthread_create(&t2, NULL, thread2_func, "Thread 2");
	(gdb)
	[New Thread 0x7ffff6e2b700 (LWP 25444)]
	31              sleep(1000);
	(gdb) i threads
	  Id   Target Id         Frame
	  3    Thread 0x7ffff6e2b700 (LWP 25444) 0x00007ffff7915911 in clone () from /lib64/libc.so.6
	  2    Thread 0x7ffff782c700 (LWP 25443) 0x00007ffff78d9bcd in nanosleep () from /lib64/libc.so.6
	* 1    Thread 0x7ffff7fe9700 (LWP 25413) main () at a.c:31
	(gdb) wa a thread 2
	Hardware watchpoint 2: a
	(gdb) c
	Continuing.
	[Switching to Thread 0x7ffff782c700 (LWP 25443)]
	Hardware watchpoint 2: a
	
	Old value = 1
	New value = 3
	thread1_func (p_arg=0x400718) at a.c:11
	11                      sleep(10);
	(gdb) c
	Continuing.
	Hardware watchpoint 2: a
	
	Old value = 3
	New value = 5
	thread1_func (p_arg=0x400718) at a.c:11
	11                      sleep(10);
	(gdb) c
	Continuing.
	Hardware watchpoint 2: a
	
	Old value = 5
	New value = 7
	thread1_func (p_arg=0x400718) at a.c:11
	11                      sleep(10);


可以看到，使用“`wa a thread 2`”命令（`wa`是`watch`命令的縮寫）以後，只有`thread1_func`改變`a`的值才會讓程序停下來。  
需要注意的是這種針對特定線程設置觀察點方式只對硬件觀察點才生效，參見[gdb手冊](https://sourceware.org/gdb/onlinedocs/gdb/Set-Watchpoints.html).

## 貢獻者

nanxiao
