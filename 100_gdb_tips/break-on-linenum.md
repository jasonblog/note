# 在文件行號上打斷點

## 例子

	/* a/file.c */
	#include <stdio.h>
	
	void print_a (void)
	{
	  puts ("a");
	}
	
	/* b/file.c */
	#include <stdio.h>
	
	void print_b (void)
	{
	  puts ("b");
	}
	
	/* main.c */
	extern void print_a(void);
	extern void print_b(void);
	
	int main(void)
	{
	  print_a();
	  print_b();
	  return 0;
	}

## 技巧

這個比較簡單，如果要在當前文件中的某一行打斷點，直接`b linenum`即可，例如：

	(gdb) b 7

也可以顯式指定文件，`b file:linenum`例如：

	(gdb) b file.c:6
	Breakpoint 1 at 0x40053b: file.c:6. (2 locations)
	(gdb) i breakpoints 
	Num     Type           Disp Enb Address            What
	1       breakpoint     keep y   <MULTIPLE>         
	1.1                         y     0x000000000040053b in print_a at a/file.c:6
	1.2                         y     0x000000000040054b in print_b at b/file.c:6

可以看出，gdb會對所有匹配的文件設置斷點。你可以通過指定（部分）路徑，來區分相同的文件名：

	(gdb) b a/file.c:6

注意：通過行號進行設置斷點的一個弊端是，如果你更改了源程序，那麼之前設置的斷點就可能不是你想要的了。

詳情參見[gdb手冊](https://sourceware.org/gdb/onlinedocs/gdb/Specify-Location.html#Specify-Location)

## 貢獻者

xmj

