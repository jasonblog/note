# 每行打印一個結構體成員 

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

默認情況下，gdb以一種“緊湊”的方式打印結構體。以上面代碼為例：

	(gdb) n
	15              printf("%d,%d,%d,%d\n", st.a, st.b, st.c, st.d);
	(gdb) p st
	$1 = {a = 1, b = 2, c = 3, d = 4, mutex = {__data = {__lock = 0, __count = 0, __owner = 0, __nusers = 0, __kind = 0,
	      __spins = 0, __list = {__prev = 0x0, __next = 0x0}}, __size = '\000' <repeats 39 times>, __align = 0}}


	


可以看到結構體的顯示很混亂，尤其是結構體裡還嵌套著其它結構體時。

可以執行“set print pretty on”命令，這樣每行只會顯示結構體的一名成員，而且還會根據成員的定義層次進行縮進：

	(gdb) set print pretty on
	(gdb) p st
	$2 = {
	  a = 1,
	  b = 2,
	  c = 3,
	  d = 4,
	  mutex = {
	    __data = {
	      __lock = 0,
	      __count = 0,
	      __owner = 0,
	      __nusers = 0,
	      __kind = 0,
	      __spins = 0,
	      __list = {
	        __prev = 0x0,
	        __next = 0x0
	      }
	    },
	    __size = '\000' <repeats 39 times>,
	    __align = 0
	  }
	}




詳情參見[gdb手冊](https://sourceware.org/gdb/onlinedocs/gdb/Print-Settings.html#index-print-settings)

## 貢獻者

nanxiao



