# 指定程序的輸入輸出設備

## 例子

	#include <stdio.h>
	
	int main(void)
	{
	  int i;
	
	  for (i = 0; i < 100; i++)
	    {
	      printf("i = %d\n", i);
	    }
	
	  return 0;
	}

## 技巧

在gdb中，缺省情況下程序的輸入輸出是和gdb使用同一個終端。你也可以為程序指定一個單獨的輸入輸出終端。

首先，打開一個新終端，使用如下命令獲得設備文件名：

	$ tty
	/dev/pts/2

然後，通過命令行選項指定程序的輸入輸出設備：

	$ gdb -tty /dev/pts/2 ./a.out
	(gdb) r

或者，在gdb中，使用命令進行設置：

	(gdb) tty /dev/pts/2

詳情參見[gdb手冊](https://sourceware.org/gdb/current/onlinedocs/gdb/Input_002fOutput.html#index-tty)

## 貢獻者

xmj

