# 是否進入帶調試信息的函數

## 例子

	#include <stdio.h>

	int func(void)
	{
		return 3;
	}
	
	int main(void)
	{
		int a = 0;
		
		a = func();
		printf("%d\n", a);
		return 0;
	}



## 技巧

使用gdb調試遇到函數時，使用step命令（縮寫為s）可以進入函數（函數必須有調試信息）。以上面代碼為例：

	(gdb) n
	12              a = func();
	(gdb) s
	func () at a.c:5
	5               return 3;
	(gdb) n
	6       }
	(gdb)
	main () at a.c:13
	13              printf("%d\n", a);

	
可以看到gdb進入了func函數。

可以使用next命令（縮寫為n）不進入函數，gdb會等函數執行完，再顯示下一行要執行的程序代碼：

	(gdb) n
	12              a = func();
	(gdb) n
	13              printf("%d\n", a);
	(gdb) n
	3
	14              return 0;



可以看到gdb沒有進入func函數。

詳情參見[gdb手冊](https://sourceware.org/gdb/onlinedocs/gdb/Continuing-and-Stepping.html)

## 貢獻者

nanxiao



