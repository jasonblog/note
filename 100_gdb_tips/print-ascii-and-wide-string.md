# 打印ASCII和寬字符字符串
## 例子
	#include <stdio.h>
	#include <wchar.h>
	
	int main(void)
	{
	        char str1[] = "abcd";
	        wchar_t str2[] = L"abcd";
	        
	        return 0;
	}

## 技巧
用gdb調試程序時，可以使用“`x/s`”命令打印ASCII字符串。以上面程序為例：  

    Temporary breakpoint 1, main () at a.c:6
	6               char str1[] = "abcd";
	(gdb) n
	7               wchar_t str2[] = L"abcd";
	(gdb) 
	9               return 0;
	(gdb) x/s str1
	0x804779f:      "abcd"

可以看到打印出了`str1`字符串的值。

打印寬字符字符串時，要根據寬字符的長度決定如何打印。仍以上面程序為例： 

    Temporary breakpoint 1, main () at a.c:6
	6               char str1[] = "abcd";
	(gdb) n
	7               wchar_t str2[] = L"abcd";
	(gdb) 
	9               return 0;
	(gdb) p sizeof(wchar_t)
	$1 = 4
	(gdb) x/ws str2
	0x8047788:      U"abcd"
由於當前平臺寬字符的長度為4個字節，則用“`x/ws`”命令。如果是2個字節，則用“`x/hs`”命令。

參見[gdb手冊](https://sourceware.org/gdb/onlinedocs/gdb/Memory.html).

## 貢獻者

nanxiao
