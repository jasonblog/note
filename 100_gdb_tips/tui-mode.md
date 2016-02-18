# 進入和退出圖形化調試界面
## 例子
	#include <stdio.h>
	
	void fun1(void)
	{
	        int i = 0;
	
	        i++;
	        i = i * 2;
	        printf("%d\n", i);
	}
	
	void fun2(void)
	{
	        int j = 0;
	
	        fun1();
	        j++;
	        j = j * 2;
	        printf("%d\n", j);
	}
	
	int main(void)
	{
	        fun2();
	        return 0;
	}


## 技巧
啟動gdb時指定“`-tui`”參數（例如：`gdb -tui program`），或者運行gdb過程中使用“`Crtl+X+A`”組合鍵，都可以進入圖形化調試界面。以調試上面程序為例：  

	   ┌──a.c──────────────────────────────────────────────────────────────────────────────────────────┐
	   │17              j++;                                                                           │
	   │18              j = j * 2;                                                                     │
	   │19              printf("%d\n", j);                                                             │
	   │20      }                                                                                      │
	   │21                                                                                             │
	   │22      int main(void)                                                                         │
	   │23      {                                                                                      │
	B+>│24              fun2();                                                                        │
	   │25              return 0;                                                                      │
	   │26      }                                                                                      │
	   │27                                                                                             │
	   │28                                                                                             │
	   │29                                                                                             │
	   │30                                                                                             │
	   │31                                                                                             │
	   │32                                                                                             │
	   └───────────────────────────────────────────────────────────────────────────────────────────────┘
	native process 22141 In: main                                               Line: 24   PC: 0x40052b
	Type "apropos word" to search for commands related to "word"...
	Reading symbols from a...done.
	(gdb) start
	Temporary breakpoint 1 at 0x40052b: file a.c, line 24.
	Starting program: /home/nan/a
	
	Temporary breakpoint 1, main () at a.c:24
	(gdb)
可以看到，顯示了當前的程序的進程號，將要執行的代碼行號，`PC`寄存器的值。  
退出圖形化調試界面也是用“`Crtl+X+A`”組合鍵。  
參見[gdb手冊](https://sourceware.org/gdb/onlinedocs/gdb/TUI.html).

## 貢獻者

nanxiao