# Makefile的基本結構



##Makefile的一般結構：
```sh
target……：dependency……
    command……
```

- –target（目標）：一個目標檔，可以是Object檔，也可
以是執行檔。還可以是一個標籤（Label）。

- –dependency（依賴）：要產生目標檔（target）所依
賴哪些檔

- –command（命令）：建立專案時需要執行的shell命令
（注：命令（command）部分的每行的縮進必須要使
用Tab鍵而不能使用多個空格）。

---
- Example

```sh
test.o: test.c test.h
    gcc –c test.c
```

當test.c或test.h檔在編譯之後又被修改，則make工具可自動重新編譯test.o，
如果在前後兩次編譯之間，test.c和test.h均沒有被修改，而且test.o還存在的話，
就沒有必要重新編譯。

---

## Makefile的基本內容


- 顯式規則：顯式規則說明如何產生一個或多個的目標檔。這是由
Makefile的書寫者明顯指出，要產生的檔，檔的依賴檔，產生的命
令。

- 變數定義。在Makefile中可以定義一系列的變數，變數一般都是字
串，當Makefile被執行時，變數的值會被擴充到相對應的引用位置
上。

- 隱含規則：由於GNU make具有自動推導功能，所以隱含規則可以比
較粗糙地簡略地書寫Makefile，然後由GNU make的自動推導功能完
成隱含規則的內容。

- 檔指示。其包括了三個部分，一個是在一個Makefile中引用另一個
Makefile，就像C語言中的include一樣；另一個是指根據某些情況指
定Makefile中的有效部分，就像C語言中的預編譯#if一樣；還有就是
定義一個多行的命令。

- 注釋。Makefile中只有行注釋，和UNIX的Shell腳本一樣，其注釋是
用“#”字元，如果你要在你的Makefile中使用“#”字元，可以用反斜框進
行轉義，如：“\#”
。


## Makefile中的變數

- Makefile中定義的變數，與C/C++語言中的巨集一樣，代
表一個文本字串，在Makefile被執行時候變數會自動地展
開在所使用的地方。Makefile中的變數可以使用在“目
標”
，
“依賴目標”
，
“命令”或Makefile的其他部分中。

- Makefile中變數的命名字可以包含字元、數位，下劃線
（可以是數位開頭），但不應該含有“:”
、
“#”
、
“=”或是空
字元（空格、Enter等）。

- Makefile中變數是大小寫敏感的，
“foo”
、
“Foo”和“FOO”是
三個不同的變數名。傳統的Makefile的變數名是全大寫的
命名方式

- 變數在聲明時需要給予初值，而在使用時，需要在變數名
前加上“$”符號

---

- Example

```sh
# makefile test for hello program
#written by Emdoor
CC=gcc
CFLAGS=
OBJS=hello.o
all: hello
hello: $(OBJS)
    $(CC) $(CFLAGS) $(OBJS) –o hello
hello.o: hello.c
    $(CC) $(CFLAGS) –c hello.c –o $(OBJS)
clean:
    rm –rf hello *.o
```

```sh
# makefile test for hello program
#written by Emdoor
CC=gcc
CFLAGS=
OBJS=hello.o

all: hello

hello: hello.o
    gcc hello.o –o hello

hello.o: hello.c
    gcc –c hello.c –o hello.o
clean:
    rm –rf hello *.o
```

## 命令的變數

```sh
AR 函式庫打包程式。預設命令是“ar”。
• AS 組合語言編譯程序。預設命令是“as”。
• CC C語言編譯程序。預設命令是“cc”。
• CXX C++語言編譯程序。預設命令是“g++”。
• CO 從 RCS檔中擴充檔程式。預設命令是“co”。
• CPP C程式的預處理器（輸出是標準輸出設備）。預設命令是“$(CC) –E”。
• FC Fortran 和 Ratfor 的編譯器和預處理程式。預設命令是“f77”。
• GET 從SCCS檔中擴充檔的程式。預設命令是“get”。
• LEX Lex方法分析器程式（針對於C或Ratfor）。預設命令是“lex”。
• PC Pascal語言編譯程序。預設命令是“pc”。
• YACC Yacc文法分析器（針對於C程式）。預設命令是“yacc”。
• YACCR Yacc文法分析器（針對於Ratfor程式）。預設命令是“yacc –r”。
• MAKEINFO 轉換Texinfo原始檔案（.texi）到Info檔程式。預設命令是“makeinfo”。
• TEX 從TeX原始檔案建立TeX DVI檔的程式。預設命令是“tex”。
• TEXI2DVI 從Texinfo原始檔案建立軍TeX DVI 檔的程式。預設命令是“texi2dvi”。
• WEAVE 轉換Web到TeX的程式。預設命令是“weave”。
• CWEAVE 轉換C Web 到 TeX的程式。預設命令是“cweave”。
• TANGLE 轉換Web到Pascal語言的程式。預設命令是“tangle”。
• CTANGLE 轉換C Web 到 C。預設命令是“ctangle”。
• RM 刪除檔命令。預設命令是“rm –f”。
```

## 命令參數變數

```sh
• ARFLAGS 函式庫打包程式AR命令的參數。預設值是“rv”
• ASFLAGS 組合語言編譯器參數。（當明顯地調用“.s”或“.S”檔案
時）。
• CFLAGS C語言編譯器參數。
• CXXFLAGS C++語言編譯器參數。
• COFLAGS RCS命令參數。
• CPPFLAGS C預處理器參數。（ C 和 Fortran 編譯器也會用
到）。
• FFLAGS Fortran語言編譯器參數。
• GFLAGS SCCS “get”程式參數。
• LDFLAGS 鏈結器參數。（如：“ld”）
• LFLAGS Lex文法分析器參數。
• PFLAGS Pascal語言編譯器參數。
• RFLAGS Ratfor 程式的Fortran 編譯器參數。
• YFLAGS Yacc文法分析器參數。
```

- Example

```sh
# makefile test for hello program
#written by Chhnet
CC=gcc
CFLAGS=

all: hello
hello: hello.o
    $(CC) $(CFLAGS) hello.o –o hello
hello.o: hello.c
    $(CC) $(CFLAGS) –c hello.c –o hello.o
clean:
    rm –rf hello *.o
```

```sh
# makefile test for hello program
#written by Chhnet
CC=gcc
CFLAGS=
OBJS=hello.o

all: hello
hello: $(OBJS)
    $(CC) $(CFLAGS) $^ -o $@
hello.o: hello.c
    $(CC) $(CFLAGS) –c $< -o $@
clean:
    rm –rf hello *.o
```

- Example


```sh
# makefile test for multi files program
#written by Emdoor
CC=gcc
CFLAGS=
OBJS=hello1.o hello2.o

all: hello
hello: $(OBJS)
    $(CC) $(CFLAGS) $^ -o $@
hello1.o: hello1.c
    $(CC) $(CFLAGS) –c $< -o $@
hello2.o: hello2.c
    $(CC) $(CFLAGS) –c $< -o $@
clean:
    rm –rf hello *.o
```

```sh
# makefile test for multi files program
#written by Chhnet
CC=gcc
CFLAGS=
CFILES=$(wildcard *.c)
OBJS=$(CFILES:%c=%.o)

all: hello
hello: $(OBJS)
    $(CC) $(CFLAGS) –o hello $(OBJS)
.c.o:
    $(CC) –c $<
clean:
    rm –rf hello *.o
```