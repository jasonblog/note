# 只允許一個線程運行
## 例子
	#include <stdio.h>
	#include <pthread.h>
	int a = 0;
	int b = 0;
	void *thread1_func(void *p_arg)
	{
	        while (1)
	        {
	                a++;
	                sleep(1);
	        }
	}
	
	void *thread2_func(void *p_arg)
	{
	        while (1)
	        {
	                b++;
	                sleep(1);
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
用gdb調試多線程程序時，一旦程序斷住，所有的線程都處於暫停狀態。此時當你調試其中一個線程時（比如執行“`step`”，“`next`”命令），所有的線程都會同時執行。以上面程序為例:  

	(gdb) b a.c:9
	Breakpoint 1 at 0x400580: file a.c, line 9.
	(gdb) r
	Starting program: /data2/home/nanxiao/a
	[Thread debugging using libthread_db enabled]
	Using host libthread_db library "/lib64/libthread_db.so.1".
	[New Thread 0x7ffff782c700 (LWP 17368)]
	[Switching to Thread 0x7ffff782c700 (LWP 17368)]
	
	Breakpoint 1, thread1_func (p_arg=0x400718) at a.c:9
	9                       a++;
	(gdb) p b
	$1 = 0
	(gdb) s
	10                      sleep(1);
	(gdb) s
	[New Thread 0x7ffff6e2b700 (LWP 17369)]
	11              }
	(gdb)
	
	Breakpoint 1, thread1_func (p_arg=0x400718) at a.c:9
	9                       a++;
	(gdb)
	10                      sleep(1);
	(gdb) p b
	$2 = 3

`thread1_func`更新全局變量`a`的值，`thread2_func`更新全局變量`b`的值。我在`thread1_func`裡`a++`語句打上斷點，當斷點第一次命中時，打印`b`的值是`0`，在單步調試`thread1_func`幾次後，`b`的值變成`3`，證明在單步調試`thread1_func`時，`thread2_func`也在執行。  
如果想在調試一個線程時，讓其它線程暫停執行，可以使用“`set scheduler-locking on`”命令：

    (gdb) b a.c:9
	Breakpoint 1 at 0x400580: file a.c, line 9.
	(gdb) r
	Starting program: /data2/home/nanxiao/a
	[Thread debugging using libthread_db enabled]
	Using host libthread_db library "/lib64/libthread_db.so.1".
	[New Thread 0x7ffff782c700 (LWP 19783)]
	[Switching to Thread 0x7ffff782c700 (LWP 19783)]
	
	Breakpoint 1, thread1_func (p_arg=0x400718) at a.c:9
	9                       a++;
	(gdb) set scheduler-locking on
	(gdb) p b
	$1 = 0
	(gdb) s
	10                      sleep(1);
	(gdb)
	11              }
	(gdb)
	
	Breakpoint 1, thread1_func (p_arg=0x400718) at a.c:9
	9                       a++;
	(gdb)
	10                      sleep(1);
	(gdb)
	11              }
	(gdb) p b
	$2 = 0

可以看到在單步調試`thread1_func`幾次後，`b`的值仍然為`0`，證明在在單步調試`thread1_func`時，`thread2_func`沒有執行。

此外，“`set scheduler-locking`”命令除了支持`off`和`on`模式外（默認是`off`），還有一個`step`模式。含義是：當用"`step`"命令調試線程時，其它線程不會執行，但是用其它命令（比如"`next`"）調試線程時，其它線程也許會執行。

這個命令依賴於具體操作系統的調度策略，使用時需注意。參見[gdb手冊](https://sourceware.org/gdb/onlinedocs/gdb/All_002dStop-Mode.html#All_002dStop-Mode).

## 貢獻者

nanxiao
