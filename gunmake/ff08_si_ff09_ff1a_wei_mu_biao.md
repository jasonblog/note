# （四）：伪目标


一般情况下，Makefile都会有一个clean目标，用于清除编译过程中产生的二进制文件。我们在第一节的Makefile就用到了这个 clean目标，该目标没有任何依赖文件，并且该目标对应的命令执行后不会生产clean文件。

像这种特点目标，它的规则所定义的命令不是去创建文件，而仅仅通过make指定目标来执行一些特定系统命令或其依赖为目标的规则（如all），称为伪目标。

一个Makefile一般都不会只有一个伪目标，如果按Makefile的“潜规则”以及其约定俗成的名字来说的话，在较大的项目的Makefile中比较常用的为目标有这些：

```sh
all：执行主要的编译工作，通常用作缺省目标，放在最前面。
Install：执行编译后的安装工作，把可执行文件、配置文件、文档等分别拷到不同的安装目录。
clean：删除编译生成的二进制文件。
distclean：删除除源文件之外的所有中间生成文件，如配置文件，文档等。
tags：为vim等编辑器生成tags文件。
help：打印当前Makefile的帮助信息，比如有哪些目标可以有make指定去执行。
```

make处理Makefile时，首先读取所有规则，建立关系依赖图。然后从缺省目标（第一个目标）或指定的目标开始执行。像clean，tags这样的目标一般不会作为缺省目标，也不会跟缺省目标有任何依赖关系，所以 make 无法生成它的依赖关系和决定它是否要执行。所以要执行这样的目标时，必须要显示的指定make该目标。就像前面我们清楚便已产生的中间二进制文件一样，需要显示执行命令：make clean。

伪目标也可以作为默认目标（如all），并且可以为其指定依赖文件。
我们先将version 1.0的Makefile完善下，我们可以加入帮助信息，tags等功能。


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

make会把执行的命令打印在屏幕上，如果我们不想把命令打印在屏幕上，只显示命令结果时，直接在命令前面加上符号“@”就可以实现。如上面help目标一样，只显示命令结果。一般我们会在make时都会输出“Compiling xxx.c…”,不输出编译时的命令。我们在后面写Makefile时可以模仿。

如果当前目录下存在一个和伪目标同名的文件时（如clean），此时如果执行命令make clean后出现如下结果：


```sh
# touch clean  
# make clean  
make: `clean' is up to date.  
```

这是因为clean文件没有依赖文件，make认为目标clean是最新的不会去执行规则对应的命令。为了解决这个问题，我们可以明确地将该目标声明为伪目标。将一个目标声明为伪目标需要将它作为特殊目标.PHONY”的依赖。如下：


```sh
.PHONY : clean  
```

这条规则写在clean:规则的后面也行，也能起到声明clean是伪目标的作用
这样修改一下之前Makefile，将所有伪目标都作为.PHONY的依赖：


```sh
.PHONY : all obj tag help clean disclean 
```

这样在当前目录下存在文件clean时执行:

```sh
# make clean  
rm -f debug.o ipc.o main.o timer.o tools.o target_bin target_bin.exe  
```

发现问题解决。
最后，给出今天最终的Makefile：

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
