# 跳轉到指定位置執行 

## 例子

	#include <stdio.h>
	
	void fun (int x)
	{
	  if (x < 0)
	    puts ("error");
	}
	
	int main (void)
	{
	  int i = 1;
	
	  fun (i--);
	  fun (i--);
	  fun (i--);
	
	  return 0;
	}

## 技巧

當調試程序時，你可能不小心走過了出錯的地方：

	(gdb) n
	13	  fun (i--);
	(gdb) 
	14	  fun (i--);
	(gdb) 
	15	  fun (i--);
	(gdb) 
	error
	17	  return 0;

看起來是在15行，調用fun的時候出錯了。常見的辦法是在15行設置個斷點，然後從頭`run`一次。

如果你的環境支持反向執行，那麼更好了。

如果不支持，你也可以直接`jump`到15行，再執行一次：

	(gdb) b 15
	Breakpoint 2 at 0x40056a: file jump.c, line 15.
	(gdb) j 15
	Continuing at 0x40056a.
	
	Breakpoint 2, main () at jump.c:15
	15	  fun (i--);
	(gdb) s
	fun (x=-2) at jump.c:5
	5	  if (x < 0)
	(gdb) n
	6	    puts ("error");

需要注意的是：

1. `jump`命令只改變pc的值，所以改變程序執行可能會出現不同的結果，比如變量i的值
2. 通過（臨時）斷點的配合，可以讓你的程序跳到指定的位置，並停下來

詳情參見[gdb手冊](https://sourceware.org/gdb/onlinedocs/gdb/Jumping.html#Jumping)

## 貢獻者

xmj

