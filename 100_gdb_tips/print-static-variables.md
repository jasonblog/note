# 打印靜態變量的值 

## 例子

	/* main.c */
	extern void print_var_1(void);
	extern void print_var_2(void);
	
	int main(void)
	{
	  print_var_1();
	  print_var_2();
	  return 0;
	}
	
	/* static-1.c */
	#include <stdio.h>
	
	static int var = 1;
	
	void print_var_1(void)
	{ 
	  printf("var = %d\n", var);
	} 
	
	/* static-2.c */
	#include <stdio.h>
	
	static int var = 2;
	
	void print_var_2(void)
	{ 
	  printf("var = %d\n", var);
	} 

## 技巧

在gdb中，如果直接打印靜態變量，則結果並不一定是你想要的：

	$ gcc -g main.c static-1.c static-2.c
	$ gdb -q ./a.out
	(gdb) start
	(gdb) p var
	$1 = 2

	$ gcc -g main.c static-2.c static-1.c
	$ gdb -q ./a.out
	(gdb) start
	(gdb) p var
	$1 = 1

你可以顯式地指定文件名（上下文）：

	(gdb) p 'static-1.c'::var
	$1 = 1
	(gdb) p 'static-2.c'::var
	$2 = 2

詳情參見[gdb手冊](https://sourceware.org/gdb/current/onlinedocs/gdb/Variables.html#Variables)

## 貢獻者

xmj

