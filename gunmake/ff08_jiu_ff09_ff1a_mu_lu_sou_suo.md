# （九）：目錄搜索


在一個較大的工程中，一般會將源代碼和二進制文件（.o 文件和可執行文件）安排在不同的目錄來進行區分管理。這種情況下，我們可以使用 make 提供的目錄搜索依賴文件功能（在指定的若干個目錄下自動搜索依賴文件）。在Makefile中，使用依賴文件的目錄搜索功能。當工程的目錄結構發生變化後，就可以做到不更改 Makefile的規則，只更改依賴文件的搜索目錄。

在我們上一節出現的問題當中，我們將.c文件統一放在src目錄下，沒有和Makefile目錄在同一目錄下，因此沒有辦法尋找到.o文件的依賴文件。make程序有一個特殊的變量VPATH，該變量可以指定依賴文件的搜索路徑，當規則的依賴文件在當前目錄不存在時，make 會在此變量所指定的目錄下去尋找這些依賴文件。通常我們都是用此變量來指定規則的依賴文件的搜索路徑。

定義變量 “VPATH”時，使用空格或者冒號（:）將多個需要搜索的目錄分開。make搜索目錄的順序是按照變量“VPATH”定義中的目錄順序進行的，當前目錄永遠是第一搜索目錄。例如如下定義

```sh
VPATH += ./src  
```

指定了依賴搜索目錄為當前目錄下的src目錄，我們可以在Makefile.rules裡面添加給VPATH變量賦值，而在包含該Makefile.rules之前給出當前模塊.c文件所在目錄。

其實我們也可以直接指定依賴文件的路徑，這樣也是可以的，如下：


```sh
$(SRC_OBJ) : $(OBJDIR)/%.o : $(MOD_SRC_DIR)/%.c
>---$(CC) -c $(CFLAGS) $(CPPFLAGS) $< -o $@  
```


但是這樣在我們更改了工程目錄結構之後，對應的依賴文件沒有在同一目錄下，又變得麻煩了，所以還不如直接給VPATH變量賦值，我們只需要指定源碼所在的目錄即可。

其實我們還有另外一種搜索文件路徑方法：使用vpath關鍵字（注意不是VPATH變量）， 它和VPATH類似，但是它可以為不同類型的文件（由文件名區分）指定不同的搜索目錄。使用方法有三種：

1、vpath PATTERN DIRECTORIES 
為所有符合模式“PATTERN”的文件指定搜索目錄“DIRECTORIES” 。多個目錄使用空格或者冒號（：）分開。

2、vpath PATTERN 
清除之前為符合模式“PATTERN”的文件設置的搜索路徑。 

3、vpath 
清除所有已被設置的文件搜索路徑。
vapth 使用方法中的“PATTERN”需要包含模式字符“%”；例如上面的定義：


```sh
VPATH += ./src  
```

可以寫為：

```sh
vpath %.c ./src 
```

現在給一個我們的Makefile.rules：

```sh
# Copyright (C) 2014 shallnew \at 163 \dot com                                                                                                                             
  
# if without a platform defined, give value "unknow" to PLATFORM  
ifndef PLATFORM  
>---PLATFORM = unknow  
endif  
  
# define a root build directory base on the platform  
# if without a SRC_BASE defined, just use local src directory  
ifeq ($(SRC_BASE),)  
>---BUILDDIR = $(MOD_SRC_DIR)  
>---OBJDIR = $(MOD_SRC_DIR)  
>---LIBDIR = $(MOD_SRC_DIR)  
>---BINDIR = $(MOD_SRC_DIR)  
else  
>---ifeq ($(DEBUG_SYMBOLS), TRUE)  
>--->---BUILDDIR = $(SRC_BASE)/build/$(PLATFORM)_dbg  
>---else  
>--->---BUILDDIR = $(SRC_BASE)/build/$(PLATFORM)  
>---endif  
>---OBJDIR = $(BUILDDIR)/obj/$(MODULE)  
>---LIBDIR = $(BUILDDIR)/lib  
>---BINDIR = $(BUILDDIR)/bin  
endif  
  
# update compilation flags base on "DEBUG_SYMBOLS"  
ifeq ($(DEBUG_SYMBOLS), TRUE)  
>---CFLAGS += -g -Wall -Werror -O0  
else  
>---CFLAGS += -Wall -Werror -O2  
endif  
  
VPATH += $(MOD_SRC_DIR)  
  
SRC_OBJ = $(patsubst %.c, $(OBJDIR)/%.o, $(notdir $(SRC_FILES)))  
  
ifeq ($(MAKELEVEL), 0)  
all : msg  
else  
all : lib bin  
endif  
  
lib : $(OBJDIR) $(LIBDIR)/$(SRC_LIB)  
  
bin : $(OBJDIR) $(BINDIR)/$(SRC_BIN)  
  
$(OBJDIR) :  
>---mkdir -p $@  
  
ifneq ($(SRC_BIN),)  
$(BINDIR)/$(SRC_BIN) : $(SRC_OBJ)  
>---$(CC) -o $@ $^ $(LDFLAGS)  
endif  
  
ifneq ($(SRC_LIB),)  
$(LIBDIR)/$(SRC_LIB) : $(SRC_OBJ)  
>---$(AR) rcs $@ $^  
>---cp $@ $(SRC_BASE)/libs  
endif  
  
$(SRC_OBJ) : $(OBJDIR)/%.o : %.c  
>---$(CC) -c $(CFLAGS) $(CPPFLAGS) $< -o $@  
  
msg:  
>---@echo "You cannot directily execute this Makefile! This Makefile should called by toplevel Makefile."  
  
  
# clean target  
clean:  
ifneq ($(SRC_LIB),)  
>--->---$(RM) $(SRC_OBJ) $(LIBDIR)/$(SRC_LIB)  
endif  
ifneq ($(SRC_BIN),)  
>--->---$(RM) $(SRC_OBJ) $(BINDIR)/$(SRC_BIN)  
endif  
  
.PHONY : all clean  
```

