# 調試已經運行的進程

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

調試已經運行的進程有兩種方法：一種是gdb啟動時，指定進程的ID：gdb program processID（也可以用-p或者--pid指定進程ID，例如：gdb program -p=10210）。以上面代碼為例，用“ps”命令已經獲得進程ID為10210：

	bash-3.2# gdb -q a 10210
	Reading symbols from /data/nan/a...done.
	Attaching to program `/data/nan/a', process 10210
	[New process 10210]
	Retry #1:
	Retry #2:
	Retry #3:
	Retry #4:
	Reading symbols from /usr/lib/libc.so.1...(no debugging symbols found)...done.
	[Thread debugging using libthread_db enabled]
	[New LWP    3        ]
	[New LWP    2        ]
	[New Thread 1 (LWP 1)]
	[New Thread 2 (LWP 2)]
	[New Thread 3 (LWP 3)]
	Loaded symbols for /usr/lib/libc.so.1
	Reading symbols from /lib/ld.so.1...(no debugging symbols found)...done.
	Loaded symbols for /lib/ld.so.1
	[Switching to Thread 1 (LWP 1)]
	0xfeeeae55 in ___nanosleep () from /usr/lib/libc.so.1
	(gdb) bt
	#0  0xfeeeae55 in ___nanosleep () from /usr/lib/libc.so.1
	#1  0xfeedcae4 in sleep () from /usr/lib/libc.so.1
	#2  0x080509ef in main () at a.c:17

如果嫌每次ps查看進程號比較麻煩，請嘗試如下腳本

```shell
# 保存為xgdb.sh（添加可執行權限）
# 用法 xgdb.sh a 
prog_bin=$1
running_name=$(basename $prog_bin)
pid=$(/sbin/pidof $running_name)
gdb attach $pid
```

	
另一種是先啟動gdb，然後用“attach”命令“附著”在進程上：

	bash-3.2# gdb -q a
	Reading symbols from /data/nan/a...done.
	(gdb) attach 10210
	Attaching to program `/data/nan/a', process 10210
	[New process 10210]
	Retry #1:
	Retry #2:
	Retry #3:
	Retry #4:
	Reading symbols from /usr/lib/libc.so.1...(no debugging symbols found)...done.
	[Thread debugging using libthread_db enabled]
	[New LWP    3        ]
	[New LWP    2        ]
	[New Thread 1 (LWP 1)]
	[New Thread 2 (LWP 2)]
	[New Thread 3 (LWP 3)]
	Loaded symbols for /usr/lib/libc.so.1
	Reading symbols from /lib/ld.so.1...(no debugging symbols found)...done.
	Loaded symbols for /lib/ld.so.1
	[Switching to Thread 1 (LWP 1)]
	0xfeeeae55 in ___nanosleep () from /usr/lib/libc.so.1
	(gdb) bt
	#0  0xfeeeae55 in ___nanosleep () from /usr/lib/libc.so.1
	#1  0xfeedcae4 in sleep () from /usr/lib/libc.so.1
	#2  0x080509ef in main () at a.c:17



如果不想繼續調試了，可以用“detach”命令“脫離”進程：

	(gdb) detach
	Detaching from program: /data/nan/a, process 10210
	(gdb) bt
	No stack.


詳情參見[gdb手冊](https://sourceware.org/gdb/current/onlinedocs/gdb/Attach.html#index-attach)

## 貢獻者

nanxiao



