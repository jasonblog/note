# 同時調試父進程和子進程 

## 例子

	#include <stdio.h>
	#include <stdlib.h>
	
	int main(void) {
	    pid_t pid;
	
	    pid = fork();
	    if (pid < 0)
	    {
	        exit(1);
	    }
	    else if (pid > 0)
	    {
	        printf("Parent\n");
	        exit(0);
	    }
	    printf("Child\n");
	    return 0;
	}



## 技巧

在調試多進程程序時，gdb默認只會追蹤父進程的運行，而子進程會獨立運行，gdb不會控制。以上面程序為例：

	(gdb) start
	Temporary breakpoint 1 at 0x40055c: file a.c, line 7.
	Starting program: /data2/home/nanxiao/a
	
	Temporary breakpoint 1, main () at a.c:7
	7           pid = fork();
	(gdb) n
	8           if (pid < 0)
	(gdb) Child
	
	12          else if (pid > 0)
	(gdb)
	14              printf("Parent\n");
	(gdb)
	Parent
	15              exit(0);

可以看到當單步執行到第8行時，程序打印出“Child” ，證明子進程已經開始獨立運行。

如果要同時調試父進程和子進程，可以使用“`set detach-on-fork off`”（默認`detach-on-fork`是`on`）命令，這樣gdb就能同時調試父子進程，並且在調試一個進程時，另外一個進程處於掛起狀態。仍以上面程序為例：

	(gdb) set detach-on-fork off
	(gdb) start
	Temporary breakpoint 1 at 0x40055c: file a.c, line 7.
	Starting program: /data2/home/nanxiao/a
	
	Temporary breakpoint 1, main () at a.c:7
	7           pid = fork();
	(gdb) n
	[New process 1050]
	8           if (pid < 0)
	(gdb)
	12          else if (pid > 0)
	(gdb) i inferior
	  Num  Description       Executable
	  2    process 1050      /data2/home/nanxiao/a
	* 1    process 1046      /data2/home/nanxiao/a
	(gdb) n
	14              printf("Parent\n");
	(gdb) n
	Parent
	15              exit(0);
	(gdb)
	[Inferior 1 (process 1046) exited normally]
	(gdb)
	The program is not being run.
	(gdb) i inferiors
	  Num  Description       Executable
	  2    process 1050      /data2/home/nanxiao/a
	* 1    <null>            /data2/home/nanxiao/a
	(gdb) inferior 2
	[Switching to inferior 2 [process 1050] (/data2/home/nanxiao/a)]
	[Switching to thread 2 (process 1050)]
	#0  0x00007ffff7af6cad in fork () from /lib64/libc.so.6
	(gdb) bt
	#0  0x00007ffff7af6cad in fork () from /lib64/libc.so.6
	#1  0x0000000000400561 in main () at a.c:7
	(gdb) n
	Single stepping until exit from function fork,
	which has no line number information.
	main () at a.c:8
	8           if (pid < 0)
	(gdb)
	12          else if (pid > 0)
	(gdb)
	17          printf("Child\n");
	(gdb)
	Child
	18          return 0;
	(gdb)



在使用“`set detach-on-fork off`”命令後，用“`i inferiors`”（`i`是`info`命令縮寫）查看進程狀態，可以看到父子進程都在被gdb調試的狀態，前面顯示“*”是正在調試的進程。當父進程退出後，用“`inferior infno`”切換到子進程去調試。

這個命令目前Linux支持，其它很多操作系統都不支持，使用時請注意。參見[gdb手冊](https://sourceware.org/gdb/onlinedocs/gdb/Forks.html)  

此外，如果想讓父子進程都同時運行，可以使用“`set schedule-multiple on`”（默認`schedule-multiple`是`off`）命令，仍以上述代碼為例：  

	(gdb) set detach-on-fork off
	(gdb) set schedule-multiple on
	(gdb) start
	Temporary breakpoint 1 at 0x40059c: file a.c, line 7.
	Starting program: /data2/home/nanxiao/a
	
	Temporary breakpoint 1, main () at a.c:7
	7           pid = fork();
	(gdb) n
	[New process 26597]
	Child
可以看到打印出了“Child”，證明子進程也在運行了。  
參見[gdb手冊](https://sourceware.org/gdb/onlinedocs/gdb/All_002dStop-Mode.html#All_002dStop-Mode)
## 貢獻者

nanxiao



