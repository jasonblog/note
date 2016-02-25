# linked_and_loader

读《深入理解计算机系统》第七章《链接》笔记。

在ELF文件的动态连接机制中，每一个外部定义的符号在全局偏移表 (Global Offset Table，GOT)中有相应的条目，如果符号是函数则在过程连接表(Procedure Linkage Table，PLT)中也有相应的条目，且一个PLT条目对应一个GOT条目。以简单的例子观察GOT与PLT的变化机制。下面是一个简单的Hello World程序：



```c
#include <stdio.h>
 
void showmsg(char *szMsg)
{
	printf("%s\n", szMsg);
}
 
int main(int argc, char **argv)
{
	char szMsg[] = "Hello, world!";
	showmsg(szMsg);
 
	return 0;
}
```
使用gcc编译：

```c
gcc -o a.out -g helloworld.c
```

使用objdump -S a.out查看反汇编代码：

```c
void showmsg(char *szMsg)
{
 8048434:	55                   	push   %ebp
 8048435:	89 e5                	mov    %esp,%ebp
 8048437:	83 ec 18             	sub    $0x18,%esp
	printf("%s\n", szMsg);
 804843a:	8b 45 08             	mov    0x8(%ebp),%eax
 804843d:	89 04 24             	mov    %eax,(%esp)
 8048440:	e8 0b ff ff ff       	call   8048350 <puts@plt>
}
 8048445:	c9                   	leave  
 8048446:	c3                   	ret
```

对printf的调用被编译器改成了puts@plt，位于0×08048350，这是一个PLT（Procedure Linkage Table）条目，往上翻查看这个地址的代码：

```c
08048350 <puts@plt>:
 8048350:	ff 25 04 a0 04 08    	jmp    *0x804a004
 8048356:	68 08 00 00 00       	push   $0x8
 804835b:	e9 d0 ff ff ff       	jmp    8048330 <_init+0x38>
```


第一条指令跳转到0x0804a004地址处的值去执行，实际上0x0804a004就是一个对应的GOT（Global Offset Table）条目的位置了。使用


```c
objdump -R a.out
```

可以看到：

```c
a.out:     file format elf32-i386
 
DYNAMIC RELOCATION RECORDS
OFFSET   TYPE              VALUE 
08049ff0 R_386_GLOB_DAT    __gmon_start__
0804a000 R_386_JUMP_SLOT   __stack_chk_fail
0804a004 R_386_JUMP_SLOT   puts
0804a008 R_386_JUMP_SLOT   __gmon_start__
0804a00c R_386_JUMP_SLOT   __libc_start_main

```

就是puts那一条的offset的值。

下面用gdb动态调试a.out，查看GOT和PLT条目的变化：

```c
gdb a.out
(gdb) l
1	#include <stdio.h>
2	
3	void showmsg(char *szMsg)
4	{
5		printf("%s\n", szMsg);
6	}
7	
8	int main(int argc, char **argv)
9	{
10		char szMsg[] = "Hello, world!";
(gdb) b main
Breakpoint 1 at 0x8048457: file helloworld.c, line 9.
(gdb) r
Starting program: /home/winson/linuxc/a.out 
 
Breakpoint 1, main (argc=1, argv=0xbffff394) at helloworld.c:9
9	{
```

现在看一下地址0x804a004里的值是多少：

```c
(gdb) x 0x804a004
0x804a004 <puts@got.plt>:	0x08048356
```

就是puts@plt的第二条指令了，可以查看puts@plt的代码，或者反汇编0×08048356处的代码：

```c
(gdb) disas 0x08048356
Dump of assembler code for function puts@plt:
   0x08048350 <+0>:	jmp    *0x804a004
   0x08048356 <+6>:	push   $0x8
   0x0804835b <+11>:	jmp    0x8048330
End of assembler dump.
```

其实第一条指令执行后就是jmp到了第二条指令。接着push一个标识，这里是0×08。第一次执行完以后，会修改GOT条目的内容（这里就是修改0x0804a004的内容了）。

给showmsg下断点，让其执行完printf之后，再次查看GOT条目的内容：


```c
(gdb) b 5
Breakpoint 2 at 0x804843a: file helloworld.c, line 5.
(gdb) c
Continuing.
 
Breakpoint 2, showmsg (szMsg=0xbffff2de "Hello, world!") at helloworld.c:5
5		printf("%s\n", szMsg);
(gdb) n
Hello, world!
6	}
(gdb) x 0x0804a004
0x804a004 <puts@got.plt>:	0x001913b0
```

可以看到现在GOT条目直接指向真正的函数了，也就是之后的调用，先跳转到PLT条目，然后经由GOT条目直接跳转到函数代码，比之前少了一些指令，更加快速。

其实就是第一次调用函数之后，会修改GOT条目的内容，第一次相对来说慢一点，以后快一点。Linux的东西刚接触，如有错误，欢迎指正。

`PLT其实是延迟绑定技术`，也就是等到调用函数的时候才进行函数地址的定位。示例代码：

```c
(gdb) disas main
Dump of assembler code for function main:
   0x0804841d <+0>:	push   %ebp
   0x0804841e <+1>:	mov    %esp,%ebp
   0x08048420 <+3>:	and    $0xfffffff0,%esp
   0x08048423 <+6>:	sub    $0x10,%esp
   0x08048426 <+9>:	movl   $0x80484d0,(%esp)
   0x0804842d <+16>:	call   0x80482f0 <puts@plt>  ; 跟踪0x080482f0
   0x08048432 <+21>:	mov    $0x0,%eax
   0x08048437 <+26>:	leave  
   0x08048438 <+27>:	ret    
End of assembler dump.
 
(gdb) b *0x0804842d
Breakpoint 1 at 0x804842d
(gdb) r
Starting program: /home/winson/Documents/test 
 
Breakpoint 1, 0x0804842d in main ()
(gdb) disas main
Dump of assembler code for function main:
   0x0804841d <+0>:	push   %ebp
   0x0804841e <+1>:	mov    %esp,%ebp
   0x08048420 <+3>:	and    $0xfffffff0,%esp
   0x08048423 <+6>:	sub    $0x10,%esp
   0x08048426 <+9>:	movl   $0x80484d0,(%esp)
=> 0x0804842d <+16>:	call   0x80482f0 <puts@plt>  ; 跟踪0x080482f0
   0x08048432 <+21>:	mov    $0x0,%eax
   0x08048437 <+26>:	leave  
   0x08048438 <+27>:	ret    
End of assembler dump.
 
(gdb) disas 0x080482f0
Dump of assembler code for function puts@plt:
   0x080482f0 <+0>:	jmp    *0x804a00c            ; 查看0x804a00c内存
   0x080482f6 <+6>:	push   $0x0
   0x080482fb <+11>:	jmp    0x80482e0
End of assembler dump.
 
(gdb) x 0x0804a00c
0x804a00c <puts@got.plt>:	0x080482f6           ; 实际上是跳转回去执行下一条指令
 
(gdb) x /30i 0x080482e0
   0x80482e0:	pushl  0x804a004
   0x80482e6:	jmp    *0x804a008                    ; 跟踪进入
   0x80482ec:	add    %al,(%eax)
   0x80482ee:	add    %al,(%eax)
   0x80482f0 <puts@plt>:	jmp    *0x804a00c
   0x80482f6 <puts@plt+6>:	push   $0x0
   0x80482fb <puts@plt+11>:	jmp    0x80482e0
   ...
 
(gdb) x /1xw 0x0804a008
0x804a008:	0xb7ff24f0
 
(gdb) b *0x080482e6
Breakpoint 2 at 0x80482e6
(gdb) c
Continuing.
 
Breakpoint 2, 0x080482e6 in ?? ()
(gdb) ni
_dl_runtime_resolve () at ../sysdeps/i386/dl-trampoline.S:28
28	../sysdeps/i386/dl-trampoline.S: No such file or directory.
(gdb) i r eip
eip            0xb7ff24f0	0xb7ff24f0 <_dl_runtime_resolve>
(gdb)
```

可以看到第一次其实是调用_dl_runtime_resolve进行GOT对应的表项的修改，以后就直接从GOT调转到真正的函数了。更详细的内容可以参考《程序员的自我修养》第200页延迟绑定（PLT）。

