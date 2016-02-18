# 替換查找源文件的目錄
## 例子
	#include <stdio.h>
	#include <time.h>
	
	int main(void) {
	        time_t now = time(NULL);
	        struct tm local = {0};
	        struct tm gmt = {0};
	
	        localtime_r(&now, &local);
	        gmtime_r(&now, &gmt);
	
	        return 0;
	}




## 技巧
有時調試程序時，源代碼文件可能已經移到其它的文件夾了。此時可以用`set substitute-path from to`命令設置新的文件夾（`to`）目錄替換舊的（`from`）。以上面程序為例：  

	(gdb) start
	Temporary breakpoint 1 at 0x400560: file a.c, line 5.
	Starting program: /home/nan/a

	Temporary breakpoint 1, main () at a.c:5
	5       a.c: No such file or directory.
	(gdb) set substitute-path /home/nan /home/ki
	(gdb) n
	6                       struct tm local = {0};
	(gdb)
	7                       struct tm gmt = {0};
	(gdb)
	9                       localtime_r(&now, &local);
	(gdb)
	10                      gmtime_r(&now, &gmt);
	(gdb)
	12                      return 0;



調試時，因為源文件已經移到`/home/ki`這個文件夾下了，所以gdb找不到源文件。使用`set substitute-path /home/nan /home/ki`命令設置源文件的查找目錄後，gdb就可以正常地解析源代碼了。  
參見[gdb手冊](https://sourceware.org/gdb/onlinedocs/gdb/Source-Path.html).

## 貢獻者

nanxiao
