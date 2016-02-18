# 直接執行函數
## 例子
	#include <stdio.h>

	int global = 1;
	
	int func(void) 
	{
		return (++global);
	}
	
	int main(void)
	{
		printf("%d\n", global);
		return 0;
	}



## 技巧
使用gdb調試程序時，可以使用“`call`”或“`print`”命令直接調用函數執行。以上面程序為例：  
 
	(gdb) start
	Temporary breakpoint 1 at 0x4004e3: file a.c, line 12.
	Starting program: /data2/home/nanxiao/a
	
	Temporary breakpoint 1, main () at a.c:12
	12              printf("%d\n", global);
	(gdb) call func()
	$1 = 2
	(gdb) print func()
	$2 = 3
	(gdb) n
	3
	13              return 0;

可以看到執行兩次`func`函數後，`global`的值變成`3`。  
參見[gdb手冊](https://sourceware.org/gdb/onlinedocs/gdb/Calling.html).

## 貢獻者

nanxiao
