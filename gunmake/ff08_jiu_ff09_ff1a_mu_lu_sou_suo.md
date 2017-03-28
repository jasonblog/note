# （九）：目录搜索


在一个较大的工程中，一般会将源代码和二进制文件（.o 文件和可执行文件）安排在不同的目录来进行区分管理。这种情况下，我们可以使用 make 提供的目录搜索依赖文件功能（在指定的若干个目录下自动搜索依赖文件）。在Makefile中，使用依赖文件的目录搜索功能。当工程的目录结构发生变化后，就可以做到不更改 Makefile的规则，只更改依赖文件的搜索目录。

在我们上一节出现的问题当中，我们将.c文件统一放在src目录下，没有和Makefile目录在同一目录下，因此没有办法寻找到.o文件的依赖文件。make程序有一个特殊的变量VPATH，该变量可以指定依赖文件的搜索路径，当规则的依赖文件在当前目录不存在时，make 会在此变量所指定的目录下去寻找这些依赖文件。通常我们都是用此变量来指定规则的依赖文件的搜索路径。

定义变量 “VPATH”时，使用空格或者冒号（:）将多个需要搜索的目录分开。make搜索目录的顺序是按照变量“VPATH”定义中的目录顺序进行的，当前目录永远是第一搜索目录。例如如下定义

```sh
VPATH += ./src  
```

指定了依赖搜索目录为当前目录下的src目录，我们可以在Makefile.rules里面添加给VPATH变量赋值，而在包含该Makefile.rules之前给出当前模块.c文件所在目录。

其实我们也可以直接指定依赖文件的路径，这样也是可以的，如下：


```sh
$(SRC_OBJ) : $(OBJDIR)/%.o : $(MOD_SRC_DIR)/%.c
>---$(CC) -c $(CFLAGS) $(CPPFLAGS) $< -o $@  
```


但是这样在我们更改了工程目录结构之后，对应的依赖文件没有在同一目录下，又变得麻烦了，所以还不如直接给VPATH变量赋值，我们只需要指定源码所在的目录即可。

其实我们还有另外一种搜索文件路径方法：使用vpath关键字（注意不是VPATH变量）， 它和VPATH类似，但是它可以为不同类型的文件（由文件名区分）指定不同的搜索目录。使用方法有三种：

1、vpath PATTERN DIRECTORIES 
为所有符合模式“PATTERN”的文件指定搜索目录“DIRECTORIES” 。多个目录使用空格或者冒号（：）分开。

2、vpath PATTERN 
清除之前为符合模式“PATTERN”的文件设置的搜索路径。 

3、vpath 
清除所有已被设置的文件搜索路径。
vapth 使用方法中的“PATTERN”需要包含模式字符“%”；例如上面的定义：


```sh
VPATH += ./src  
```

可以写为：

```sh
vpath %.c ./src 
```

现在给一个我们的Makefile.rules：

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

