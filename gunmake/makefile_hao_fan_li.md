# Makefile 好範例


自动化变量

```
$@	表示规则中的目标。
$<	表示规则中的第一个条件。
$?	表示规则中所有比目标新的条件，组成一个列表，以空格分隔。
$^	表示规则中的所有条件，组成一个列表，以空格分隔。
```

###示例1


```sh
main: main.o stack.o maze.o
    gcc main.o stack.o maze.o -o main
```

可以改写成：


```sh
main: main.o stack.o maze.o
    gcc $^ -o $@
```

###示例2

有时候希望只对更新过的条件进行操作，例如有一个库文件libsome.a依赖于几个目标文件：


```sh
libsome.a: foo.o bar.o lose.o win.o 
    ar r libsome.a $?
    ranlib libsome.a
```

##常用变量

```sh
变量名          说明
AR              静态库打包命令的名字，缺省值是ar。
ARFLAGS         静态库打包命令的选项，缺省值是rv。
AS              汇编器的名字，缺省值是as。
ASFLAGS         汇编器的选项，没有定义。
CC              C编译器的名字，缺省值是cc。
CFLAGS          C编译器的选项，没有定义。
CXX             C++编译器的名字，缺省值是g++。
CXXFLAGS        C++编译器的选项，没有定义。
CPP             C预处理器的名字，缺省值是$(CC) -E。
CPPFLAGS        C预处理器的选项，没有定义。
LD              链接器的名字，缺省值是ld。
LDFLAGS         链接器的选项，没有定义。
TARGET          _ARCH 和目标平台相关的命令行选项，没有定义。
OUTPUT          _OPTION 输出的命令行选项，缺省值是-o $@。
LINK.o          把.o文件链接在一起的命令行，缺省值是$(CC) $(LDFLAGS) $(TARGET_ARCH)。
LINK.c          把.c文件链接在一起的命令行，缺省值是$(CC) $(CFLAGS) $(CPPFLAGS) $(LDFLAGS) $(TARGET_ARCH)。
LINK.cc         把.cc文件（C++源文件）链接在一起的命令行，缺省值是$(CXX) $(CXXFLAGS) $(CPPFLAGS) $(LDFLAGS) $(TARGET_ARCH)。
COMPILE.c       编译.c文件的命令行，缺省值是$(CC) $(CFLAGS) $(CPPFLAGS) $(TARGET_ARCH) -c。
COMPILE.cc      编译.cc文件的命令行，缺省值是$(CXX) $(CXXFLAGS) $(CPPFLAGS) $(TARGET_ARCH) -c。
RM              删除命令的名字，缺省值是rm -f。
```


