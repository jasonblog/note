# linux 下動態鏈接實現原理


##符號重定位

講動態鏈接之前，得先說說符號重定位。

c/c++ 程序的編譯是以文件為單位進行的，因此每個 c/cpp 文件也叫作一個編譯單元(translation unit), 源文件先是被編譯成一個個目標文件, 再由鏈接器把這些目標文件組合成一個可執行文件或庫，鏈接的過程，其核心工作是解決模塊間各種符號(變量，函數)相互引用的問題，對符號的引用本質是對其在內存中具體地址的引用，因此確定符號地址是編譯，鏈接，加載過程中一項不可缺少的工作，這就是所謂的符號重定位。本質上來說，符號重定位要解決的是當前編譯單元如何訪問`「外部」`符號這個問題。

因為編譯是以源文件為單位進行的，編譯器此時並沒有一個全局的視野，因此對一個編譯單元內的符號它是無力確定其最終地址的，而對於可執行文件來說，在現代操作系統上，程序加載運行的地址是固定或可以預期的，因此在鏈接時，鏈接器可以直接計算分配該文件內各種段的絕對或相對地址。所以對於可執行文件來說，`符號重定位是在鏈接時完成的`(如果可執行文件引用了動態庫裡的函數，則情況稍有不同)。但對於動態鏈接庫來說，因為動態庫的加載是在運行時，且加載的地址不固定，因此沒法事先確定該模塊的起始地址，所以對動態庫的符號重定位，只能`推遲`。

符號重定位既指在當前目標文件內進行重定位，也包括在不同目標文件，甚至不同模塊間進行重定位，這裡面有什麼不同嗎？如果是同一個目標文件內，或者在同一個模塊內，鏈接後，各個符號的相對地址就已經確定了，看起來似乎不用非得要知道最後的絕對地址才能引用這些符號，這說起來好像也有道理，但事實不是這樣，x86 上 mov 之類訪問程序中數據段的指令，它要求操作數是絕對地址，而對於函數調用，雖然是以相對地址進行調用，但計算`相對地址也只限於在當前目標文件內進行，跨目標文件跨模塊間的調用，編譯期也是做不到的，只能等鏈接時或加載時才能進行相對地址的計算`，因此重定位這個過程是不能缺少的，事實上目前來說，對於動態鏈接即使是當前目標文件內，如果是`全局非靜態函數，那麼它也是需要進行重定位的`，當然這裡面有別的原因，比如說使得能實現 LD_PRELOAD 的功能等。


##鏈接時符號重定位

鏈接時符號重定位指的是在鏈接階段對符號進行重定位，一般來說，構建一個可執行文件可以簡單分為兩個步驟：編譯及鏈接，如下例子，我們嘗試使用靜態鏈接的方式構建一個可執行文件：


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

正如前面所說，此時符號的重定位在鏈接時進行，那麼在編譯時，編譯器是怎麼生成代碼來引用那些還沒有重定位的符號呢？讓我們先編譯一下，再來看看目標文件的內容


```sh
// x86_64, linux 2.6.9
-bash-3.00$ gcc -c a.c main.c -g
-bash-3.00$ objdump -S a.o
```

然後得到如下輸出(對於 main.o 中對 g_func 的引用，實現是一樣的，故略)：

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

從中可以看到，目標文件裡的 .txt 段地址從 0 開始，其中地址為7的指令用於把參數 a 放到寄存器 %eax 中，而地址 a 處的指令則把 %eax 中的內容與 g_share 相加，注意這裡 g_share 的地址為：0(%rip). 顯然這個地址是錯的，編譯器當前並不知道 g_share 這個變量最後會被分配到哪個地址上，因此在這兒只是隨便用一個假的來代替，等著到接下來鏈接時，再把該處地址進行修正。那麼，鏈接器怎麼知道目標文件中哪些地方需要修正呢？很簡單，編譯器編譯文件時時，會建立一系列表項，用來記錄哪些地方需要在重定位時進行修正，這些表項叫作“重定位表”(relocatioin table)：


```sh
-bash-3.00$ objdump -r a.o
```

```sh
a.o:     file format elf64-x86-64

RELOCATION RECORDS FOR [.text]:
OFFSET           TYPE              VALUE
000000000000000c R_X86_64_PC32     g_share+0xfffffffffffffffc
```


如上最後一行，這條記錄記錄了在當前編譯單元中，哪兒對 g_share 進行了引用，其中 offset 用於指明需要修改的位置在該段中的偏移，TYPE 則指明要怎樣去修改，因為 cpu 的尋址方式不是唯一的，尋址方式不同，地址的形式也有所不同，這個 type 用於指明怎麼去修改, value 則是配合 type 來最後計算該符號地址的。

有了如上信息，鏈接器在把目標文件合併成一個可執行文件並分配好各段的加載地址後，就可以重新計算那些需要重定位的符號的具體地址了, 如下我們可以看到在可執行文件中，對 g_share(0x40496處), g_func(0x4047a處)的訪問已經被修改成了具體的地址：

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

當然，重定位時修改指令的具體方式還牽涉到比較多的細節很囉嗦，這裡就不細說了。

##加載時符號重定位

前面描述了靜態鏈接時，怎麼解決符號重定位的問題，那麼當我們使用動態鏈接來構建程序時，這些符號重定位問題是怎麼解決的呢？目前來說，Linux 下 ELF 主要支持兩種方式：加載時符號重定位及地址無關代碼。地址無關代碼接下來會講，對於加載時重定位，其原理很簡單，它與鏈接時重定位是一致的，只是把重定位的時機放到了動態庫被加載到內存之後，由動態鏈接器來進行。


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

注意其中地址 4fa 及 516 處的指令：此兩處分別對 g_share 及 g_func 進行了訪問，顯然此時它們的地址仍然是假地址，這些地址在動態庫加載完成後會被動態鏈接器進行重定位，最終修改為正確的地址，這看起來與靜態鏈接時進行重定位是一樣的過程，但實現上有幾個關鍵的不同之處：

- 因為不允許對可執行文件的代碼段進行加載時符號重定位，因此如果可執行文件引用了動態庫中的數據符號，則在該可執行文件內對符號的重定位必須在鏈接階段完成，為做到這一點，鏈接器在構建可執行文件的時候，會在當前可執行文件的數據段裡分配出相應的空間來作為該符號真正的內存地址，等到運行時加載動態庫後，再在動態庫中對該符號的引用進行重定位：把對該符號的引用指向可執行文件數據段裡相應的區域。

- ELF 文件對動態庫中的函數調用採用了所謂的`"延遲綁定"(lazy binding)`, 動態庫中的函數在其第一次調用發生時才去查找其真正的地址，因此我們不需要在調用動態庫函數的地方直接填上假的地址，而是使用了一些跳轉地址作為替換，這裡先不細說。

至此，我們可以發現加載時重定位實際上是一個重新修改動態庫代碼的過程，但我們知道，不同的進程即使是對同一個動態庫也很可能是加載到不同地址上，因此當以加載時重定位的方式來使用動態庫時，該動態庫就沒法做到被各個進程所共享，而只能在每個進程中 copy 一份：因為符號重定位後，該動態庫與在別的進程中就不同了，可見此時動態庫節省內存的優勢就不復存在了。


##地址無關代碼(PIC, position independent code)

從前面的介紹我們知道裝載時重定位有重大的`缺點`：

- 它不能使動態庫的指令代碼被共享。
- 程序啟動加載動態庫後，符號重定位會比較花時間，特別是動態庫多且複雜的情況下。
為了克服這些缺陷，ELF 引用了一種叫作地址無關代碼的實現方案，該解決方案通過對變量及函數的訪問加一層跳轉來實現，非常的靈活。


###1.模塊內部符號的訪問

模塊內部符號在這裡指的是：static 類型的變量與函數，這種類型的符號比較簡單，對於 static 函數來說，因為在動態庫編譯完後，它在模塊內的相對地址就已經確定了，而 x86 上函數調用只用到相對地址，因此此時根本連重定位都不需要進行，編譯時就能確定地址，稍微麻煩一點的是訪問數據，因為訪問數據需要絕對地址，但動態庫未被加載時，絕對地址是沒法得知的，怎麼辦呢？

ELF 在這裡使用了一個小技巧，根據當前 IP 值來動態計算數據的絕對地址，它的原理很簡單，當動態庫編譯好之後，庫中的數據段，代碼段的相對位置就已經固定了，此時對任意一條指令來說，該指令的地址與數據段的距離都是固定的，那麼，只要程序在運行時獲取到當前指令的地址，就可以直接加上該固定的位移，從而得到所想要訪問的數據的絕對地址了，下面我們用實例驗證一下：


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

以上代碼在x86 linux 下編譯，再反彙編看看得到如下結果：

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

現在我們來分析驗證一下：首先是地址 567 的指令有些怪，這兒不深究，簡單來說，x86 下沒有指令可以取當前 ip 的值，因此這兒使了個技巧通過函數調用來獲取 ip 值(x86_64 下就不用這麼麻煩)，這個技巧的原理在於進行函數調用時要將返回地址壓到棧上，此時通過讀這個棧上的值就可以獲得下一條指令的地址了，在這兒我們只要知道指令 56c 執行後，%ecx 中包含了當前指令的地址，也就是 0x56c，再看 56d 及 573 兩條指令，得知 %ecx + 0x1160 + 0x20 = 0x16ec 就是 573 指令所需要訪問的地址，這個地址指向哪裡了呢？


```sh
-bash-3.00$ objdump -s liba.so
```

```sh
Contents of section .data:
 16e0 e0160000 f4150000 01000000 02000000  ................
 ```
 
結果是數據段裡的第二個 int，也就是 g_share2!


### 2.模塊間符號的訪問

模塊間的符號訪問比模塊內的符號訪問要麻煩很多，因為動態庫運行時被加載到哪裡是未知的，為了能使得代碼段裡對數據及函數的引用與具體地址無關，只能再作一層跳轉，ELF 的做法是在動態庫的數據段中加一個表項，叫作 GOT(global offset table), GOT 表格中放的是數據全局符號的地址，該表項在動態庫被加載後由動態加載器進行初始化，動態庫內所有對數據全局符號的訪問都到該表中來取出相應的地址，即可做到與具體地址了，而該表作為動態庫的一部分，訪問起來與訪問模塊內的數據是一樣的。

仍然使用前面的例子，我們來看看 g_func 是怎麼訪問 g_share 變量的。

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

上面的輸出中，508 與 50d 處的指令用於獲取 ip 值， 執行完 50d 後， %ebx 中放的是 0x50d， 地址 50e 用於計算 g_share 在 GOT 中的地址 0x50d + 0x11bf + 0xfffffff0 = 0x16bc, 我們檢查一下該地址是不是 GOT：

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

顯然，0x16bc 就是 GOT 表的第一項。

事實上，ELF 文件中還包含了一個重定位段，裡面記錄了哪些符號需要進行重定位，我���可以通過它驗證一下上面的計算是否與之匹配：


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

如上輸出， g_share 的地址在 0x16bc，與前面的計算完全吻合！
致此，模塊間的數據訪問就介紹完了，模塊間的函數調用在實現原理上是一樣的，也需要經過一個類似 GOT 的表格進行跳轉，但在具體實現上，ELF 為了實現所謂延遲綁定而作了更精細的處理，接下來會介紹。

## 延遲加載

我們知道，動態庫是在程序啟動的時候加載進來的，加載後，動態鏈接器需要對其作一系列的初始化，如符號重定位，這些工作是比較費時的，特別是對函數的重定位，那麼我們能不能把對函數的重定位延遲進行呢？這個改進是很有意義的，畢竟很多時候，一個動態庫裡可能包含很多的全局函數，但是我們往往可能只用到了其中一小部分而已，完全沒必要把那些沒用到的函數也過早進行重定位，具體來說，就是應該等到第一次發生對該函數的調用時才進行符號綁定 -- 此謂之延遲綁定。

延遲綁定的實現步驟如下：

- 建立一個 GOT.PLT 表，該表用來放全局函數的實際地址，但最開始時，該裡面放的不是真實的地址而是一個跳轉，接下來會講。
- 對每一個全局函數，鏈接器生成一個與之相對應的影子函數，如 fun@plt。
- 所有對 fun 的調用，都換成對 fun@plt 的調用，每個`fun@plt`長成如下樣子：

```sh
fun@plt:
jmp *(fun@got.plt)
push index
jmp _init
```

其中第一條指令直接從 got.plt 中去拿真實的函數地址，如果已經之前已經發生過調用，got.plt 就已經保存了真實的地址，如果是第一次調用，則 got.plt 中放的是 fun@plt 中的第二條指令，這就使得當執行第一次調用時，fun@plt中的第一條指令其實什麼事也沒做，直接繼續往下執行，第二條指令的作用是把當前要調用的函數在 got.plt 中的編號作為參數傳給 _init()，而 _init() 這個函數則用於把 fun 進行重定位，然後把結果寫入到 got.plt 相應的地方，最後直接跳過去該函數。

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
 
 如上彙編，指令 536, 53b, 53c, 用於計算 got.plt 的具體位置，計算方式與前面對數據的訪問原理是一樣的，經計算此時, %ebx = 0x53b + 0x1191 = 0x16cc, 注意指令 54e， 該指令調用了函數 g_func@plt:
 
 
 ```sh
 00000428 <g_func@plt>:
 428:   ff a3 0c 00 00 00       jmp    *0xc(%ebx)
 42e:   68 00 00 00 00          push   $0x0
 433:   e9 e0 ff ff ff          jmp    418 <_init+0x18>
 ```
 
 注意到此時， %ebx 中放的是 got.plt 的地址，g_func@plt 的第一條指令用於獲取 got.plt 中 func 的具體地址， func 放在 0xc + %ebx = 0xc + 0x16cc = 0x16d8, 這個地址裡放的是什麼呢？我們查一下重定位表：
 
 
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

可見，該地址裡放的就是 g_func 的具體地址，那此時 0x16d8 放的是真正的地址了嗎？我們再看看 got.plt:

```sh
Contents of section .got.plt:
 16cc fc150000 00000000 00000000 2e040000  ................
 16dc 3e040000
```


16d8 處的內容是: 2e040000, 小端序，換回整形就是 0x000042e, 該地址就是 `fun@plt` 的第二條指令！是不是覺得有點兒繞？你可以定下心來再看一遍，其實不繞，而是很巧妙。

##後話

對動態鏈接庫來說，加載時重定位與鏈接時重定位各有優缺點，前者使得動態庫的代碼段不能被多個進程間所共享，加載動態庫時也比較費時，但是加載完成後，因為對符號的引用不需要進行跳轉，程序運行的效率相對是較高的。而對地址無關的代碼，它的缺點是動態庫的體積相對較大，畢竟增加了很多表項及相關的函數，另外就運行時對全局符號的引用需要通過表格進行跳轉，程序執行的效率不可避免有所損失，優點嘛，就是動態庫加載比較快，而且代碼可以在多個進程間共享，對整個系統而言，可以大大節約對內存的使用，這個好處的吸引力是非常大的，所以你可以看到，目前來說在常用的動態庫使用上，PIC 相較而言是更加被推崇的，道理在此。


##【引用】
《程序員的自我修養》

http://eli.thegreenplace.net/2011/08/25/load-time-relocation-of-shared-libraries/

http://www.iecc.com/linker/linker10.html

https://www.technovelty.org/c/position-independent-code-and-x86-64-libraries.html
