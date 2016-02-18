# 使用“$_thread”變量
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
gdb從7.2版本引入了`$_thread`這個“`convenience variable`”，用來保存當前正在調試的線程號。這個變量在寫斷點命令或是命令腳本時會很有用。以上面程序為例:
	
	(gdb) wa a
	Hardware watchpoint 2: a
	(gdb) command 2
	Type commands for breakpoint(s) 2, one per line.
	End with a line saying just "end".
	>printf "thread id=%d\n", $_thread
	>end

首先設置了觀察點：“wa a”（`wa`是`watch`命令縮寫），也就是當`a`的值發生變化時，程序會暫停，接下來在`commands`語句中打印線程號。  
然後繼續執行程序：

	(gdb) c
	Continuing.
	[New Thread 0x7ffff782c700 (LWP 20928)]
	[Switching to Thread 0x7ffff782c700 (LWP 20928)]
	Hardware watchpoint 2: a
	
	Old value = 0
	New value = 1
	thread1_func (p_arg=0x400718) at a.c:11
	11                      sleep(10);
	thread id=2
	(gdb) c
	Continuing.
	[New Thread 0x7ffff6e2b700 (LWP 20929)]
	[Switching to Thread 0x7ffff6e2b700 (LWP 20929)]
	Hardware watchpoint 2: a
	
	Old value = 1
	New value = 2
	thread2_func (p_arg=0x400721) at a.c:20
	20                      sleep(10);
	thread id=3

可以看到程序暫停時，會打印線程號：“`thread id=2`”或者“`thread id=3`”。  
參見[gdb手冊](https://sourceware.org/gdb/onlinedocs/gdb/Threads.html).

## 貢獻者

nanxiao
