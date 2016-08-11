# 談談C語言的全域變數和 Linux Object 檔案的關係


印象中以前學過C語言中跨檔案的全域變數是這樣宣告的。
檔案一：`int g_var1;`
檔案二：`extern int g_val1;`

然而，我從來沒有想過如果沒有extern的情況會發生什麼狀況。加上之前看過的objdump和nm後手癢，所以把可能的排列組合看看可能發生什麼事。

這邊要先知道common object表示object檔案已經紀錄下這個symbol，最後link的時候才會決定要怎麼處理。會放在common section [出處](http://stackoverflow.com/questions/18335256/what-does-com-stand-for-in-objdump-symbol-table)。

由於結果和分析又臭又長，連我自己都不太想回去看。建議先看[結論](#elf-concl)，有疑惑再回來看結果和分析吧。

## 目錄

* [測試環境](#elf-env)
* [測試程式](#elf-ex)
* [測試結果](#elf-res)
* [結論](#elf-concl)
* [參考資料](#elf-ref)


<a name="elf-env"></a>
## 測試環境

```
$ lsb_release -a
No LSB modules are available.
Distributor ID:	Ubuntu
Description:	Ubuntu 14.04.1 LTS
Release:	14.04
Codename:	trusty
```

* GNU objdump (GNU Binutils for Ubuntu) 2.24
* GNU nm (GNU Binutils for Ubuntu) 2.24


<a name="elf-ex"></a>
## 測試程式
非常簡單，就是有兩個檔案存放全域變數，每次測試時先更改全域變數
* 是否有初始值
* 宣告方式是否有`extern`
全部的排列組合放在[這邊](#elf-res)

- var1.c

```c
int g_var1;
```

- var2.c

```c 
int g_var1;
```
- main.c

```c
#include <stdio.h>
extern int g_var1;
int main()
{
    printf("Hello %d\n", g_var1);
    return 0;
}
```

- Makefile

```sh
TARGET=glvar
SRCS=var_1.c var_2.c main.c
OBJS=$(patsubst %.c, %.o, $(SRCS))
CFLAGS=-g

$(TARGET): $(OBJS)
	$(CC) -o $@ $^

test: $(TARGET)
	objdump -t $(OBJS) $(TARGET)
	nm -A $(OBJS) $(TARGET)
	./$(TARGET)

clean:
	rm -f $(TARGET) $(OBJS)
```


## 測試結果

| var1.c 宣告方式         | var2.c宣告方式         | 編譯結果  | nm 和 objdump 分析  |
| --------------------- | --------------------- | ----     | ------------------: |
| int g_var;            | int g_var;            |  成功     | [分析](#elf-res1)  |
| int g_var = 2;        | int g_var;            |  成功     | [分析](#elf-res2)  |
| int g_var;            | int g_var = 3;        |  成功     | [分析](#elf-res3)  |
| int g_var = 2;        | int g_var = 3;        |  失敗     | [分析](#elf-res4)  |

| var1.c 宣告方式         | var2.c宣告方式             | 編譯結果  | nm 和 objdump 分析  |
| --------------------- | ------------------------- | ----     | ------------------: |
| int g_var;            | extern <br>int g_var;     | 成功      | [分析](#elf-res5)  |
| int g_var = 2;        | extern <br>int g_var;     | 成功      | [分析](#elf-res6)  |
| int g_var;            | extern <br>int g_var = 3; | 有警告    | [分析](#elf-res7)  |
| int g_var = 2;        | extern <br>int g_var = 3; | 失敗      | [分析](#elf-res8)  |

| var1.c 宣告方式            | var2.c宣告方式              | 編譯結果  | nm 和 objdump 分析  |
| ------------------------- | ------------------------- | ----     | ------------------: |
| extern <br>int g_var;     | extern <br>int g_var;     | link失敗  | [分析](#elf-res9)  |
| extern <br>int g_var = 2; | extern <br>int g_var;     | 有警告    | [分析](#elf-res10)  |
| extern <br>int g_var      | extern <br>int g_var = 3; | 有警告    | [分析](#elf-res11)  |
| extern <br>int g_var = 2; | extern <br>int g_var = 3; | link失敗  | [分析](#elf-res12)  |

## 分析 var1.c: int g_var; var2.c: int g_var; 的結果

* `objdump -t` 顯示object檔案的symbol table欄位資料：
  * var1.o: 0000000000000004       O *COM*  0000000000000004 g_var1
      * 這是一個object，放在commom section
  * var2.o: 0000000000000004       O *COM*  0000000000000004 g_var1
      * 這是一個object，放在commom section。
  * main.o: 0000000000000000         *UND*  0000000000000000 g_var1
      * 這是一個undefined symbol
  * glvar: 0000000000601044 g     O .bss   0000000000000004              g_var1
      * 這個symbol放在`.bss`，也就是未初始化的全域變數
      * 這是最後產生的executable object
* `nm -A` 顯示object檔案的symbol：
  * var_1.o:0000000000000004 C g_var1
      * 這是一個common symbol。
  * var_2.o:0000000000000004 C g_var1
      * 這是一個common symbol。
  * main.o:                 U g_var1
      * 這是一個undefined symbol
  * glvar: `glvar:0000000000601044 B g_var1`
      * 這是最後產生的executable object
      * 這個symbol放在`.bss`，也就是未初始化的全域變數

## 分析 var1.c: int g_var = 2; var2.c: int g_var; 的結果

* `objdump -t` 顯示object檔案的symbol table欄位資料：
  * var1.o: 0000000000000000 g     O .data	0000000000000004 g_var1
      * 這是一個全域變數物件，放在有初始值的`.data` section
  * var2.o: 0000000000000004       O *COM*  0000000000000004 g_var1
      * 這是一個object，放在commom section
  * main.o: 0000000000000000         *UND*  0000000000000000 g_var1
      * 這是一個undefined symbol
  * glvar: 0000000000601040 g     O .data	0000000000000004              g_var1
      * 這是一個全域變數物件，放在有初始值的`.data` section
      * 這是最後產生的executable object
* `nm -A` 顯示object檔案的symbol：
  * var_1.o:0000000000000000 D g_var1
      * 這個symbol是全域變數，並且放在有初始值的資料section
  * var_2.o:0000000000000004 C g_var1
      * 這是一個common symbol。
  * main.o:                 U g_var1
      * 這是一個undefined symbol
  * glvar:0000000000601040 D g_var1
      * 這是最後產生的executable object
      * 這個symbol是全域變數，並且放在有初始值的資料section
      
## 分析 var1.c: int g_var2 = 2; var2.c:  int g_var; 的結果
和上面的差別只有var1.o和var2.o的結果對調而已。

## 分析 var1.c: int g_var2 = 2; var2.c:  int g_var = 3; 的結果
* 錯誤訊息： g_var1重複定義

```sh
$ make test
cc -g   -c -o var_1.o var_1.c
cc -g   -c -o var_2.o var_2.c
cc -g   -c -o main.o main.c
cc -o glvar var_1.o var_2.o main.o
var_2.o:(.data+0x0): multiple definition of `g_var1'
var_1.o:(.data+0x0): first defined here
collect2: error: ld returned 1 exit status
make: *** [glvar] Error 1
```

* `objdump -t` 顯示object檔案的symbol table欄位資料：
  * var1.o: 0000000000000000 g     O .data	0000000000000004 g_var1
      * 這是一個全域變數物件，放在有初始值的`.data` section
  * var2.o: 0000000000000000 g     O .data	0000000000000004 g_var1
      * 這是一個全域變數物件，放在有初始值的`.data` section
  * main.o: 0000000000000000         *UND*  0000000000000000 g_var1
      * 這是一個undefined symbol 
* `nm -A` 顯示object檔案的symbol：
  * var_1.o:0000000000000000 D g_var1
      * 這個symbol是全域變數，並且放在有初始值的資料section
  * var_2.o:0000000000000000 D g_var1
      * 這個symbol是全域變數，並且放在有初始值的資料section    
  * main.o:                 U g_var1
      * 這是一個undefined symbol

## 分析 var1.c: int g_var; var2.c: extern int g_var; 的結果

* `objdump -t` 顯示object檔案的symbol table欄位資料：
  * var1.o: 0000000000000004       O *COM*	0000000000000004 g_var1
      * 這是一個object，放在commom section
  * var2.o: 沒有g_var1這個entry
  * main.o: 0000000000000000         *UND*  0000000000000000 g_var1
      * 這是一個undefined symbol
  * glvar: 0000000000601040 g     O .data	0000000000000004              g_var1
      * 這是一個全域變數物件，放在有初始值的`.data` section
      * 這是最後產生的executable object
* `nm -A` 顯示object檔案的symbol：
  * var_1.o:0000000000000004 C g_var1
      * 這是一個common symbol
  * var_2.o: 沒有symbol
  * main.o:                 U g_var1
      * 這是一個undefined symbol
  * glvar:0000000000601040 B g_var1
      * 這是最後產生的executable object
      * 這個symbol是全域變數，並且放在沒有初始值的資料section

## 分析 var1.c: int g_var = 2; var2.c: extern int g_var; 的結果

* `objdump -t` 顯示object檔案的symbol table欄位資料：
  * var1.o: 0000000000000000 g     O .data	0000000000000004 g_var1
      * 這是一個全域變數物件，放在有初始值的`.data` section
  * var2.o: 沒有g_var1這個entry
  * main.o: 0000000000000000         *UND*  0000000000000000 g_var1
      * 這是一個undefined symbol
  * glvar: 0000000000601040 g     O .data	0000000000000004              g_var1
      * 這是一個全域變數物件，放在有初始值的`.data` section
      * 這是最後產生的executable object
* `nm -A` 顯示object檔案的symbol：
  * var_1.o:0000000000000000 D g_var1
      * 這個symbol是全域變數，並且放在有初始值的資料section
  * var_2.o: 沒有symbol
  * main.o:                 U g_var1
      * 這是一個undefined symbol
  * glvar:0000000000601040 D g_var1
      * 這是最後產生的executable object
      * 這個symbol是全域變數，並且放在有初始值的資料section

## 分析 var1.c: int g_var; var2.c: extern int g_var = 3; 的結果

* 得到警告和錯誤：

```
$ make test
rm -f glvar  var_1.o  var_2.o  main.o
cc -g   -c -o var_1.o var_1.c
cc -g   -c -o var_2.o var_2.c
var_2.c:1:12: warning: ‘g_var1’ initialized and declared ‘extern’ [enabled by default]
 extern int g_var1 = 3;
            ^
cc -g   -c -o main.o main.c
cc -o glvar var_1.o var_2.o main.o
var_2.o:(.data+0x0): multiple definition of `g_var1'
var_1.o:(.data+0x0): first defined here
collect2: error: ld returned 1 exit status
make: *** [glvar] Error 1
```

* `objdump -t` 顯示object檔案的symbol table欄位資料：
  * var1.o: 0000000000000004       O *COM*	0000000000000004 g_var1
      * 這是一個object，放在commom section
  * var2.o: **0000000000000000 g     O .data	0000000000000004 g_var1**
      * 這是一個全域變數物件，放在有初始值的`.data` sectio
  * main.o: 0000000000000000         *UND*  0000000000000000 g_var1
      * 這是一個undefined symbol
  * glvar: 0000000000601040 g     O .data	0000000000000004              g_var1
      * 這是一個全域變數物件，放在有初始值的`.data` section
      * 這是最後產生的executable object
* `nm -A` 顯示object檔案的symbol：
  * var_1.o:0000000000000000 D g_var1
      * 這是一個common symbol
  * var_2.o:0000000000000000 D g_var1
      * **這個symbol是全域變數，並且放在有初始值的資料section**
  * main.o:                 U g_var1
      * 這是一個undefined symbol
  * glvar:0000000000601040 D g_var1
      * 這是最後產生的executable object
      * 這個symbol是全域變數，並且放在有初始值的資料section
      
## 分析 var1.c: int g_var = 2; var2.c: extern int g_var = 3; 的結果

* 得到警告和link錯誤：

```sh
$  make test
cc -g   -c -o var_1.o var_1.c
cc -g   -c -o var_2.o var_2.c
var_2.c:1:12: warning: ‘g_var1’ initialized and declared ‘extern’ [enabled by default]
 extern int g_var1 = 3;
            ^
cc -g   -c -o main.o main.c
cc -o glvar var_1.o var_2.o main.o
var_2.o:(.data+0x0): multiple definition of `g_var1'
var_1.o:(.data+0x0): first defined here
collect2: error: ld returned 1 exit status
make: *** [glvar] Error 1
```

* `objdump -t` 顯示object檔案的symbol table欄位資料：
  * var1.o: 0000000000000000 g     O .data	0000000000000004 g_var1
      * 這是一個全域變數物件，放在有初始值的`.data` section
  * var2.o: 0000000000000000 g     O .data	0000000000000004 g_var1
      * 這是一個全域變數物件，放在有初始值的`.data` section
  * main.o: 0000000000000000         *UND*  0000000000000000 g_var1
      * 這是一個undefined symbol 
* `nm -A` 顯示object檔案的symbol：
  * var_1.o:0000000000000000 D g_var1
      * 這個symbol是全域變數，並且放在有初始值的資料section
  * var_2.o:0000000000000000 D g_var1
      * 這個symbol是全域變數，並且放在有初始值的資料section    
  * main.o:                 U g_var1
      * 這是一個undefined symbol

## 分析 var1.c: extern int g_var; var2.c: extern int g_var; 的結果

* link時發生錯誤

```sh
$ make test
cc -g   -c -o var_1.o var_1.c
cc -g   -c -o var_2.o var_2.c
cc -g   -c -o main.o main.c
cc -o glvar var_1.o var_2.o main.o
main.o: In function `main':
/home/wen/work/practice/Linux_Programming_Practice/16_global_var/main.c:5: undefined reference to `g_var1'
collect2: error: ld returned 1 exit status
make: *** [glvar] Error 1
```

* `objdump -t` 顯示object檔案的symbol table欄位資料：
  * var1.o: 沒有g_var這個 entry
  * var2.o: 沒有g_var這個 entry
  * main.o: 0000000000000000         *UND*  0000000000000000 g_var1
      * 這是一個undefined symbol 
* `nm -A` 顯示object檔案的symbol：
  * var_1.o: 沒有symbol
  * var_2.o: 沒有symbol
  * main.o:                 U g_var1
      * 這是一個undefined symbol

## 分析 var1.c: extern int g_var = 2; var2.c: extern int g_var; 的結果

* 得到警告：

```sh
var_1.c:1:12: warning: ‘g_var1’ initialized and declared ‘extern’ [enabled by default]
```

* `objdump -t` 顯示object檔案的symbol table欄位資料：
  * var1.o: 0000000000000000 g     O .data	0000000000000004 g_var1
    * 這是一個全域變數物件，放在有初始值的`.data` section
  * var2.o: 沒有g_var這個 entry
  * main.o: 0000000000000000         *UND*  0000000000000000 g_var1
      * 這是一個undefined symbol 
* `nm -A` 顯示object檔案的symbol：
  * var_1.o:0000000000000000 D g_var1
      * 這個symbol是全域變數，並且放在有初始值的資料section
  * var_2.o: 沒有symbol
  * main.o:                 U g_var1
      * 這是一個undefined symbol

## 分析 var1.c: extern int g_var2; var2.c: extern int g_var = 3; 的結果
和上面的差別只有var1.o和var2.o的結果對調而已。

## 分析 var1.c: extern int g_var2 = 2; var2.c: extern int g_var = 3; 的結果

* 得到警告和錯誤：

```sh
$ make clean ; make test
rm -f glvar  var_1.o  var_2.o  main.o
cc -g   -c -o var_1.o var_1.c
var_1.c:1:12: warning: ‘g_var1’ initialized and declared ‘extern’ [enabled by default]
 extern int g_var1 = 2;
            ^
cc -g   -c -o var_2.o var_2.c
var_2.c:1:12: warning: ‘g_var1’ initialized and declared ‘extern’ [enabled by default]
 extern int g_var1 = 3;
            ^
cc -g   -c -o main.o main.c
cc -o glvar var_1.o var_2.o main.o
var_2.o:(.data+0x0): multiple definition of `g_var1'
var_1.o:(.data+0x0): first defined here
collect2: error: ld returned 1 exit status
make: *** [glvar] Error 1
```

* `objdump -t` 顯示object檔案的symbol table欄位資料：
  * var1.o: 0000000000000000 g     O .data	0000000000000004 g_var1
      * 這是一個全域變數物件，放在有初始值的`.data` section
  * var2.o: 0000000000000000 g     O .data	0000000000000004 g_var1
      * 這是一個全域變數物件，放在有初始值的`.data` section
  * main.o: 0000000000000000         *UND*  0000000000000000 g_var1
      * 這是一個undefined symbol 
* `nm -A` 顯示object檔案的symbol：
  * var_1.o:0000000000000000 D g_var1
      * 這個symbol是全域變數，並且放在有初始值的資料section
  * var_2.o:0000000000000000 D g_var1
      * 這個symbol是全域變數，並且放在有初始值的資料section    
  * main.o:                 U g_var1
      * 這是一個undefined symbol


## 結論

* common symbol表示object檔案已經紀錄下這個symbol，先放在common section。最後link的時候才會決定要怎麼處理。 [出處](http://stackoverflow.com/questions/18335256/what-does-com-stand-for-in-objdump-symbol-table)
* `gcc -c`產生的object檔案中未初始化的全域變數會放在common section而不是`.bss` section，原因是link時期其他的object可能有相同名稱的symbol，而這些相同的symbol可能有初始化也可能沒有。而linker需要全盤瞭解後再做出決定要放在`.bss`還是`.data` section。
* 最後link出來的executable object檔案不會有common symbol
* 多個檔案宣告同樣名稱的全域變數時，編譯時會依下面的情況處理
  * 超過兩個檔案有設初始值，噴錯誤
  * 都沒設定初始值，產生的object檔案放common section。link時放`.bss` section。
  * 一個檔案有設定初始值，其他沒有設定初始值的情形
      * 有設定初始值那個檔案的object檔案變數放在`.data`
      * 其他沒有設定初始值有設定初始值那個檔案的object檔案變數放在common section
      * link時放`.data` section
* 使用`extern`全域變數的時候
  * 程式碼沒用到該全域變數在未link前的object 檔案不會有該symbol
  * 程式碼用到該全域變數在未link前的object 檔案該symbol為undefined
  * 程式碼給初始值會產生警告，並且會被新增到該object 檔案的symbol table，以至於其他檔案宣告該全域變數會產生重複定義的錯誤。

## 參考資料

* [stackoverflow: What does *COM* stand for in objdump symbol table?](http://stackoverflow.com/questions/18335256/what-does-com-stand-for-in-objdump-symbol-table)
* [從 Singleton 談 constructor](http://www.slideshare.net/lubatang/singleton-constructor)
* [stackoverflow: .bss vs COMMON: what goes where?](http://stackoverflow.com/questions/16835716/bss-vs-common-what-goes-where)
* [Sw@y's Notes: ELF中的.bss section和COMMON section](http://swaywang.blogspot.tw/2012/06/elfbss-sectioncommon-section.html)
* [GNU linker ld: Linker Scripts](https://sourceware.org/binutils/docs/ld/Scripts.html#Scripts)
* `man nm`
