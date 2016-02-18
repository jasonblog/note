# 使用“$_exitcode”變量
## 例子
	int main(void)
	{
	    return 0;
	}


## 技巧
當被調試的程序正常退出時，gdb會使用`$_exitcode`這個“`convenience variable`”記錄程序退出時的“`exit code`”。以調試上面程序為例:
	
	[root@localhost nan]# gdb -q a
	Reading symbols from a...done.
	(gdb) start
	Temporary breakpoint 1 at 0x400478: file a.c, line 3.
	Starting program: /home/nan/a
	
	Temporary breakpoint 1, main () at a.c:3
	3               return 0;
	(gdb) n
	4       }
	(gdb)
	0x00000034e421ed1d in __libc_start_main () from /lib64/libc.so.6
	(gdb)
	Single stepping until exit from function __libc_start_main,
	which has no line number information.
	[Inferior 1 (process 1185) exited normally]
	(gdb) p $_exitcode
	$1 = 0

可以看到打印的`$_exitcode`的值為`0`。  
改變程序，返回值改為`1`：

	int main(void)
	{
	    return 0;
	}
接著調試：  

	[root@localhost nan]# gdb -q a
	Reading symbols from a...done.
	(gdb) start
	Temporary breakpoint 1 at 0x400478: file a.c, line 3.
	Starting program: /home/nan/a
	
	Temporary breakpoint 1, main () at a.c:3
	3               return 1;
	(gdb)
	(gdb) n
	4       }
	(gdb)
	0x00000034e421ed1d in __libc_start_main () from /lib64/libc.so.6
	(gdb)
	Single stepping until exit from function __libc_start_main,
	which has no line number information.
	[Inferior 1 (process 2603) exited with code 01]
	(gdb) p $_exitcode
	$1 = 1

可以看到打印的`$_exitcode`的值變為`1`。  
參見[gdb手冊](https://sourceware.org/gdb/onlinedocs/gdb/Convenience-Vars.html).

## 貢獻者

nanxiao
