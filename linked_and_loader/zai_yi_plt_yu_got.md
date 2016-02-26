# 再議PLT與GOT


PLT（Procedure Linkage Table）的作用是將位置無關的符號轉移到絕對地址。當一個外部符號被調用時，PLT 去引用 GOT 中的其符號對應的絕對地址，然後轉入並執行。

GOT（Global Offset Table）：用於記錄在 ELF 文件中所用到的共享庫中符號的絕對地址。在程序剛開始運行時，GOT 表項是空的，當符號第一次被調用時會動態解析符號的絕對地址然後轉去執行，並將被解析符號的絕對地址記錄在 GOT 中，第二次調用同一符號時，由於 GOT 中已經記錄了其絕對地址，直接轉去執行即可（不用重新解析）。

其中PLT 對應.plt section ，而GOT對應 .got.plt ，主要對應函數的絕對地址。通過readelf查看可執行文件，我們發現還存在.got section，這個section主要對應動態鏈接庫中變量的絕對地址。我們還要注意PLT section在代碼鏈接的時候已經存在，存在於代碼段中，而GOT存在於數據段中。

在我先前的博文ELF文件的加載，我簡單的對PLT和GOT進行了介紹。這裡我們增加複雜度，重新對這個動態鏈接機制進行分析。

- foo.c

```c
#include <stdio.h>
 
void foo(int i)
{
     printf("Test %d\n",i);
}
```

- main.c

```c
#include <stdio.h>
#include <unistd.h>
 
extern void foo(int i);
 
int main()
{
     printf("Test 1\n");
     printf("Test 2\n");
     foo(3);
     foo(4);
     return 0;
}
```

然後我們將foo.c 編譯成為liba.so: gcc -shared -fPIC -g3 -o libfoo.so foo.c
下面編譯主main函數：gcc -Wall -g3 -o main main.c -lfoo -L /tmp/libfoo.so

下面我們對這個main進行調試,我們在printf與foo上都打上斷點,然後開始運行，調試


```sh
(gdb) disassemble
Dump of assembler code for function main:
   0x00000000004006e6 <+0>:   push   %rbp
   0x00000000004006e7 <+1>:   mov    %rsp,%rbp
=> 0x00000000004006ea <+4>:    mov    $0x4007a0,%edi
   0x00000000004006ef <+9>:   callq  0x4005b0 <puts@plt>
   0x00000000004006f4 <+14>:  mov    $0x4007a0,%edi
   0x00000000004006f9 <+19>:  callq  0x4005b0 <puts@plt>
   0x00000000004006fe <+24>:  callq  0x4005e0 <foo@plt>
   0x0000000000400703 <+29>:  callq  0x4005e0 <foo@plt>
   0x0000000000400708 <+34>:  mov    $0x0,%eax
   0x000000000040070d <+39>:  pop    %rbp
   0x000000000040070e <+40>:  retq
End of assembler dump.
...
(gdb) si
0x00000000004005b0 in puts@plt ()
```

這裡我們進入了.plt 中尋找puts函數，查看0x601018中的值，我們發現地址0x004005b6，而0x004005b6正是jmpq的下一條指令，執行完跳如GOT表中查找函數絕對地址。這樣做避免了GOT表表是否為真實值檢查，如果為空那麼尋址，否則直接調用。

```sh
(gdb) disassemble
Dump of assembler code for function puts@plt:
=> 0x00000000004005b0 <+0>:    jmpq   *0x200a62(%rip)        # 0x601018 <puts@got.plt>
   0x00000000004005b6 <+6>:   pushq  $0x0
   0x00000000004005bb <+11>:  jmpq   0x4005a0
End of assembler dump.
(gdb) x/32x 0x601018
0x601018 <puts@got.plt>:  0x004005b6  0x00000000  0x25e20610  0x0000003e
0x601028 <__gmon_start__@got.plt>:    0x004005d6  0x00000000  0x004005e6  0x00000000
...
End of assembler dump.
(gdb) si
0x00000000004005a0 in ?? ()
(gdb)
0x00000000004005a6 in ?? ()
(gdb)
0x0000003e25615b70 in _dl_runtime_resolve () from /lib64/ld-linux-x86-64.so.2
(gdb) disassemble
Dump of assembler code for function _dl_runtime_resolve:
=> 0x0000003e25615b70 <+0>:    sub    $0x78,%rsp
   0x0000003e25615b74 <+4>:   mov    %rax,0x40(%rsp)
...
   0x0000003e25615b9c <+44>:  bndmov %bnd1,0x10(%rsp)
   0x0000003e25615ba2 <+50>:  bndmov %bnd2,0x20(%rsp)
   0x0000003e25615ba8 <+56>:  bndmov %bnd3,0x30(%rsp)
   0x0000003e25615bae <+62>:  mov    0x8i0(%rsp),%rsi
   0x0000003e25615bb6 <+70>:  mov    0x78(%rsp),%rd
   0x0000003e25615bbb <+75>:  callq  0x3e2560e990 <_dl_fixup>
...
```

這個_dl_runtime_resolve 來自於ld-linux-x86-64.so.2文件，然後在ld中調用_dl_fixup 將真實的puts函數地址填入GOT表中,當程序再次調入puts函數中時，直接jmpq跳轉到0x25e6fa70地址執行。

```sh
(gdb) disassemble 
Dump of assembler code for function puts@plt:
=> 0x00000000004005b0 <+0>:    jmpq   *0x200a62(%rip)        # 0x601018 <puts@got.plt>
   0x00000000004005b6 <+6>:   pushq  $0x0
   0x00000000004005bb <+11>:  jmpq   0x4005a0
End of assembler dump.
(gdb) x/32 0x601018
0x601018 <puts@got.plt>:  0x25e6fa70  0x0000003e  0x25e20610  0x0000003e
0x601028 <__gmon_start__@got.plt>:    0x004005d6  0x00000000  0x004005e6  0x00000000
(gdb) n
Single stepping until exit from function _dl_runtime_resolve,
which has no line number information.
0x0000003e25e6fa70 in puts () from /lib64/libc.so.6
```

下面來說明foo的執行：當代碼第一次執行foo函數，進程查找GOT表，找不到該函數，這個時候跳轉到PLT[0] 使用_dl_runtime_resolve查找foo函數的絕對地址，當找到該函數絕對地址後，進入foo函數執行，foo函數中存在printf () 函數，這個函數和之前main函數中的printf() 不同，重新使用_dl_runtime_resolve 查找libc中的puts函數，將其插入到GOT表中。


```sh
(gdb) disassemble
Dump of assembler code for function foo@plt:
   0x00000000004005e0 <+0>:   jmpq   *0x200a4a(%rip)        # 0x601030 <foo@got.plt>
=> 0x00000000004005e6 <+6>:    pushq  $0x3
   0x00000000004005eb <+11>:  jmpq   0x4005a0
End of assembler dump.
...
(gdb)
0x00007ffff7df85a0 in puts@plt () from libfoo.so
(gdb)
0x00007ffff7df85a6 in puts@plt () from libfoo.so
(gdb)
0x00007ffff7df85ab in puts@plt () from libfoo.so
(gdb)
0x00007ffff7df8590 in ?? () from libfoo.so
(gdb)
0x00007ffff7df8596 in ?? () from libfoo.so
(gdb)
0x0000003e25615b70 in _dl_runtime_resolve () from /lib64/ld-linux-x86-64.so.2
```

當再次使用libfoo.so 中的foo函數，直接跳轉GOT執行即可,無需再次查找。


```sh
(gdb)
0x00007ffff7df86db  5       printf(...);
(gdb)
0x00007ffff7df85a0 in puts@plt () from libfoo.so
(gdb)
0x0000003e25e6fa70 in puts () from /lib64/libc.so.6
...
```

##總結：

`ld-linux-x86-64.so.2 是一個動態鏈接庫，負責查找程序所使用的函數絕對地址，並將其寫入到GOT表中，以供後續調用`。其中GOT[0]為空，GOT[1]和GOT[2]用於保存查找的絕對函數地址，GOT[1]保存的是一個地址，指向已經加載的共享庫的鏈表地址；GOT[2]保存的是一個函數的地址，定義如下：GOT[2] = &_dl_runtime_resolve，這個函數的主要作用就是找到某個符號的地址，並把它寫到與此符號相關的GOT項中，然後將控制轉移到目標函數，而後面的GOT[3]，GOT[4]…都是通過_dl_fixup 添加的。



![](./images/289baeed-3f91-3651-b81b-159632d1cf45.png)





##參考：
http://www.lizhaozhong.info/archives/524
http://flint.cs.yale.edu/cs422/doc/ELF_Format.pdf
http://rickgray.me/2015/08/07/use-gdb-to-study-got-and-plt.html
http://blog.csdn.net/anzhsoft/article/details/18776111

