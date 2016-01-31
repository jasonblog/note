# 設計

本章是「手把手教你構建 C 語言編譯器」系列的第二篇，我們要從整體上講解如何設計我們的 C 語言編譯器。

首先要說明的是，雖然標題是編譯器，但實際上我們構建的是 C 語言的解釋器，這意味著我們可以像運行腳本一樣去運行 C 語言的源代碼文件。這麼做的理由有兩點：

- 解釋器與編譯器僅在代碼生成階段有區別，而其它方面如詞法分析、語法分析是一樣的。
- 解釋器需要我們實現自己的虛擬機與指令集，而這部分能幫助我們瞭解計算機的工作原理。

### 編譯器的構建流程

一般而言，編譯器的編寫分為 3 個步驟：

- 詞法分析器，用於將字符串轉化成內部的表示結構。
- 語法分析器，將詞法分析得到的標記流（token）生成一棵語法樹。
- 目標代碼的生成，將語法樹轉化成目標代碼。

已經有許多工具能幫助我們處理階段1和2，如 flex 用於詞法分析，bison 用於語法分析。只是它們的功能都過於強大，屏蔽了許多實現上的細節，對於學習構建編譯器幫助不大。所以我們要完全手寫這些功能。

所以我們會根據下面的流程：

- 構建我們自己的虛擬機以及指令集。這後生成的目標代碼便是我們的指令集。
- 構建我們的詞法分析器
- 構建語法分析器

### 編譯器的框架

我們的編譯器主要包括 4 個函數：

- next() 用於詞法分析，獲取下一個標記，它將自動忽略空白字符。
- program() 語法分析的入口，分析整個 C 語言程序。
- expression(level) 用於解析一個表達式。
- eval() 虛擬機的入口，用於解釋目標代碼。


這裡有一個單獨用於解析「表達式」的函數 expression 是因為表達式在語法分析中相對獨立並且比較複雜，所以我們將它單獨作為一個模塊（函數）。

因為我們的源代碼看起來就像是：

```c
#include <stdio.h>
#include <stdlib.h>
#include <memory.h>
#include <string.h>

int token;            // current token
char *src, *old_src;  // pointer to source code string;
int poolsize;         // default size of text/data/stack
int line;             // line number

void next() {
    token = *src++;
    return;
}

void expression(int level) {
    // do nothing
}

void program() {
    next();                  // get next token
    while (token > 0) {
        printf("token is: %c\n", token);
        next();
    }
}


int eval() { // do nothing yet
    return 0;
}

int main(int argc, char **argv)
{
    int i, fd;

    argc--;
    argv++;

    poolsize = 256 * 1024; // arbitrary size
    line = 1;

    if ((fd = open(*argv, 0)) < 0) {
        printf("could not open(%s)\n", *argv);
        return -1;
    }

    if (!(src = old_src = malloc(poolsize))) {
        printf("could not malloc(%d) for source area\n", poolsize);
        return -1;
    }

    // read the source file
    if ((i = read(fd, src, poolsize-1)) <= 0) {
        printf("read() returned %d\n", i);
        return -1;
    }
    src[i] = 0; // add EOF character
    close(fd);

    program();
    return eval();
}
```
上面的代碼看上去挺複雜，但其實內容不多，就是讀取一個源代碼文件，逐個讀取每個字符，並輸出每個字符。這裡重要的是注意每個函數的作用，後面的文章中，我們將逐個填充每個函數的功能，最終構建起我們的編譯器。


本節的代碼可以在 [Github](https://github.com/lotabout/write-a-C-interpreter/tree/step-0) 上下載，也可以直接 clone

