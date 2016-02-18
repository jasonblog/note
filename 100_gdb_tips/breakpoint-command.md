# 使用斷點命令改變程序的執行

## 例子

	#include <stdio.h>
	#include <stdlib.h>
	
	void drawing (int n)
	{
	  if (n != 0)
	    puts ("Try again?\nAll you need is a dollar, and a dream.");
	  else
	    puts ("You win $3000!");
	}
	
	int main (void)
	{
	  int n;
	
	  srand (time (0));
	  n = rand () % 10;
	  printf ("Your number is %d\n", n);
	  drawing (n);
	
	  return 0;
	}

## 技巧

這個例子程序可能不太好，只是可以用來演示下斷點命令的用法：

	(gdb) b drawing
	Breakpoint 1 at 0x40064d: file win.c, line 6.
	(gdb) command 1
	Type commands for breakpoint(s) 1, one per line.
	End with a line saying just "end".
	>silent
	>set variable n = 0
	>continue
	>end
	(gdb) r
	Starting program: /home/xmj/tmp/a.out 
	[Thread debugging using libthread_db enabled]
	Using host libthread_db library "/lib/x86_64-linux-gnu/libthread_db.so.1".
	Your number is 6
	You win $3000!
	[Inferior 1 (process 4134) exited normally]

可以看到，當程序運行到斷點處，會自動把變量n的值修改為0，然後繼續執行。

如果你在調試一個大程序，重新編譯一次會花費很長時間，比如調試編譯器的bug，那麼你可以用這種方式在gdb中先實驗性的修改下試試，而不需要修改源碼，重新編譯。

詳情參見[gdb手冊](https://sourceware.org/gdb/onlinedocs/gdb/Break-Commands.html#Break-Commands)

## 貢獻者

xmj

