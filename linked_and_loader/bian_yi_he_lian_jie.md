# 編譯和連結


## Hello World

```c
#include <stdio.h>

int main()
{
    printf("Hello World\n");    
    return 0;
}
```

編譯一個最簡單的 Hello World 檔案指令為

```sh
$ gcc hello.c
$ ./a.out
$ Hello World
```

編譯過程分為 4 個步驟

```sh
1.前置處理 = 預處理 = 前編譯 (Preprocessing)
2.編譯 (Compilation)
3.組譯 (Assemby)
4.連結 (Linking)
```

##前置處理、預處理、前編譯
以下兩種指令只對 source code 做前置處理


```sh
$ gcc -E hello.c -o hello.i
或
$ cpp hello.c > hello.i
```


前置處理主要處理開頭為#的指令，例如 #include、#define
處理規則如下

- 刪除所有 #define，並展開所有巨集，例如

```c
#define PI 3.1415
int main()
{
    int radius = 5*5*PI;  
    return 0;
}
// 會變為
int main()
{
    int radius = 5*5*3.1415;
    return 0;
}
```

-  處理條件前置指令，#if、#else、#elf、#else、endif
-  刪除 #include，將包含內容插入檔案中
-  刪除註解 //、/**/
-  添加行號和檔案名稱標識，例如 # 2 “hello.c” 2，編譯時會用到
-  保留所有 #pragma 編譯器指令

所有巨集都會在前置處理中被展開，可以用此方法來判斷你巨集是否有寫對

## 編譯

編譯會把前置處理後的檔案做分析和最佳化
產生相對應的組合語言程式碼

```c
$ gcc -S hello.i -o hello.s
或是
/* 1 是數字 one 不是小寫 L
   cc1 是直接將原始碼做"前編譯"和"編譯"兩個動作 */
$ /usr/lib/gcc/x86_64-linux-gnu/4.8/cc1 hello.c  

上述指令皆會產生 hello.s
$ cat hello.s

// 內容如下
        .file   "hello.c"
        .section        .rodata
.LC0:
        .string "Hello World"
        .text
        .globl  main
        .type   main, @function
main:
.LFB0:
        .cfi_startproc
        pushq   %rbp
        .cfi_def_cfa_offset 16
        .cfi_offset 6, -16
        movq    %rsp, %rbp
        .cfi_def_cfa_register 6
        movl    $.LC0, %edi
        call    puts
        movl    $0, %eax
        popq    %rbp
        .cfi_def_cfa 7, 8
        ret
        .cfi_endproc
.LFE0:
        .size   main, .-main
        .ident  "GCC: (Ubuntu 4.8.4-2ubuntu1~14.04) 4.8.4"
        .section        .note.GNU-stack,"",@progbits
```

另外不同語言使用的編譯程式不同

```sh
C       : cc1
C++     : cc1plus
Obj-C   : cc1obj
fortran : f771
Java    : jcl
```

## 組譯

編譯將程式碼轉為`組語`

組譯將組語碼轉為`機器碼`

組語和機器碼是`一對一對應`，所以組譯只是`單純轉換`而已

組譯器是 as


```sh
as hello.s -o hello.o
或是
gcc -c hello.s -o hello.o

生成的 .o 檔(目的檔)內容為
hexdump hello.o
0000000 457f 464c 0102 0001 0000 0000 0000 0000
0000010 0001 003e 0001 0000 0000 0000 0000 0000
0000020 0000 0000 0000 0000 0130 0000 0000 0000
0000030 0000 0000 0040 0000 0000 0040 000d 000a
0000040 4855 e589 00bf 0000 e800 0000 0000 00b8
0000050 0000 5d00 48c3 6c65 6f6c 5720 726f 646c
0000060 0000 4347 3a43 2820 6255 6e75 7574 3420
0000070 382e 342e 322d 6275 6e75 7574 7e31 3431
0000080 302e 2e34 2933 3420 382e 342e 0000 0000
0000090 0014 0000 0000 0000 7a01 0052 7801 0110
00000a0 0c1b 0807 0190 0000 001c 0000 001c 0000
00000b0 0000 0000 0015 0000 4100 100e 0286 0d43
00000c0 5006 070c 0008 0000 2e00 7973 746d 6261
00000d0 2e00 7473 7472 6261 2e00 6873 7473 7472
00000e0 6261 2e00 6572 616c 742e 7865 0074 642e
00000f0 7461 0061 622e 7373 2e00 6f72 6164 6174
......

如果要直接從 .c 轉成 .o 可以使用
等於: 前編譯->編譯->組譯
gcc -c hello.c -o hello.o
```

## 連結

有了 hello.o 還是不行執行

hello.o 需要連結其他的 .o 才能真正可執行

連結需使用 ld 幫忙


```sh
//省略路徑
ld 
-static
crtl.o
crti.o
crtbeginT.o
hello.o

-start-group
-lgcc
-lgcc_eh
-lc
-end-group
crtend.o
cern.o
```

##編譯器流程
編譯器過程如下

```sh
             掃描器(Scanner)                        
原始碼     ------------------>   Tokens

             語法分析器(Parser)  
Tokens     -------------------->  SyntaxTree

             語意分析器(Semantic Analyzer)
SyntaxTree ------------------------------> Commented SyntaxTree

                       最佳化
Commented SyntaxTree ---------> Intermediate Representation(中間語言)


以上為前端
=======================================================================
以下為後端

           程式碼產生器(Code Generator)
中間語言 ------------------------------> Target Code(目的碼)

        程式碼最佳化器(Target Code Optimizer)
目的碼 ---------------------------------------> Final Target Code
```

##掃描器(Scanner)

舉例: 原始碼 array[idx] = ( idx + 4 )*( 2 + 6 )
原始碼經過掃描器之後會分解出許多非空白的 Tokens


```sh
// Tokens
1.      array
2.      [
3.      idx
4.      ]
5.      =
6.      (
7.      idx
8.      +
9.      4
10.     )
11.     *
12.     (
13.     2
14.     +
15.     6
16.     )
```
這些 Tokens 可分為幾種類別


```sh
關鍵字              
識別字             -> 放入符號表
字面量(數字，字串) -> 放入文字表 
特殊符號(+,-,*,/)
```

掃描器為 lex，可參考這裡
另外巨集展開是前置處理器的工作，不在掃描器之中做
##語法分析器(Parser)
語法分析器將 Tokens 分析產生 SyntaxTree 類似以下圖案表示

```sh
          =  
       /      \
      /        \
    []          *
   /   \       /  \
array  idx    /    \
             +      +
            / \    / \
         idx   4  2   6

```

語法分析器為yacc，不同語言只需要改變規則
不用再寫一個語法分析器

##語意分析器(Semantic Analyzer)
前面只是分析你的語法是否有錯誤
他只能分析靜態語意(編譯階段可以確定的語意)
不能分析動態語意(執行階段可以確定的語意)
靜態語意如類型轉換等
例如你把浮點數指派給指標，這種作法是沒意義的
所以語意分析器就會回報錯誤，分析完之後會將 SyntaxTree 加上類型


```sh
          =  
         (Int)
       /      \
      /        \
    []          *
   (Int)       (Int)
   /    \       /  \
array   idx    /    \
(Int)  (Int)  +       +
            (Int)    (Int)
            /   \     /  \
         idx     4   2    6
        (Int)  (Int)(Int) (Int)

```
##產生中間語言
最佳化會將 Commented SyntaxTree 轉成中間語言

中間語言分為:三位址碼或虛擬碼

以三位址碼來說，Commented SyntaxTree 會變為


```sh
t1 = 2 + 6
t2 = idx + 4
t3 = t2 * t1
array[idx] = t3

// 簡化之後為
t2 = idx + 4
t2 = t2 * 8 
array[idx] = t2
```

編譯器分為前後端

前端先轉為和平臺無關的中間碼

後端再將中間碼轉為對應平臺的目的機器碼

所以可以使用同一個前端，換不同平臺之後再換後端即可


## 目的碼

程式碼產生器(Code Generator)將中間語言轉為目的碼

此過程會依賴平臺的，因為不同平臺的長度、register、類型都不同

舉例如果是 x86 平臺

```sh
movl idx, %ecx
addl $4,  %ecx
mull $8,  %ecx
movl idx, %eax
movl %ecx,array(,eax,4)
```

之後再使用程式碼最佳化器最佳化

```sh
movl idx, %edx
leal 32(,%edx,8), %eax
movl %eax, array(,%edx,4)
```

以上都只是將程式碼編成目的檔

但真正變數要放在記憶體哪個位址是還沒有決定的

所以變數位址都要等到連結器來決定

## 連結器

假設跳轉指令是 0001 開頭，後 4 bit 為位址

所以第一行表示要跳到 0100(4) 的位置

也就是要執行 1000 0111 這行指令

```sh
0 0001 0100
1
2
3
4 1000 0111
5
```

如果之後中間又插入其他程式碼，那位址勢必要重新寫

這樣稱為`重定(Relocation)`

之後組語發明，所以第一行可改為 jmp foo ( 假設foo為第5行位址 )

組譯器作用就是計算 foo 位址，不管中間加入或減少多少指令

組譯器會重新計算 foo 位址


## 靜態連結
連結過程為: 位址和記憶體配置、符號解析、重定