# 設置讀觀察點
## 例子
	#include <stdio.h>
	#include <pthread.h>
	
	int a = 0;
	
	void *thread1_func(void *p_arg)
	{
	        while (1)
	        {
	                printf("%d\n", a);
	                sleep(10);
	        }
	}
	
	int main(void)
	{
	        pthread_t t1;
	
	        pthread_create(&t1, NULL, thread1_func, "Thread 1");
	
	        sleep(1000);
	        return;
	}


## 技巧
gdb可以使用“`rwatch`”命令設置讀觀察點，也就是當發生讀取變量行為時，程序就會暫停住。以上面程序為例:  

	(gdb) start
	Temporary breakpoint 1 at 0x4005f3: file a.c, line 19.
	Starting program: /data2/home/nanxiao/a
	[Thread debugging using libthread_db enabled]
	Using host libthread_db library "/lib64/libthread_db.so.1".
	
	Temporary breakpoint 1, main () at a.c:19
	19              pthread_create(&t1, NULL, thread1_func, "Thread 1");
	(gdb) rw a
	Hardware read watchpoint 2: a
	(gdb) c
	Continuing.
	[New Thread 0x7ffff782c700 (LWP 5540)]
	[Switching to Thread 0x7ffff782c700 (LWP 5540)]
	Hardware read watchpoint 2: a
	
	Value = 0
	0x00000000004005c6 in thread1_func (p_arg=0x40071c) at a.c:10
	10                      printf("%d\n", a);
	(gdb) c
	Continuing.
	0
	Hardware read watchpoint 2: a
	
	Value = 0
	0x00000000004005c6 in thread1_func (p_arg=0x40071c) at a.c:10
	10                      printf("%d\n", a);
	(gdb) c
	Continuing.
	0
	Hardware read watchpoint 2: a
	
	Value = 0
	0x00000000004005c6 in thread1_func (p_arg=0x40071c) at a.c:10
	10                      printf("%d\n", a);



可以看到，使用“`rw a`”命令（`rw`是`rwatch`命令的縮寫）以後，每次訪問`a`的值都會讓程序停下來。  
需要注意的是`rwatch`命令只對硬件觀察點才生效，參見[gdb手冊](https://sourceware.org/gdb/onlinedocs/gdb/Set-Watchpoints.html).

## 貢獻者

nanxiao
