# 進入不帶調試信息的函數

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

默認情況下，gdb不會進入不帶調試信息的函數。以上面代碼為例：

	(gdb) n
	15              printf("%d,%d,%d,%d\n", st.a, st.b, st.c, st.d);
	(gdb) s
	1,2,3,4
	16              return 0;

	
可以看到由於printf函數不帶調試信息，所以“s”命令（s是“step”縮寫）無法進入printf函數。

可以執行“set step-mode on”命令，這樣gdb就不會跳過沒有調試信息的函數：

	(gdb) set step-mode on
	(gdb) n
	15              printf("%d,%d,%d,%d\n", st.a, st.b, st.c, st.d);
	(gdb) s
	0x00007ffff7a993b0 in printf () from /lib64/libc.so.6
	(gdb) s
	0x00007ffff7a993b7 in printf () from /lib64/libc.so.6


可以看到gdb進入了printf函數，接下來可以使用調試彙編程序的辦法去調試函數。

詳情參見[gdb手冊](https://sourceware.org/gdb/onlinedocs/gdb/Continuing-and-Stepping.html)

## 貢獻者

nanxiao



