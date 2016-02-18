# 忽略斷點 

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

在設置斷點以後，可以忽略斷點，命令是“`ignore bnum count`”：意思是接下來`count`次編號為`bnum`的斷點觸發都不會讓程序中斷，只有第`count + 1`次斷點觸發才會讓程序中斷。以上面程序為例：

	(gdb) b 10
	Breakpoint 1 at 0x4004e3: file a.c, line 10.
	(gdb) ignore 1 5
	Will ignore next 5 crossings of breakpoint 1.
	(gdb) r
	Starting program: /data2/home/nanxiao/a
	
	Breakpoint 1, main () at a.c:10
	10                                      sum += i;
	(gdb) p i
	$1 = 6


可以看到設定忽略斷點前`5`次觸發後，第一次斷點斷住時，打印`i`的值是`6`。如果想讓斷點下次就生效，可以將`count`置為`0`：“`ignore 1 0`”。

詳情參見[gdb手冊](https://sourceware.org/gdb/onlinedocs/gdb/Conditions.html)

## 貢獻者

nanxiao



