# 查看信號處理信息
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
	        signal(SIGINT, handler);
	        signal(SIGALRM, handler);
	        
	        while (1)
	        {
	                sleep(1);
	        }
	        return 0;
	}

## 技巧
用gdb調試程序時，可以用“`i signals`”命令（或者“`i handle`”命令，`i`是`info`命令縮寫）查看gdb如何處理進程收到的信號:  

	(gdb) i signals 
	Signal        Stop      Print   Pass to program Description
	
	SIGHUP        Yes       Yes     Yes             Hangup
	SIGINT        Yes       Yes     No              Interrupt
	SIGQUIT       Yes       Yes     Yes             Quit
	......
	SIGALRM       No        No      Yes             Alarm clock
	......

第一項（`Signal`）：標示每個信號。  
第二項（`Stop`）：表示被調試的程序有對應的信號發生時，gdb是否會暫停程序。  
第三項（`Print`）：表示被調試的程序有對應的信號發生時，gdb是否會打印相關信息。  
第四項（`Pass to program`）：gdb是否會把這個信號發給被調試的程序。  
第五項（`Description`）：信號的描述信息。

從上面的輸出可以看到，當`SIGINT`信號發生時，gdb會暫停被調試的程序，並打印相關信息，但不會把這個信號發給被調試的程序。而當`SIGALRM`信號發生時，gdb不會暫停被調試的程序，也不打印相關信息，但會把這個信號發給被調試的程序。  

啟動gdb調試上面的程序，同時另起一個終端，先後發送`SIGINT`和`SIGALRM`信號給被調試的進程，輸出如下：  

	Program received signal SIGINT, Interrupt.
	0xfeeeae55 in ___nanosleep () from /lib/libc.so.1
	(gdb) c
	Continuing.
	Receive signal: 14

可以看到收到`SIGINT`時，程序暫停了，也輸出了信號信息，但並沒有把`SIGINT`信號交由進程處理（程序沒有輸出）。而收到`SIGALRM`信號時，程序沒有暫停，也沒有輸出信號信息，但把`SIGALRM`信號交由進程處理了（程序打印了輸出）。


參見[gdb手冊](https://sourceware.org/gdb/onlinedocs/gdb/Signals.html).

## 貢獻者

nanxiao
