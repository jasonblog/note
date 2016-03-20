# linux 下動態鏈接實現原理


##符號重定位

講動態鏈接之前，得先說說符號重定位。

c/c++ 程序的编译是以文件为单位进行的，因此每个 c/cpp 文件也叫作一个编译单元(translation unit), 源文件先是被编译成一个个目标文件, 再由链接器把这些目标文件组合成一个可执行文件或库，链接的过程，其核心工作是解决模块间各种符号(变量，函数)相互引用的问题，对符号的引用本质是对其在内存中具体地址的引用，因此确定符号地址是编译，链接，加载过程中一项不可缺少的工作，这就是所谓的符号重定位。本质上来说，符号重定位要解决的是当前编译单元如何访问`「外部」`符号这个问题。

因为编译是以源文件为单位进行的，编译器此时并没有一个全局的视野，因此对一个编译单元内的符号它是无力确定其最终地址的，而对于可执行文件来说，在现代操作系统上，程序加载运行的地址是固定或可以预期的，因此在链接时，链接器可以直接计算分配该文件内各种段的绝对或相对地址。所以对于可执行文件来说，`符号重定位是在链接时完成的`(如果可执行文件引用了动态库里的函数，则情况稍有不同)。但对于动态链接库来说，因为动态库的加载是在运行时，且加载的地址不固定，因此没法事先确定该模块的起始地址，所以对动态库的符号重定位，只能推迟。

符号重定位既指在当前目标文件内进行重定位，也包括在不同目标文件，甚至不同模块间进行重定位，这里面有什么不同吗？如果是同一个目标文件内，或者在同一个模块内，链接后，各个符号的相对地址就已经确定了，看起来似乎不用非得要知道最后的绝对地址才能引用这些符号，这说起来好像也有道理，但事实不是这样，x86 上 mov 之类访问程序中数据段的指令，它要求操作数是绝对地址，而对于函数调用，虽然是以相对地址进行调用，但计算相对地址也只限于在当前目标文件内进行，跨目标文件跨模块间的调用，编译期也是做不到的，只能等链接时或加载时才能进行相对地址的计算，因此重定位这个过程是不能缺少的，事实上目前来说，对于动态链接即使是当前目标文件内，如果是全局非静态函数，那么它也是需要进行重定位的，当然这里面有别的原因，比如说使得能实现 LD_PRELOAD 的功能等。


##链接时符号重定位

链接时符号重定位指的是在链接阶段对符号进行重定位，一般来说，构建一个可执行文件可以简单分为两个步骤：编译及链接，如下例子，我们尝试使用静态链接的方式构建一个可执行文件：


```c
// file: a.c
int g_share = 1;

int g_func(int a)
{
    g_share += a;
    return a * 3;
}

// file: main.c
extern int g_share;
extern int g_func(int a);

int main()
{
    int a = 42;
    a = g_func(a);
    return 0;
}
```

正如前面所说，此时符号的重定位在链接时进行，那么在编译时，编译器是怎么生成代码来引用那些还没有重定位的符号呢？让我们先编译一下，再来看看目标文件的内容


```sh
// x86_64, linux 2.6.9
-bash-3.00$ gcc -c a.c main.c -g
-bash-3.00$ objdump -S a.o
```

然后得到如下输出(对于 main.o 中对 g_func 的引用，实现是一样的，故略)：

```sh
a.o:     file format elf64-x86-64

Disassembly of section .text:

0000000000000000 <g_func>:
int g_share = 1;

int g_func(int a)
{
    0:   55                      push   %rbp
    1:   48 89 e5                mov    %rsp,%rbp
    4:   89 7d fc                mov    %edi,0xfffffffffffffffc(%rbp)
    g_share += a;
    7:   8b 45 fc                mov    0xfffffffffffffffc(%rbp),%eax
    a:   01 05 00 00 00 00       add    %eax,0(%rip)        # 10 <g_func+0x10>
    return a * 2;
    10:   8b 45 fc                mov    0xfffffffffffffffc(%rbp),%eax
    13:   01 c0                   add    %eax,%eax
}

15:   c9                      leaveq
16:   c3                      retq
```

从中可以看到，目标文件里的 .txt 段地址从 0 开始，其中地址为7的指令用于把参数 a 放到寄存器 %eax 中，而地址 a 处的指令则把 %eax 中的内容与 g_share 相加，注意这里 g_share 的地址为：0(%rip). 显然这个地址是错的，编译器当前并不知道 g_share 这个变量最后会被分配到哪个地址上，因此在这儿只是随便用一个假的来代替，等着到接下来链接时，再把该处地址进行修正。那么，链接器怎么知道目标文件中哪些地方需要修正呢？很简单，编译器编译文件时时，会建立一系列表项，用来记录哪些地方需要在重定位时进行修正，这些表项叫作“重定位表”(relocatioin table)：


```sh
-bash-3.00$ objdump -r a.o
```

```sh
a.o:     file format elf64-x86-64

RELOCATION RECORDS FOR [.text]:
OFFSET           TYPE              VALUE
000000000000000c R_X86_64_PC32     g_share+0xfffffffffffffffc
```


如上最后一行，这条记录记录了在当前编译单元中，哪儿对 g_share 进行了引用，其中 offset 用于指明需要修改的位置在该段中的偏移，TYPE 则指明要怎样去修改，因为 cpu 的寻址方式不是唯一的，寻址方式不同，地址的形式也有所不同，这个 type 用于指明怎么去修改, value 则是配合 type 来最后计算该符号地址的。

有了如上信息，链接器在把目标文件合并成一个可执行文件并分配好各段的加载地址后，就可以重新计算那些需要重定位的符号的具体地址了, 如下我们可以看到在可执行文件中，对 g_share(0x40496处), g_func(0x4047a处)的访问已经被修改成了具体的地址：

```sh
-bash-3.00$ gcc -o am a.o main.o
-bash-3.00$ objdump -S am
```

```sh
// skip some of the ouput
 
extern int g_func(int a);

int main()
{
    400468:       55                      push   %rbp
    400469:       48 89 e5                mov    %rsp,%rbp
    40046c:       48 83 ec 10             sub    $0x10,%rsp
    int a = 42;
    400470:       c7 45 fc 2a 00 00 00    movl   $0x2a,0xfffffffffffffffc(%rbp)
    a = g_func(a);
    400477:       8b 7d fc                mov    0xfffffffffffffffc(%rbp),%edi
    40047a:       e8 0d 00 00 00          callq  40048c <g_func>
    40047f:       89 45 fc                mov    %eax,0xfffffffffffffffc(%rbp)
    return 0;
    400482:       b8 00 00 00 00          mov    $0x0,%eax
}
400487:       c9                      leaveq
400488:       c3                      retq
400489:       90                      nop
40048a:       90                      nop
40048b:       90                      nop

000000000040048c <g_func>:
int g_share = 1;

int g_func(int a)
{
    40048c:       55                      push   %rbp
    40048d:       48 89 e5                mov    %rsp,%rbp
    400490:       89 7d fc                mov    %edi,0xfffffffffffffffc(%rbp)
    g_share += a;
    400493:       8b 45 fc                mov    0xfffffffffffffffc(%rbp),%eax
    400496:       01 05 dc 03 10 00       add    %eax,1049564(%rip)        # 500878 <g_share>
    return a * 2;
    40049c:       8b 45 fc                mov    0xfffffffffffffffc(%rbp),%eax
    40049f:       01 c0                   add    %eax,%eax
}
4004a1:       c9                      leaveq
4004a2:       c3                      retq

// skip some of the ouput
```

当然，重定位时修改指令的具体方式还牵涉到比较多的细节很啰嗦，这里就不细说了。

##加载时符号重定位

前面描述了静态链接时，怎么解决符号重定位的问题，那么当我们使用动态链接来构建程序时，这些符号重定位问题是怎么解决的呢？目前来说，Linux 下 ELF 主要支持两种方式：加载时符号重定位及地址无关代码。地址无关代码接下来会讲，对于加载时重定位，其原理很简单，它与链接时重定位是一致的，只是把重定位的时机放到了动态库被加载到内存之后，由动态链接器来进行。


```c
int g_share = 1;

int g_func(int a)
{
    g_share += a;
    return a * 2;
}

int g_func2()
{
    int a = 2;
    int b = g_func(3);

    return a + b;
}
```

```sh
// compile on 32bit linux OS
-bash-3.00$ gcc -c a.c main.c
-bash-3.00$ gcc -shared -o liba.so a.o
-bash-3.00$ gcc -o am main.o -L. -la
-bash-3.00$ objdump -S liba.so
```

```sh
// skip some of the output
000004f4 <g_func>:
int g_share = 1;

int g_func(int a)
{
    4f4:   55                      push   %ebp
    4f5:   89 e5                   mov    %esp,%ebp
    g_share += a;
    4f7:   8b 45 08                mov    0x8(%ebp),%eax
    4fa:   01 05 00 00 00 00       add    %eax,0x0
    return a * 2;
    500:   8b 45 08                mov    0x8(%ebp),%eax
    503:   d1 e0                   shl    %eax
}
505:   c9                      leave  
506:   c3                      ret    

00000507 <g_func2>:

int g_func2()
{
    507:   55                      push   %ebp
    508:   89 e5                   mov    %esp,%ebp
    50a:   83 ec 08                sub    $0x8,%esp
    int a = 2;
    50d:   c7 45 fc 02 00 00 00    movl   $0x2,0xfffffffc(%ebp)
    int b = g_func(3);
    514:   6a 03                   push   $0x3
    516:   e8 fc ff ff ff          call   517 <g_func2+0x10>
    51b:   83 c4 04                add    $0x4,%esp
    51e:   89 45 f8                mov    %eax,0xfffffff8(%ebp)

    return a + b;
    521:   8b 45 f8                mov    0xfffffff8(%ebp),%eax
    524:   03 45 fc                add    0xfffffffc(%ebp),%eax
}
527:   c9                      leave  

// skip some of the output
```

注意其中地址 4fa 及 516 处的指令：此两处分别对 g_share 及 g_func 进行了访问，显然此时它们的地址仍然是假地址，这些地址在动态库加载完成后会被动态链接器进行重定位，最终修改为正确的地址，这看起来与静态链接时进行重定位是一样的过程，但实现上有几个关键的不同之处：

- 因为不允许对可执行文件的代码段进行加载时符号重定位，因此如果可执行文件引用了动态库中的数据符号，则在该可执行文件内对符号的重定位必须在链接阶段完成，为做到这一点，链接器在构建可执行文件的时候，会在当前可执行文件的数据段里分配出相应的空间来作为该符号真正的内存地址，等到运行时加载动态库后，再在动态库中对该符号的引用进行重定位：把对该符号的引用指向可执行文件数据段里相应的区域。

- ELF 文件对动态库中的函数调用采用了所谓的`"延迟绑定"(lazy binding)`, 动态库中的函数在其第一次调用发生时才去查找其真正的地址，因此我们不需要在调用动态库函数的地方直接填上假的地址，而是使用了一些跳转地址作为替换，这里先不细说。

至此，我们可以发现加载时重定位实际上是一个重新修改动态库代码的过程，但我们知道，不同的进程即使是对同一个动态库也很可能是加载到不同地址上，因此当以加载时重定位的方式来使用动态库时，该动态库就没法做到被各个进程所共享，而只能在每个进程中 copy 一份：因为符号重定位后，该动态库与在别的进程中就不同了，可见此时动态库节省内存的优势就不复存在了。


##地址无关代码(PIC, position independent code)

从前面的介绍我们知道装载时重定位有重大的`缺点`：

- 它不能使动态库的指令代码被共享。
- 程序启动加载动态库后，符号重定位会比较花时间，特别是动态库多且复杂的情况下。
为了克服这些缺陷，ELF 引用了一种叫作地址无关代码的实现方案，该解决方案通过对变量及函数的访问加一层跳转来实现，非常的灵活。


###1.模块内部符号的访问

模块内部符号在这里指的是：static 类型的变量与函数，这种类型的符号比较简单，对于 static 函数来说，因为在动态库编译完后，它在模块内的相对地址就已经确定了，而 x86 上函数调用只用到相对地址，因此此时根本连重定位都不需要进行，编译时就能确定地址，稍微麻烦一点的是访问数据，因为访问数据需要绝对地址，但动态库未被加载时，绝对地址是没法得知的，怎么办呢？

ELF 在这里使用了一个小技巧，根据当前 IP 值来动态计算数据的绝对地址，它的原理很简单，当动态库编译好之后，库中的数据段，代码段的相对位置就已经固定了，此时对任意一条指令来说，该指令的地址与数据段的距离都是固定的，那么，只要程序在运行时获取到当前指令的地址，就可以直接加上该固定的位移，从而得到所想要访问的数据的绝对地址了，下面我们用实例验证一下：


```c
int g_share = 1;
static int g_share2 = 2;

int g_func(int a)
{
    g_share += a;
    return a * 2;
}

int g_func2()
{
    int a = 2;
    int b = g_func(3);

    return a + b;
}

static int g_fun3()
{
    g_share2 += 3;
    return g_share2 - 1;
}

static int g_func4()
{
    int a = g_fun3();

    a + 2;
    return a;
}
```

以上代码在x86 linux 下编译，再反汇编看看得到如下结果：

```sh
-bash-3.00$ gcc -o liba.so -fPIC -shared a.c
-bash-3.00$ objdump -S liba.so 
```

```sh
// skip some of the output
00000564 <g_fun3>:
 564:   55                      push   %ebp
 565:   89 e5                   mov    %esp,%ebp
 567:   e8 00 00 00 00          call   56c <g_fun3+0x8>
 56c:   59                      pop    %ecx
 56d:   81 c1 60 11 00 00       add    $0x1160,%ecx
 573:   83 81 20 00 00 00 03    addl   $0x3,0x20(%ecx)
 57a:   8b 81 20 00 00 00       mov    0x20(%ecx),%eax
 580:   48                      dec    %eax
 581:   c9                      leave  
 582:   c3                      ret    
// skip some of the output
```

现在我们来分析验证一下：首先是地址 567 的指令有些怪，这儿不深究，简单来说，x86 下没有指令可以取当前 ip 的值，因此这儿使了个技巧通过函数调用来获取 ip 值(x86_64 下就不用这么麻烦)，这个技巧的原理在于进行函数调用时要将返回地址压到栈上，此时通过读这个栈上的值就可以获得下一条指令的地址了，在这儿我们只要知道指令 56c 执行后，%ecx 中包含了当前指令的地址，也就是 0x56c，再看 56d 及 573 两条指令，得知 %ecx + 0x1160 + 0x20 = 0x16ec 就是 573 指令所需要访问的地址，这个地址指向哪里了呢？


```sh
-bash-3.00$ objdump -s liba.so
```

```sh
Contents of section .data:
 16e0 e0160000 f4150000 01000000 02000000  ................
 ```
 
结果是数据段里的第二个 int，也就是 g_share2!


### 2.模块间符号的访问

模块间的符号访问比模块内的符号访问要麻烦很多，因为动态库运行时被加载到哪里是未知的，为了能使得代码段里对数据及函数的引用与具体地址无关，只能再作一层跳转，ELF 的做法是在动态库的数据段中加一个表项，叫作 GOT(global offset table), GOT 表格中放的是数据全局符号的地址，该表项在动态库被加载后由动态加载器进行初始化，动态库内所有对数据全局符号的访问都到该表中来取出相应的地址，即可做到与具体地址了，而该表作为动态库的一部分，访问起来与访问模块内的数据是一样的。

仍然使用前面的例子，我们来看看 g_func 是怎么访问 g_share 变量的。

```sh
00000504 <g_func>:
 504:   55                      push   %ebp
 505:   89 e5                   mov    %esp,%ebp
 507:   53                      push   %ebx
 508:   e8 00 00 00 00          call   50d <g_func+0x9>
 50d:   5b                      pop    %ebx
 50e:   81 c3 bf 11 00 00       add    $0x11bf,%ebx
 514:   8b 8b f0 ff ff ff       mov    0xfffffff0(%ebx),%ecx
 51a:   8b 93 f0 ff ff ff       mov    0xfffffff0(%ebx),%edx
 520:   8b 45 08                mov    0x8(%ebp),%eax
 523:   03 02                   add    (%edx),%eax
 525:   89 01                   mov    %eax,(%ecx)
 527:   8b 45 08                mov    0x8(%ebp),%eax
 52a:   d1 e0                   shl    %eax
 52c:   5b                      pop    %ebx
 52d:   c9                      leave  
 52e:   c3                      ret    
```

上面的输出中，508 与 50d 处的指令用于获取 ip 值， 执行完 50d 后， %ebx 中放的是 0x50d， 地址 50e 用于计算 g_share 在 GOT 中的地址 0x50d + 0x11bf + 0xfffffff0 = 0x16bc, 我们检查一下该地址是不是 GOT：

```sh
-bash-3.00$ objdump -h liba.so
```

```sh
liba.so:     file format elf32-i386

Sections:
Idx Name          Size      VMA       LMA       File off  Algn
//skip some of the output

 16 .got          00000010  000016bc  000016bc  000006bc  2**2
                  CONTENTS, ALLOC, LOAD, DATA
```

显然，0x16bc 就是 GOT 表的第一项。

事实上，ELF 文件中还包含了一个重定位段，里面记录了哪些符号需要进行重定位，我���可以通过它验证一下上面的计算是否与之匹配：


```sh
-bash-3.00$ objdump -R liba.so
```


```sh
liba.so:     file format elf32-i386

DYNAMIC RELOCATION RECORDS
OFFSET   TYPE              VALUE
000016e0 R_386_RELATIVE    *ABS*
000016e4 R_386_RELATIVE    *ABS*
000016bc R_386_GLOB_DAT    g_share
000016c0 R_386_GLOB_DAT    __cxa_finalize
000016c4 R_386_GLOB_DAT    _Jv_RegisterClasses
000016c8 R_386_GLOB_DAT    __gmon_start__
000016d8 R_386_JUMP_SLOT   g_func
000016dc R_386_JUMP_SLOT   __cxa_finalize
```

如上输出， g_share 的地址在 0x16bc，与前面的计算完全吻合！
致此，模块间的数据访问就介绍完了，模块间的函数调用在实现原理上是一样的，也需要经过一个类似 GOT 的表格进行跳转，但在具体实现上，ELF 为了实现所谓延迟绑定而作了更精细的处理，接下来会介绍。

## 延迟加载

我们知道，动态库是在程序启动的时候加载进来的，加载后，动态链接器需要对其作一系列的初始化，如符号重定位，这些工作是比较费时的，特别是对函数的重定位，那么我们能不能把对函数的重定位延迟进行呢？这个改进是很有意义的，毕竟很多时候，一个动态库里可能包含很多的全局函数，但是我们往往可能只用到了其中一小部分而已，完全没必要把那些没用到的函数也过早进行重定位，具体来说，就是应该等到第一次发生对该函数的调用时才进行符号绑定 -- 此谓之延迟绑定。

延迟绑定的实现步骤如下：

- 建立一个 GOT.PLT 表，该表用来放全局函数的实际地址，但最开始时，该里面放的不是真实的地址而是一个跳转，接下来会讲。
- 对每一个全局函数，链接器生成一个与之相对应的影子函数，如 fun@plt。
- 所有对 fun 的调用，都换成对 fun@plt 的调用，每个`fun@plt`长成如下样子：

```sh
fun@plt:
jmp *(fun@got.plt)
push index
jmp _init
```

其中第一条指令直接从 got.plt 中去拿真实的函数地址，如果已经之前已经发生过调用，got.plt 就已经保存了真实的地址，如果是第一次调用，则 got.plt 中放的是 fun@plt 中的第二条指令，这就使得当执行第一次调用时，fun@plt中的第一条指令其实什么事也没做，直接继续往下执行，第二条指令的作用是把当前要调用的函数在 got.plt 中的编号作为参数传给 _init()，而 _init() 这个函数则用于把 fun 进行重定位，然后把结果写入到 got.plt 相应的地方，最后直接跳过去该函数。

```sh
0000052f <g_func2>:
 52f:   55                      push   %ebp
 530:   89 e5                   mov    %esp,%ebp
 532:   53                      push   %ebx
 533:   83 ec 14                sub    $0x14,%esp
 536:   e8 00 00 00 00          call   53b <g_func2+0xc>
 53b:   5b                      pop    %ebx
 53c:   81 c3 91 11 00 00       add    $0x1191,%ebx
 542:   c7 45 f8 02 00 00 00    movl   $0x2,0xfffffff8(%ebp) // a = 2
 549:   83 ec 0c                sub    $0xc,%esp
 54c:   6a 03                   push   $0x3 // push argument 3 for g_func.
 54e:   e8 d5 fe ff ff          call   428 <g_func@plt>
 553:   83 c4 10                add    $0x10,%esp
 556:   89 45 f4                mov    %eax,0xfffffff4(%ebp)
 559:   8b 45 f4                mov    0xfffffff4(%ebp),%eax
 55c:   03 45 f8                add    0xfffffff8(%ebp),%eax
 55f:   8b 5d fc                mov    0xfffffffc(%ebp),%ebx
 562:   c9                      leave  
 563:   c3                      ret 
 ```
 
 如上汇编，指令 536, 53b, 53c, 用于计算 got.plt 的具体位置，计算方式与前面对数据的访问原理是一样的，经计算此时, %ebx = 0x53b + 0x1191 = 0x16cc, 注意指令 54e， 该指令调用了函数 g_func@plt:
 
 
 ```sh
 00000428 <g_func@plt>:
 428:   ff a3 0c 00 00 00       jmp    *0xc(%ebx)
 42e:   68 00 00 00 00          push   $0x0
 433:   e9 e0 ff ff ff          jmp    418 <_init+0x18>
 ```
 
 注意到此时， %ebx 中放的是 got.plt 的地址，g_func@plt 的第一条指令用于获取 got.plt 中 func 的具体地址， func 放在 0xc + %ebx = 0xc + 0x16cc = 0x16d8, 这个地址里放的是什么呢？我们查一下重定位表：
 
 
 ```sh
 -bash-3.00$ objdump -R liba.so

liba.so:     file format elf32-i386

DYNAMIC RELOCATION RECORDS
OFFSET   TYPE              VALUE
000016e0 R_386_RELATIVE    *ABS*
000016e4 R_386_RELATIVE    *ABS*
000016bc R_386_GLOB_DAT    g_share
000016c0 R_386_GLOB_DAT    __cxa_finalize
000016c4 R_386_GLOB_DAT    _Jv_RegisterClasses
000016c8 R_386_GLOB_DAT    __gmon_start__
000016d8 R_386_JUMP_SLOT   g_func
000016dc R_386_JUMP_SLOT   __cxa_finalize
```

可见，该地址里放的就是 g_func 的具体地址，那此时 0x16d8 放的是真正的地址了吗？我们再看看 got.plt:

```sh
Contents of section .got.plt:
 16cc fc150000 00000000 00000000 2e040000  ................
 16dc 3e040000
```


16d8 处的内容是: 2e040000, 小端序，换回整形就是 0x000042e, 该地址就是 `fun@plt` 的第二条指令！是不是觉得有点儿绕？你可以定下心来再看一遍，其实不绕，而是很巧妙。

##后话

对动态链接库来说，加载时重定位与链接时重定位各有优缺点，前者使得动态库的代码段不能被多个进程间所共享，加载动态库时也比较费时，但是加载完成后，因为对符号的引用不需要进行跳转，程序运行的效率相对是较高的。而对地址无关的代码，它的缺点是动态库的体积相对较大，毕竟增加了很多表项及相关的函数，另外就运行时对全局符号的引用需要通过表格进行跳转，程序执行的效率不可避免有所损失，优点嘛，就是动态库加载比较快，而且代码可以在多个进程间共享，对整个系统而言，可以大大节约对内存的使用，这个好处的吸引力是非常大的，所以你可以看到，目前来说在常用的动态库使用上，PIC 相较而言是更加被推崇的，道理在此。


##【引用】
《程序员的自我修养》

http://eli.thegreenplace.net/2011/08/25/load-time-relocation-of-shared-libraries/

http://www.iecc.com/linker/linker10.html

https://www.technovelty.org/c/position-independent-code-and-x86-64-libraries.html
