# GDB 反向調試（Reverse Debugging）


使用調試器時最常用的功能就是step, next, continue，這幾個調試命令都是「往下執行」的， 但是很多時候會有這種需求：你在調試的過程中多跳過了幾步而錯過中間過程，這時候不得不重頭調試一遍，非常麻煩。而GDB從7.0版本開始支持反向調試功能，也就是允許你倒退著運行程序，或者說撤銷程序執行的步驟從而會到以前的狀態。
 
直觀地來看，加入你正在使用GDB7.0以上版本的調試器並且運行在支持反向調試的平臺，你就可以用以下幾條命令來調試程序：
reverse-continue
反向運行程序知道遇到一個能使程序中斷的事件（比如斷點，觀察點，異常）。
 
reverse-step
反向運行程序到上一次被執行的源代碼行。
 
reverse-stepi
反向運行程序到上一條機器指令
 
reverse-next
反向運行到上一次被執行的源代碼行，但是不進入函數。
 
reverse-nexti
反向運行到上一條機器指令，除非這條指令用來返回一個函數調用、整個函數將會被反向執行。
 
reverse-finish
反向運行程序回到調用當前函數的地方。
 
set exec-direction [forward | reverse]
設置程序運行方向，可以用平常的命令step和continue等來執行反向的調試命令。
 
上面的反向運行也可以理解為撤銷後面運行的語句所產生的效果，回到以前的狀態。
 
好的，接下來我們來試試看如何反向調試。
首先確認自己的平臺支持進程記錄回放（Process Record and Replay），當在調試器啟用進程記錄回放功能時，調試器會記錄下子進程，也就是被調試進程的每一步的運行狀態與上一步運行狀態的差異，需要撤銷的時候就可以很方便回到上一步。
假設我們有以下C程序：

```c
int main(int argc, const char* argv[])
{
    int a = 0;
    a = 1;
    a = 2;
    return 0;
}
```

將它編譯並加上調試符號：


```sh
gcc -Wall -g a.c 
```

開始調試：
```sh
gdb a.out  
```
查看一下源代碼：
```sh
(gdb) l
1	int main(int argc, const char *argv[])
2	{
3	  int a = 0;
4	  a = 1;
5	  a = 2;
6	  return 0;
7	}

接下來設置一個斷點在第三行：

```sh
(gdb) b 3
Breakpoint 1 at 0x804839a: file a.c, line 3.
```

運行，程序會在第三行的地方停下來：

```sh
(gdb) r
Starting program: /home/cheryl/a.out 
Breakpoint 1, main (argc=1, argv=0xbffff3e4) at a.c:3
3	  int a = 0;
```

給變量a設置監視點方便我們觀察：


```sh
(gdb) watch a
Hardware watchpoint 2: a
```

啟動進程記錄回放：

```sh
(gdb) record  
```

現在每運行一步調試器都會記錄下變化，以便回溯。我們連續執行3條語句。

```sh
(gdb) n
4	  a = 1;
(gdb) 
Hardware watchpoint 2: a
Old value = 0
New value = 1
main (argc=1, argv=0xbffff3e4) at a.c:5
5	  a = 2;
(gdb) 
Hardware watchpoint 2: a
Old value = 1
New value = 2
main (argc=1, argv=0xbffff3e4) at a.c:6
6	  return 0;
```


可以看到，a的值先是從0變為了1，然後變為2，如果想讓程序倒退回到以前的狀態怎麼辦？可以用reverse-next命令：

```sh
(gdb) reverse-next
Hardware watchpoint 2: a
Old value = 2
New value = 1
main (argc=1, argv=0xbffff3e4) at a.c:5
5	  a = 2;
(gdb) 
Hardware watchpoint 2: a
Old value = 1
New value = 0
main (argc=1, argv=0xbffff3e4) at a.c:4
4	  a = 1;
(gdb) 
No more reverse-execution history.
main (argc=1, argv=0xbffff3e4) at a.c:3
3	  int a = 0;
(gdb) 
```

這樣程序就倒退到了我們啟動進程記錄回放的地方，a的值經過兩步回到了最初的狀態。
若需要關閉進程記錄回放，可以使用record stop：

```sh
(gdb) record stop
Process record is stoped and all execution log is deleted.
```

## 參考：《Reverse Debugging with GDB

http://sourceware.org/gdb/wiki/ReverseDebug

