# 打印數組的索引下標

## 例子

	#include <stdio.h>
	
	int num[10] = { 
	  1 << 0,
	  1 << 1,
	  1 << 2,
	  1 << 3,
	  1 << 4,
	  1 << 5,
	  1 << 6,
	  1 << 7,
	  1 << 8,
	  1 << 9
	};
	
	int main (void)
	{
	  int i;
	
	  for (i = 0; i < 10; i++)
	    printf ("num[%d] = %d\n", i, num[i]);
	
	  return 0;
	}

## 技巧

在gdb中，當打印一個數組時，缺省是不打印索引下標的：

	(gdb) p num
	$1 = {1, 2, 4, 8, 16, 32, 64, 128, 256, 512}

如果要打印索引下標，則可以通過如下命令進行設置：

	(gdb) set print array-indexes on

	(gdb) p num
	$2 = {[0] = 1, [1] = 2, [2] = 4, [3] = 8, [4] = 16, [5] = 32, [6] = 64, [7] = 128, [8] = 256, [9] = 512}

詳情參見[gdb手冊](https://sourceware.org/gdb/onlinedocs/gdb/Print-Settings.html#index-set-print)

## 貢獻者

xmj

