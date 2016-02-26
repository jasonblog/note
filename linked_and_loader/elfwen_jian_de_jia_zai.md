# ELF文件的加载


linux 中用户态与内核态的联系是通过system call进行调用的。最近由于学习了不同的调式工具，以及解析elf文件的objdump，正好可以来熟练一下载入流程。
首先，我先介绍一下ELF文件格式：

```c
#define EI_NIDENT 16
typedef struct{
    unsigned char e_ident[EI_NIDENT];    //目标文件标识信息
    Elf32_Half e_type;                             //目标文件类型
    Elf32_Half e_machine;                       //目标体系结构类型
    Elf32_Word e_version;                      //目标文件版本
    Elf32_Addr e_entry;                          //程序入口的虚拟地址,若没有，可为0
    Elf32_Off e_phoff;                            //程序头部表格（Program Header Table）的偏移量（按字节计算）,若没有，可为0
    Elf32_Off e_shoff;                            //节区头部表格（Section Header Table）的偏移量（按字节计算）,若没有，可为0
    Elf32_Word e_flags;                        //保存与文件相关的，特定于处理器的标志。标志名称采用 EF_machine_flag的格式。
    Elf32_Half e_ehsize;                        //ELF 头部的大小（以字节计算）。
    Elf32_Half e_phentsize;                   //程序头部表格的表项大小（按字节计算）。
    Elf32_Half e_phnum;                      //程序头部表格的表项数目。可以为 0。
    Elf32_Half e_shentsize;                  //节区头部表格的表项大小（按字节计算）。
    Elf32_Half e_shnum;      //节区头部表格的表项数目。可以为 0。
    Elf32_Half e_shstrndx;  //节区头部表格中与节区名称字符串表相关的表项的索引。如果文件没有节区名称字符串表，此参数可以为 SHN_UNDEF。
}Elf32_Ehdr;
```


我们先用一个最简单的hello world来说明elf文件载入流程。


gcc在编译时，所有程序的链接都是动态链接的，也就是说需要解释器。由此可见，我们的Hello World程序在被内核加载到内存，内核跳到用户空间后并不是执行Hello World的，而是先把控制权交到用户空间的解释器，由解释器加载运行用户程序所需要的动态库（Hello World需要libc），然后控制权才会转移到用户程序。

首先我们要知道`glic就是一个kernel 与 app 的交流窗口`，`ld_linux_so是一个动态装入器（linked dynamic）`

当我们call PLT表时，就是动态调用so文件，其实也可以称为重定向。printf函数的调用已经换成了puts函数。其中的call指令就是调用puts函数。但从上面的代码可以看出，它调用的是puts@plt这个标号，它代表什么意思呢？在进一步说明符号的动态解析过程以前，需要先了解两个概念，一个是global offset table就是GOT表，一个是procedure linkage table（PLT）。

首先 我们先反汇编这个elf文件

```sh
Breakpoint 1, 0x0000000000400530 in main ()
Missing separate debuginfos, use: debuginfo-install glibc-2.16-34.fc18.x86_64
(gdb) disassemble
Dump of assembler code for function main:
   0x000000000040052c <+0>:   push   %rbp
   0x000000000040052d <+1>:   mov    %rsp,%rbp
=> 0x0000000000400530 <+4>:    sub    $0x10,%rsp
   0x0000000000400534 <+8>:   mov    %edi,-0x4(%rbp)
   0x0000000000400537 <+11>:  mov    %rsi,-0x10(%rbp)
   0x000000000040053b <+15>:  mov    $0x400600,%edi
   0x0000000000400540 <+20>:  callq  0x400410 <puts@plt>
   0x0000000000400545 <+25>:  mov    $0x0,%eax
   0x000000000040054a <+30>:  leaveq
   0x000000000040054b <+31>:  retq
End of assembler dump.
(gdb) stepi
0x0000000000400534 in main ()
(gdb)
0x0000000000400537 in main ()
(gdb)
0x000000000040053b in main ()
(gdb)
0x0000000000400540 in main ()
(gdb)
0x0000000000400410 in puts@plt ()
(gdb) disassemble
Dump of assembler code for function puts@plt:
<strong>=> 0x0000000000400410 <+0>:  jmpq   *0x200c02(%rip)        # 0x601018 <puts@got.plt></strong>
   0x0000000000400416 <+6>:   pushq  $0x0
   0x000000000040041b <+11>:  jmpq   0x400400
End of assembler dump.
(gdb) stepi
0x0000000000400400 in ?? ()
(gdb) disassemble
No function contains program counter for selected frame.
(gdb) stepi
0x0000000000400406 in ?? ()
(gdb) stepi
0x0000003cf16149b0 in _dl_runtime_resolve () from /lib64/ld-linux-x86-64.so.2   #动态装入函数
(gdb) disassemble
<strong>Dump of assembler code for function _dl_runtime_resolve:</strong>
=> 0x0000003cf16149b0 <+0>:    sub    $0x38,%rsp
   0x0000003cf16149b4 <+4>:   mov    %rax,(%rsp)
   0x0000003cf16149b8 <+8>:   mov    %rcx,0x8(%rsp)
   0x0000003cf16149bd <+13>:  mov    %rdx,0x10(%rsp)
   0x0000003cf16149c2 <+18>:  mov    %rsi,0x18(%rsp)
   0x0000003cf16149c7 <+23>:  mov    %rdi,0x20(%rsp)
   0x0000003cf16149cc <+28>:  mov    %r8,0x28(%rsp)
   0x0000003cf16149d1 <+33>:  mov    %r9,0x30(%rsp)
   0x0000003cf16149d6 <+38>:  mov    0x40(%rsp),%rsi
   0x0000003cf16149db <+43>:  mov    0x38(%rsp),%rdi
   0x0000003cf16149e0 <+48>:  callq  0x3cf160e310 <_dl_fixup>
   <strong>0x0000003cf16149e5 <+53>:    mov    %rax,%r11</strong>
   0x0000003cf16149e8 <+56>:  mov    0x30(%rsp),%r9
   0x0000003cf16149ed <+61>:  mov    0x28(%rsp),%r8
   0x0000003cf16149f2 <+66>:  mov    0x20(%rsp),%rdi
   0x0000003cf16149f7 <+71>:  mov    0x18(%rsp),%rsi
   0x0000003cf16149fc <+76>:  mov    0x10(%rsp),%rdx
   0x0000003cf1614a01 <+81>:  mov    0x8(%rsp),%rcx
   0x0000003cf1614a06 <+86>:  mov    (%rsp),%rax
   0x0000003cf1614a0a <+90>:  add    $0x48,%rsp
<strong>   0x0000003cf1614a0e <+94>:    jmpq   *%r11</strong>
End of assembler dump.
(gdb)
```

`从上面，我们能看出stepi中间有两项是空的，那正是GOT[0]是空的，GOT[1] GOT[2]是一个函数的地址。`

`GOT表的前三项保留，GOT[1]保存的是一个地址，指向已经加载的共享库的链表地址（前面提到加载的共享库会形成一个链表）；GOT[2]保存的是一个函数的地址(dl_runtime_resolve这个函数的入口)，这个函数的主要作用就是找到某个符号的地址，并把它写到与此符号相关的GOT项中，然后将控制转移到目标函数。`


从调用puts函数到现在，总共有两次压栈操作，一次是压入puts函数的重定向信息的偏移量，一次是GOT[1]（共享库链表的地址）。上面的movl操作就是将这数据分别取到一系列的寄存器，然后调用_dl_fixup（从寄存器取参数），此函数完成的功能就是找到puts函数的实际加载地址，并将它写到GOT中，然后通过寄存器将此值返回给_dl_runtime_resolve。栈顶为puts函数的返回地址（main函数中call指令的下一条指令），这样，当puts函数返回时，就返回到正确的位置。


```sh
Disassembly of section .plt:
 
0000000000400400 <puts@plt-0x10>:
  400400:   ff 35 02 0c 20 00       pushq  0x200c02(%rip)        # 601008 <_GLOBAL_OFFSET_TABLE_+0x8>
  400406:   ff 25 04 0c 20 00       jmpq   *0x200c04(%rip)        # 601010 <_GLOBAL_OFFSET_TABLE_+0x10>
  40040c:   0f 1f 40 00             nopl   0x0(%rax)
 
0000000000400410 <puts@plt>:
  400410:   ff 25 02 0c 20 00       jmpq   *0x200c02(%rip)        # 601018 <_GLOBAL_OFFSET_TABLE_+0x18>
  400416:   68 00 00 00 00          pushq  $0x0
  40041b:   e9 e0 ff ff ff          jmpq   400400 <_init+0x20>
 
0000000000400420 <__libc_start_main@plt>:
  400420:   ff 25 fa 0b 20 00       jmpq   *0x200bfa(%rip)        # 601020 <_GLOBAL_OFFSET_TABLE_+0x20>
  400426:   68 01 00 00 00          pushq  $0x1
  40042b:   e9 d0 ff ff ff          jmpq   400400 <_init+0x20>
 
0000000000400430 <__gmon_start__@plt>:
  400430:   ff 25 f2 0b 20 00       jmpq   *0x200bf2(%rip)        # 601028 <_GLOBAL_OFFSET_TABLE_+0x28>
  400436:   68 02 00 00 00          pushq  $0x2
  40043b:   e9 c0 ff ff ff          jmpq   400400 <_init+0x20>
  ```
  
除PLT0以外（就是puts@plt-0x10所标记的内容），其它的所有PLT项的形式都是一样的，而且最后的jmp指令都是0x400400，即PLT0为目标的。


```sh
(gdb) x/w 0x601018
0x601018 <puts@got.plt>:  0x00400416
(gdb) disassemble
Dump of assembler code for function puts@plt:
=> 0x0000000000400410 <+0>:    jmpq   *0x200c02(%rip)        # 0x601018 <puts@got.plt>
   0x0000000000400416 <+6>:   pushq  $0x0
   0x000000000040041b <+11>:  jmpq   0x400400
End of assembler dump.
(gdb) x/32x 0x601018
0x601018 <puts@got.plt>:  0x00400416  0x00000000  0xf1a21910  0x0000003c
0x601028 <__gmon_start__@got.plt>:    0x00400436  0x00000000  0x00000000  0x00000000
0x601038:   0x00000000  0x00000000  0x00000000  0x00000000
0x601048:   0x00000000  0x00000000  0x00000000  0x00000000

```

从上面可以看出，这个地址就是GOT表中的一项。它里面的内容是0x00400416，即puts@plt中的第二条指令。原来链接器在把所需要的共享库加载进内存后，并没有把共享库中的函数的地址写到GOT表项中，而是延迟到函数的第一次调用时，才会对函数的地址进行定位。

这样我们就从so文件中加载了一个库函数。

`当然，如果是第二次调用puts函数，那么就不需要这么复杂的过程，而只要通过GOT表中已经确定的函数地址直接进行跳转即可。下图是前面过程的一个示意图，红色为第一次函数调用的顺序，蓝色为后续函数调用的顺序（第1步都要执行）。`

##参考

http://blog.csdn.net/flydream0/article/details/8719036
