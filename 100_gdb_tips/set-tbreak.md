# 設置臨時斷點 

## 例子

	#include <stdio.h>
	#include <pthread.h>
	
	typedef struct
	{
	        int a;
	        int b;
	        int c;
	        int d;
	        pthread_mutex_t mutex;
	}ex_st;
	
	int main(void) {
	        ex_st st = {1, 2, 3, 4, PTHREAD_MUTEX_INITIALIZER};
	        printf("%d,%d,%d,%d\n", st.a, st.b, st.c, st.d);
	        return 0;
	}



## 技巧

在使用gdb時，如果想讓斷點只生效一次，可以使用“tbreak”命令（縮寫為：tb）。以上面程序為例：

	(gdb) tb a.c:15
	Temporary breakpoint 1 at 0x400500: file a.c, line 15.
	(gdb) i b
	Num     Type           Disp Enb Address            What
	1       breakpoint     del  y   0x0000000000400500 in main at a.c:15
	(gdb) r
	Starting program: /data2/home/nanxiao/a
	
	Temporary breakpoint 1, main () at a.c:15
	15              printf("%d,%d,%d,%d\n", st.a, st.b, st.c, st.d);
	(gdb) i b
	No breakpoints or watchpoints.

	


首先在文件的第15行設置臨時斷點，當程序斷住後，用“i b”（"info breakpoints"縮寫）命令查看斷點，發現斷點沒有了。也就是斷點命中一次後，就被刪掉了。

詳情參見[gdb手冊](https://sourceware.org/gdb/onlinedocs/gdb/Set-Breaks.html)

## 貢獻者

nanxiao



