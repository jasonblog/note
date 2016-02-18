# 記錄執行gdb的過程
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
用gdb調試程序時，可以使用“`set logging on`”命令把執行gdb的過程記錄下來，方便以後自己參考或是別人幫忙分析。默認的日誌文件是“`gdb.txt`”，也可以用“`set logging file file`”改成別的名字。以上面程序為例：  

    (gdb) set logging file log.txt
	(gdb) set logging on
	Copying output to log.txt.
	(gdb) start
	Temporary breakpoint 1 at 0x8050abe: file a.c, line 6.
	Starting program: /data1/nan/a 
	[Thread debugging using libthread_db enabled]
	[New Thread 1 (LWP 1)]
	[Switching to Thread 1 (LWP 1)]
	
	Temporary breakpoint 1, main () at a.c:6
	6               char str1[] = "abcd";
	(gdb) n
	7               wchar_t str2[] = L"abcd";
	(gdb) x/s str1
	0x804779f:      "abcd"
	(gdb) n       
	9               return 0;
	(gdb) x/ws str2
	0x8047788:      U"abcd"
	(gdb) q
	A debugging session is active.
	
	        Inferior 1 [process 9931    ] will be killed.
	
	Quit anyway? (y or n) y

執行完後，查看log.txt文件：

	bash-3.2# cat log.txt 
	Temporary breakpoint 1 at 0x8050abe: file a.c, line 6.
	Starting program: /data1/nan/a 
	[Thread debugging using libthread_db enabled]
	[New Thread 1 (LWP 1)]
	[Switching to Thread 1 (LWP 1)]
	
	Temporary breakpoint 1, main () at a.c:6
	6               char str1[] = "abcd";
	7               wchar_t str2[] = L"abcd";
	0x804779f:      "abcd"
	9               return 0;
	0x8047788:      U"abcd"
	A debugging session is active.
	
	        Inferior 1 [process 9931    ] will be killed.
	
	Quit anyway? (y or n)
可以看到log.txt詳細地記錄了gdb的執行過程。

此外“`set logging overwrite on`”命令可以讓輸出覆蓋之前的日誌文件；而 “`set logging redirect on`”命令會讓gdb的日誌不會打印在終端。    
參見[gdb手冊](https://sourceware.org/gdb/onlinedocs/gdb/Logging-Output.html).

## 貢獻者

nanxiao
