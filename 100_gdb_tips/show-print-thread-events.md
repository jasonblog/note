# 不顯示線程啟動和退出信息
## 例子
	#include <stdio.h>
	#include <pthread.h>
	
	void *thread_func(void *p_arg)
	{
	       sleep(10);
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
默認情況下，gdb檢測到有線程產生和退出時，會打印提示信息，以上面程序為例:  

	(gdb) r
	Starting program: /data/nan/a
	[Thread debugging using libthread_db enabled]
	[New Thread 1 (LWP 1)]
	[New LWP    2        ]
	[New LWP    3        ]
	[LWP    2         exited]
	[New Thread 2        ]
	[LWP    3         exited]
	[New Thread 3        ]


如果不想顯示這些信息，可以使用“`set print thread-events off`”命令，這樣當有線程產生和退出時，就不會打印提示信息：

    (gdb) set print thread-events off
	(gdb) r
	Starting program: /data/nan/a
	[Thread debugging using libthread_db enabled]



可以看到不再打印相關信息。

這個命令有些平臺不支持，使用時需注意。參見[gdb手冊](https://sourceware.org/gdb/onlinedocs/gdb/Threads.html).

## 貢獻者

nanxiao
