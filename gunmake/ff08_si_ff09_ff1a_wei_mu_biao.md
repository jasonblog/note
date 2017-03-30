# （四）：偽目標


一般情況下，Makefile都會有一個clean目標，用於清除編譯過程中產生的二進制文件。我們在第一節的Makefile就用到了這個 clean目標，該目標沒有任何依賴文件，並且該目標對應的命令執行後不會生產clean文件。

像這種特點目標，它的規則所定義的命令不是去創建文件，而僅僅通過make指定目標來執行一些特定系統命令或其依賴為目標的規則（如all），稱為偽目標。

一個Makefile一般都不會只有一個偽目標，如果按Makefile的“潛規則”以及其約定俗成的名字來說的話，在較大的項目的Makefile中比較常用的為目標有這些：

```sh
all：執行主要的編譯工作，通常用作缺省目標，放在最前面。
Install：執行編譯後的安裝工作，把可執行文件、配置文件、文檔等分別拷到不同的安裝目錄。
clean：刪除編譯生成的二進制文件。
distclean：刪除除源文件之外的所有中間生成文件，如配置文件，文檔等。
tags：為vim等編輯器生成tags文件。
help：打印當前Makefile的幫助信息，比如有哪些目標可以有make指定去執行。
```

make處理Makefile時，首先讀取所有規則，建立關係依賴圖。然後從缺省目標（第一個目標）或指定的目標開始執行。像clean，tags這樣的目標一般不會作為缺省目標，也不會跟缺省目標有任何依賴關係，所以 make 無法生成它的依賴關係和決定它是否要執行。所以要執行這樣的目標時，必須要顯示的指定make該目標。就像前面我們清楚便已產生的中間二進制文件一樣，需要顯示執行命令：make clean。

偽目標也可以作為默認目標（如all），並且可以為其指定依賴文件。
我們先將version 1.0的Makefile完善下，我們可以加入幫助信息，tags等功能。


```sh
# A common Makefile for c programs, version 1.1  
  
# Copyright (C) 2014 shallnew \at 163 \dot com  
   
CFLAGS += -g -Wall -Werror -O2  
CPPFLAGS += -I. -I./inc  
LDFLAGS += -lpthread  
   
# SRC_OBJ = $(patsubst %.c, %.o, $(wildcard *.c))  
SRC_FILES = $(wildcard *.c)  
SRC_OBJ = $(SRC_FILES:.c=.o)  
SRC_BIN = target_bin  
   
all : $(SRC_BIN)  
   
$(SRC_BIN) : $(SRC_OBJ)  
>---$(CC) -o $@ $^ $(LDFLAGS)  
   
obj : $(SRC_OBJ)  
   
tags:  
>---ctags -R  
   
help:  
>---@echo "===============A common Makefile for cprograms=============="  
>---@echo "Copyright (C) 2014 liuy0711 \at 163 \dotcom"  
>---@echo "The following targets are support:"  
>---@echo  
>---@echo " all             - (==make) compile and link"  
>---@echo " obj             - just compile, without link"  
>---@echo " clean           - clean target"  
>---@echo " distclean       - clean target and otherinformation"  
>---@echo " tags            - create ctags for vim editor"  
>---@echo " help            - print help information"  
>---@echo  
>---@echo "To make a target, do 'make [target]'"  
>---@echo "========================= Version 1.1======================="  
   
# clean target  
clean:  
>---$(RM) $(SRC_OBJ) $(SRC_BIN) $(SRC_BIN).exe  
   
distclean:  
>---$(RM) $(SRC_OBJ) $(SRC_BIN) $(SRC_BIN).exe tags *~  
```

make會把執行的命令打印在屏幕上，如果我們不想把命令打印在屏幕上，只顯示命令結果時，直接在命令前面加上符號“@”就可以實現。如上面help目標一樣，只顯示命令結果。一般我們會在make時都會輸出“Compiling xxx.c…”,不輸出編譯時的命令。我們在後面寫Makefile時可以模仿。

如果當前目錄下存在一個和偽目標同名的文件時（如clean），此時如果執行命令make clean後出現如下結果：


```sh
# touch clean  
# make clean  
make: `clean' is up to date.  
```

這是因為clean文件沒有依賴文件，make認為目標clean是最新的不會去執行規則對應的命令。為瞭解決這個問題，我們可以明確地將該目標聲明為偽目標。將一個目標聲明為偽目標需要將它作為特殊目標.PHONY”的依賴。如下：


```sh
.PHONY : clean  
```

這條規則寫在clean:規則的後面也行，也能起到聲明clean是偽目標的作用
這樣修改一下之前Makefile，將所有偽目標都作為.PHONY的依賴：


```sh
.PHONY : all obj tag help clean disclean 
```

這樣在當前目錄下存在文件clean時執行:

```sh
# make clean  
rm -f debug.o ipc.o main.o timer.o tools.o target_bin target_bin.exe  
```

發現問題解決。
最後，給出今天最終的Makefile：

```sh
# A common Makefile for c programs, version 1.1                                                                                                                                            
# Copyright (C) 2014 shallnew \at 163 \dot com  
   
CFLAGS += -g -Wall -Werror -O2  
CPPFLAGS += -I. -I./inc  
LDFLAGS += -lpthread  
   
# SRC_OBJ = $(patsubst %.c, %.o, $(wildcard *.c))  
SRC_FILES = $(wildcard *.c)  
SRC_OBJ = $(SRC_FILES:.c=.o)  
SRC_BIN = target_bin  
   
all : $(SRC_BIN)  
   
$(SRC_BIN) : $(SRC_OBJ)  
>---$(CC) -o $@ $^ $(LDFLAGS)  
   
obj : $(SRC_OBJ)  
   
tag:  
>---ctags -R  
   
help:  
>---@echo "===============A common Makefile for cprograms=============="  
>---@echo "Copyright (C) 2014 liuy0711 \at 163 \dotcom"  
>---@echo "The following targets are support:"  
>---@echo  
>---@echo " all             - (==make) compile and link"  
>---@echo " obj             - just compile, without link"  
>---@echo " clean           - clean target"  
>---@echo " distclean       - clean target and other information"  
>---@echo " tags            - create ctags for vim editor"  
>---@echo " help            - print help information"  
>---@echo  
>---@echo "To make a target, do 'make [target]'"  
>---@echo "========================= Version 1.1======================="  
   
# clean target  
clean:  
>---$(RM) $(SRC_OBJ) $(SRC_BIN) $(SRC_BIN).exe  
   
distclean:  
>---$(RM) $(SRC_OBJ) $(SRC_BIN) $(SRC_BIN).exe tags *~  
  
.PHONY : all obj tag help clean disclean  
```
