# 支持預處理器宏信息

## 例子

	#include <stdio.h>
	
	#define NAME "Joe"
	
	int main()
	{
	  printf ("Hello %s\n", NAME);
	  return 0;
	}

## 技巧

使用`gcc -g`編譯生成的程序，是不包含預處理器宏信息的：

	(gdb) p NAME
	No symbol "NAME" in current context.

如果想在gdb中查看宏信息，可以使用`gcc -g3`進行編譯：

	(gdb) p NAME
	$1 = "Joe"

關於預處理器宏的命令，參見[gdb手冊](https://sourceware.org/gdb/onlinedocs/gdb/Macros.html#Macros)

## 貢獻者

xmj

