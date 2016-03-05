# OS dev的Bochs调试


最近在弄一个自己的hobby OS，作为菜鸟在调试时候积累一些经验，记录一下。
##Bochs调试
Bochs自带调试功能，但是如果你是apt装上的是不行的，下源码来自己编译，编译选项为：


```sh
./configure --enable-debugger --enable-disasm
```

这个我只是尝试过，在OS的loader阶段可能会用到，当如果进入C语言实现部分的代码如何调试?我希望看到C的源码级别调试，而不是汇编的。

##Bochs + gdb调试

同样需要在编译的时候加上选项，这个选项必须注意，否则在gdb调试的时候会出现”Cannot find bounds of current function”之类的问题。

```sh
./configure --enable-debugger --enable-disasm --enable-gdb-stub
```

诡异的是这个–enable-gdb-stub选项和上面的 –enable-debugger选项只能`二选一`。也行，编译出来后重命名吧。编译完成后在Bochs的配置文件.bashrc中加上这么一行:

```sh
gdbstub: enabled=1, port=1234, text_base=0, data_base=0, bss_base=0
```

另外注意kernel的代码也需要加入-g编译选项。最后在编译完成后的文件是带调试信息的，但是我们在用Bochs启动的img文件不需要这些，现在比如kernel.elf是带编译信息的kernel 文件，用下面的这个步骤去掉调试信息，据说也可以用strip来。


```sh
cmd="objcopy -R .pdr -R .comment -R .note -S -O binary kernel.elf kernel.bin"

cat boot.bin setup.bin kernel.bin > ../a.img;
Bochs 使用的是这个a.img文件， gdb载入的是kernel.elf文件。
```

启动Bochs后会等待gdb连进来(其实Qemu也可以这样进行调试的)，查资料过程中发现还可在调试的目录加上.gdbinit，这样每次启动gdb就不那么麻烦了：

```sh
file ./objs/kernel.elf
target remote localhost:1234
set disassembly-flavor intel
b kmain
```

## 一些有用tips

OS的代码中经常会有内联汇编，有的时候一条内联过去就崩溃了，所以在gdb里需要查看反汇编语句和registers。下面这些gdb指令比较有用：

```sh
(gdb) info line main.c:26  (查看main.c:26行在目标文件中的位置，为0x1cbc) 
Line 26 of "./kernel/main.c" starts at address 0x1cbc <kmain> and ends at 0x1cc2 <kmain+6>.

(gdb) info line *0x1cbc  (上面的反操作)
Line 26 of "./kernel/main.c" starts at address 0x1cbc <kmain> and ends at 0x1cc2 <kmain+6>.


(反汇编kmain函数，箭头指向的是当前运行的汇编代码)
(gdb) disas kmain  
Dump of assembler code for function kmain:
=> 0x00001cbc <+0>:	push   ebp
   0x00001cbd <+1>:	mov    ebp,esp
   0x00001cbf <+3>:	sub    esp,0x28
   0x00001cc2 <+6>:	mov    eax,DWORD PTR [ebp+0x8]
   0x00001cc5 <+9>:	mov    ds:0x5ccc,eax
   0x00001cca <+14>:	call   0x2a29 <init_video>
   0x00001ccf <+19>:	mov    DWORD PTR [esp+0x4],0xb
   0x00001cd7 <+27>:	mov    DWORD PTR [esp],0x4777
   0x00001cde <+34>:	call   0x2a40 <puts_color_str>
   0x00001ce3 <+39>:	mov    DWORD PTR [esp+0x4],0xa
   0x00001ceb <+47>:	mov    DWORD PTR [esp],0x478d
   0x00001cf2 <+54>:	call   0x2a40 <puts_color_str>
   0x00001cf7 <+59>:	cli    
   0x00001cf8 <+60>:	call   0x3876 <time_init>
   0x00001cfd <+65>:	call   0xc13 <gdt_init>
```

要正确的看到反汇编最好设置好gdb里面的汇编指令集，对于Nasm设置”set disassembly-flavor intel”,在.gdbinit里面弄好就行。
最后info registers查看cpu寄存器内容，info registers %eax只查看eax内容，而 info all-registers会把cpu的所有寄存器内容显示出来，不过cr0,cr3这些貌似没有 :(。看看这里GDB参考。


