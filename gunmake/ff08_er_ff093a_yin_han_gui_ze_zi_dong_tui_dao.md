# （二）:隐含规则自动推导


上一节的Makefile勉强可用，但还写的比较繁琐，不够简洁。对每一个.c源文件，都需要写一个生成其对应的.o目标文件的规则，如果有几百个或上千个源文件，都手动来写，还不是很麻烦，这也不够自动化啊。
这样，我们把生成.o目标文件的规则全部删除掉，就是这样一个Makefile文件：


```sh

target_bin : main.o debug.o ipc.o timer.o tools.o  
>---gcc -o target_bin main.o debug.o ipc.o timer.o tools.o  
   
clean:  
>---rm *.o target_bin  
```

这下简洁了不少，这样也能用吗？试试看吧先，make一下：


```sh
# make  
cc    -c -o main.o main.c  
cc    -c -o debug.o debug.c  
cc    -c -o ipc.o ipc.c  
cc    -c -o timer.o timer.c  
cc    -c -o tools.o tools.c  
gcc -o target_bin main.o debug.o ipc.o timer.o tools.o  
```

原来酱紫都可以啊！！target_bin后面那一群依赖文件怎么生成呢？不是没有生成*.o目标文件的规则了吗？再看屏幕编译输出内容：

```sh
cc    -c -o main.o main.c  
cc    -c -o debug.o debug.c  
cc    -c -o ipc.o ipc.c  
cc    -c -o timer.o timer.c  
cc    -c -o tools.o tools.c 
```

怎么长的和之前不太一样呢，尤其是前面那个cc是何物？

其实make可以自动推导文件以及文件依赖关系后面的命令，于是我们就没必要去在每一个*.o文件后都写上类似的命令，因为，我们的 make 会自动推导依赖文件，并根据隐含规则自己推导命令。所以上面.o文件是由于make自动推导出的依赖文件以及命令来生成的。

下面来看看make是如何推导的。

命令make –p可以打印出很多默认变量和隐含规则。Makefile变量可以理解为C语言的宏，直接展开即可（后面会讲到）。取出我们关心的部分：


```sh
# default  
OUTPUT_OPTION = -o $@  
# default  
CC = cc  
# default  
COMPILE.c = $(CC) $(CFLAGS) $(CPPFLAGS) $(TARGET_ARCH) –c  
# Implicit Rules  
%.o: %.c  
#  commands to execute (built-in):  
>---$(COMPILE.c) $(OUTPUT_OPTION) $<  
```

其中cc是一个符号链接，指向gcc，这就可以解释为什么我们看到的编译输出为cc，其实还是使用gcc在编译。

```sh
# ll /usr/bin/cc    
lrwxrwxrwx. 1 root root 3 Dec  3  2013 /usr/bin/cc -> gcc  
```

变量$(CFLAGS) $(CPPFLAGS) $(TARGET_ARCH)都为空。所以%.o: %.c规则命令展开为


```sh
cc    -c -o $@ $<  
```

再看屏幕输出编译内容，摘取一条：

```sh
cc    -c -o main.o main.c  
```

是不是看出点什么？`$@和main.o`对应，$<和main.c对应。其实$@和$<是两个变量。$@为规则中的目标，$<为规则中的第一个依赖文件。%.o:%.c是一种称为模式规则的特殊规则。因为main.o符合该模模式，再推导出依赖文件main.c，最终推导出整个规则为：

```sh
main.o : main.c：  
>--- cc    -c -o main.o main.c 
```

其余几个目标也同样推导。make自动推导的功能为我们减少了不少的Makefile代码，尤其是对源文件比较多的大型工程，我们的Makefile可以不用写得那么繁琐了。
最后，今天的Makefile相对于上一节进化成这个样子了：


```sh
target_bin : main.o debug.o ipc.o timer.o tools.o  
>---gcc -o target_bin main.o debug.o ipc.o timer.o tools.o  
   
clean:  
>---rm *.o target_bin  
```

