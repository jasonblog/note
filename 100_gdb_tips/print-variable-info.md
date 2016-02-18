# 打印變量的類型和所在文件 

## 例子

	#include <stdio.h>
	
	struct child {
	  char name[10];
	  enum { boy, girl } gender;
	};
	
	struct child he = { "Tom", boy };
	
	int main (void)
	{
	  static struct child she = { "Jerry", girl };
	  printf ("Hello %s %s.\n", he.gender == boy ? "boy" : "girl", he.name);
	  printf ("Hello %s %s.\n", she.gender == boy ? "boy" : "girl", she.name);
	  return 0;
	}

## 技巧

在gdb中，可以使用如下命令查看變量的類型：

	(gdb) whatis he
	type = struct child

如果想查看詳細的類型信息：

	(gdb) ptype he
	type = struct child {
	    char name[10];
	    enum {boy, girl} gender;
	}

如果想查看定義該變量的文件：

	(gdb) i variables he
	All variables matching regular expression "he":
	
	File variable.c:
	struct child he;
	
	Non-debugging symbols:
	0x0000000000402030  she
	0x00007ffff7dd3380  __check_rhosts_file

哦，gdb會顯示所有包含（匹配）該表達式的變量。如果只想查看完全匹配給定名字的變量：

	(gdb) i variables ^he$
	All variables matching regular expression "^he$":
	
	File variable.c:
	struct child he;

注：`info variables`不會顯示局部變量，即使是static的也沒有太多的信息。

詳情參見[gdb手冊](https://sourceware.org/gdb/onlinedocs/gdb/Symbols.html)

## 貢獻者

xmj

