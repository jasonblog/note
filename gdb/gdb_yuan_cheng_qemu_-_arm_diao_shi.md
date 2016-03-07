# gdb 远程qemu-arm调试


把 c 编译成 arm 指令的可运行文件

 /usr/bin/arm-linux-gnueabi-g++ hello.cpp 
cat hello.cpp 

```c
#include <stdio.h>

void crash()
{
    char* a = 0;
    *a = 0;
}

int main()
{
    printf("hello world\n");
    crash();
    printf("after crash\n");

    return 0;
}
```



直接执行报错。由于 host 是 linux x86

```sh
$ ./a.out 
-bash: ./a.out: cannot execute binary file
```

要用 qemu-arm 来执行， 结果是期望的

```sh
qemu-arm -L  /usr/arm-linux-gnueabi/  a.out 

hello world
qemu: uncaught target signal 11 (Segmentation fault) - core dumped
Segmentation fault (core dumped)
```

进行远程调试（关键是添加 -g 參数，指定port为1235) 


```sh
qemu-arm -g 1235 -L  /usr/arm-linux-gnueabi/  a.out 
```

运行用 linux-x86 的 gdb 并不能打印 symbol

```sh
(gdb) target remote :1235
Remote debugging using :1235
(gdb) c
Continuing.

Program received signal SIGSEGV, Segmentation fault.
0x00000000 in ?? ()
(gdb) bt
#0  0x00000000 in ?? ()
Cannot access memory at address 0x0
(gdb) file /home/payne/hello/a.out
A program is being debugged already.
Are you sure you want to change the file? (y or n) y
Reading symbols from /home/payne/hello/a.out...(no debugging symbols found)...done.
(gdb) bt
#0  0x00000000 in ?? ()
Cannot access memory at address 0x0
(gdb) 
```

怀疑要使用 arm 的 gdb 
參见 http://mazhijing.blog.51cto.com/215535/40759。 编译了 arm 的gdb, 运行后定位到 crash()

```sh
qemu-arm -L  /usr/arm-linux-gnueabi/  ./gdb 
(gdb) target remote :1235
Remote debugging using :1235
warning: Can not parse XML target description; XML support was disabled at compile time
0x40801c40 in ?? ()
(gdb) file /home/payne/hello/a.out
A program is being debugged already.
Are you sure you want to change the file?
 (y or n) y
Reading symbols from /home/payne/hello/a.out...(no debugging symbols found)...done.
(gdb) c
Continuing.

Program received signal SIGSEGV, Segmentation fault.
0x0000841e in crash() ()
(gdb)   
```


##结论：
难道说 arm 的 gdbserver, 就仅仅能用 arm 的 gdb？