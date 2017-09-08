# Makefile 語法和示範
###### tags: `sysprog2017`

## 何謂 Makefile ?
* make 指令雖然有很多內建的功能，但它也無法知道如何建立應用程式。故必須提供一個檔案，即 makefile，告訴 make 如何建立應用程式。
* makefile 與專案的原始碼檔案，通常放在同一個目錄中。
* 可以同時有很多不同的 makefile 管理專案的不同部分。
* make 命令和 makefile 的結合，不僅控制原始碼的編譯，也可以用來準備使用手冊文件、安裝應用程式到目的目錄中。

## 使用 Makefile 的好處
* 如果這個專案沒有編譯過，那麼我們的所有程式碼都要編譯並被連結
* 如果這個專案的某幾份程式碼被修改，那麼我們只編譯被修改的程式，並連結目標程式
* 如果這個專案的標頭檔被改變了，那麼我們需要編譯引用了這幾個標頭檔的程式碼，並連結目標程式

## makefile 語法
```
target: 目標檔1 目標檔2 
<tab>gcc -o 欲建立的執行檔 目標檔1 目標檔2
```
* target 與相依檔案(就是目標檔)之間需以『:』隔開
* `<tab>` 需要在命令行的第一個字元
* makefile 語法中之 `<tab>` 與空白所有的法則必須在同一行，而且行首必須為 `<tab>`；不能為空白。
* 在 makefile 中，行尾如果有一個空白，會造成 make 命令執行錯誤。
* makefile 的註解（comment）：
    * 如同 C 原始碼檔案一般，在 makefile 中，以 `#` 為行首的文字都是註解。
    * makefile 中的註解只是協助作者和其它人，了解 makefile 的內容。
* makefile 的語法規則：
    * makefile 是由很多相依性項目（dependencies）和法則（rules）所組成。
    * 相依性項目，描述目標項目（target，要產生的檔案）和產生該檔案之相關的原始碼檔案。
    * 法則是說明如何根據相依性檔案，來建立目標項目。
    * make 命令利用 makefile，先決定依序建立哪些目標項目，再決定依序喚起哪些法則。
    * **相依性項目（dependency)**
```shell=
myapp: main.o 2.o 3.o
main.o: main.c a.h
2.o: 2.c a.h b.h
3.o: 3.c b.h c.h
```
  * 例如：目標項目 myapp 與 main.o、2.o、3.o 相關，main.o 與 main.c、a.h 相關，以此類推。
改變 b.h，需要重建 2.o 和 3.o，因為 2.o 和 3.o 被改變，又需要重建 myapp。
以 gcc 的 -MM 選項以 makefile 的格式，輸出相依性項目。

## 語法
* **變數（巨集）定義**
  可以讓我們脫離那些冗長乏味的編譯選項，為編寫makefile文件帶來很大的方便，如︰
  `　OBJECTS= filea.o fileb.o filec.o `
  使用時在前面加`$()`的符號，如︰`$(OBJECTS)`
* **:=**
變數的值決定於它在 Makefile 中的位置，而不是整個 Makefile 展開後最終的值
* **?=**
若變數未定義，則替它指定新的值。否則，採用原有的值
如︰`FOO ?= bar`
若 FOO 未定義，則 FOO = bar；若 FOO 已定義，則 FOO 的值維持不變。
* **+=**
此時 CFLAGS 的值就變成 -Wall -g -O2 
```shell=
CFLAGS = -Wall -g
CFLAGS += -O2
```
* **自動化變數**
	* $@  工作目標檔名
	* $<  第一個必要條件的檔名
	* $^  所有必要條件的檔名, 並以空格隔開這些檔名. (這份清單已經拿掉重複的檔名)
	* $*  工作目標的主檔名.
* **萬用字元**
makefile 中所用的萬用字元是 % ，代表所有可能的字串，前後可以接指定的字串來表示某些固定樣式的字串。例如 `%.c` 表示結尾是 .c 的所有字串。因此我們改寫 makefile 如下
```shell=
CC = gcc
OBJS = a.o b.o c.o

all: test

%.o: %.c
  $(CC) -c -o $@ $<

test: $(OBJS)
  $(CC) -o test &^
```

* **特別字元**
	* @  不要顯示執行的指令 
		* 因執行 make 指令後會在終端機印出正在執行的指令
	* `-` 表示即使該行指令出錯，也不會中斷執行
		* 而 make 只要遇到任何錯誤就會中斷執行。但像是在進行 clean 時，也許根本沒有任何檔案可以 clean，因而 rm 會傳回錯誤值，因而導致 make 中斷執行。我們可以利用 - 來關閉錯誤中斷功能，讓 make 不會因而中斷。

例：
```shell=
.PHONY: clean
clean:
    @echo "Clean..."
    -rm *.o 
```

* **目標**

這個項目所要建立的檔案，必須以 `:` 結尾。例：
```shell=
foo.o: common.h
    gcc -c foo.c 
```
* 其中，foo.o 是這個項目要建立的檔案；common.h 是相依性的項目/檔案；而 gcc -c foo.c 則為要產生這個項目所要執行的指令。
* make 在編譯時，若發現 target 比較新，也就是 dependencies 都比 target 舊，那麼將不會重新建立 target，如此可以避免不必要的編譯動作。
* 若該項目並非檔案，則為 fake 項目。如此一來將不會建立 target 檔案。但為了避免 make 有時會無去判斷 target 是否為檔案或 fake 項目，建議利用 .PHONY 來指定該項目為 fake 項目。例：

```shell=
.PHONY: clean
clean:
    rm *.o
```

在上例中，若不使用 .PHONY 來指定 clean 為 fake 項目的話，若目錄中同時存在了一個名為 clean 的檔案，則 clean 這個項目將被視為要建立 clean 這個檔案，但 clean 這個項目卻又沒有任何的 dependencies，也因此，clean 項目將永遠被視為 up-to-date，永遠不會被執行。
因為利用了 .PHONY 來指定 clean 為 fake 項目，所以 make 不會去檢查目錄中是否存在了一個名為 clean 的檔案。如此也可以提昇 make 的執行效率。

## 引用及參考連結
* [Makefile 概念與基礎I](https://dywang.csie.cyut.edu.tw/dywang/linuxProgram/node49.html)
* [Makefile 概念與基礎II](https://read01.com/7K2D3.html)
* [詳盡語法](http://tetralet.luna.com.tw/?op=ViewArticle&articleId=185)