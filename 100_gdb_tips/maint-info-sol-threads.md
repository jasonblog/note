# 在Solaris上使用maintenance命令查看線程信息




## 技巧
用gdb調試多線程程序時，如果想查看線程信息，可以使用“i threads”命令（i是info命令縮寫），例如:  

	(gdb) i threads
    106 process 2689429      0xff04af84 in __lwp_park () from /lib/libc.so.1
    105 process 2623893      0xff04af84 in __lwp_park () from /lib/libc.so.1
    104 process 2558357      0xff04af84 in __lwp_park () from /lib/libc.so.1
    103 process 2492821      0xff04af84 in __lwp_park () from /lib/libc.so.1



在Solaris操作系統上，gdb為Solaris量身定做了一個查看線程信息的命令：“maint info sol-threads”（maint是maintenance命令縮寫），例如:

	(gdb) maint info sol-threads
	user   thread #1, lwp 1, (active)
	user   thread #2, lwp 2, (active)    startfunc: monitor_thread
	user   thread #3, lwp 3, (asleep)    startfunc: mem_db_thread
    - Sleep func: 0x000aa32c


可以看到相比於info命令，maintenance命令顯示了更多信息。例如線程當前狀態（active，asleep），入口函數（startfunc）等。

參見[gdb手冊](https://sourceware.org/gdb/onlinedocs/gdb/Threads.html)

## 貢獻者

nanxiao

