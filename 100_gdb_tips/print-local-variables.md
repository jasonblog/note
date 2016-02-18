# 打印函數局部變量的值 

## 例子

	#include <stdio.h>

	void fun_a(void)
	{
		int a = 0;
		printf("%d\n", a);
	}
	
	void fun_b(void)
	{
		int b = 1;
		fun_a();
		printf("%d\n", b);
	}
	
	void fun_c(void)
	{
		int c = 2;
		fun_b();
		printf("%d\n", c);
	}
	
	void fun_d(void)
	{
		int d = 3;
		fun_c();
		printf("%d\n", d);
	}
	
	int main(void)
	{
		int var = -1;
		fun_d();
		return 0;
	}

## 技巧一

如果要打印函數局部變量的值，可以使用“bt full”命令（bt是backtrace的縮寫）。首先我們在函數fun_a裡打上斷點，當程序斷住時，顯示調用棧信息：

	(gdb) bt
	#0  fun_a () at a.c:6
	#1  0x000109b0 in fun_b () at a.c:12
	#2  0x000109e4 in fun_c () at a.c:19
	#3  0x00010a18 in fun_d () at a.c:26
	#4  0x00010a4c in main () at a.c:33


接下來，用“bt full”命令顯示各個函數的局部變量值：

	(gdb) bt full
	#0  fun_a () at a.c:6
	        a = 0
	#1  0x000109b0 in fun_b () at a.c:12
	        b = 1
	#2  0x000109e4 in fun_c () at a.c:19
	        c = 2
	#3  0x00010a18 in fun_d () at a.c:26
	        d = 3
	#4  0x00010a4c in main () at a.c:33
	        var = -1


也可以使用如下“bt full n”，意思是從內向外顯示n個棧楨，及其局部變量，例如：

	(gdb) bt full 2
	#0  fun_a () at a.c:6
	        a = 0
	#1  0x000109b0 in fun_b () at a.c:12
	        b = 1
	(More stack frames follow...)


而“bt full -n”，意思是從外向內顯示n個棧楨，及其局部變量，例如：

	(gdb) bt full -2
	#3  0x00010a18 in fun_d () at a.c:26
	        d = 3
	#4  0x00010a4c in main () at a.c:33
	        var = -1


詳情參見[gdb手冊](https://sourceware.org/gdb/onlinedocs/gdb/Backtrace.html)

## 技巧二

如果只是想打印當前函數局部變量的值，可以使用如下命令：

	(gdb) info locals
	a = 0

詳情參見[gdb手冊](https://sourceware.org/gdb/onlinedocs/gdb/Frame-Info.html#index-info-locals)

## 貢獻者

nanxiao

xmj

