# 選擇函數堆棧幀
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
用gdb調試程序時，當程序暫停後，可以用“`frame n`”命令選擇函數堆棧幀，其中`n`是層數。以上面程序為例：  

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

可以看到程序斷住後，最內層的函數幀為第`0`幀。執行`frame 2`命令後，當前的堆棧幀變成了`fun3`的函數幀。

也可以用“`frame addr`”命令選擇函數堆棧幀，其中`addr`是堆棧地址。仍以上面程序為例：  

    (gdb) frame 2
	#2  0x0000000000400586 in func3 (a=10) at test.c:18
	18              c = 2 * func2(a);
	(gdb) i frame
	Stack level 2, frame at 0x7fffffffe590:
	 rip = 0x400586 in func3 (test.c:18); saved rip = 0x40059e
	 called by frame at 0x7fffffffe5a0, caller of frame at 0x7fffffffe568
	 source language c.
	 Arglist at 0x7fffffffe580, args: a=10
	 Locals at 0x7fffffffe580, Previous frame's sp is 0x7fffffffe590
	 Saved registers:
	  rbp at 0x7fffffffe580, rip at 0x7fffffffe588
	(gdb) frame 0x7fffffffe568
	#1  0x0000000000400560 in func2 (a=10) at test.c:11
	11              c = 2 * func1(a);
使用“`i frame`”命令可以知道`0x7fffffffe568`是`func2`的函數堆棧幀地址，使用“`frame 0x7fffffffe568`”可以切換到`func2`的函數堆棧幀。

參見[gdb手冊](https://sourceware.org/gdb/onlinedocs/gdb/Selection.html#Selection).

## 貢獻者

nanxiao
