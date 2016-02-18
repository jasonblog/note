# 按何種方式解析腳本文件 

## 例子

	#include <stdio.h>

	typedef struct
	{
	        int a;
	        int b;
	        int c;
	        int d;
	}ex_st;
	
	int main(void) {
	        ex_st st = {1, 2, 3, 4};
	        printf("%d,%d,%d,%d\n", st.a, st.b, st.c, st.d);
	        return 0;
	}



## 技巧

gdb支持的腳本文件分為兩種：一種是隻包含gdb自身命令的腳本，例如“.gdbinit”文件，當gdb在啟動時，就會執行“.gdbinit”文件中的命令；此外，gdb還支持其它一些語言寫的腳本文件（比如python）。  
gdb用“`set script-extension`”命令來決定按何種格式來解析腳本文件。它可以取3個值：  
a）`off`：所有的腳本文件都解析成gdb的命令腳本；  
b）`soft`：根據腳本文件擴展名決定如何解析腳本。如果gdb支持解析這種腳本語言（比如python），就按這種語言解析，否則就按命令腳本解析；  
c）`strict`：根據腳本文件擴展名決定如何解析腳本。如果gdb支持解析這種腳本語言（比如python），就按這種語言解析，否則不解析；  
以上面程序為例，進行調試：

	(gdb) start
	Temporary breakpoint 1 at 0x4004cd: file a.c, line 12.
	Starting program: /data2/home/nanxiao/a
	
	Temporary breakpoint 1, main () at a.c:12
	12              ex_st st = {1, 2, 3, 4};
	(gdb) q
	A debugging session is active.
	
	        Inferior 1 [process 24249] will be killed.
	
	Quit anyway? (y or n) y


可以看到gdb退出時，默認行為會提示用戶是否退出。

下面寫一個腳本文件（gdb.py），但內容是一個gdb命令，不是真正的python腳本。用途是退出gdb時不提示：

	set confirm off
再次開始調試：  

	(gdb) start
	Temporary breakpoint 1 at 0x4004cd: file a.c, line 12.
	Starting program: /data2/home/nanxiao/a
	
	Temporary breakpoint 1, main () at a.c:12
	12              ex_st st = {1, 2, 3, 4};
	(gdb) show script-extension
	Script filename extension recognition is "soft".
	(gdb) source gdb.py
	  File "gdb.py", line 1
	    set confirm off
	              ^
	SyntaxError: invalid syntax


可以看到“`script-extension`”默認值是`soft`，接下來執行“`source gdb.py`”,會按照pyhton語言解析gdb.py文件，但是由於這個文件實質上是一個gdb命令腳本，所以解析出錯。  
再執行一次：  

	(gdb) start
	Temporary breakpoint 1 at 0x4004cd: file a.c, line 12.
	Starting program: /data2/home/nanxiao/a
	
	Temporary breakpoint 1, main () at a.c:12
	12              ex_st st = {1, 2, 3, 4};
	(gdb) set script-extension off
	(gdb) source gdb.py
	(gdb) q
	[root@linux:~]$
這次把“`script-extension`”值改為`off`，所以腳本會按gdb命令腳本去解析，可以看到這次腳本命令生效了。
  
參見[gdb手冊](https://sourceware.org/gdb/onlinedocs/gdb/Extending-GDB.html)
## 貢獻者

nanxiao



