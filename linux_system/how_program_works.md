# How program works


##1.gdb常用調試命令

要用gdb調試的話，編譯命令需要添加-g參數，例如

```sh
gcc -g main.c -o main 
```

```sh
b linenum          在第 linenum行打斷點
l                           顯示源代碼;
Ctrl-d                  退出gdb
where                 顯示當前程序運行位置
print  /d $eax    十進制地方式打印$eax 值，/x是十六進制，/t是二進制
c                          執行到下一個斷點
n                          下一行
layout split          把當前Terminal分割成兩半，上面顯示源碼及彙編，下面可以輸入調試命令，效果如下：
```

![](./images/20130511003531696)

##2.Example.c程序分析

程序代碼：

```c
#include <stdio.h>

int g(int x)
{
    return x + 3;
}

int f(int x)
{
    return g(x);
}

int main(void)
{
    printf("Hello\n");
    return f(8) + 1;
}

```
將源代碼編譯為二進制文件又需要經過以下四個步驟：預處理(cpp) → 編譯(gcc或g++) → 彙編(as) → 鏈接(ld) ；括號中表示每個階段所使用的程序，它們分別屬於 GCC 和 Binutils 軟件包。

用gcc的編譯參數和生成的對應文件。

2.1預編譯

```c
gcc -E Example.c -o Example.cpp  
```

生成的cpp文件內容如下：

```c
...
...
...
//a lot of extern statement

extern char* ctermid(char* __s) __attribute__((__nothrow__ , __leaf__));
# 910 "/usr/include/stdio.h" 3 4
extern void flockfile(FILE* __stream) __attribute__((__nothrow__ , __leaf__));



extern int ftrylockfile(FILE* __stream) __attribute__((__nothrow__ ,
        __leaf__)) ;


extern void funlockfile(FILE* __stream) __attribute__((__nothrow__ , __leaf__));
# 940 "/usr/include/stdio.h" 3 4

# 2 "Example.c" 2

int g(int x)
{
    return x + 3;
}

int f(int x)
{
    return g(x);
}

int main(void)
{
    return f(8) + 1;
}
```
主要代碼基本沒有變化，添加了很多extern聲明。

分析
預編譯的主要作用如下：

```sh
●將源文件中以”include”格式包含的文件複製到編譯的源文件中。
●用實際值替換用“#define”定義的字符串。
●根據“#if”後面的條件決定需要編譯的代碼。
```

在該階段，編譯器將C源代碼中的包含的頭文件stdio.h編譯進來,生成擴展的c程序。當對一個源文件進行編譯時， 系統將自動引用預處理程序對源程序中的預處理部分作處理， 處理完畢自動進入對源程序的編譯。

2.2編譯
執行編譯的結果是得到彙編代碼。

```c
gcc -S Example.c -o Example.s  
```

生成.s文件內容如下：

```sh
.file   "Example.c"
.text
.globl  g
.type   g, @function
g:
.LFB0:
.cfi_startproc
pushl   % ebp               ;
ebp寄存器內容壓棧
.cfi_def_cfa_offset 8
.cfi_offset 5, -8
movl    % esp, % ebp         ;
esp值賦給ebp，設置函數的棧基址。
.cfi_def_cfa_register 5
movl    8( % ebp), % eax      ;
將ebp + 8所指向內存的內容存至eax
addl    $3, % eax           ;
將3與eax中的數值相加，結果存至eax中
popl    % ebp               ;
ebp中的內容出棧
.cfi_restore 5
.cfi_def_cfa 4, 4
ret
.cfi_endproc
.LFE0:
.size   g, . - g
.globl  f
.type   f, @function
f:
.LFB1:
.cfi_startproc
pushl   % ebp              ;
ebp寄存器內容壓棧
.cfi_def_cfa_offset 8
.cfi_offset 5, -8
movl    % esp, % ebp        ;
esp值賦給ebp，設置函數的棧基址。
.cfi_def_cfa_register 5
subl    $4, % esp          ;
esp下移動四個單位
movl    8( % ebp), % eax     ;
將ebp + 8所指向內存的內容存至eax
movl    % eax, ( % esp)      ;
將eax存至esp所指內存中
call    g                 ;
調用g函數
leave                     ;
將ebp值賦給esp，pop先前棧內的上級函數棧的基地址給ebp，恢復原棧基址
.cfi_restore 5
.cfi_def_cfa 4, 4
ret                              ;
函數返回，回到上級調用
.cfi_endproc
.LFE1:
.size   f, . - f
.globl  main
.type   main, @function
main:
.LFB2:
.cfi_startproc
pushl   % ebp               ;
ebp寄存器內容壓棧
.cfi_def_cfa_offset 8 
.cfi_offset 5, -8
movl    % esp, % ebp         ;
esp值賦給ebp，設置函數的棧基址。
.cfi_def_cfa_register 5
subl    $4, % esp           ;
esp下移動四個單位
movl    $8, ( % esp)         ;
將8存入esp所指向的內存空間
call    f                  ;
調用f函數
addl    $1, % eax           ;
將1與eax的內容相加
leave                      ;
將ebp值賦給esp，pop先前棧內的上級函數棧的基地址給ebp，恢復原棧基址 
.cfi_restore 5
.cfi_def_cfa 4, 4
ret                        ;
函數返回，回到上級調用
.cfi_endproc
.LFE2:
.size   main, . - main
.ident  "GCC: (SUSE Linux) 4.7.1 20120723 [gcc-4_7-branch revision 189773]"
.section    .comment.SUSE.OPTs, "MS", @progbits, 1
.string "ospwg"
.section    .note.GNU - stack, "", @progbits
```

### 分析
第1行為gcc留下的文件信息；第2行標識下面一段是代碼段，第3、4行表示這是g函數的入口，第5行為入口標號；6~20行為 g 函數體，稍後 分析；21行為 f 函數的代碼段的大小；22、23行表示這是 f 函數的入口；24行為入口標識，25到41為 f 函數的彙編實現；42行為f函數的代碼段的大小；43、44行表示這是main函數的入口；45行為入口標識，46到62為main函數的彙編實現；63行為main函數的代碼段的大小；54到67行為 gcc留下的信息。
具體程序運行時內存的調用情況如下圖：

![](./images/20130514003641341)
![](./images/20130514003627788)

以.cfi開頭的命令如.cfi_startproc，主要用於作用是`出現異常時stack的回滾(unwind)`，而回滾的過程是一級級CFA往上回退，直到異常被catch。
這裡不做討論，需要詳細瞭解的[點這裡](http://www.logix.cz/michal/devel/gas-cfi/)。
每一個函數在開始都會調用到

```sh
pushl %ebp      ;ebp寄存器內容壓棧，即保存函數的上級調用函數的棧基地址         
movl %esp,%ebp  ;esp值賦給ebp，設置函數的棧基址
```
主要作用是保存當前程序執行的狀態。
還有兩句在函數調用結束時也會出現：

```sh
leave ; 將ebp值賦給esp，pop先前棧內的上級函數棧的基地址給ebp，恢復原棧基址 
ret ; 函數返回，回到上級調用 
```

用於在函數執行完後回到執行前的狀態。

還有要注意的是彙編中的`push和pop`

pop系列指令的格式是：
```sh
pop destination
pop指令把棧頂指定長度的數據存放到destination中，並且設置相應的esp的值使它始終指向棧頂位置。
push剛好相反。
```

```sh
pushl %eax 等價於 
subl $4 %esp
movl %eax (%esp)
popl %eax 等價於
movl (%esp) %eax
addl %4 %esp
```

2.3彙編
彙編之後得到的是.o文件，終端執行命令：

```sh
as Example.s -o Example.o  
```

在終端用vim打開：

```sh
vim -b Example.o 
```

用16進制進行查看，在vim中輸入

```sh
:%!xxd 
```

結果如下（未完全顯示）

![](./images/20130513010815829)

##分析
目標文件就是源代碼編譯後但未進行鏈接的那些中間文件，包含有編譯後的機器指令代碼，還包括鏈接時所需要的一些信息，比如符號表、調試信息、字符串等。
可以查看目標文件的信息，在終端執行

```sh
file Example.o  
```
得到：
Example.o: ELF 32-bit LSB `relocatable`, Intel 80386, version 1 (SYSV), not stripped
其中的relocatable指出該文件為ELF中的可重定位文件類型。

2.4鏈接
鏈接後的文件為可執行文件，在linux中沒有擴展名。
終端執行：

```sh
gcc Example.o -o Example  
```
執行Example，終端運行：

```sh
./Example  
```

運行結果：

![](./images/20130513012938226)

##分析
用file命令查看Example屬性：

Example: ELF 32-bit LSB `executable`, Intel 80386, version 1 (SYSV), dynamically linked (uses shared libs), for GNU/Linux 2.6.16, BuildID[sha1]=0xffdc8de348d59ce38f1f933e55b7a5c55184ef39, not stripped

其中的executable指出該文件為ELF中的可執行文件類型。
由於程序沒有任何打印語句，所以程序執行完之後就直接退出了。

###3.計算機工作流程-單任務和多任務

暫且討論最簡單的計算機，只包含CPU，存儲器，I/O控制芯片
如果一個用戶在同一時間只能運行一個應用程序，則對應的操作系統稱為單任務操作系統，如MS-DOS。
如果用戶在同一時間可以運行多個應用程序（每個應用程序被稱作一個任務），則這樣的操作系統被稱為多任務操作系統，如windows 7,Mac OS 。
在最早期的單任務計算機中，用戶一次只能運行一個程序，計算機首先從外存中加載程序到內存，然後依次執行程序指令，完全執行完畢之後才可以加載、執行下一個程序。

由於當時CPU的資源十分珍貴，為了充分利用，在這之後出現了多道程序，當某個程序暫時無需使用CPU時，監控程序就把另外的正在等待CPU資源的程序啟動，使得CPU充分利用。缺點是程序的運行沒有優先級。

在這之後又出現了分時系統，程序運行模式變成了一種協作的模式，即每個程序運行一段時間以後都主動讓出CPU。

分時系統繼續發展就到了今天的多任務系統 - 所有的程序都以進程的方式運行在比操作系統權限更低的級別，每個進程都有自己的獨立空間，CPU由操作系統統一進行分配，每個進程根據進程優先級的高低都有獲得CPU的機會。

多任務的實現主要依靠MMU（Memory Management Unit：內存管理單元）。
MMU的主要工作就是將程序的虛擬地址（編譯器和鏈接器計算的）轉換成內存的物理地址（硬件電路決定的）。
MMU可以通過重定位任務地址而不需要移動在內存中的任務。任務的物理內存只是簡單的通過激活與不激活頁表來實現映射到虛擬內存。

 4.參考資料
程序員的自我修養—鏈接、裝載與庫 
  
Computer Systems: A Programmer's Perspective  3rd Edith
