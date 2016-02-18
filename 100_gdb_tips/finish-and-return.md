# 退出正在調試的函數

## 例子

	#include <stdio.h>

	int func(void)
	{
	    int i = 0;
	
	    i += 2;
	    i *= 10;
	
	    return i;
	}
	
	int main(void)
	{
	    int a = 0;
	
	    a = func();
	    printf("%d\n", a);
	    return 0;
	}



## 技巧

當單步調試一個函數時，如果不想繼續跟蹤下去了，可以有兩種方式退出。

第一種用“`finish`”命令，這樣函數會繼續執行完，並且打印返回值，然後等待輸入接下來的命令。以上面代碼為例：

	(gdb) n
	17          a = func();
	(gdb) s
	func () at a.c:5
	5               int i = 0;
	(gdb) n
	7               i += 2;
	(gdb) fin
	find    finish
	(gdb) finish
	Run till exit from #0  func () at a.c:7
	0x08050978 in main () at a.c:17
	17          a = func();
	Value returned is $1 = 20

	
可以看到當不想再繼續跟蹤`func`函數時，執行完“`finish`”命令，gdb會打印結果：“`20`”，然後停在那裡。

詳情參見[gdb手冊](https://sourceware.org/gdb/onlinedocs/gdb/Continuing-and-Stepping.html)

第二種用“`return`”命令，這樣函數不會繼續執行下面的語句，而是直接返回。也可以用“`return expression`”命令指定函數的返回值。仍以上面代碼為例：

	(gdb) n
	17          a = func();
	(gdb) s
	func () at a.c:5
	5               int i = 0;
	(gdb) n
	7               i += 2;
	(gdb) n
	8               i *= 10;
	(gdb) re
	record              remove-inferiors    return              reverse-next        reverse-step
	refresh             remove-symbol-file  reverse-continue    reverse-nexti       reverse-stepi
	remote              restore             reverse-finish      reverse-search
	(gdb) return 40
	Make func return now? (y or n) y
	#0  0x08050978 in main () at a.c:17
	17          a = func();
	(gdb) n
	18          printf("%d\n", a);
	(gdb)
	40
	19          return 0;



可以看到“`return`”命令退出了函數並且修改了函數的返回值。

詳情參見[gdb手冊](https://sourceware.org/gdb/onlinedocs/gdb/Returning.html#Returning)

## 貢獻者

nanxiao



