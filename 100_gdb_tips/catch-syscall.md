# 為系統調用設置catchpoint
## 例子
	#include <stdio.h>

	int main(void)
	{
	    char p1[] = "Sam";
	    char *p2 = "Bob";
	
	    printf("p1 is %s, p2 is %s\n", p1, p2);
	    return 0;
	}



## 技巧
使用gdb調試程序時，可以使用`catch syscall [name | number]`為關注的系統調用設置`catchpoint`，以上面程序為例：  

	(gdb) catch syscall mmap
	Catchpoint 1 (syscall 'mmap' [9])
	(gdb) r
	Starting program: /home/nan/a
	
	Catchpoint 1 (call to syscall mmap), 0x00000034e3a16f7a in mmap64 ()
	   from /lib64/ld-linux-x86-64.so.2
	(gdb) c
	Continuing.
	
	Catchpoint 1 (returned from syscall mmap), 0x00000034e3a16f7a in mmap64 ()
	   from /lib64/ld-linux-x86-64.so.2


可以看到當`mmap`調用發生後，gdb會暫停程序的運行。  
也可以使用系統調用的編號設置`catchpoint`，仍以上面程序為例：  

	(gdb) catch syscall 9
	Catchpoint 1 (syscall 'mmap' [9])
	(gdb) r
	Starting program: /home/nan/a
	
	Catchpoint 1 (call to syscall mmap), 0x00000034e3a16f7a in mmap64 ()
	   from /lib64/ld-linux-x86-64.so.2
	(gdb) c
	Continuing.
	
	Catchpoint 1 (returned from syscall mmap), 0x00000034e3a16f7a in mmap64 ()
	   from /lib64/ld-linux-x86-64.so.2
	(gdb) c
	Continuing.
	
	Catchpoint 1 (call to syscall mmap), 0x00000034e3a16f7a in mmap64 ()
	   from /lib64/ld-linux-x86-64.so.2
可以看到和使用`catch syscall mmap`效果是一樣的。（系統調用和編號的映射參考具體的`xml`文件，以我的系統為例，就是在`/usr/local/share/gdb/syscalls`文件夾下的`amd64-linux.xml`。）

如果不指定具體的系統調用，則會為所有的系統調用設置`catchpoint`，仍以上面程序為例：  

	(gdb) catch syscall
	Catchpoint 1 (any syscall)
	(gdb) r
	Starting program: /home/nan/a
	
	Catchpoint 1 (call to syscall brk), 0x00000034e3a1618a in brk ()
	   from /lib64/ld-linux-x86-64.so.2
	(gdb) c
	Continuing.
	
	Catchpoint 1 (returned from syscall brk), 0x00000034e3a1618a in brk ()
	   from /lib64/ld-linux-x86-64.so.2
	(gdb)
	Continuing.
	
	Catchpoint 1 (call to syscall mmap), 0x00000034e3a16f7a in mmap64 ()
	   from /lib64/ld-linux-x86-64.so.2



參見[gdb手冊](https://sourceware.org/gdb/onlinedocs/gdb/Set-Catchpoints.html).

## 貢獻者

nanxiao
