# 信號發生時是否打印信號信息
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
用gdb調試程序時，可以用“`handle signal print/noprint`”命令設置當信號發生時，是否打印信號信息，以上面程序為例:  

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

可以看到，默認情況下，發生`SIGHUP`信號時，gdb會暫停程序的執行，並打印收到信號的信息。此時需要執行`continue`命令繼續程序的執行。

接下來用“`handle SIGHUP noprint`”命令設置當`SIGHUP`信號發生時，gdb不打印信號信息，執行如下：

	(gdb) handle SIGHUP noprint 
	Signal        Stop      Print   Pass to program Description
	SIGHUP        No        No      Yes             Hangup
	(gdb) r
	Starting program: /data1/nan/test 
	[Thread debugging using libthread_db enabled]
	Receive signal: 1

需要注意的是，設置`noprint`的同時，默認也會設置`nostop`。可以看到，程序收到`SIGHUP`信號發生時，沒有暫停，也沒有打印信號信息。而是繼續執行。

如果想恢復之前的行為，用“`handle SIGHUP print`”命令即可。

參見[gdb手冊](https://sourceware.org/gdb/onlinedocs/gdb/Signals.html).

## 貢獻者

nanxiao
