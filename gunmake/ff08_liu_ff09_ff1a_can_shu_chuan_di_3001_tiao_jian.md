# （六）：參數傳遞、條件判斷、include


在多個Makefile嵌套調用時，有時我們需要傳遞一些參數給下一層Makefile。比如我們在頂層Makefile裡面定義的打開調試信息變量DEBUG_SYMBOLS，我們希望在進入子目錄執行子Makefile時該變量仍然有效，這是需要將該變量傳遞給子Makefile，那怎麼傳遞呢？這裡有兩種方法：

`1.`     在上層Makefile中使用”export”關鍵字對需要傳遞的變量進行聲明。比如：


```sh
DEBUG_SYMBOLS = TRUE  
export DEBUG_SYMBOLS 
```


當不希望將一個變量傳遞給子 make 時，可以使用指示符 “unexport”來聲明這個變量。
export一般用法是在定義變量的同時對它進行聲明。如下：

```sh
export DEBUG_SYMBOLS = TRUE  
```

`2.`     在命令行上指定變量。比如：
   

```sh
$(MAKE) -C xxx DEBUG_SYMBOLS = TRUE  
```

這樣在進入子目錄xxx執行make時該變量也有效。
 
像編程語言一樣，Makefile也有自己的條件語句。條件語句可以根據一個變量值來控制make的執行邏輯。比較常用的條件語句是ifeq –else-endif、ifneq-else-endif、ifdef-else-endif。

ifeq關鍵字用來判斷參數是否相等。

比如判斷是否生成調試信息可以這麼用：


```sh
ifeq ($(DEBUG_SYMBOLS), TRUE)  
>---CFLAGS += -g -Wall -Werror -O0  
else  
>---CFLAGS += -Wall -Werror -O2  
endif  
```


Ifneq和ifeq作用相反，此關鍵字是用來判斷參數是否不相等。

ifdef關鍵字用來判斷一個變量是否已經定義。

後兩個關鍵字用法和ifeq類似。
 
現在我們繼續改進我們上一節的Makefile，上一節的Makefile完成Makefile的嵌套調用，每一個模塊都有自己的Makefile。其實每個模塊的Makefile都大同小異，只需要改改最後編譯成生成的目標名稱或者編譯鏈接選項，規則都差不多，那麼我們是否可以考慮將規則部分提取出來，每個模塊只需修改各自變量即可。這樣是可行的，我們將規則單獨提取出來，寫一個Makefile.rule，將他放在頂層Makefile同目錄下，其他模塊內部的Makefile只需要include該Makefile就可以了。如下：

```sh
include $(SRC_BASE)/Makefile.rule  
```

include類似於C語言的頭文件包含，你把它理解為為本替換就什麼都明白了。

這樣以後規則有修改的話我們直接修改該Makefile就可以了，就不用進入每一個模塊去修改，這樣也便於維護。

這樣我們今天頂層Makefile稍作修改：


```sh
# Top Makefile for C program                                                                                                                                                               
# Copyright (C) 2014 shallnew \at 163 \dot com  
   
export DEBUG_SYMBOLS = TRUE  
   
DIR = src  
MODULES = $(shell ls $(DIR))  
# MODULES = ipc main tools  
   
all : $(MODULES)  
   
$(MODULES):  
>---$(MAKE) -C $(DIR)/$@  
   
main:tools ipc  
   
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
>---@echo " clean            - clean target"  
>---@echo " distclean        - clean target and otherinformation"  
>---@echo " tags             - create ctags for vimeditor"  
>---@echo " help             - print help information"  
>---@echo  
>---@echo "To make a target, do 'make[target]'"  
>---@echo "========================= Version2.2 ======================="  
   
.PHONY : all clean distclean tags help  
```

目前我們頂層目錄下的目錄樹為：


```sh
.  
├── include  
│   ├── common.h  
│   ├── ipc  
│   │   └── ipc.h  
│   └── tools  
│       ├── base64.h  
│       ├── md5.h  
│       └── tools.h  
├── libs  
├── Makefile  
├── Makefile.rule  
└── src  
    ├── ipc  
    │  ├──inc  
    │  ├──Makefile  
    │  └──src  
    │       └── ipc.c  
    ├── main  
    │  ├──inc  
    │  ├──Makefile  
    │  └──src  
    │       ├── main.c  
    │       └── main.c~  
    └── tools  
        ├── inc  
        ├── Makefile  
        └── src  
            ├── base64.c  
            ├── md5.c  
            └── tools.c  
   
14 directories, 16 files  
```

每個子模塊下的Makefile刪除規則後修改為如下：
 

```sh
SRC_BASE = ../..  
   
CFLAGS +=  
CPPFLAGS += -I. -I./inc -I$(SRC_BASE)/include  
   
# SRC_OBJ = $(patsubst %.c, %.o, $(wildcard *.c))  
SRC_FILES = $(wildcard src/*.c)  
SRC_OBJ = $(SRC_FILES:.c=.o)  
SRC_LIB = libtools.a  
   
include $(SRC_BASE)/Makefile.rule  
```

而處於頂層目錄下的Makefile.rule專門處理各模塊編譯鏈接時需要的規則。內容如下：

```sh
# Copyright (C) 2014 shallnew \at 163 \dot com                                                                                                                                             
   
ifeq ($(DEBUG_SYMBOLS), TRUE)  
>---CFLAGS += -g -Wall -Werror -O0  
else  
>---CFLAGS += -Wall -Werror -O2  
endif  
   
all : $(SRC_BIN) $(SRC_LIB)  
   
ifneq ($(SRC_BIN),)  
$(SRC_BIN) : $(SRC_OBJ)  
>---$(CC) -o $@ $^ $(LDFLAGS)  
endif  
   
ifneq ($(SRC_LIB),)  
$(SRC_LIB) : $(SRC_OBJ)  
>---$(AR) rcs $@ $^  
>---cp $@ $(SRC_BASE)/libs  
endif  
          
# clean target  
clean:  
>---$(RM) $(SRC_OBJ) $(SRC_LIB) $(SRC_BIN)$(SRC_BIN).exe  
   
distclean:  
>---$(RM) $(SRC_OBJ) $(SRC_LIB) $(SRC_BIN)$(SRC_BIN).exe $(SRC_BASE)/libs/* $(SRC_BASE)/tags *~  
   
.PHONY : all clean disclean  
```

我們將Makefile.rule放在頂層有可能會一不小心在命令行上面執行了該Makefile，如下：

```sh
# make -f Makefile.rule  
make: Nothing tobe done for `all'.  
```

由於我們沒有定義變量$(SRC_BIN)和$(SRC_LIB)，偽目標all沒有任何依賴，所以編譯是無法成功的。這裡我們我們應該禁止直接執行該Makefile。

在make裡面有這樣一個變量：MAKELEVEL，它在多級調用的 make 執行過程中。變量代表了調用的深度。在 make 一級級的執行過程中變量MAKELEVEL的值不斷的發生變化，通過它的值我們可以瞭解當前make 遞歸調用的深度。頂層的MAKELEVEL的值為“0” 、下一級時為“1” 、再下一級為“2”.......，所以我們希望一個子目錄的Makefile必須被上層 make 調用才可以執行，而不允許直接執行，我們可以判斷變量MAKELEVEL來控制。所以我們這一節最終的Makefile.rule為：


```sh
# Copyright (C)2014 shallnew \at 163 \dot com  
   
ifeq ($(DEBUG_SYMBOLS),TRUE)  
>---CFLAGS +=-g -Wall -Werror -O0  
else  
>---CFLAGS +=-Wall -Werror -O2  
endif  
   
ifeq($(MAKELEVEL), 0)                                                                                                                                                                     
all : msg  
else  
all : $(SRC_BIN)$(SRC_LIB)  
endif  
   
ifneq ($(SRC_BIN),)  
$(SRC_BIN) :$(SRC_OBJ)  
>---$(CC) -o $@$^ $(LDFLAGS)  
endif  
   
ifneq($(SRC_LIB),)  
$(SRC_LIB) :$(SRC_OBJ)  
>---$(AR) rcs$@ $^  
>---cp $@$(SRC_BASE)/libs  
endif  
   
msg:  
>---@echo"You cannot directily execute this Makefile! This Makefile should calledby toplevel Makefile."  
   
# clean target  
clean:  
>---$(RM)$(SRC_OBJ) $(SRC_LIB) $(SRC_BIN) $(SRC_BIN).exe  
   
distclean:  
>---$(RM)$(SRC_OBJ) $(SRC_LIB) $(SRC_BIN) $(SRC_BIN).exe $(SRC_BASE)/libs/*$(SRC_BASE)/tags *~  
   
.PHONY : all cleandisclean   
```

此時再直接執行該Makefile：

```sh
# make -f Makefile.rule  
You cannot directily execute this Makefile! This Makefile should called by toplevel Makefile. ```


