# 給程序發送信號
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
用gdb調試程序的過程中，當被調試程序停止後，可以用“`signal signal_name`”命令讓程序繼續運行，但會立即給程序發送信號。以上面程序為例:  

	(gdb) r
	`/data1/nan/test' has changed; re-reading symbols.
	Starting program: /data1/nan/test 
	[Thread debugging using libthread_db enabled]
	^C[New Thread 1 (LWP 1)]
	
	Program received signal SIGINT, Interrupt.
	[Switching to Thread 1 (LWP 1)]
	0xfeeeae55 in ___nanosleep () from /lib/libc.so.1
	(gdb) signal SIGHUP
	Continuing with signal SIGHUP.
	Receive signal: 1

可以看到，當程序暫停後，執行`signal SIGHUP`命令，gdb會發送信號給程序處理。

可以使用“`signal 0`”命令使程序重新運行，但不發送任何信號給進程。仍以上面程序為例：

	Program received signal SIGHUP, Hangup.
	0xfeeeae55 in ___nanosleep () from /lib/libc.so.1
	(gdb) signal 0
	Continuing with no signal.

可以看到，`SIGHUP`信號發生時，gdb停住了程序，但是由於執行了“`signal 0`”命令，所以程序重新運行後，並沒有收到`SIGHUP`信號。

使用`signal`命令和在shell環境使用`kill`命令給程序發送信號的區別在於：在shell環境使用`kill`命令給程序發送信號，gdb會根據當前的設置決定是否把信號發送給進程，而使用`signal`命令則直接把信號發給進程。

參見[gdb手冊](https://sourceware.org/gdb/onlinedocs/gdb/Signaling.html#Signaling).

## 貢獻者

nanxiao
