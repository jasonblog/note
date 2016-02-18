# 列出函數的名字

## 例子

	#include <stdio.h>
	#include <pthread.h>
	void *thread_func(void *p_arg)
	{
	        while (1)
	        {
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

使用gdb調試時，使用“`info functions`”命令可以列出可執行文件的所有函數名稱。以上面代碼為例：

	(gdb) info functions
	All defined functions:
	
	File a.c:
	int main(void);
	void *thread_func(void *);
	
	Non-debugging symbols:
	0x0805079c  _PROCEDURE_LINKAGE_TABLE_
	0x080507ac  _cleanup@plt
	0x080507bc  atexit
	0x080507bc  atexit@plt
	0x080507cc  __fpstart
	0x080507cc  __fpstart@plt
	0x080507dc  exit@plt
	0x080507ec  __deregister_frame_info_bases@plt
	0x080507fc  __register_frame_info_bases@plt
	0x0805080c  _Jv_RegisterClasses@plt
	0x0805081c  sleep
	0x0805081c  sleep@plt
	0x0805082c  pthread_create@plt
	0x0805083c  _start
	0x080508b4  _mcount
	0x080508b8  __do_global_dtors_aux
	0x08050914  frame_dummy
	0x080509f4  __do_global_ctors_aux
	0x08050a24  _init
	0x08050a31  _fini

	
可以看到會列出函數原型以及不帶調試信息的函數。

另外這個命令也支持正則表達式：“`info functions regex`”，這樣只會列出符合正則表達式的函數名稱，例如：

	(gdb) info functions thre*
	All functions matching regular expression "thre*":
	
	File a.c:
	void *thread_func(void *);
	
	Non-debugging symbols:
	0x0805082c  pthread_create@plt




可以看到gdb只會列出名字裡包含“`thre`”的函數。

詳情參見[gdb手冊](https://sourceware.org/gdb/onlinedocs/gdb/Symbols.html)

## 貢獻者

nanxiao



