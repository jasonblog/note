# （一）：基本规则


一般一个稍大的Linux项目会有很多个源文件组成，最终的可执行程序也是由这许多个源文件编译链接而成的。编译是把一个.c或.cpp文件编译成中间代码.o文件，链接是就使用这些中间代码文件生成可执行文件。比如在当前项目目录下有如下源文件：


```sh
# ls  
common.h  debug.c  debug.h  ipc.c  ipc.h  main.c  tags  timer.c  timer.h  tools.c  tools.h   
```

以上源代码可以这样编译：


```sh
# gcc -o target_bin main.c debug.c ipc.c timer.c tools.c 
```

如果之后修改了其中某一个文件（如tools.c），再执行一下上一行代码即可，但如果有成千上万个源文件这样编译肯定是不够合理的。此时我们可以按下面步骤来编译：


```sh
# gcc -c debug.c  
# gcc -c ipc.c  
# gcc -c main.c  
# gcc -c timer.c  
# gcc -c tools.c  
# gcc -o target_bin main.o debug.o ipc.o timer.o tools.o  
```

如果其中tools.c修改了，只需要编译该文件，再执行最后生成可执行文件的操作，也就是做如下两步操作即可：


```sh
# gcc -c tools.c  
# gcc -o target_bin main.o debug.o ipc.o timer.o tools.o  
```

这样做看上去应该很合理了。但是如果修改了多个文件，就很可能忘了编译某一文件，那么运行时就很有可能出错。如果是common.h文件修改了，那么包含该头文件的所有.c文件都需要重新编译，这样一来的话就更复杂更容易出错了。看来这种方法也不够好，手动处理很容易出错。那有没有一种自动化的处理方式呢？有的，那就是写一个Makefile来处理编译过程。
下面给一个简单的Makefile，在源代码目录下建一个名为Makefile的文件：


```sh
target_bin : main.o debug.o ipc.o timer.o tools.o  
>---gcc -o target_bin main.o debug.o ipc.o timer.o tools.o  
   
main.o: main.c common.h                                                                                                                                                                     
>---gcc -c main.c  
   
debug.o: debug.c debug.h common.h  
>---gcc -c debug.c  
   
ipc.o: ipc.c ipc.h common.h  
>---gcc -c ipc.c  
   
timer.o: timer.c timer.h common.h  
>---gcc -c timer.c  
   
tools.o: tools.c tools.h common.h  
>---gcc -c tools.c  
```

然后在命令行上执行命令：

```sh
# make   
gcc -c main.c  
gcc -c debug.c  
gcc -c ipc.c  
gcc -c timer.c  
gcc -c tools.c  
gcc -o target_bin main.o debug.o ipc.o timer.o tools.o  
#  
# ls  
common.h  common.h~  debug.c  debug.h  debug.o  ipc.c  ipc.h  ipc.o  main.c  main.o  Makefile  Makefile~  tags  target_bin  timer.c  timer.h  timer.o  tools.c  tools.h  tools.o  
#  
```

可见在该目录下生成了.o文件以及target_bin可执行文件。现在我们只需要执行一个make命令就可以完成所有编译工作，无需像之前一样手动执行所有动作，make命令会读取当前目录下的Makefile文件然后完成编译步骤。从编译过程输出到屏幕的内容看得到执行make命令之后所做的工作，其实就是我们之前手动执行的那些命令。现在来说一下什么是Makefile？
所谓Makefile我的理解其实就是由一组组编译规则组成的文件，每条规则格式大致为：


```sh
target ... : prerequisites ...   
>---command  
        ...  
```

其中target是目标文件，可以为可执行文件、*.o文件或标签。Prerequisites是产生target所需要的源文件或*.o文件，可以是另一条规则的目标。commond是要产生该目标需要执行的操作系统命令，该命令必须以tab（文中以>---标示tab字符）开头，不可用空格代替。

说白了就是要产生target，需要依赖后面的prerequisites文件，然后执行commond来产生来得到target。这和我们之前手动执行每条编译命令是一样的，其实就是定义好一个依赖关系，我们把产生每个文件的依赖文件写好，最终自动执行编译命令。

比如在我们给出的Makefile例子中target_bin main.o等就是target，main.o debug.o ipc.o timer.o tools.o是target_bin的prerequisites，gcc -o target_bin main.o debug.o ipc.o timer.o tools.o就是commond，把所有的目标文件编译为最终的可执行文件target，而main.c common.h是main.o的prerequisites，其gcc -c main.c命令生成target所需要的main.o文件。

在该例子中，Makefile工作过程如下：

1. 首先查找第一条规则目标，第一条规则的目标称为缺省目标，只要缺省目标更新了就算完成任务了，其它工作都是为这个目的而做的。 该Makefile中第一条规则的目标target_bin，由于我们是第一次编译，target_bin文件还没生成，显然需要更新，但此时依赖文件main.o debug.o ipc.o timer.o tools.o都没有生成，所以需要先更新这些文件，然后才能更新target_bin。

2. 所以make会进一步查找以这些依赖文件main.o debug.o ipc.o timer.o tools.o为目标的规则。首先找main.o，该目标也没有生成，该目标依赖文件为main.c common.h，文件存在，所以执行规则命令gcc -c main.c，生成main.o。其他target_bin所需要的依赖文件也同样操作。

3. 最后执行gcc -o target_bin main.o debug.o ipc.o timer.o tools.o，更新target_bin。

在没有更改源代码的情况下，再次运行make：

```c
# make  
make: `target_bin' is up to date.  
#  
```

得到提示目标target_bin已经是最新的了。
如果修改文件main.c之后，再运行make：

```sh
# vim main.c  
# make  
gcc -c main.c  
gcc -o target_bin main.o debug.o ipc.o timer.o tools.o  
```

此时make会自动选择受影响的目标重新编译：

首先更新缺省目标，先检查target_bin是否需要更新，这需要检查其依赖文件main.o debug.o ipc.o timer.o tools.o是否需要更新。

其次发现main.o需要更新，因为main.o目标的依赖文件main.c最后修改时间比main.o晚，所以需要执行生成目标main.o的命令：gcc -c main.c更新main.o。

最后发现目标target_bin的依赖文件main.o有更新过，所以执行相应命令gcc -o target_bin main.o debug.o ipc.o timer.o tools.o更新target_bin。

总结下，执行一条规则步骤如下：

1. 先检查它的依赖文件，如果依赖文件需要更新，则执行以该文件为目标的的规则。如果没有该规则但找到文件，那么该依赖文件不需要更新。如果没有该规则也没有该文件，则报错退出。

2. 再检查该文件的目标，如果目标不存在或者目标存在但依赖文件修改时间比他要晚或某依赖文件已更新，那么执行该规则的命令。

由此可见，Makefile可以自动发现更新过的文件，自动重新生成目标，使用Makefile比自己手动编译比起来，不仅效率高，还减少了出错的可能性。
 
Makefile中有很多目标，我们可以编译其中一个指定目标，只需要在make命令后面带上目标名称即可。如果不指定编译目标的话make会编译缺省的目标，也就是第一个目标，在本文给出的Makefile第一个目标为target_bin。如果只修改了tools.c文件的话，我们可能只想看看我们的更改的源代码是否有语法错误而又不想重新编译这个工程的话可以执行如下命令：


```sh
# make tools.o   
gcc -c tools.c 
```

编译成功，这里又引出一个问题，如果继续执行同样的命令：

```sh
# make tools.o  
make: `tools.o' is up to date.  
```

我们先手动删掉tools.o文件再执行就可以了，怎么又是手动呢？我们要自动，要自动！！好吧，我们加一个目标来删除这些编译过程中产生的临时文件，该目标为clean。
我们在上面Makefile最后加上如下内容：

```sh
clean:  
>---rm *.o target_bin  
```

当我们直接make命令时不会执行到该目标，因为没有被默认目标target_bin目标或以target_bin依赖文件为目标的目标包含在内。我们要执行该目标需要在make时指定目标即可。如下：

```sh
# make clean  
rm *.o target_bin  
```

可见clean目标被执行到了，再执行make时make就会重新生成所有目标对应的文件，因为执行make clean时，那些文件被清除了。

clean目标应该存在与你的Makefile当中，它既可以方便你的二次编译，又可以保持的源文件的干净。该目标一般放在最后，不可放在最开头，否则会被当做缺省目标被执行，这很可能不是你的意愿。

最后总结一下，Makefile只是告诉了make命令如何来编译和链接程序，告诉make命令生成目标文件需要的文件，具体的编译链接工作是你的目标对应的命令在做。

给一个今天完整的makefile：


```sh
target_bin : main.o debug.o ipc.o timer.o tools.o  
>---gcc -o target_bin main.o debug.o ipc.o timer.o tools.o  
   
main.o: main.c common.h                                                                                                                                                                     
>---gcc -c main.c  
   
debug.o: debug.c debug.h common.h  
>---gcc -c debug.c  
   
ipc.o: ipc.c ipc.h common.h  
>---gcc -c ipc.c  
   
timer.o: timer.c timer.h common.h  
>---gcc -c timer.c  
   
tools.o: tools.c tools.h common.h  
>---gcc -c tools.c  
   
clean:  
>---rm *.o target_bin  
```
给一个今天完整的makefile：



