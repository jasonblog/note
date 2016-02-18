# 顯示彙編代碼窗口
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
使用gdb圖形化調試界面時，可以使用“`layout asm`”命令顯示彙編代碼窗口。以調試上面程序為例：  

		    ┌───────────────────────────────────────────────────────────────────────────────────────────────┐
	  >│0x40052b <main+4>               callq  0x4004f3 <fun2>                                         │
	   │0x400530 <main+9>               mov    $0x0,%eax                                               │
	   │0x400535 <main+14>              leaveq                                                         │
	   │0x400536 <main+15>              retq                                                           │
	   │0x400537                        nop                                                            │
	   │0x400538                        nop                                                            │
	   │0x400539                        nop                                                            │
	   │0x40053a                        nop                                                            │
	   │0x40053b                        nop                                                            │
	   │0x40053c                        nop                                                            │
	   │0x40053d                        nop                                                            │
	   │0x40053e                        nop                                                            │
	   │0x40053f                        nop                                                            │
	   │0x400540 <__libc_csu_fini>      repz retq                                                      │
	   │0x400542                        data16 data16 data16 data16 nopw %cs:0x0(%rax,%rax,1)          │
	   │0x400550 <__libc_csu_init>      mov    %rbp,-0x28(%rsp)                                        │
	   └───────────────────────────────────────────────────────────────────────────────────────────────┘
	native process 44658 In: main                                               Line: 24   PC: 0x40052b
	
	(gdb) start
	Temporary breakpoint 1 at 0x40052b: file a.c, line 24.
	Starting program: /home/nan/a
	
	Temporary breakpoint 1, main () at a.c:24
	(gdb)

可以看到，顯示了當前的程序的彙編代碼。  
如果既想顯示源代碼，又想顯示彙編代碼，可以使用“`layout split`”命令：  

	   ┌──a.c──────────────────────────────────────────────────────────────────────────────────────────┐
	  >│24              fun2();                                                                        │
	   │25              return 0;                                                                      │
	   │26      }                                                                                      │
	   │27                                                                                             │
	   │28                                                                                             │
	   │29                                                                                             │
	   │30                                                                                             │
	   └───────────────────────────────────────────────────────────────────────────────────────────────┘
	  >│0x40052b <main+4>       callq  0x4004f3 <fun2>                                                 │
	   │0x400530 <main+9>       mov    $0x0,%eax                                                       │
	   │0x400535 <main+14>      leaveq                                                                 │
	   │0x400536 <main+15>      retq                                                                   │
	   │0x400537                nop                                                                    │
	   │0x400538                nop                                                                    │
	   │0x400539                nop                                                                    │
	   │0x40053a                nop                                                                    │
	   └───────────────────────────────────────────────────────────────────────────────────────────────┘
	native process 44658 In: main                                               Line: 24   PC: 0x40052b
	
	(gdb) start
	Temporary breakpoint 1 at 0x40052b: file a.c, line 24.
	Starting program: /home/nan/a
	
	Temporary breakpoint 1, main () at a.c:24
	(gdb)

可以看到上面顯示的是源代碼，下面顯示的是彙編代碼。  
參見[gdb手冊](https://sourceware.org/gdb/onlinedocs/gdb/TUI-Commands.html).

## 貢獻者

nanxiao
