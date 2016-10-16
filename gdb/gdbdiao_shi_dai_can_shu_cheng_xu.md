# gdb调试带参数程序


一般来说GDB主要调试的是C/C++的程序。要调试C/C++的程序，首先在编译时，我们必须要 
把调试信息加到可执行文件中。使用编译 器（cc/gcc/g++）的 -g 参数可以做到这一点。如： 

```sh
> cc -g hello.c -o hello 
> g++ -g hello.cpp -o hello 
```


如果没有-g，你将看不见程序的函数名、变量名，所代替的全是 运行时的内存地址。当你用 
-g把调试信息加入之后，并成功编译目标代码以后，让我们来看看如何用gdb来调试他。 

启动 GDB的方法有以下几种： 

```sh
1、gdb <program> 
program也就是你的执行文件，一般在当然目录下。 

2、gdb <program> core 
用gdb同时调试一个运行程序和core文件，core是程序非法执行后core dump后产生 
的文件。 

3、gdb <program> <PID> 
如果你的程序是一个服务程序，那么你可以指定这个服务程序运行时的进程ID。gd 
b会自动attach上去，并调试他。program应该在 PATH环境变量中搜索得到。 
```


GDB启动时，可以加上一些GDB的启动开关，详细的开关可以用gdb -help查看。我在下面只 
例举一些比较常用的参数： 

```
-symbols <file> 
-s <file> 
从指定文件中读取符号表。 
```

```
-se file 
从指定文件中读取符号表信息，并把他用在可执行文件中。 
```

```
-core <file> 
-c <file> 
调试时core dump的core文件。 
```

```
-directory <directory> 
-d <directory> 
加入一个源文件的搜索路径。默认搜索路径是环境变量中PATH所定义的路径。
```


当以gdb <program>方式启动gdb后，gdb会在PATH路径和当前目录中搜索<program>的源文件 
。 如要确认gdb是否读到源文件，可使用l或list命令，看看gdb是否能列出源代码。 

在gdb中，运行程序使用r或是run命令。 程序的运行，你有可能需要设置下面四方面的事。 
```sh
1、程序运行参数。 
set args 可指定运行时参数。（如：set args 10 20 30 40 50） 
show args 命令可以查看设置好的运行参数。 

2、 运行环境。 
path <dir> 可设定程序的运行路径。 
show paths 查看程序的运行路径。 
set environment varname [=value] 设置环境变量。如：set env USER=hchen 
show environment [varname] 查看环境变量。 

3、工作目录。 
cd <dir> 相当于shell的cd命令。 
pwd 显示当前的所在目录。 

4、程序的输入输出。 
info terminal 显示你程序用到的终端的模式。 
使用重定向控制程序输出。如：run > outfile 
tty命令可以指写输入输出的终端设备。如：tty /dev/ttyb
```


具体来说就是

例如一个程序名为prog 参数为 `-l a -C abc`
则，运行
```
gcc/g++ -g  prog.c/cpp -o prog
```

就可以用gdb调试程序prog
#gdb prog
进入gdb调试界面
输入参数命令set args 后面加上程序所要用的参数，注意，不再带有程序名，直接加参数，如：

```
set args -l a -C abc
```

回车后输入
r
即可开始运行