# （三）：变量的使用


仔细研究我们的之前Makefile发现，我们还有改进的地方，就是此处：


```sh
target_bin : main.o debug.o ipc.o timer.o tools.o  
>---gcc -o target_bin main.o debug.o ipc.o timer.o tools.o  
```

如果增加一个源文件xx.c的话，需要在两处或多处增加xx.o文件。我们可以使用变量来解决这个问题。之前说过，Makefile的变量就像C语言的宏一样，使用时在其位置上直接展开。变量在声明时赋予初值，在引用变量时需要给在变量名前加上“$”符号，但最好用小括号“（）”或是大括号“{}”把变量给包括起来。

默认目标target_bin也在多处出现了，该文件也可以使用变量代替。
修改我们的Makefile如下：

```sh
SRC_OBJ = main.o debug.o ipc.o timer.o tools.o  
SRC_BIN = target_bin  
$(SRC_BIN) : $(SRC_OBJ)  
>---gcc -o $(SRC_BIN) $(SRC_OBJ)  
  
clean:  
>---rm $(SRC_OBJ) $(SRC_BIN)  
```

这样每次有新增的文件是只需要在SRC_OBJ变量里面增加一个文件即可。要修改最终目标的名字是可以只修改变量SRC_BIN。
其实在之前还说过特殊变量：

```sh
$@，表示规则中的目标。
$<，表示规则中的第一个依赖文件。
$?，表示规则中所有比目标新的条件，组成一个列表，以空格分隔。
$^，表示规则中的所有条件，组成一个列表，以空格分隔。
```

上一节我们看到make -p有很多自定义的变量，比如CC。其中很多变量我们可以直接使用或修改其变量值或增加值。我们的Makefile中可以使用CC（默认值为cc）、RM（默认值为rm -f）。
 
由此可见我们的Makefile还可以进一步修改：

```sh
SRC_OBJ = main.o debug.o ipc.o timer.o tools.o  
SRC_BIN = target_bin  
$(SRC_BIN) : $(SRC_OBJ)  
>---$(CC) -o $@ $^

clean:  
>---$(RM) $(SRC_OBJ) $(SRC_BIN)  
```

这样的Makefile编译也是可用的。

但是这样的Makefile还是需要我们手动添加文件，还是不够自动化，最好增删文件都要修改Makefile。伟大的人类真是太懒了！！于是乎，他们发明了一个函数wilcard（函数后面会讲到），它可以用来获取指定目录下的所有的.c文件列表。这样的话我们可以自动获取当前目录下所有.c源文件，然后通过其他方法再得到.o文件列表，这样的话就不需要在每次增删文件时去修改Makefile了。所谓其他方法这里给出两种：

1.     使用patsubst函数。在$(patsubst %.c,%.o,$(dir) )中，patsubst把$(dir)中的变量符合后缀是.c的全部替换成.o。

2.     变量值的替换。 我们可以替换变量中的共有的部分，其格式是“$(var:a=b)”或“${var:a=b}”，其意思是，把变量“var”中所有以“a”字串“结尾”的“a”替换成“b”字串。


修改后的Makefile如下：

```sh
# SRC_OBJ = $(patsubst %.c, %.o, $(wildcard *.c))
SRC = $(wildcard *.c)  
SRC_OBJ = $(SRC:.c=.o)  
SRC_BIN = target_bin  
   
$(SRC_BIN) : $(SRC_OBJ)  
>---$(CC) -o $@ $^  
   
clean:  
>---$(RM) $(SRC_OBJ) $(SRC_BIN)  
```

其中# 后面的内容为注释。
这样终于满足了那些懒人的想法了。可见在使用变量时，的确可以是编译变得更自动化。
 
其实变量的定义有三种运算符=、:=、?=、+=。

`1.`     =运算符可以读取到后面定义的变量。比如：

```sh
VAR = $(VAR2)  
VAR2 = hello_make  
   
all:  
>---@echo =====$(VAR)=====  
```

运行结果为：

```sh
#  
=====hello_make=====  
#  
```

但是这种定义可能会导致并非我们意愿的事发生，并不是很符合C语言的编程习惯。

`2.`     :=运算符在遇到变量定义时立即展开。

```sh
VAR := $(VAR2)                                                                                                                                                                                          
VAR2 = hello_make  
   
all:  
>---@echo =====$(VAR)=====  
```

运行结果为：

```sh
#  
==========  
# 
```

`3.`     ?=运算符在复制之前先做判断变量是否已经存在。例如var1 ?= $(var2)的意思是：如果var1没有定义过，那么?=相当于=，如果var1先前已经定义了，则什么也不做，不会给var重新赋值。


`4.`     +=运算符是给变了追加值。如果变量还没有定义过就直接用+=赋值，那么+=相当于=
 
如何使用这几个运算符要看实际情况，有时一个大的工程可能有许多Makefile组成，变量可能在多个Makefile中都在使用，这时可能使用+=比较好。使用:=有时可能比要好。

有时在编译程序时，我们需要编译器给出警告，或加入调试信息，或告知编译器优化可执行文件。编译时C编译器的选项CFLAGS使用的较多，默认没有提供值，我们可以给该变量赋值。有时我们还需要使用链接器选项LFLAGS告诉链接器链接时需要的库文件。可能我们还需要给出包含头文件的路径，因为头文件很可能和源文件不再同一目录。所以，我们今天的Makefile加上部分注释又更新了：


```sh
# A commonMakefile for c programs, version 1.0  
# Copyright (C)2014 shallnew \at 163 \dot com  
   
CFLAGS += -g -Wall-Werror -O2  
CPPFLAGS += -I.-I./inc                                                                                                                                                                    
LDFLAGS +=-lpthread  
   
# SRC_OBJ =$(patsubst %.c, %.o, $(wildcard *.c))  
SRC_FILES =$(wildcard *.c)  
SRC_OBJ =$(SRC_FILES:.c=.o)  
SRC_BIN =target_bin  
   
$(SRC_BIN) :$(SRC_OBJ)  
>---$(CC) -o $@$^ $(LDFLAGS)  
   
clean:  
>---$(RM)$(SRC_OBJ) $(SRC_BIN)  
```

编译：


```sh
# make  
cc -g -Wall-Werror -O2 -I. -I./inc  -c -o debug.odebug.c  
cc -g -Wall-Werror -O2 -I. -I./inc  -c -o ipc.oipc.c  
cc -g -Wall-Werror -O2 -I. -I./inc  -c -o main.omain.c  
cc -g -Wall-Werror -O2 -I. -I./inc  -c -o timer.otimer.c  
cc -g -Wall-Werror -O2 -I. -I./inc  -c -o tools.otools.c  
cc -o target_bindebug.o ipc.o main.o timer.o tools.o -lpthread  
```

可见我们的预编译选项，编译选项都用到了，之前我们说过make的使用隐含规则自动推导：

```sh
COMPILE.c = $(CC) $(CFLAGS) $(CPPFLAGS) $(TARGET_ARCH) –c  
```

其中变量CFLAGS 和 CPPFLAGS均是我们给出的，变量$(TARGET_ARCH)未给，所以在编译输出可以看到-c前面有2个空，最早未给变量是有四个空。

目前给出的Makefile基本上可以适用于那些源代码全部在同一目录下的简单项目，并且基本上在增删文件时不需要再去手动修改Makefile代码。在新的一个项目只需要把该Makefile拷贝到源代码目录下，再修改一下你需要编译的可执行文件名称以及你需要的编译连接选项即可。

后面章节将会讲到如何写多目录源代码工程下的Makefile。

最后，今天的最终Makefile是这样的：


```sh
# A commonMakefile for c programs, version 1.0  
# Copyright (C)2014 shallnew \at 163 \dot com  
   
CFLAGS += -g -Wall-Werror -O2  
CPPFLAGS += -I.-I./inc                                                                                                                                                                    
LDFLAGS +=-lpthread  
   
# SRC_OBJ =$(patsubst %.c, %.o, $(wildcard *.c))  
SRC_FILES =$(wildcard *.c)  
SRC_OBJ =$(SRC_FILES:.c=.o)  
SRC_BIN =target_bin  
   
$(SRC_BIN) :$(SRC_OBJ)  
>---$(CC) -o $@$^ $(LDFLAGS)  
   
clean:  
>---$(RM)$(SRC_OBJ) $(SRC_BIN)  
```