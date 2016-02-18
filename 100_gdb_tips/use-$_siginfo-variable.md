# 使用“$_siginfo”變量
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
在某些平臺上（比如Linux）使用gdb調試程序，當有信號發生時，gdb在把信號丟給程序之前，可以通過`$_siginfo`變量讀取一些額外的有關當前信號的信息，這些信息是`kernel`傳給信號處理函數的。以上面程序為例:  

	Program received signal SIGHUP, Hangup.
	0x00000034e42accc0 in __nanosleep_nocancel () from /lib64/libc.so.6
	Missing separate debuginfos, use: debuginfo-install glibc-2.12-1.132.el6.x86_64
	(gdb) ptype $_siginfo
	type = struct {
	    int si_signo;
	    int si_errno;
	    int si_code;
	    union {
	        int _pad[28];
	        struct {...} _kill;
	        struct {...} _timer;
	        struct {...} _rt;
	        struct {...} _sigchld;
	        struct {...} _sigfault;
	        struct {...} _sigpoll;
	    } _sifields;
	}
	(gdb) ptype $_siginfo._sifields._sigfault
	type = struct {
	    void *si_addr;
	}
	(gdb) p $_siginfo._sifields._sigfault.si_addr
	$1 = (void *) 0x850e

我們可以瞭解`$_siginfo`變量裡每個成員的類型，並且可以讀到成員的值。


參見[gdb手冊](https://sourceware.org/gdb/onlinedocs/gdb/Signaling.html#Signaling).

## 貢獻者

nanxiao
