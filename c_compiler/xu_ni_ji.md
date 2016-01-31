# 虛擬機


本章是「手把手教你構建 C 語言編譯器」系列的第三篇，本章我們要構建一臺虛擬的電腦，設計我們自己的指令集，運行我們的指令集，說得通俗一點就是自己實現一套彙編語言。它們將作為我們的編譯器最終輸出的目標代碼。

計算機的內部工作原理

我們關心計算機的三個基本部件：CPU、寄存器及內存。代碼（彙編指令）以二進制的形式保存在內存中，CPU 從中一條條地加載指令執行。程序運行的狀態保存在寄存器中。

###記憶體

我們從內存開始說起。現代的操作系統都不直接使用內存，而是使用虛擬內存。虛擬內存可以理解為一種映射，在我們的程序眼中，我們可以使用全部的內存地址，而操作系統需要將它映射到實際的內存上。當然，這些並不重要，重要的是一般而言，進程的內存會被分成幾個段：

- 代碼段（text）用於存放代碼（指令）。
- 數據段（data）用於存放初始化了的數據，如int i = 10;，就需要存放到數據段中。
- 未初始化數據段（bss）用於存放未初始化的數據，如 int i[1000];，因為不關心其中的真正數值，所以單獨存放可以節省空間，減少程序的體積。
- 棧（stack）用於處理函數調用相關的數據，如調用幀（calling frame）或是函數的局部變量等。
- 堆（heap）用於為程序動態分配內存。

它們在內存中的位置類似於下圖：

```c
+------------------+
|    stack   |     |      high address
|    ...     v     |
|                  |
|                  |
|                  |
|                  |
|    ...     ^     |
|    heap    |     |
+------------------+
| bss  segment     |
+------------------+
| data segment     |
+------------------+
| text segment     |      low address
+------------------+
```

但我們的虛擬機並不模擬完整的計算機，我們只關心三個內容：代碼段、數據段以及棧。其中的數據段我們只存放字符串，因為我們的編譯器並不支持初始化變量，因此我們也不需要未初始化數據段。理論上我們的虛擬器需要維護自己的堆用於內存分配，但實際實現上較為複雜且與編譯無關，故我們引入一個指令MSET，使我們能直接使用編譯器（解釋器）中的內存。

綜上，我們需要首先在全局添加如下代碼：

```c
int *text,            // text segment
    *old_text,        // for dump text segment
    *stack;           // stack
char *data;           // data segment

```


注意這裡的類型，雖然是int型，但理解起來應該作為無符號的整型，因為我們會在代碼段（text）中存放如指針/內存地址的數據，它們就是無符號的。其中數據段（data）由於只存放字符串，所以是 char * 型的

接著，在main函數中加入初始化代碼，真正為其分配內存：

```c
int main() {
    close(fd);
    ...

    // allocate memory for virtual machine
    if (!(text = old_text = malloc(poolsize))) {
        printf("could not malloc(%d) for text area\n", poolsize);
        return -1;
    }
    if (!(data = malloc(poolsize))) {
        printf("could not malloc(%d) for data area\n", poolsize);
        return -1;
    }
    if (!(stack = malloc(poolsize))) {
        printf("could not malloc(%d) for stack area\n", poolsize);
        return -1;
    }

    memset(text, 0, poolsize);
    memset(data, 0, poolsize);
    memset(stack, 0, poolsize);

    ...
    program();
}
```

### 寄存器

計算機中的寄存器用於存放計算機的運行狀態，真正的計算機中有許多不同種類的寄存器，但我們的虛擬機中只使用 4 個寄存器，分別如下：

- PC 程序計數器，它存放的是一個內存地址，該地址中存放著 下一條 要執行的計算機指令。
- SP 指針寄存器，永遠指向當前的棧頂。注意的是由於棧是位於高地址並向低地址增長的，所以入棧時 SP 的值減小。
- BP 基址指針。也是用於指向棧的某些位置，在調用函數時會使用到它。
- AX 通用寄存器，我們的虛擬機中，它用於存放一條指令執行後的結果。

要理解這些寄存器的作用，需要去理解程序運行中會有哪些狀態。而這些寄存器只是用於保存這些狀態的。

在全局中加入如下定義：

```c
int *pc, *bp, *sp, ax, cycle; // virtual machine registers
```

在 main 函數中加入初始化代碼，注意的是PC在初始應指向目標代碼中的main函數，但我們還沒有寫任何編譯相關的代碼，因此先不處理。代碼如下：

```c
memset(stack, 0, poolsize);
...

bp = sp = (int *)((int)stack + poolsize);
ax = 0;

...
program();
```

與 CPU 相關的是指令集，我們將專門作為一個小節。

### 指令集

指令集是 CPU 能識別的命令的集合，也可以說是 CPU 能理解的語言。這裡我們要為我們的虛擬機構建自己的指令集。它們基於 x86 的指令集，但要更為簡單。

首先在全局變量中加入一個枚舉類型，這是我們要支持的全部指令：

```c
// instructions
enum { LEA ,IMM ,JMP ,CALL,JZ  ,JNZ ,ENT ,ADJ ,LEV ,LI  ,LC  ,SI  ,SC  ,PUSH,
       OR  ,XOR ,AND ,EQ  ,NE  ,LT  ,GT  ,LE  ,GE  ,SHL ,SHR ,ADD ,SUB ,MUL ,DIV ,MOD ,
       OPEN,READ,CLOS,PRTF,MALC,MSET,MCMP,EXIT };
```


這些指令的順序安排是有意的，稍後你會看到，帶有參數的指令在前，沒有參數的指令在後。這種順序的唯一作用就是在打印調試信息時更加方便。但我們講解的順序並不依據它。

#### MOV

MOV 是所有指令中最基礎的一個，它用於將數據放進寄存器或內存地址，有點類似於 C 語言中的賦值語句。x86 的 MOV 指令有兩個參數，分別是源地址和目標地址：MOV dest, source （Intel 風格），表示將 source 的內容放在 dest 中，它們可以是一個數、寄存器或是一個內存地址。

一方面，我們的虛擬機只有一個寄存器，另一方面，識別這些參數的類型（是數還是地址）是比較困難的，因此我們將 MOV 指令拆分成 5 個指令，這些指令只接受一個參數，如下：

- IMM <num> 將 <num> 放入寄存器 ax 中。
- LC 將對應地址中的字符載入 ax 中，要求 ax 中存放地址。
- LI 將對應地址中的整數載入 ax 中，要求 ax 中存放地址。
- SC 將 ax 中的數據作為字符存放入地址中，要求棧頂存放地址。
- SI 將 ax 中的數據作為整數存放入地址中，要求棧頂存放地址。



你可能會覺得將一個指令變成了許多指令，整個系統就變得複雜了，但實際情況並非如此。首先是 MOV 指令其實有許多變種，根據類型的不同有 MOVB, MOVW 等指令，我們這裡的 LC/SC 和 LI/SI 就是對應字符型和整型的存取操作。

但最為重要的是，通過將 MOV 指令拆分成這些指令，只有 IMM 需要有參數，且不需要判斷類型，所以大大簡化了實現的難度。

在 eval() 函數中加入下列代碼：

```c
void eval() {
    int op, *tmp;
    while (1) {
        if (op == IMM)       {ax = *pc++;}                                     // load immediate value to ax
        else if (op == LC)   {ax = *(char *)ax;}                               // load character to ax, address in ax
        else if (op == LI)   {ax = *(int *)ax;}                                // load integer to ax, address in ax
        else if (op == SC)   {ax = *(char *)*sp++ = ax;}                       // save character to address, value in ax, address on stack
        else if (op == SI)   {*(int *)*sp++ = ax;}                             // save integer to address, value in ax, address on stack
    }

    ...
    return 0;
}
```

其中的 *sp++ 的作用是退棧，相當於 POP 操作。

這裡要解釋的一點是，為什麼 SI/SC 指令中，地址存放在棧中，而 LI/LC 中，地址存放在 ax 中？原因是默認計算的結果是存放在 ax 中的，而地址通常是需要通過計算獲得，所以執行 LI/LC 時直接從 ax 取值會更高效。另一點是我們的 PUSH 指令只能將 ax 的值放到棧上，而不能以值作為參數，詳細見下文。


#### PUSH

在 x86 中，PUSH 的作用是將值或寄存器，而在我們的虛擬機中，它的作用是將 ax 的值放入棧中。這樣做的主要原因是為了簡化虛擬機的實現，並且我們也只有一個寄存器 ax 。代碼如下：

```c
else if (op == PUSH) {*--sp = ax;}                                     // push the value of ax onto the stack
```

#### JMP

JMP <addr> 是跳轉指令，無條件地將當前的 PC 寄存器設置為指定的 <addr>，實現如下：


```c
else if (op == JMP)  {pc = (int *)*pc;}                                // jump to the address
```

要記得，pc 寄存器指向的是 下一條 指令。所以此時它存放的是 JMP 指令的參數，即 <addr> 的值。

#### JZ/JNZ

為了實現 if 語句，我們需要條件判斷相關的指令。這裡我們只實現兩個最簡單的條件判斷，即結果（ax）為零或不為零情況下的跳轉。

實現如下：

```c
else if (op == JZ)   {pc = ax ? pc + 1 : (int *)*pc;}                   // jump if ax is zero
else if (op == JNZ)  {pc = ax ? (int *)*pc : pc + 1;}                   // jump if ax is zero
```

#### 子函數調用

這是彙編中最難理解的部分，所以合在一起說，要引入的命令有 CALL, ENT, ADJ 及 LEV。

首先我們介紹 CALL <addr> 與 RET 指令，CALL 的作用是跳轉到地址為 <addr> 的子函數，RET 則用於從子函數中返回。

為什麼不能直接使用 JMP 指令呢？原因是當我們從子函數中返回時，程序需要回到跳轉之前的地方繼續運行，這就需要事先將這個位置信息存儲起來。反過來，子函數要返回時，就需要獲取並恢復這個信息。因此實際中我們將 PC 保存在棧中。如下：

```c
else if (op == CALL) {*--sp = (int)(pc+1); pc = (int *)*pc;}           // call subroutine
//else if (op == RET)  {pc = (int *)*sp++;}                              // return from subroutine;
```

這裡我們把 RET 相關的內容註釋了，是因為之後我們將用 LEV 指令來代替它。

在實際調用函數時，不僅要考慮函數的地址，還要考慮如何傳遞參數和如何返回結果。這裡我們約定，如果子函數有返回結果，那麼就在返回時保存在 ax 中，它可以是一個值，也可以是一個地址。那麼參數的傳遞呢？

各種編程語言關於如何調用子函數有不同的約定，例如 C 語言的調用標準是：

- 由調用者將參數入棧。
- 調用結束時，由調用者將參數出棧。
- 參數逆序入棧。

事先聲明一下，我們的編譯器參數是順序入棧的，下面的例子（C 語言調用標準）取自 [維基百科](https://en.wikipedia.org/wiki/X86_calling_conventions)：

```c
int callee(int, int, int);

int caller(void)
{
	int i, ret;

	ret = callee(1, 2, 3);
	ret += 5;
	return ret;
}
```

會生成如下的 x86 彙編代碼：

```c
caller:
	; make new call frame
	push    ebp
	mov     ebp, esp
        sub     1, esp       ; save stack for variable: i
	; push call arguments
	push    3
	push    2
	push    1
	; call subroutine 'callee'
	call    callee
	; remove arguments from frame
	add     esp, 12
	; use subroutine result
	add     eax, 5
	; restore old call frame
        mov     esp, ebp
	pop     ebp
	; return
	ret
```

上面這段代碼在我們自己的虛擬機裡會有幾個問題：

- push ebp，但我們的 PUSH 指令並無法指定寄存器。
- mov ebp, esp，我們的 MOV 指令同樣功能不足。
- add esp, 12，也是一樣的問題（儘管我們還沒定義）。

也就是說由於我們的指令過於簡單（如只能操作ax寄存器），所以用上面提到的指令，我們連函數調用都無法實現。而我們又不希望擴充現有指令的功能，因為這樣實現起來就會變得複雜，因此我們採用的方法是增加指令集。畢竟我們不是真正的計算機，增加指令會消耗許多資源（錢）。

#### ENT

ENT <size> 指的是 enter，用於實現 『make new call frame' 的功能，即保存當前的棧指針，同時在棧上保留一定的空間，用以存放局部變量。對應的彙編代碼為：

```c
; make new call frame
push    ebp
mov     ebp, esp
       sub     1, esp       ; save stack for variable: i
```

實現如下：

```c
else if (op == ENT)  {*--sp = (int)bp; bp = sp; sp = sp - *pc++;}      // make new stack frame
```

#### ADJ

ADJ <size> 用於實現 『remove arguments from frame'。在將調用子函數時壓入棧中的數據清除，本質上是因為我們的 ADD 指令功能有限。對應的彙編代碼為：


```c
; remove arguments from frame
add     esp, 12
```

實現如下：


```c
else if (op == ADJ)  {sp = sp + *pc++;}                                // add esp, <size>
```

#### LEV

本質上這個指令並不是必需的，只是我們的指令集中並沒有 POP 指令。並且三條指令寫來比較麻煩且浪費空間，所以用一個指令代替。對應的彙編指令為：


```c
; restore old call frame
       mov     esp, ebp
pop     ebp
; return
ret
```

具體的實現如下：

```c
else if (op == LEV)  {sp = bp; bp = (int *)*sp++; pc = (int *)*sp++;}  // restore call frame and PC
```


注意的是，LEV 已經把 RET 的功能包含了，所以我們不再需要 RET 指令。

#### LEA

上面的一些指令解決了調用幀的問題，但還有一個問題是如何在子函數中獲得傳入的參數。這裡我們首先要瞭解的是當參數調用時，棧中的調用幀是什麼樣的。我們依舊用上面的例子（只是現在用「順序」調用參數）：

```c
sub_function(arg1, arg2, arg3);

|    ....       | high address
+---------------+
| arg: 1        |    new_bp + 4
+---------------+
| arg: 2        |    new_bp + 3
+---------------+
| arg: 3        |    new_bp + 2
+---------------+
|return address |    new_bp + 1
+---------------+
| old BP        | <- new BP
+---------------+
| local var 1   |    new_bp - 1
+---------------+
| local var 2   |    new_bp - 2
+---------------+
|    ....       |  low address
```


所以為了獲取第一個參數，我們需要得到 new_bp + 4，但就如上面的說，我們的 ADD 指令無法操作除 ax 外的寄存器，所以我們提供了一個新的指令：LEA <offset>

實現如下：
```c
else if (op == LEA)  {ax = (int)(bp + *pc++);}                         // load address for arguments.
```

以上就是我們為了實現函數調用需要的指令了。


#### 運算符指令

我們為 C 語言中支持的運算符都提供對應彙編指令。每個運算符都是二元的，即有兩個參數，第一個參數放在棧頂，第二個參數放在 ax 中。這個順序要特別注意。因為像 -，/ 之類的運算符是與參數順序有關的。計算後會將棧頂的參數退棧，結果存放在寄存器 ax 中。因此計算結束後，兩個參數都無法取得了（彙編的意義上，存在內存地址上就另當別論）。

實現如下：


```c
else if (op == OR)  ax = *sp++ | ax;
else if (op == XOR) ax = *sp++ ^ ax;
else if (op == AND) ax = *sp++ & ax;
else if (op == EQ)  ax = *sp++ == ax;
else if (op == NE)  ax = *sp++ != ax;
else if (op == LT)  ax = *sp++ < ax;
else if (op == LE)  ax = *sp++ <= ax;
else if (op == GT)  ax = *sp++ >  ax;
else if (op == GE)  ax = *sp++ >= ax;
else if (op == SHL) ax = *sp++ << ax;
else if (op == SHR) ax = *sp++ >> ax;
else if (op == ADD) ax = *sp++ + ax;
else if (op == SUB) ax = *sp++ - ax;
else if (op == MUL) ax = *sp++ * ax;
else if (op == DIV) ax = *sp++ / ax;
else if (op == MOD) ax = *sp++ % ax;
```

#### 內置函數

程序要有用，除了核心的邏輯外還需要輸入輸出，如 C 語言中我們經常使用的 printf 函數就是用於輸出。但是 printf 函數的實現本身就十分複雜，如果我們的編譯器要達到自舉，就勢必要實現 printf 之類的函數，但它又與編譯器沒有太大的聯繫，因此我們繼續實現新的指令，從虛擬機的角度予以支持。

編譯器中我們需要用到的函數有：exit, open, close, read, printf, malloc, memset 及 memcmp。代碼如下：

```c
else if (op == EXIT) { printf("exit(%d)", *sp); return *sp;}
else if (op == OPEN) { ax = open((char *)sp[1], sp[0]); }
else if (op == CLOS) { ax = close(*sp);}
else if (op == READ) { ax = read(sp[2], (char *)sp[1], *sp); }
else if (op == PRTF) { tmp = sp + pc[1]; ax = printf((char *)tmp[-1], tmp[-2], tmp[-3], tmp[-4], tmp[-5], tmp[-6]); }
else if (op == MALC) { ax = (int)malloc(*sp);}
else if (op == MSET) { ax = (int)memset((char *)sp[2], sp[1], *sp);}
else if (op == MCMP) { ax = memcmp((char *)sp[2], (char *)sp[1], *sp);}
```

這裡的原理是，我們的電腦上已經有了這些函數的實現，因此編譯編譯器時，這些函數的二進制代碼就被編譯進了我們的編譯器，因此在我們的編譯器/虛擬機上運行我們提供的這些指令時，這些函數就是可用的。換句話說就是不需要我們自己去實現了。

最後再加上一個錯誤判斷：

```c
else {
    printf("unknown instruction:%d\n", op);
    return -1;
}
```


### 測試

下面我們用我們的彙編寫一小段程序，來計算 10+20，在 main 函數中加入下列代碼：


```c
int main(int argc, char *argv[])
{
    ax = 0;
    ...

    i = 0;
    text[i++] = IMM;
    text[i++] = 10;
    text[i++] = PUSH;
    text[i++] = IMM;
    text[i++] = 20;
    text[i++] = ADD;
    text[i++] = PUSH;
    text[i++] = EXIT;
    pc = text;

    ...
    program();
}
```

編譯程序 gcc xc-tutor.c，運行程序：./a.out hello.c。輸出

```c
exit(30)
```

注意我們的之前的程序需要指令一個源文件，只是現在還用不著，但從結果可以看出，我們的虛擬機還是工作良好的。

### 小結

本章中我們回顧了計算機的內部運行原理，並仿照 x86 彙編指令設計並實現了我們自己的指令集。

本章的代碼可以在 Github 上下載，也可以直接 clone
```c
git clone -b step-1 https://github.com/lotabout/write-a-C-interpreter
```

實際計算機中，添加一個新的指令需要設計許多新的電路，會增加許多的成本，但我們的虛擬機中，新的指令幾乎不消耗資源，因此我們可以利用這一點，用更多的指令來完成更多的功能，從而簡化具體的實現。