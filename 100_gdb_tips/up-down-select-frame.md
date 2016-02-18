# 向上或向下切換函數堆棧幀
## 例子
	#include <stdio.h>

	int func1(int a)
	{
	        return 2 * a;
	}
	
	int func2(int a)
	{
	        int c = 0;
	        c = 2 * func1(a);
	        return c;
	}
	
	int func3(int a)
	{
	        int c = 0;
	        c = 2 * func2(a);
	        return c;
	}
	
	int main(void)
	{
	        printf("%d\n", func3(10));
	        return 0;
	}

## 技巧
用gdb調試程序時，當程序暫停後，可以用“`up n`”或“`down n`”命令向上或向下選擇函數堆棧幀，其中`n`是層數。以上面程序為例：  

    (gdb) b test.c:5
	Breakpoint 1 at 0x40053d: file test.c, line 5.
	(gdb) r
	Starting program: /home/nanxiao/test
	
	Breakpoint 1, func1 (a=10) at test.c:5
	5               return 2 * a;
	(gdb) bt
	#0  func1 (a=10) at test.c:5
	#1  0x0000000000400560 in func2 (a=10) at test.c:11
	#2  0x0000000000400586 in func3 (a=10) at test.c:18
	#3  0x000000000040059e in main () at test.c:24
	(gdb) frame 2
	#2  0x0000000000400586 in func3 (a=10) at test.c:18
	18              c = 2 * func2(a);
	(gdb) up 1
	#3  0x000000000040059e in main () at test.c:24
	24              printf("%d\n", func3(10));
	(gdb) down 2
	#1  0x0000000000400560 in func2 (a=10) at test.c:11
	11              c = 2 * func1(a);


可以看到程序斷住後，先執行“`frame 2`”命令，切換到`fun3`函數。接著執行“`up 1`”命令，此時會切換到`main`函數，也就是會往外層的堆棧幀移動一層。反之，當執行“`down 2`”命令後，又會向內層堆棧幀移動二層。如果不指定`n`，則`n`默認為`1`.

還有“`up-silently n`”和“`down-silently n`”這兩個命令，與“`up n`”和“`down n`”命令區別在於，切換堆棧幀後，不會打印信息，仍以上面程序為例：  

    (gdb) up
	#2  0x0000000000400586 in func3 (a=10) at test.c:18
	18              c = 2 * func2(a);
	(gdb) bt
	#0  func1 (a=10) at test.c:5
	#1  0x0000000000400560 in func2 (a=10) at test.c:11
	#2  0x0000000000400586 in func3 (a=10) at test.c:18
	#3  0x000000000040059e in main () at test.c:24
	(gdb) up-silently
	(gdb) i frame
	Stack level 3, frame at 0x7fffffffe5a0:
	 rip = 0x40059e in main (test.c:24); saved rip = 0x7ffff7a35ec5
	 caller of frame at 0x7fffffffe590
	 source language c.
	 Arglist at 0x7fffffffe590, args:
	 Locals at 0x7fffffffe590, Previous frame's sp is 0x7fffffffe5a0
	 Saved registers:
	  rbp at 0x7fffffffe590, rip at 0x7fffffffe598

可以看到從`func3`切換到`main`函數堆棧幀時，並沒有打印出相關信息。

參見[gdb手冊](https://sourceware.org/gdb/onlinedocs/gdb/Selection.html#Selection).

## 貢獻者

nanxiao
