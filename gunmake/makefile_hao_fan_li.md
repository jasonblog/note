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


##函数
###函数的调用语法
函数调用，很像变量的使用，也是以“$”来标识的，其语法如下:


```sh
$(<function> <arguments>)
```

或是

```sh
${<function> <arguments>}
```

这里，`<function>`就是函数名，make 支持的函数不多。`<arguments>`是函数的参数，参数间以逗号“，”分隔，而函数名和参数之间以“空格”分隔。函数调用以 “$” 开头，以 圆括号或花括号 把函数名和参数括起。感觉很像一个变量，是不是?函数中的参数可以使用变量，为了风格的统一，函数和变量的括号最好一样，如使用 `$(subst a,b,$(x))` 这样的形式，而不是 `$(subst a,b,${x})`的形式。因为统一会更清楚，也会减少一些不必要的麻烦。


还是来看一个示例:

```sh
comma:= ,
empty:=
space:= $(empty) $(empty)
foo:= a b c
bar:= $(subst $(space),$(comma),$(foo))
```

在这个示例中，$(comma)的值是一个逗号。$(space)使用了$(empty)定义了一个空格，$(foo)的值是“a b c”，$(bar)的定义用，调用了函数“subst”，这是一个替换函数，这个函数有三个参数，第一个参数是被替换字串，第二个参数是替换字串，第三个参数是替换操作作用的字串。这个函数也就是把$(foo)中的空格替换成逗号，所以$(bar)的值是“a,b,c”。


###字符串处理函数
####subst

```sh
$(subst <from>,<to>,<text>)
```

- 名称:字符串替换函数——subst。
- 功能:把字串<text>中的<from>字符串替换成<to>。
- 返回:函数返回被替换过后的字符串。
- 示例: $(subst ee,EE,feet on the street) 把“feet on the street”中的“ee”替换成“EE”，返回结果是“fEEt on the strEEt”。
- 

#### patsubst

```sh
$(patsubst <pattern>,<replacement>,<text>)
```

- 名称:模式字符串替换函数——patsubst。

- 功能:查找`<text>`中的单词(单词以“空格”、“Tab”或“回车”“换行”分隔)是否符合模式`<pattern>`，如果匹配的话，则以`<replacement>`替换。这里，`<pattern>`可以包括通配符%，表示任意长度的字串。如果`<replacement>`中也包含%，那么，<replacement>中的这个%`<pattern>`中的那个%所代表的字串。(可以用“\”来转义，以\%来表示真实含义的%字符)

- 返回:函数返回被替换过后的字符串。

- 示例:` $(patsubst %.c,%.o,x.c.c bar.c) ` 把字串“x.c.c bar.c”符合模式[%.c]的单词替换成[%.o]，返回结果是“x.c.o bar.o”

- 备注: 这和我们前面“变量章节”说过的相关知识有点相似。

如:

`$(var:<pattern>=<replacement>)` 相当于 `(patsubst <pattern>,<replacement>,$(var))`

而 `$(var: <suffix>=<replacement>)` 则相当于 `$(patsubst %<suffix>,%<replacement>,$(var))` 。

例如有: `objects = foo.o bar.o baz.`

那么， `$(objects:.o.c)=` 和 `$(patsubst %.o,%.c,$(objects))` 是一样的。


#### strip

```sh
$(strip <string>)
```

- 名称:去空格函数——strip。
- 功能:去掉`<string>`字串中开头和结尾的空字符。
- 返回:返回被去掉空格的字符串值。
- 例： `$(strip a b c )` 把字串“a b c ”去到开头和结尾的空格，


#### findstring
```sh
$(findstring <find>,<in>)
```

- 名称:查找字符串函数——findstring。
- 功能:在字串<in>中查找<find>字串。
- 返回:如果找到，那么返回<find>，否则返回空字符串。
- 示例:

```sh
$(findstring a,a b c)
$(findstring a,b c)
```

```sh
第一个函数返回“a”字符串，第二个返回“”字符串(空字符串)
```

#### filter

```sh
$(filter <pattern...>,<text>)
```

- 名称:过滤函数——filter。
- 功能:以`<pattern>`模式过滤`<text>`字符串中的单词， 保留  符合模式`<pattern>`的单词。可以有多个模式。
- 返回:返回符合模式`<pattern>`的字串。
- 示例:



