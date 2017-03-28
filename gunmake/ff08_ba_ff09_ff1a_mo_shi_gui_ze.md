# （八）：模式规则


上一节讲到目录创建成功，目标文件没有生产到对应目录下，这里我们先给目标文件加上对应目录，这样的话产生对应的目标文件会直接生成到对应目录。我们先给库文件目标和可执行文件目标加上路径，如下：

```sh
lib : $(OBJDIR) $(LIBDIR)/$(SRC_LIB)  
  
bin : $(OBJDIR) $(BINDIR)/$(SRC_BIN)  
  
$(OBJDIR) :  
>---@echo "   MKDIR $(notdir $@)..."  
>---@mkdir -p $@  
  
ifneq ($(SRC_BIN),)  
$(BINDIR)/$(SRC_BIN) : $(SRC_OBJ)  
>---$(CC) -o $@ $^ $(LDFLAGS)  
endif  
  
ifneq ($(SRC_LIB),)  
$(LIBDIR)/$(SRC_LIB) : $(SRC_OBJ)  
>---$(AR) rcs $@ $^  
>---cp $@ $(SRC_BASE)/libs  
endif  
```

此时再执行make，完成后查看build目录树：


```sh
build/  
└── unix_dbg  
    ├── bin  
    │   └── target_bin  
    ├── lib  
    │   ├── libipc.a  
    │   └── libtools.a  
    └── obj  
        ├── ipc  
        ├── main  
        └── tools  
```

可以看到，生成的目标是在对应目录下。我们乘胜追击，把.o文件也将其修改了。我们之前的每个模块Makefile大致是这样写的：


```sh
SRC_BASE = ../..
CFLAGS +=
CPPFLAGS += -I. -I./inc -I$(SRC_BASE)/include
# SRC_OBJ = $(patsubst %.c, %.o, $(wildcard *.c))
SRC_FILES = $(wildcard src/*.c)
SRC_OBJ = $(SRC_FILES:.c=.o)
SRC_LIB = xx.a
include $(SRC_BASE)/Makefile.rule  
```

其中SRC_OBJ在此处给出，然后再在Makefile.rule中使用，此处的.o文件会在.c文件相同目录下生成，所以我们现在需要将.o文件加上路径，由于取得路径是在Makefile.rule里面，所以我们可以统一在Makefile.rule里面给变量SRC_OBJ赋值，大致如下：

SRC_OBJ = $(patsubst %.c, $(OBJDIR)/%.o, $(notdir $(SRC_FILES)))                                                                                                                          
这里用到函数patsubst、notdir，关于函数会在后面讲到。这样.o文件作为目标生成之后就会生成到相应目录里面了。
此时再编译：


```sh
# make  
make[1]: Entering directory `/home/Myprojects/example_make/version-2.9/src/ipc'  
make[1]: *** No rule to make target `../../build/unix_dbg/obj/ipc/ipc.o', needed by `../../build/unix_dbg/lib/libipc.a'.  Stop.  
make[1]: Leaving directory `/home/Myprojects/example_make/version-2.9/src/ipc'  
make: *** [ipc] Error 2  
```

发现出错了，并且是在生成目标文件ipc.o时没有成功，查看build目录树也没有生成.o文件。为什么会生成失败呢？

我们没有给出生成.o目标的规则，之前可以生成是因为make有通过隐含规则来自动推导的能力（这个之前有讲到，链接过去）。在我们没有修改之前，生成.o通过隐含规则来完成：


```sh
%.o: %.c  
#  commands to execute (built-in):  
>---$(COMPILE.c) $(OUTPUT_OPTION) $<  
```

因为所有的.o目标符合该规则，所以会自动推导生成.o文件。我们现在在..o前面加上路径后没有符合生成.o的隐含模式规则了，所以就没有生成该文件，导致编译出错。那怎么办呢？没有隐含模式规则，我们可以自己写符合生成该目标的模式规则。

模式规则类似于普通规则，只是在模式规则中，目标文件是一个带有模式字符“%”的文件，使用模式来匹配目标文件。在目标文件名中“%”匹配的部分称为“茎”。使用模式规则时，目标文件匹配之后得到“茎”，依赖根据“茎”产生对应的依赖文件，这个依赖文件必须是存在的或者可被创建的。

所以，我们增加一条模式规则如下：

```sh
$(OBJDIR)/%.o : %.c  
>---$(CC) -c $(CFLAGS) $(CPPFLAGS) $< -o $@  
```

该模式规则中目标文件是$(OBJDIR)/%.o，那么现在有了符合生成我们需要的.o文件的规则了，编译一下：

```sh
# make  
make[1]: Entering directory `/home/Myprojects/example_make/version-2.9/src/ipc'  
make[1]: *** No rule to make target `../../build/unix_dbg/obj/ipc/ipc.o', needed by `../../build/unix_dbg/lib/libipc.a'.  Stop.  
make[1]: Leaving directory `/home/Myprojects/example_make/version-2.9/src/ipc'  
make: *** [ipc] Error 2  
```
发现还是不对，不是已经增加了模式规则了吗，为何还是没有生成.o文件。

我们这里先说说静态模式规则：

一个规则中可以有多个目标，规则所定义的命令对所有的目标有效。一个具有多目标的规则相当于多个规则。 规则的命令对不同的目标的执行效果不同， 因为在规则的命令中可能使用了自动化变量 “$@” 。 多目标规则意味着所有的目标具有相同的依赖文件。多目标通常用在以下两种情况：虽然在多目标的规则中， 可以根据不同的目标使用不同的命令 （在命令行中使用自动化变量 “$@” ）。但是， 多目标的规则并不能做到根据目标文件自动改变依赖文件 （像上边例子中使用自动化变量“$@”改变规则的命令一样） 。需要实现这个目的是，要用到make的静态模式。

静态模式规则是这样一个规则：规则存在多个目标， 并且不同的目标可以根据目标文件的名字来自动构造出依赖文件。静态模式规则比多目标规则更通用， 它不需要多个目标具有相同的依赖。但是静态模式规则中的依赖文件必须是相类似的而不是完全相同
的。静态模式规则语法如下：


```sh
<targets ...>: <target-pattern>: <prereq-patterns ...>    
<commands>    
....  
```

比如下面是一个静态模式规则：

```sh
objects = foo.o bar.o  
  
all: $(objects)  
  
$(objects): %.o: %.c  
$(CC) -c $(CFLAGS) $< -o $@  
```

该规则描述了所有的.o文件的依赖文件为对应的.c文件，对于目标“foo.o” ，取其茎“foo”替代对应的依赖模式“%.c”中的模式字符“%”之后可得到目标的依赖文件“foo.c”。这就是目标“foo.o”的依赖关系“foo.o: foo.c”，规则的命令行描述了如何完成由“foo.c”编译生成目标“foo.o” 。命令行中“$<”和“$@”是自动化变量，“$<” 表示规则中的第一个依赖文件， “$@” 表示规则中的目标文件。上边的这个规则描述了以下两个具体的规则：


```sh
foo.o : foo.c  
>---$(CC) -c $(CFLAGS) foo.c -o foo.o  
bar.o : bar.c  
>---$(CC) -c $(CFLAGS) bar.c -o bar.o  
```

那静态模式规则和普通的模式规则（非静态模式规则）有什么去区别呢？两者都是用目标模式和依赖模式来构建目标的规则中的文件依赖关系，两者不同的地方是 make 在执行时使用它们的时机。

静态模式规则只能用在规则中明确指出的那些文件的重建过程中。不能用在除此之外的任何文件的重建过程中，并且它对指定的每一个目标来说是唯一的。如果一个目标存在于两个规则，并且这两个规则都定义了命令， make 执行时就会提示错误。

非静态模式规则可被用在任何和它相匹配的目标上，当一个目标文件同时符合多个目标模式时，make将会把第一个目标匹配的模式规则作为重建它的规则。

那有没有想过如果我们指定了模式规则后，那还有隐含规则呢，那怎么选择执行哪一个模式规则呢？Makefile中明确指定的模式规则会覆盖隐含模式规则。就是说如果在Makefile中出现了一个对目标文件合适可用的模式规则，那么make就不会再为这个目标文件寻找其它隐含规则，而直接使用在Makefile中出现的这个规则。在使用时，明确规则永远优先于隐含规则。


我们继续说之前的那个问题，我们定义了模式规则后还是没有生成.o文件，我们现在将其改为静态规则再试试就看，如下：

```sh
$(SRC_OBJ) : $(OBJDIR)/%.o : %.c
>---$(CC) -c $(CFLAGS) $(CPPFLAGS) $< -o $@  
```
执行后：

```sh
# make  
make[1]: Entering directory `/home/Myprojects/example_make/version-2.9/src/ipc'  
make[1]: *** No rule to make target `ipc.c', needed by `../../build/unix_dbg/obj/ipc/ipc.o'.  Stop.  
make[1]: Leaving directory `/home/Myprojects/example_make/version-2.9/src/ipc'  
make: *** [ipc] Error 2  
```

发现提示没有文件ipc.c，这说明没有生成.o的原因是没有.c文件，我很好奇的是为何使用非静态模式为何不提示呢？（还没搞懂，再研究研究，知道的可以给个提示哈~~）

缺少依赖文件，为何没有*.c文件，仔细想想我们的.o文件没有和.c文件在同一目录。在我们工程中，将源代码和二进制文件（.o 文件和可执行文件）安排在不同的目录来进行区分管理。这种情况下，我们可以使用 make 提供的目录搜索依赖文件功能。该功能在下一节讲述，这一节说的够多了，有点累了。可惜最终还是没有给出一个可用的Makefile，在下一节将会给出。



