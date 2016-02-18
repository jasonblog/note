# 信號發生時是否把信號丟給程序處理
## 例子
	#include <stdio.h>
	#include <signal.h>
	
	void handler(int sig);
	
	void handler(int sig)
	{
	        signal(sig, handler);
	        printf("Receive signal: %d\n", sig);
	}
	
	int main(void) {
	        signal(SIGHUP, handler);
	        
	        while (1)
	        {
	                sleep(1);
	        }
	        return 0;
	}

## 技巧
用gdb調試程序時，可以用“`handle signal pass(noignore)/nopass(ignore)`”命令設置當信號發生時，是否把信號丟給程序處理.其中`pass`和`noignore`含義相同，`nopass`和`ignore`含義相同。以上面程序為例:  

	(gdb) i signals 
	Signal        Stop      Print   Pass to program Description
	
	SIGHUP        Yes       Yes     Yes             Hangup
	......

	(gdb) r
	Starting program: /data1/nan/test 
	[Thread debugging using libthread_db enabled]
	[New Thread 1 (LWP 1)]
	
	Program received signal SIGHUP, Hangup.
	[Switching to Thread 1 (LWP 1)]
	0xfeeeae55 in ___nanosleep () from /lib/libc.so.1
	(gdb) c
	Continuing.
	Receive signal: 1

可以看到，默認情況下，發生`SIGHUP`信號時，gdb會把信號丟給程序處理。

接下來用“`handle SIGHUP nopass`”命令設置當`SIGHUP`信號發生時，gdb不把信號丟給程序處理，執行如下：

	(gdb) handle SIGHUP nopass
	Signal        Stop      Print   Pass to program Description
	SIGHUP        Yes       Yes     No              Hangup
	(gdb) c
	Continuing.
	
	Program received signal SIGHUP, Hangup.
	0xfeeeae55 in ___nanosleep () from /lib/libc.so.1
	(gdb) c
	Continuing.
可以看到，`SIGHUP`信號發生時，程序沒有打印“Receive signal: 1”，說明gdb沒有把信號丟給程序處理。

如果想恢復之前的行為，用“`handle SIGHUP pass`”命令即可。

參見[gdb手冊](https://sourceware.org/gdb/onlinedocs/gdb/Signals.html).

## 貢獻者

nanxiao
