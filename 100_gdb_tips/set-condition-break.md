# 設置條件斷點 

## 例子

	#include <stdio.h>
		
	int main(void)
	{
	        int i = 0;
			int sum = 0;

			for (i = 1; i <= 200; i++)
			{
				sum += i;
			}
		
			printf("%d\n", sum);
	        return 0;
	}



## 技巧

gdb可以設置條件斷點，也就是隻有在條件滿足時，斷點才會被觸發，命令是“`break … if cond`”。以上面程序為例：

	(gdb) start
	Temporary breakpoint 1 at 0x4004cc: file a.c, line 5.
	Starting program: /data2/home/nanxiao/a
	
	Temporary breakpoint 1, main () at a.c:5
	5                       int i = 0;
	(gdb) b 10 if i==101
	Breakpoint 2 at 0x4004e3: file a.c, line 10.
	(gdb) r
	Starting program: /data2/home/nanxiao/a
	
	Breakpoint 2, main () at a.c:10
	10                                      sum += i;
	(gdb) p sum
	$1 = 5050

可以看到設定斷點只在`i`的值為`101`時觸發，此時打印`sum`的值為`5050`。

詳情參見[gdb手冊](https://sourceware.org/gdb/onlinedocs/gdb/Set-Breaks.html)

## 貢獻者

nanxiao



