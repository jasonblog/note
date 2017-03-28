# （五）：嵌套執行


在大一些的項目裡面，所有源代碼不會只放在同一個目錄，一般各個功能模塊的源代碼都是分開的，各自放在各自目錄下，並且頭文件和.c源文件也會有各自的目錄，這樣便於項目代碼的維護。這樣我們可以在每個功能模塊目錄下都寫一個Makefile，各自Makefile處理各自功能的編譯鏈接工作，這樣我們就不必把所有功能的編譯鏈接都放在同一個Makefile裡面，這可使得我們的Makefile變得更加簡潔，並且編譯的時候可選擇編譯哪一個模塊，這對分塊編譯有很大的好處。

現在我所處於工程目錄樹如下：


```sh
├── include  
│   ├── common.h  
│   ├── ipc  
│   │   └── ipc.h  
│   └── tools  
│       ├── base64.h  
│       ├── md5.h  
│       └── tools.h  
├── Makefile  
├── src  
│   ├── ipc  
│   │   ├── inc  
│   │   ├── Makefile  
│   │   └── src  
│   │       └── ipc.c  
│   ├── main  
│   │   ├── inc  
│   │   ├── Makefile  
│   │   └── src  
│   │       ├── main.c  
│   │       └── main.c~  
│   └── tools  
│       ├── inc  
│       ├── Makefile  
│       └── src  
│           ├── base64.c  
│           ├── md5.c  
│           └── tools.c  
└── tags  
  
13 directories, 16 files  
```

這樣組織項目源碼要比之前合理一些，那這樣怎麼來寫Makefile呢？我們可以在每個目錄下寫一個Makefile，通過最頂層的Makefile一層一層的向下嵌套執行各層Makefile。那麼我們最頂層的Makefile簡單點的話可以這樣寫：


```sh
# top Makefile for xxx  
  
all :  
>---$(MAKE) -C src  
  
tags:  
>---ctags -R  
  
clean :  
>---$(MAKE) -C src clean  
  
.PHONY : all clean tags  
```

命令：
```sh
>---$(MAKE) -C src
```
就是進入src目錄繼續執行該目錄下的Makefile。然後src目錄下的Makefile在使用同樣的方法進入下一級目錄tools、main、ipc，再執行該目錄下的Makefile。其實這樣有些麻煩，我們可以直接從頂層目錄進入最後的目錄執行make。再加入一些偽目標完善下，我們的頂層Makefile就出來了：

```sh
# Top Makefile for C program  
  
# Copyright (C) 2014 shallnew \at 163 \dot com  
  
all :  
>---$(MAKE) -C src/ipc  
>---$(MAKE) -C src/tools  
>---$(MAKE) -C src/main  
  
tags:  
>---ctags -R  
  
help:  
>---@echo "===============A common Makefilefor c programs=============="  
>---@echo "Copyright (C) 2014 liuy0711 \at 163\dot com"  
>---@echo "The following targets aresupport:"  
>---@echo  
>---@echo " all              - (==make) compile and link"  
>---@echo " obj              - just compile, withoutlink"  
>---@echo " clean            - clean target"  
>---@echo " distclean        - clean target and otherinformation"  
>---@echo " tags             - create ctags for vimeditor"  
>---@echo " help             - print help information"  
>---@echo  
>---@echo "To make a target, do 'make[target]'"  
>---@echo "========================= Version2.0 ======================="  
  
obj:  
>---$(MAKE) -C src/ipc obj  
>---$(MAKE) -C src/tools obj  
>---$(MAKE) -C src/main obj  
  
clean :  
>---$(MAKE) -C src/ipc clean  
>---$(MAKE) -C src/tools clean  
>---$(MAKE) -C src/main clean  
  
distclean:  
>---$(MAKE) -C src/ipc distclean  
>---$(MAKE) -C src/tools distclean  
>---$(MAKE) -C src/main distclean  
  
.PHONY : all clean distclean tags help  
```

當我們這樣組織源代碼時，最下面層次的Makefile怎麼寫呢？肯定不可以將我們上一節的Makefile（version 1.1）直接拷貝到功能模塊目錄下，需要稍作修改。不能所有的模塊都最終生成各自的可執行文件吧，我們目前是一個工程，所以最後只會生成一個可執行程序。我們這樣做，讓主模塊目錄生成可執行文件，其他模塊目錄生成靜態庫文件，主模塊鏈接時要用其他模塊編譯產生的庫文件來生成最終的程序。將上一節Makefile稍作修改得出編譯庫文件Makefile和編譯可執行文件Makefile分別如下：


```sh
# A Makefile to generate archive file  
# Copyright (C) 2014 shallnew \at 163 \dot com  
  
  
CFLAGS += -g -Wall -Werror -O2  
CPPFLAGS += -I. -I./inc -I../../include  
  
# SRC_OBJ = $(patsubst %.c, %.o, $(wildcard *.c))  
SRC_FILES = $(wildcard src/*.c)  
SRC_OBJ = $(SRC_FILES:.c=.o)  
SRC_LIB = libtools.a  
  
all : $(SRC_LIB)  
  
$(SRC_LIB) : $(SRC_OBJ)  
>---$(AR) rcs $@ $^  
>---cp $@ ../../libs  
  
obj : $(SRC_OBJ)  
  
# clean target  
clean:  
>---$(RM) $(SRC_OBJ) $(SRC_LIB)  
  
distclean:  
>---$(RM) $(SRC_OBJ) $(SRC_LIB) tags *~  
  
.PHONY : all obj clean disclean  
```

```sh
# A Makefile to generate executive file                                                                                                                                                     
# Copyright (C) 2014 shallnew \at 163 \dot com  
  
CFLAGS += -g -Wall -Werror -O2  
CPPFLAGS += -I. -I./inc -I../../include  
LDFLAGS += -lpthread -L../../libs -ltools -lipc  
  
  
# SRC_OBJ = $(patsubst %.c, %.o, $(wildcard *.c))  
SRC_FILES = $(wildcard src/*.c)  
SRC_OBJ = $(SRC_FILES:.c=.o)    
SRC_BIN = target_bin            
  
all : $(SRC_BIN)  
  
$(SRC_BIN) : $(SRC_OBJ)         
>---$(CC) -o $@ $^ $(LDFLAGS)   
  
obj : $(SRC_OBJ)  
  
# clean target  
clean:  
>---$(RM) $(SRC_OBJ) $(SRC_BIN) $(SRC_BIN).exe  
  
distclean:  
>---$(RM) $(SRC_OBJ) $(SRC_BIN) $(SRC_BIN).exe tags*~  
  
.PHONY : all obj clean disclean  
```

最後在頂層執行：

```sh
# make clean  
  
make -C src/ipc clean  
make[1]: Entering directory`/home/Myprojects/example_make/version-3.0/src/ipc'  
rm -f src/ipc.o libipc.a  
make[1]: Leaving directory`/home/Myprojects/example_make/version-3.0/src/ipc'  
make -C src/tools clean  
make[1]: Entering directory `/home/Myprojects/example_make/version-3.0/src/tools'  
rm -f src/base64.o src/md5.o src/tools.o libtools.a  
make[1]: Leaving directory`/home/Myprojects/example_make/version-3.0/src/tools'  
make -C src/main clean  
make[1]: Entering directory`/home/Myprojects/example_make/version-3.0/src/main'  
rm -f src/main.o target_bin target_bin.exe  
make[1]: Leaving directory`/home/Myprojects/example_make/version-3.0/src/main'  
# make  
make -C src/ipc  
make[1]: Entering directory`/home/Myprojects/example_make/version-3.0/src/ipc'  
cc -g -Wall -Werror -O2 -I. -I./inc-I../../include  -c -o src/ipc.osrc/ipc.c  
ar rcs libipc.a src/ipc.o  
cp libipc.a ../../libs  
make[1]: Leaving directory `/home/Myprojects/example_make/version-3.0/src/ipc'  
make -C src/tools  
make[1]: Entering directory`/home/Myprojects/example_make/version-3.0/src/tools'  
cc -g -Wall -Werror -O2 -I. -I./inc-I../../include  -c -o src/base64.osrc/base64.c  
cc -g -Wall -Werror -O2 -I. -I./inc -I../../include  -c -o src/md5.o src/md5.c  
cc -g -Wall -Werror -O2 -I. -I./inc-I../../include  -c -o src/tools.osrc/tools.c  
ar rcs libtools.a src/base64.o src/md5.o src/tools.o  
cp libtools.a ../../libs  
make[1]: Leaving directory`/home/Myprojects/example_make/version-3.0/src/tools'  
make -C src/main  
make[1]: Entering directory`/home/Myprojects/example_make/version-3.0/src/main'  
cc -g -Wall -Werror -O2 -I. -I./inc-I../../include  -c -o src/main.osrc/main.c  
cc -o target_bin src/main.o -lpthread -L../../libs -ltools-lipc  
make[1]: Leaving directory`/home/Myprojects/example_make/version-3.0/src/main'  
#  
```

最後生成了可執行程序文件。這樣的話一個工程的各個模塊就變得獨立出來了，不但源碼分開了，而且各自有各自的Makefile，並且各個功能模塊是可獨立編譯的。

我們發現頂層Makefile還有可以改進的地方，就是在進入下一層目錄是要重複寫多次，如下：


```sh
>---$(MAKE) -C src/ipc  
>---$(MAKE) -C src/tools  
>---$(MAKE) -C src/main 
```

每增加一個目錄都要在多個偽目標裡面加入一行，這樣不夠自動化啊，於是我們想到shell的循環語 句，我們可以在每條規則的命令處使用for循環。如下：

```sh

DIR = src  
SUBDIRS = $(shell ls $(DIR))  
  
all :  
    @for subdir in $(SUBDIRS); \  
    do $(MAKE) -C $(DIR)/$$subdir; \
    done 
```

這樣懶人有可以高興很久了。不過還有問題：

上面for循環會依次進入系統命令ls列出的目錄，但我們對每個目錄的make順序可能有要求，在該項目當中，main目錄下的Makefile必須最後執行，因為最終的鏈接需要其他目錄編譯生成的庫文件，否則會執行失敗。並且在當前的Makefile中，當子目錄執行make出現錯誤時，make不會退出。在最終執行失敗的情況下，我們很難根據錯誤的提示定位出具體是是那個目錄下的Makefile出現錯誤。這給問題定位造成了很大的困難。為了避免這樣的問題，在命令執行錯誤後make退出。


所以將剛才的Makefile修改為如下



```sh
DIR = src  
SUBDIRS = $(shell ls $(DIR))  
   
all :  
>---@for subdir in $(SUBDIRS); \  
>---do $(MAKE) -C $(DIR)/$$subdir || exit 1; \
>---done  
```

這樣在執行出錯時立馬退出，但這樣還是沒有解決問題，編譯錯誤還是會出現。那怎麼解決呢？

我們可以通過增加規則來限制make執行順序，這樣就要用到偽目標，對每一個模塊我們都為他寫一條規則，每個模塊名稱是目標，最後需要執行的模塊目標又是其他模塊的目標，這樣就限制了make順序。在執行到最後需要執行的目標時，發現存在依賴，於是先更新依賴的目標，這樣就不會出錯了。並且這樣的話，我們還可以對指定模塊進行編譯，比如我只修改了tools模塊，我只想看看我修改的這個模塊代碼是否可以編譯通過，我可以在編譯時這樣：


```sh
# make tools  
make -C src/tools  
make[1]: Entering directory`/home/Myprojects/example_make/version-2.1/src/tools'  
cc -g -Wall -Werror -O2 -I. -I./inc-I../../include  -c -o src/base64.o src/base64.c  
cc -g -Wall -Werror -O2 -I. -I./inc-I../../include  -c -o src/md5.osrc/md5.c  
cc -g -Wall -Werror -O2 -I. -I./inc-I../../include  -c -o src/tools.osrc/tools.c  
ar rcs libtools.a src/base64.o src/md5.o src/tools.o  
cp libtools.a ../../libs  
make[1]: Leaving directory`/home/Myprojects/example_make/version-2.1/src/tools'  
```

還有另外一種方法也可以解決此問題，就是手動列出需要進入執行的模塊名稱（這裡就是目錄了），把最後需要執行的模塊放在最後，這樣for循環執行時最後需要編譯鏈接的模塊就放在最後了，不會像我們之前那樣make是按照使用系統命令ls列出模塊目錄的順序來執行。ls列出目錄是按照每個目錄的名稱來排序的，我們總不能要求寫代碼的時候最後執行的模塊的名稱必須是以z開頭的吧，總之不現實。

我們的頂層Makefile又進化了，也是這一節最終Makefile：


```sh
# Top Makefile for C program  
# Copyright (C) 2014 shallnew \at 163 \dot com  
  
DIR = src  
MODULES = $(shell ls $(DIR))  
# MODULES = ipc main tools  
  
all : $(MODULES)  
   
$(MODULES):  
>---$(MAKE) -C $(DIR)/$@  
   
main:tools ipc  
   
obj:  
>---@for subdir in $(MODULES); \  
>---do $(MAKE) -C $(DIR)/$$subdir $@; \  
>---done  
   
clean :  
>---@for subdir in $(MODULES); \  
>---do $(MAKE) -C $(DIR)/$$subdir $@; \  
>---done  
   
distclean:  
>---@for subdir in $(MODULES); \  
>---do $(MAKE) -C $(DIR)/$$subdir $@; \  
>---done  
   
   
tags:  
>---ctags -R  
  
help:  
>---@echo "===============A common Makefilefor c programs=============="  
>---@echo "Copyright (C) 2014 liuy0711 \at 163\dot com"  
>---@echo "The following targets aresupport:"  
>---@echo  
>---@echo " all              - (==make) compile and link"  
>---@echo " obj              - just compile, withoutlink"  
>---@echo " clean            - clean target"  
>---@echo " distclean        - clean target and otherinformation"  
>---@echo " tags             - create ctags for vimeditor"  
>---@echo " help             - print help information"  
>---@echo  
>---@echo "To make a target, do 'make[target]'"  
>---@echo "========================= Version2.0 ======================="  
   
.PHONY : all clean distclean tags help  
```
