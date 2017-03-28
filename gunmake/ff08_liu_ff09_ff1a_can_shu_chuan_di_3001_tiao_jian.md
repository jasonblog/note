# （六）：参数传递、条件判断、include


在多个Makefile嵌套调用时，有时我们需要传递一些参数给下一层Makefile。比如我们在顶层Makefile里面定义的打开调试信息变量DEBUG_SYMBOLS，我们希望在进入子目录执行子Makefile时该变量仍然有效，这是需要将该变量传递给子Makefile，那怎么传递呢？这里有两种方法：

`1.`     在上层Makefile中使用”export”关键字对需要传递的变量进行声明。比如：


```sh
DEBUG_SYMBOLS = TRUE  
export DEBUG_SYMBOLS 
```


当不希望将一个变量传递给子 make 时，可以使用指示符 “unexport”来声明这个变量。
export一般用法是在定义变量的同时对它进行声明。如下：

```sh
export DEBUG_SYMBOLS = TRUE  
```

`2.`     在命令行上指定变量。比如：
   

```sh
$(MAKE) -C xxx DEBUG_SYMBOLS = TRUE  
```

这样在进入子目录xxx执行make时该变量也有效。
 
像编程语言一样，Makefile也有自己的条件语句。条件语句可以根据一个变量值来控制make的执行逻辑。比较常用的条件语句是ifeq –else-endif、ifneq-else-endif、ifdef-else-endif。

ifeq关键字用来判断参数是否相等。

比如判断是否生成调试信息可以这么用：


```sh
ifeq ($(DEBUG_SYMBOLS), TRUE)  
>---CFLAGS += -g -Wall -Werror -O0  
else  
>---CFLAGS += -Wall -Werror -O2  
endif  
```


Ifneq和ifeq作用相反，此关键字是用来判断参数是否不相等。

ifdef关键字用来判断一个变量是否已经定义。

后两个关键字用法和ifeq类似。
 
现在我们继续改进我们上一节的Makefile，上一节的Makefile完成Makefile的嵌套调用，每一个模块都有自己的Makefile。其实每个模块的Makefile都大同小异，只需要改改最后编译成生成的目标名称或者编译链接选项，规则都差不多，那么我们是否可以考虑将规则部分提取出来，每个模块只需修改各自变量即可。这样是可行的，我们将规则单独提取出来，写一个Makefile.rule，将他放在顶层Makefile同目录下，其他模块内部的Makefile只需要include该Makefile就可以了。如下：

```sh
include $(SRC_BASE)/Makefile.rule  
```

include类似于C语言的头文件包含，你把它理解为为本替换就什么都明白了。

这样以后规则有修改的话我们直接修改该Makefile就可以了，就不用进入每一个模块去修改，这样也便于维护。

这样我们今天顶层Makefile稍作修改：


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

目前我们顶层目录下的目录树为：


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

每个子模块下的Makefile删除规则后修改为如下：
 

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

而处于顶层目录下的Makefile.rule专门处理各模块编译链接时需要的规则。内容如下：

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

我们将Makefile.rule放在顶层有可能会一不小心在命令行上面执行了该Makefile，如下：

```sh
# make -f Makefile.rule  
make: Nothing tobe done for `all'.  
```

由于我们没有定义变量$(SRC_BIN)和$(SRC_LIB)，伪目标all没有任何依赖，所以编译是无法成功的。这里我们我们应该禁止直接执行该Makefile。

在make里面有这样一个变量：MAKELEVEL，它在多级调用的 make 执行过程中。变量代表了调用的深度。在 make 一级级的执行过程中变量MAKELEVEL的值不断的发生变化，通过它的值我们可以了解当前make 递归调用的深度。顶层的MAKELEVEL的值为“0” 、下一级时为“1” 、再下一级为“2”.......，所以我们希望一个子目录的Makefile必须被上层 make 调用才可以执行，而不允许直接执行，我们可以判断变量MAKELEVEL来控制。所以我们这一节最终的Makefile.rule为：


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

此时再直接执行该Makefile：

```sh
# make -f Makefile.rule  
You cannot directily execute this Makefile! This Makefile should called by toplevel Makefile. ```


