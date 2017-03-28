# （十）：make内嵌函数及make命令显示


这一节我们讲一下make的函数，在之前的章节已经讲到了几个函数：wildcard、patsubst、notdir、shell等。一般函数的调用格式如下：

$(funcname arguments)

或

$(funcname arguments)

其中funcname是需要调用函数的函数名称，应该是make内嵌函数；arguments是函数参数，参数和函数名之间使用空格分割，如果存在多个参数时，参数之间使用逗号“,”分开。函数调用以“$”开头，使用成对的圆括号或花括号把函数名和参数括起，一般使用圆括号。

下面来看一下常用的一些函数：

`1.`    获取匹配模式文件名函数—wildcard 。<br>
用法：$(wildcard PATTERN)
该函数会列出当前目录下所有符合模式“PATTERN”格式的文件名。返回空格分割的、存在当前目录下的所有符合模式“PATTERN”的文件名。
例如：


```sh
SRC_FILES = $(wildcard src/*.c)  
```

返回src目录下所有.c文件列表。

`2.`    字符串替换函数—subst。<br>
用法：$(subst FROM,TO,TEXT)
该函数把字串“TEXT”中的“FROM”字符替换为“TO”，返回替换后的新字符串。

`3.`    模式替换函数—patsubst。<br>
用法：$(patsubst PATTERN,REPLACEMENT,TEXT)

该函数搜索“TEXT”中以空格分开的单词，将符合模式“TATTERN”替换为“REPLACEMENT” 。参数“PATTERN”中可以使用模式通配符“%”，来代表一个单词中的若干字符。如果参数“REPLACEMENT”中也包含一个“%” ，那么“REPLACEMENT”中的“%”将是“TATTERN”中的那个“%”所代表的字符串。

例如：

```sh
SRC_OBJ = $(patsubst %.c, %.o, $(SRC_FILES))
```

将SRC_FILES中所有.c文件替换为.o返回给变量SRC_OBJ。<br>
此函数功能类似之前讲过的变量替换，http://blog.csdn.net/shallnet/article/details/37529935
变量替换格式是“$(var:a=b)”或“${var:a=b}”，其意思是，把变量“var”中所有以“a”字串“结尾”的“a”替换成“b”字串。<br>
例如我们存在一个代表所有.c 文件的变量。定义为“src_files = a.c b.c c.c” 。<br>
为了得到这些.c文件所对应的.o源文件。如下两种使用可以得到同一种结果：

```sh
$(objects:.c=.o)  
$(patsubst %.c,%.o,$( src_files))  
```

`4.`    过滤函数—filter。<br>
用法：$(filter PATTERN…,TEXT)
该函数过滤掉字串“TEXT”中所有不符合模式“PATTERN”的单词，保留所有符合此模式的单词。可以使用多个模式。模式中一般需要包含模式字符“%” 。存在多个模式时，模式表达式之间使用空格分割。返回空格分割的“TEXT”字串中所有符合模式“PATTERN”的字串。

`5.`    反过滤函数—filter-out。<br>
用法：$(filter-out PATTERN...,TEXT)
和“filter”函数实现的功能相反。过滤掉字串“TEXT”中所有符合模式“PATTERN” 的单词， 保留所有不符合此模式的单词。 可以有多个模式。存在多个模式时，模式表达式之间使用空格分割。

`6.`    取目录函数—dir。<br>
用法：$(dir NAMES…)
从文件名序列“NAMES…”中取出各个文件名的目录部分。文件名的目录部分就是包含在文件名中的最后一个斜线（ “/” ） （包括斜线）之前的部分。返回空格分割的文件名序列“NAMES…”中每一个文件的目录部分。如果文件名中没有斜线，认为此文件为当前目录（ “./” ）下的文件。

`7.`    取文件名函数——notdir。<br>
用法：$(notdir NAMES…)
从文件名序列“NAMES…”中取出非目录部分。目录部分是指最后一个斜线（ “/” ） （包括斜线）之前的部分。删除所有文件名中的目录部分，只保留非目录部分。文件名序列“NAMES…”中每一个文件的非目录部分。

`8.`    取后缀函数—suffix。<br>
用法：$(suffix NAMES…) 
函数从文件名序列“NAMES…”中取出各个文件名的后缀。后缀是文件名中最后一个以点“.”开始的（包含点号）部分，如果文件名中不包含一个点号，则为空。 返回以空格分割的文件名序列“NAMES…”中每一个文件的后缀序列。

`9.`    取前缀函数—basename<br>
用法：$(basename NAMES…)
从文件名序列“NAMES…”中取出各个文件名的前缀部分（点号之后的部分） 。前缀部分指的是文件名中最后一个点号之前的部分。 返回空格分割的文件名序列“NAMES…”中各个文件的前缀序列。如果文件没有前缀，则返回空字串。

这里仅仅讲到一些常用的函数，还有一些函数没有讲到，用到的时候可以去翻翻makefile手册。

通常情况下make在编译时会打印出当前正在执行的命令，当编译链接选项很长时，会输出很多东西在屏幕上，如果我 不想再屏幕上看到很多东西，我们可以在命令前面加上@，这样命令就不会输出到屏幕了。我们这样尝试修改下：


```sh
# make  
make[1]: Entering directory `/home/Myprojects/example_make/version-3.1/src/ipc'  
make[1]: Leaving directory `/home/Myprojects/example_make/version-3.1/src/ipc'  
make[1]: Entering directory `/home/Myprojects/example_make/version-3.1/src/tools'  
make[1]: Leaving directory `/home/Myprojects/example_make/version-3.1/src/tools'  
make[1]: Entering directory `/home/Myprojects/example_make/version-3.1/src/main'  
make[1]: Leaving directory `/home/Myprojects/example_make/version-3.1/src/main'  
```

发现只有进入目录和退出目录的显示，这样很难知道目前编译过程。其实我们可以在规则命令处加入一行类似打印：

```sh
@echo "do something......"
```

这样可以输出目前正在做的事，又不会输出正在执行命令。现在将规则修改下如下：

```sh
$(OBJDIR) :  
>---@echo "   MKDIR $(notdir $@)..."  
>---@mkdir -p $@  
  
ifneq ($(SRC_BIN),)  
$(BINDIR)/$(SRC_BIN) : $(SRC_OBJ)  
>---@echo "   LINK $(notdir $@)..."  
>---@$(CC) -o $@ $^ $(LDFLAGS)  
endif  
  
ifneq ($(SRC_LIB),)  
$(LIBDIR)/$(SRC_LIB) : $(SRC_OBJ)  
>---@echo "    ARCHIVE $(notdir $@)..."  
>---@$(AR) rcs $@ $^  
>---@echo "    COPY $@ to $(SRC_BASE)/libs"  
>---@cp $@ $(SRC_BASE)/libs  
endif  
  
$(SRC_OBJ) : $(OBJDIR)/%.o : %.c  
>---@echo "    COMPILE $(notdir $<)..."  
>---@$(CC) -c $(CFLAGS) $(CPPFLAGS) $< -o $@  
```

编译输出如下：

```sh
# make  
make[1]: Entering directory `/home/Myprojects/example_make/version-3.1/src/ipc'  
    COMPILE ipc.c...  
    ARCHIVE libipc.a...  
    COPY ../../build/unix_dbg/lib/libipc.a to ../../libs  
make[1]: Leaving directory `/home/Myprojects/example_make/version-3.1/src/ipc'  
make[1]: Entering directory `/home/Myprojects/example_make/version-3.1/src/tools'  
    COMPILE base64.c...  
    COMPILE md5.c...  
    COMPILE tools.c...  
    ARCHIVE libtools.a...  
    COPY ../../build/unix_dbg/lib/libtools.a to ../../libs  
make[1]: Leaving directory `/home/Myprojects/example_make/version-3.1/src/tools'  
make[1]: Entering directory `/home/Myprojects/example_make/version-3.1/src/main'  
    COMPILE main.c...  
   LINK target_bin...  
make[1]: Leaving directory `/home/Myprojects/example_make/version-3.1/src/main'  
```

其中目录切换的输出仍然很多，我们可以将其关闭，这需要使用到make的参数，在make -C是指定--no-print- 
directory参数。我们将顶层目录下Makefile规则修改如下：

```sh
$(BUILDDIR):  
>---@echo "    Create directory $@ ..."  
>---mkdir -p $(BUILDDIR)/bin $(BUILDDIR)/lib  
  
$(MODULES):  
>---@$(MAKE) -C $(DIR)/$@ MODULE=$@ --no-print-directory  
  
main:tools ipc  
  
clean :  
>---@for subdir in $(MODULES); \  
>---do $(MAKE) -C $(DIR)/$$subdir MODULE=$$subdir $@ --no-print-directory; \  
>---done  
编译输出：  
# make  
    COMPILE ipc.c...  
    ARCHIVE libipc.a...  
    COPY ../../build/unix_dbg/lib/libipc.a to ../../libs  
    COMPILE base64.c...  
    COMPILE md5.c...  
    COMPILE tools.c...  
    ARCHIVE libtools.a...  
    COPY ../../build/unix_dbg/lib/libtools.a to ../../libs  
    COMPILE main.c...  
   LINK target_bin...  
# make clean  
rm -f  ../../build/unix_dbg/obj/ipc/ipc.o ../../build/unix_dbg/lib/libipc.a  
rm -f  ../../build/unix_dbg/obj/main/main.o ../../build/unix_dbg/bin/target_bin  
rm -f  ../../build/unix_dbg/obj/tools/base64.o  ../../build/unix_dbg/obj/tools/md5.o     
../../build/unix_dbg/obj/tools/tools.o ../../build/unix_dbg/lib/libtools.a  
```

这样看上去输出清爽多了。其实我们也可以使用make -s 来全面禁止命令的显示。



