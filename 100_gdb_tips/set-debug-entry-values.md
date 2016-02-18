# 打印尾調用堆棧幀信息
## 例子
	#include<stdio.h>
	void a(void)
	{
	        printf("Tail call frame\n");
	}
	
	void b(void)
	{
	        a();
	}
	
	void c(void)
	{
	        b();
	}
	
	int main(void)
	{
	        c();
	        return 0;
	}

## 技巧
當一個函數最後一條指令是調用另外一個函數時，開啟優化選項的編譯器常常以最後被調用的函數返回值作為調用者的返回值，這稱之為“尾調用（Tail call）”。以上面程序為例，編譯程序（使用‘-O’）：  

    gcc -g -O -o test test.c
查看`main`函數彙編代碼：

	(gdb) disassemble main
    Dump of assembler code for function main:
    0x0000000000400565 <+0>:     sub    $0x8,%rsp
    0x0000000000400569 <+4>:     callq  0x400536 <a>
    0x000000000040056e <+9>:     mov    $0x0,%eax
    0x0000000000400573 <+14>:    add    $0x8,%rsp
    0x0000000000400577 <+18>:    retq
可以看到`main`函數直接調用了函數`a`，根本看不到函數`b`和函數`c`的影子。
  
在函數`a`入口處打上斷點，程序停止後，打印堆棧幀信息：  

	(gdb) i frame
	Stack level 0, frame at 0x7fffffffe590:
	 rip = 0x400536 in a (test.c:4); saved rip = 0x40056e
	 called by frame at 0x7fffffffe5a0
	 source language c.
	 Arglist at 0x7fffffffe580, args:
	 Locals at 0x7fffffffe580, Previous frame's sp is 0x7fffffffe590
	 Saved registers:
	  rip at 0x7fffffffe588
看不到尾調用的相關信息。  

可以設置“`debug entry-values`”選項為非0的值，這樣除了輸出正常的函數堆棧幀信息以外，還可以輸出尾調用的相關信息：  

	(gdb) set debug entry-values 1
	(gdb) b test.c:4
	Breakpoint 1 at 0x400536: file test.c, line 4.
	(gdb) r
	Starting program: /home/nanxiao/test
	
	Breakpoint 1, a () at test.c:4
	4       {
	(gdb) i frame
	tailcall: initial:
	Stack level 0, frame at 0x7fffffffe590:
	 rip = 0x400536 in a (test.c:4); saved rip = 0x40056e
	 called by frame at 0x7fffffffe5a0
	 source language c.
	 Arglist at 0x7fffffffe580, args:
	 Locals at 0x7fffffffe580, Previous frame's sp is 0x7fffffffe590
	 Saved registers:
	  rip at 0x7fffffffe588

可以看到輸出了“`tailcall: initial:`”信息。  

參見[gdb手冊](https://sourceware.org/gdb/onlinedocs/gdb/Tail-Call-Frames.html).

## 貢獻者

nanxiao
