# 修改被調試程序的二進制文件

## 例子

	#include <stdio.h>
	#include <stdlib.h>
	
	void drawing (int n)
	{
	  if (n != 0)
	    puts ("Try again?\nAll you need is a dollar, and a dream.");
	  else
	    puts ("You win $3000!");
	}
	
	int main (void)
	{
	  int n;
	
	  srand (time (0));
	  n = rand () % 10;
	  printf ("Your number is %d\n", n);
	  drawing (n);
	
	  return 0;
	}

## 技巧

gdb不僅可以用來調試程序，還可以修改程序的二進制代碼。

缺省情況下，gdb是以只讀方式加載程序的。可以通過命令行選項指定為可寫：

	$ gcc -write ./a.out
	(gdb) show write
	Writing into executable and core files is on.

也可以在gdb中，使用命令設置並重新加載程序：

	(gdb) set write on
	(gdb) file ./a.out

接下來，查看反彙編：

	(gdb) disassemble /mr drawing 
	Dump of assembler code for function drawing:
	5	{
	   0x0000000000400642 <+0>:	55	push   %rbp
	   0x0000000000400643 <+1>:	48 89 e5	mov    %rsp,%rbp
	   0x0000000000400646 <+4>:	48 83 ec 10	sub    $0x10,%rsp
	   0x000000000040064a <+8>:	89 7d fc	mov    %edi,-0x4(%rbp)
	
	6	  if (n != 0)
	   0x000000000040064d <+11>:	83 7d fc 00	cmpl   $0x0,-0x4(%rbp)
	   0x0000000000400651 <+15>:	74 0c	je     0x40065f <drawing+29>
	
	7	    puts ("Try again?\nAll you need is a dollar, and a dream.");
	   0x0000000000400653 <+17>:	bf e0 07 40 00	mov    $0x4007e0,%edi
	   0x0000000000400658 <+22>:	e8 b3 fe ff ff	callq  0x400510 <puts@plt>
	   0x000000000040065d <+27>:	eb 0a	jmp    0x400669 <drawing+39>
	
	8	  else
	9	    puts ("You win $3000!");
	   0x000000000040065f <+29>:	bf 12 08 40 00	mov    $0x400812,%edi
	   0x0000000000400664 <+34>:	e8 a7 fe ff ff	callq  0x400510 <puts@plt>
	
	10	}
	   0x0000000000400669 <+39>:	c9	leaveq 
	   0x000000000040066a <+40>:	c3	retq   
	
	End of assembler dump.

修改二進制代碼（注意大小端和指令長度）：

	(gdb) set variable *(short*)0x400651=0x0ceb
	(gdb) disassemble /mr drawing 
	Dump of assembler code for function drawing:
	5	{
	   0x0000000000400642 <+0>:	55	push   %rbp
	   0x0000000000400643 <+1>:	48 89 e5	mov    %rsp,%rbp
	   0x0000000000400646 <+4>:	48 83 ec 10	sub    $0x10,%rsp
	   0x000000000040064a <+8>:	89 7d fc	mov    %edi,-0x4(%rbp)
	
	6	  if (n != 0)
	   0x000000000040064d <+11>:	83 7d fc 00	cmpl   $0x0,-0x4(%rbp)
	   0x0000000000400651 <+15>:	eb 0c	jmp    0x40065f <drawing+29>
	
	7	    puts ("Try again?\nAll you need is a dollar, and a dream.");
	   0x0000000000400653 <+17>:	bf e0 07 40 00	mov    $0x4007e0,%edi
	   0x0000000000400658 <+22>:	e8 b3 fe ff ff	callq  0x400510 <puts@plt>
	   0x000000000040065d <+27>:	eb 0a	jmp    0x400669 <drawing+39>
	
	8	  else
	9	    puts ("You win $3000!");
	   0x000000000040065f <+29>:	bf 12 08 40 00	mov    $0x400812,%edi
	   0x0000000000400664 <+34>:	e8 a7 fe ff ff	callq  0x400510 <puts@plt>
	
	10	}
	   0x0000000000400669 <+39>:	c9	leaveq 
	   0x000000000040066a <+40>:	c3	retq   
	
	End of assembler dump.

可以看到，條件跳轉指令“je”已經被改為無條件跳轉“jmp”了。

退出，運行一下：

	$ ./a.out 
	Your number is 2
	You win $3000!

詳情參見[gdb手冊](https://sourceware.org/gdb/onlinedocs/gdb/Patching.html#Patching)

## 貢獻者

xmj

