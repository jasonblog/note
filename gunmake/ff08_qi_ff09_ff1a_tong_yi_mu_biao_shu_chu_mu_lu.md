# （七）：统一目标输出目录


上一节我们把规则单独提取出来，方便了Makefile的维护，每个模块只需要给出关于自己的一些变量，然后再使用统一的规则Makefile。这一节我们继续改进我们的Makefile，到目前为止我们的Makefile编译链接输出的目标都在源文件同目录下或模块Makefile同一目录下，当一个项目大了之后，这样会显得很乱，寻找编译输出的文件也比较困难。既然Makefile本身就是按照我们的的规则来编译链接程序，那么我们就可以指定其编译链接目标的目录，这样，我们可以清楚输出文件的地方，并且在清除已编译的目标时直接删除指定目录即可，不需要一层一层的进入源代码目录进行删除，这样又提高了效率。

既然要统一目标输出目录，那么该目录就需要存在，所以我们可以增加一条规则来创建这些目录，包括创建可执行文件的目录、链接库文件的目录以及.o文件的目录。并且目录还可以通过条件判断根据是否产生调试信息来区分开相应的目标文件。一般一个工程的顶层目录下都会有一个build目录来存放编译的目标文件结果，目前我的工程目录下通过Makefile创建的目录build的目录树如下：



```sh
build/            //build根目录  
├── unix        //unix平台项目下不带调试信息输出目录  
│   ├── bin    //存放可执行文件目录  
│   ├── lib    //存放可文件目录  
│   └── obj    //存放.o文件目录，该目录下将每个模块生成的.o文件各自的目录下面  
│       ├── ipc  
│       ├── main  
│       └── tools  
└── unix_dbg   ////unix平台项目下带调试信息输出目录  
    ├── bin  
    ├── lib  
    └── obj  
        ├── ipc  
        ├── main  
        └── tools  
  
14 directories, 0 files  
```

以上目录中bin和lib目录在顶层Makefile中创建，obj及其下面模块子目录在各模块的Makefile里面创建。
顶层Makefile创建目录如下：

```sh
ifeq ($(DEBUG_SYMBOLS), TRUE)  
>---BUILDDIR = ./build/$(PLATFORM)_dbg  
else  
>---BUILDDIR = ./build/$(PLATFORM)  
endif  
  
all : $(BUILDDIR) $(MODULES)  
  
$(BUILDDIR):  
>---@echo "    Create directory $@ ..."  
>---mkdir -p $(BUILDDIR)/bin $(BUILDDIR)/lib
```

我们在all目标里面增加了其依赖目标BUILDDIR，该目标对应的规则为创建bin目录和lib目录。这样每次编译之前都会创建目录。


各模块内部Makefile创建生成.O文件的目录，如上目录树所示。类似于顶层Makefile，各模块内部Makefile需要根据平台、编译调试信息、以及模块名称来生成需要的目录名称，然后再增加创建该目录的规则。因为每个模块都会做这些处理，所以我们将这部分写在规则Makefile(Makefile.rule)里面，如下：


```sh
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
……  
ifeq ($(MAKELEVEL), 0)  
all : msg  
else  
all : lib bin  
endif  
  
lib : $(OBJDIR) $(SRC_LIB)  
  
bin : $(OBJDIR) $(SRC_BIN)                                                                                                                         
  
$(OBJDIR) :  
>---@echo "   MKDIR $(notdir $@)..."  
>---@mkdir -p $@  
```
此时我们编译一下后查看build目录：

```sh
build/  
└── unix_dbg  
    ├── bin  
    ├── lib  
    └── obj  
        ├── ipc  
        ├── main  
        └── tools  
  
7 directories, 0 files   
```

由于我们是开启了调试信息，所以创建了unix_dbg目录，并且该目录下创建了bin、lib、obj目录及其模块目录，但我们没有发现有文件存放在里面。

到目前为止，这一节仅仅讲述如何创建统一的目标文件存放目录，但是要想将编译生成的目标文件自动生成到这些目录还没有完成。其实我们只需要给目标加上路径即可，但还是有一些详细的地方需要处理，具体的我们会在下一节中讲到，这一节暂不给出最后的Makefile。



