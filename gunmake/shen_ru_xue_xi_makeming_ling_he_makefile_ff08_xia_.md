# 深入学习Make命令和Makefile（下）


make是Linux下的一款程序自动维护工具，配合makefile的使用，就能够根据程序中模块的修改情况，自动判断应该对那些模块重新编译，从而保证软件是由最新的模块构成。 
本文分为上下两部分，我们在上一篇文章中分别介绍了make和makefile的一些基本用法，在本文中，我们会对make和makefile的功能做进一步的介绍。

##一、构建多个目标

有时候，我们想要在一个makefile中生成多个单独的目标文件，或者将多个命令放在一起，比如，在下面的示例mymakefile3中我们将添加一个clean 选项来清除不需要的目标文件，然后用install选项将生成的应用程序移动到另一个目录中去。这个makefile跟前面的mymakefile较为相似，不同之处笔者用黑体加以标识：


```sh
all: main
# 使用的编译器
CC = gcc
# 安装位置
INSTDIR = /usr/local/bin
# include文件所在位置
INCLUDE = .
# 开发过程中所用的选项
CFLAGS = -g -Wall –ansi
# 发行时用的选项
# CFLAGS = -O -Wall –ansi

main: main.o f1.o f2.o
    $(CC) -o main main.o f1.o f2.o
main.o: main.c def1.h
    $(CC) -I$(INCLUDE) $(CFLAGS) -c main.c
f1.o: f1.c def1.h def2.h
    $(CC) -I$(INCLUDE) $(CFLAGS) -c f1.c
f2.o: f2.c def2.h def3.h
    $(CC) -I$(INCLUDE) $(CFLAGS) -c f2.c

clean:
    -rm main.o f1.o f2.o
install: main
@if [ -d $(INSTDIR) ]; \
then \
cp main $(INSTDIR);\
chmod a+x $(INSTDIR)/main;\
chmod og-w $(INSTDIR)/main;\
echo “Installed in $(INSTDIR)“;\
else \
echo “Sorry, $(INSTDIR) does not exist”;\
fi
```
在这个makefile中需要注意的是，虽然这里有一个特殊的目标all，但是最终还是将main作为目标。因此，如果执行make命令时没有在命令行中给出一个特定目标的话，仍然会编译连接main程序。 

其次要注意后面的两个目标：clean和install。目标clean没有依赖模块，因为没有时间标记可供比较，所以它总被执行；它的实际意图是引出后面的rm命令来删除某些目标文件。我们看到rm命令以-开头，这时即使表示make将忽略命令结果，所以即使没有目标供rm命令删除而返回错误时，make clean依然继续向下执行。 

接下来的目标install依赖于main，所以make知道必须在执行安装命令前先建立main。用于安装的指令由一些shell命令组成。 

因为make调用shell来执行规则，并且为每条规则生成一个新的shell，所以要用一个shell来执行这些命令的话，必须添加反斜杠，以使所有命令位于同一个逻辑行上。这条命令用@开头，表示在执行规则前不会向标准输出打印命令。 
为了安装应用程序，目标install会一条接一条地执行若干命令，并且执行下一个之前，不会检查上一条命令是否成功。若想只有当前面的命令取得成功时，随后的命令才得以执行的话，可以在命令中加入&&，如下所示：


```sh
@if [ -d $(INSTDIR) ]; \
then \
cp main $(INSTDIR) &&\
chmod a+x $(INSTDIR)/main && \
chmod og-w $(INSTDIR/main && \
echo “Installed in $(INSTDIR)“ ;\
else \
echo “Sorry, $(INSTDIR) does not exist” ; false ; \
fi
```

这是shell的“与”指令，只有当在前的命令成功时随后的命令才被执行。这里不必关心前面命令是否取得成功，只需注意这种用法就可以了。 

要想在/usr/local/bin目录安装新命令必须具有特权，所以调用make install命令之前，可以让Makefile使用一个不同的安装目录，或者修改该目录的权限，或切换到root用户。如下所示：



```sh
$ rm *.o main
$ make -f Mymakefile3
gcc -I. -g -Wall -ansi -c main.c
gcc -I. -g -Wall -ansi -c f1.c
gcc -I. -g -Wall -ansi -c f2.c
gcc -o main main.o f1.o f2.o
$ make -f Mymakefile3
make: Nothing to be done for ‘all’.
$ rm main
$ make -f Mymakefile3 install
gcc -o main main.o f1.o f2.o
Installed in /usr/local/bin
$ make -f Mymakefile3 clean
rm main.o f1.o f2.o
$
```

让我们对此作一简单介绍，首先删除main和所有目标文件程序，由于将all作为目标，所以make命令会重新编译main。当我们再次执行make命令时，由于main是最新的，所以make什么也不做。之后，我们删除main程序文件，并执行make install，这会重新建立二进制文件main并将其复制到安装目录。最后，运行make clean命令，来删去所有目标程序。

##二、内部规则

迄今为止，我们已经能够在makefile中给出相应的规则来指出具体的处理过程。实际上，除了我们显式给出的规则外，make还具有许多内部规则，这些规则是由预先规定的目标、依赖文件及其命令组成的相关行。在内部规则的帮助下，可以使makefile变得更加简洁，尤其是在具有许多源文件的时候。现在以实例加以说明，首先建立一个名为foo.c的C程序源文件，文件内容如下所示：

```c
#include <stdio.h>
int main()
{
    printf(“Hello World\n”);
    exit(EXIT_SUCCESS);
}
```

现在让我们用make命令来编译它：

```c
$ make foo
cc foo.c -o foo
$
```

您会惊奇地发现，尽管我们没有指定makefile，但是make仍然能知道如何调用编译器，并且调用的是cc而不是gcc编译器。这在Linux上没有问题，因为cc常常会链接到gcc程序。这完全得益于make内建的内部规则，另外这些内部规则通常使用宏，所以只要为这些宏指定新的值，就可以改变内部规则的默认动作，如下所示：

```c
$ rm foo
$ make CC=gcc CFLAGS=”-Wall -g” foo
gcc -Wall -g foo.c -o foo
$
```

用make命令加-p选项后，可以打印出系统缺省定义的内部规则。它们包括系统预定义的宏、以及产生某些种类后缀的文件的内部相关行。内部规则涉及的文件种类很多，它不仅包括C源程序文件及其目标文件，还包括SCCS文件、yacc文件和lex文件，甚至还包括Shell文件。 
当然，我们更关心的是如何利用内部规则来简化makefile，比如让内部规则来负责生成目标，而只指定依赖关系，这样makefile就简洁多了，如下所示：


```c
main.o: main.c def1.h
f1.o: f1.c def1.h def2.h
f2.o: f2.c def2.h def3.h
```

### 三、后缀规则

前面我们已经看到，有些内部规则会根据文件的后缀（相当于Windows系统中的文件扩展名）来采取相应的处理。换句话说，这样当make见到带有一种后缀的文件时，就知道使用哪些规则来建立一个带有另外一种后缀的文件，最常见的是用以.c结尾的文件来建立以.o结尾的文件，即把源文件编译成目标程序，但是不连接。


现在举例说明后缀规则的应用。有时候，我们需要在不同的平台下编译源文件，例如Windows和Linux。假设我们的源代码是C++编写的，那么Windows下其后缀则为.cpp。不过Linux使用的make版本没有编译.cpp文件的内部规则，倒是有一个用于.cc的规则，因为在UNIX操作系统中c++文件扩展名通常为.cc。


这时候，要么为每个源文件单独指定一条规则，要么为make建立一条新规则，告诉它如何用.cpp为扩展名的源文件来生成目标文件。如果项目中的源文件较多的话，后缀规则就可以派上用场了。要添加一条新后缀规则，首先在makefile文件中加入一行来告诉make新后缀是什么；然后就可以添加使用这个新后缀的规则了。这时，make要用到一条专用的语法：


```sh
.<旧后缀名>.<新后缀名>:
```

它的作用是定义一条通用规则，用来将带有旧后缀名的文件变成带有新后缀名的文件，文件名保持不变，如要将.cpp文件编译成.o文件，可以使用一个新的通用规则：

```sh
.SUFFIXES: .cpp
.cpp.o:
$(CC) -xc++ $(CFLAGS) -I$(INCLUDE) -c $<
```

上面的.cpp .o:告诉make 这些规则用于把后缀为.cpp的文件转换成后缀为.o的文件。其中的标志-xc++的作用是告诉gcc这次要编译的源文件是c++源文件。这里，我们使用一个宏$<来通指需要编译的文件的名称，不管这些文件名具体是什么。我们只需知道，所有以.cpp为后缀的文件将被编译成以.o为后缀的文件，例如以是app.cpp的文件将变成app.o。


注意，我们只跟make说明如何把.cpp文件变成.o文件就行了，至于如何从目标程序文件变成二进制可执行文件，因为make早已知晓，所以就不用我们费心了。所以，当我们调用make程序时，它会使用新规则把类似app .cpp这样的程序变成app.o，然后使用内部规则将app.o文件连接成一个可执行文件app。

现在，make已经知道如何处理扩展名为.cpp的c++源文件，除此之外，我们还可以通过后缀规则将文件从一种类型转换为另一种类型。不过，较新版本的make包含一个语法可以达到同样的效果。例如，模式规则使用%作为匹配文件名的通配符，而不单独依赖于文件扩展名。以下模式规则相当于上面处理.cpp的规则，具体如下所示：

```sh
%.cpp: %o
$(CC) -xc++ $(CFLAGS) -I$(INCLUDE) -c $<
```

## 四、用make管理程序库
一般来说，程序库也是一种由一组目标程序构成的以.a为扩展名的文件，所以，Make命令也可以用来管理这些程序库。实际上，为了简化程序库的管理，make程序还专门设有一个语法：

```sh
lib (file.o)
```

这意味着目标文件file.o以库文件lib.a的形式存放，这意味着lib.a库依赖于目标程序file.o。此外，make命令还具有一个内部规则用来管理程序库，该规则相当于如下内容：

```sh
.c.a:
$(CC) -c $(CFLAGS) $<
$(AR) $(ARFLAGS) $@ $*.o
```

其中，宏$(AR)和$(ARFLAGS)分别表示指令AR和选项rv。如上所见，要告诉make用.c文件生成.a库，必须用到两个规则：第一个规则是说把源文件编译成一个目标程序文件。第二个规则表示使用ar 指令向库中添加新的目标文件。

所以，如果我们有一个名为filed的库，其中含有bar.o文件，那么第一规则中的$<会被替换为bar.c；在第二个规则中的$@被库名filed.a所替代，而$*将被bar所替代。

下面举例说明如何用make来管理库。实际上，用make来管理程序库的规则是很简单的。比如，我们可以将前面示例加以修改，让f1.o和f2.o放在一个称为mylib.a的程序库中，这时的Makefile几乎无需改变，而新的mymakefile4看上去是这样的：

```sh
all: main
# 使用的编译器
CC = gcc
# 安装位置
INSTDIR = /usr/local/bin
# include文件所在位置
INCLUDE = .
# 开发过程中使用的选项
CFLAGS = -g -Wall –ansi
# 用于发行时的选项
# CFLAGS = -O -Wall –ansi
# 本地库
MYLIB = mylib.a

main: main.o $(MYLIB)
    $(CC) -o main main.o $(MYLIB)
    $(MYLIB): $(MYLIB)(f1.o) $(MYLIB)(f2.o)
    main.o: main.c def1.h
    f1.o: f1.c def1.h def2.h
    f2.o: f2.c def2.h def3.h

clean:
    -rm main.o f1.o f2.o $(MYLIB)
install: main
@if [ -d $(INSTDIR) ]; \
then \
cp main $(INSTDIR);\
chmod a+x $(INSTDIR)/main;\
chmod og-w $(INSTDIR)/main;\
echo “Installed in $(INSTDIR)“;\
else \
echo “Sorry, $(INSTDIR) does not exist”;\
fi
```
注意：我们是如何让省缺规则来替我们完成大部分工作的。如今，我们可以试一下新版的makefile的工作情况：


```sh
$ rm -f main *.o mylib.a

$ make -f Mymakefile4
gcc -g -Wall -ansi -c -o main.o main.c
gcc -g -Wall -ansi -c -o f1.o f1.c
ar rv mylib.a f1.o
a - f1.o
gcc -g -Wall -ansi -c -o f2.o f2.c
ar rv mylib.a f2.o
a - f2.o
gcc -o main main.o mylib.a

$ touch def3.h

$ make -f Mymakefile4
gcc -g -Wall -ansi -c -o f2.o f2.c
ar rv mylib.a f2.o
r - f2.o
gcc -o main main.o mylib.a
$
```
现在对上面的例子做必要的说明。首先删除全部目标程序文件和程序库，然后让make重新构建main，因为当连接main.o时需要用到库，所以要先编译和创建库。此后，我们还测试f2.o的依赖关系，我们知道如果def3.h发生了改变，那么必须重新编译f2.c，事实表明make在重新构建main可执行文件之前，正确地编译了f2.c并更新了库。


## 五、Makefile和子目录

如果你的项目比较大的话，可以考虑将某些文件组成一个库，然后单独存放到一个子目录内。这时，对于makefile有两种处理方法，下面分别介绍。

第一种方法：在子目录中放置一个辅助makefile，然后把这个子目录中的源文件编译成一个程序库，最后将这个库复制到主目录中。上级目录中的主要makefile可以放上一个规则，通过调用辅助makefile来建立该库：

```sh
mylib.a:
(cd mylibdirectory;$(MAKE))
```
这样的话，我们就会总是构建mylib.a，因为冒号右边为空。当make调用该规则构建该库时，它会切换到子目录mylibdirectory中，然后调用一个新的make命令来管理该库。因为调用了一个新的shell来完成此任务，所以使用makefile的程序不必进行目录切换。不过，被调用的shell是在一个不同的目录中利用该规则构建该库的，所以括弧能确保所有处理都是由一个shell完成的。

第二种方法：在单个makefile中使用更多的宏，不过这些附加的宏需要在目录名上加D并且为文件名加上F。例如，可以用下面的命令来覆盖内建的.c.o后缀规则：

```sh
.c.o:
$(CC) $(CFLAGS) -c $(@D)/$(
```

为在子目录编译文件，并将目标放在子目录中，可以用像下面这样的依赖关系和规则来更新当前目录中的库：

```sh
mylib.a: mydir/f1.o mydir/f2.o
ar -rv mylib.a $?
```

上述两种方法都是可行的，至于使用哪一种，需要根据您的项目的具体情况来决定。


## 六、GNU make和gcc的有关选项

如果您当前正在使用GNU make和GNU gcc编译器的话，那么它们还分别有一个额外的选项可以使用，下面分别加以说明。

我们首先介绍用于make程序的-jN 选项。这个选项允许make同时执行N条命令。这样的话，就可以将该项目的多个部分单独进行编译，make将同时调用多个规则。如果具有许多源文件的话，这样做能够节约大量编译时间。

其次，gcc还有一个-MM选项可用，该选项会为make生成一个依赖关系表。在一个含有大量源文件的项目中，很可能每个源文件都包含一组头文件，而头文件有时又会包含其它头文件，这时正确区分依赖关系就比较难了。这时为了防止遗漏，最笨的方法就是让每个源文件依赖于所有头文件，但这显然没有必要；另一方面，如果你遗漏一些依赖关系的话，就根本就无法编译通过。这时，我们就可以用gcc的-MM选项来生成一张依赖关系表，例如：

```sh
$ gcc -MM main.c f1.c f2.c
main.o: main.c def1.h
f1.o: f1.c def1.h def2.h
f2.o: f2.c def2.h def3.h
$
```

这时，gcc编译器会扫描所有源文件，并生产一张满足makefile格式要求的依赖关系表，我们只须将它保存到一个临时文件内，然后将其插入makefile即可。

##七、小结

继上一篇文章之后，本文又对make和makefile的一些高级应用作了相应的介绍，至此，我们已经对make和makefile在程序开发中的应用有了一个较为全面的认识，希望本文能对读者的学习和工作有所帮助。
