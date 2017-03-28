# （五）：嵌套执行


在大一些的项目里面，所有源代码不会只放在同一个目录，一般各个功能模块的源代码都是分开的，各自放在各自目录下，并且头文件和.c源文件也会有各自的目录，这样便于项目代码的维护。这样我们可以在每个功能模块目录下都写一个Makefile，各自Makefile处理各自功能的编译链接工作，这样我们就不必把所有功能的编译链接都放在同一个Makefile里面，这可使得我们的Makefile变得更加简洁，并且编译的时候可选择编译哪一个模块，这对分块编译有很大的好处。

现在我所处于工程目录树如下：


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

这样组织项目源码要比之前合理一些，那这样怎么来写Makefile呢？我们可以在每个目录下写一个Makefile，通过最顶层的Makefile一层一层的向下嵌套执行各层Makefile。那么我们最顶层的Makefile简单点的话可以这样写：


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
就是进入src目录继续执行该目录下的Makefile。然后src目录下的Makefile在使用同样的方法进入下一级目录tools、main、ipc，再执行该目录下的Makefile。其实这样有些麻烦，我们可以直接从顶层目录进入最后的目录执行make。再加入一些伪目标完善下，我们的顶层Makefile就出来了：

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

当我们这样组织源代码时，最下面层次的Makefile怎么写呢？肯定不可以将我们上一节的Makefile（version 1.1）直接拷贝到功能模块目录下，需要稍作修改。不能所有的模块都最终生成各自的可执行文件吧，我们目前是一个工程，所以最后只会生成一个可执行程序。我们这样做，让主模块目录生成可执行文件，其他模块目录生成静态库文件，主模块链接时要用其他模块编译产生的库文件来生成最终的程序。将上一节Makefile稍作修改得出编译库文件Makefile和编译可执行文件Makefile分别如下：


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

最后在顶层执行：

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

最后生成了可执行程序文件。这样的话一个工程的各个模块就变得独立出来了，不但源码分开了，而且各自有各自的Makefile，并且各个功能模块是可独立编译的。

我们发现顶层Makefile还有可以改进的地方，就是在进入下一层目录是要重复写多次，如下：


```sh
>---$(MAKE) -C src/ipc  
>---$(MAKE) -C src/tools  
>---$(MAKE) -C src/main 
```

每增加一个目录都要在多个伪目标里面加入一行，这样不够自动化啊，于是我们想到shell的循环语 句，我们可以在每条规则的命令处使用for循环。如下：


```c
DIR = src  
SUBDIRS = $(shell ls $(DIR))  
  
all :  
    @for subdir in $(SUBDIRS); \  
    do $(MAKE) -C $(DIR)/$$subdir; \
    done 
```

这样懒人有可以高兴很久了。不过还有问题：

上面for循环会依次进入系统命令ls列出的目录，但我们对每个目录的make顺序可能有要求，在该项目当中，main目录下的Makefile必须最后执行，因为最终的链接需要其他目录编译生成的库文件，否则会执行失败。并且在当前的Makefile中，当子目录执行make出现错误时，make不会退出。在最终执行失败的情况下，我们很难根据错误的提示定位出具体是是那个目录下的Makefile出现错误。这给问题定位造成了很大的困难。为了避免这样的问题，在命令执行错误后make退出。


所以将刚才的Makefile修改为如下



```sh
DIR = src  
SUBDIRS = $(shell ls $(DIR))  
   
all :  
>---@for subdir in $(SUBDIRS); \  
>---do $(MAKE) -C $(DIR)/$$subdir || exit 1; \
>---done  
```

这样在执行出错时立马退出，但这样还是没有解决问题，编译错误还是会出现。那怎么解决呢？

我们可以通过增加规则来限制make执行顺序，这样就要用到伪目标，对每一个模块我们都为他写一条规则，每个模块名称是目标，最后需要执行的模块目标又是其他模块的目标，这样就限制了make顺序。在执行到最后需要执行的目标时，发现存在依赖，于是先更新依赖的目标，这样就不会出错了。并且这样的话，我们还可以对指定模块进行编译，比如我只修改了tools模块，我只想看看我修改的这个模块代码是否可以编译通过，我可以在编译时这样：


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

还有另外一种方法也可以解决此问题，就是手动列出需要进入执行的模块名称（这里就是目录了），把最后需要执行的模块放在最后，这样for循环执行时最后需要编译链接的模块就放在最后了，不会像我们之前那样make是按照使用系统命令ls列出模块目录的顺序来执行。ls列出目录是按照每个目录的名称来排序的，我们总不能要求写代码的时候最后执行的模块的名称必须是以z开头的吧，总之不现实。

我们的顶层Makefile又进化了，也是这一节最终Makefile：


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
