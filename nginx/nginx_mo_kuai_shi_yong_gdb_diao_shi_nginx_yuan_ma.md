# nginx 模块 使用gdb调试nginx源码

工欲善其事必先利其器，如何使用调试工具gdb一步步调试nginx是了解nginx的重要手段。

ps：本文的目标人群是像我这样初接触Unix编程的同学，如果有什么地方错误请指正。

##熟悉gdb的使用
这里就不说了，谷歌一搜一堆，这里推荐一篇文章：GDB 命令详细解释

请重点看一下step，run，break，list，info，continue命令


##下载nginx源码

这里使用nginx-1.0.14

![](./images/201205101644521909.png)
src是源代码，auto文件夹是configure运行时候的各种命令集合

##修改config并编译
由于gdb需要gcc的时候加上`-g`参数，这样生成的文件才能使用gdb调试，因此我们要对源码做一下小改动

修改auto/cc/conf文件
```sh
ngx_compile_opt="-c"
```
变为
```sh
ngx_compile_opt="-c -g"
```
执行configure
```sh
./configure --prefix=/home/yejianfeng/nginx/
```
确认
发现多出了objs文件夹，里面有Makefile文件

确认一下-g参数是否加上了

vim objs/Makefile

![](./images/201205101644526860.png)


确认已经加上了-g，

make

生成可执行文件

![](./images/201205101644538463.png)

至此，nginx可执行文件已经成功编译了。

##生成gdb所需要的core文件

gdb的core文件是为了保证在调试过程中遇到问题可以随时重新启动调试进程，更便于调试。

###启动nginx
在objs目录下 ./nginx
```sh
ps aux|grep nginx
```

![](./images/201205101644535365.png)

看到nginx启动了一个master进程（18237）和一个worker进程（18238）

###在objs目录下运行命令
gcore 18237


![](./images/20120510164454839.png)

看到core.188237生成了。

###杀死nginx进程，包括master和worker

```sh
./nginx -s stop（或者直接使用kill）
```

##启动调试

```sh
[root@localhost objs]# gdb ./nginx core.18237
```
此时进入gdb模式

![](./images/201205101644558232.png)


## gdb打断点并调试

使用l（list）命令查看main 函数，这个main函数是在src/core/nginx.c里面，是nginx的入口函数

```sh
(gdb) l #列出源文件，只列出10行，请使用l- 和 l 或者l +行号 或者 l +函数名 进行定位

(gdb) break 215 #在哪行打断点

(gdb) r #run，从main函数开始运行程序
```

![](./images/201205101644563673.png)

此时进入另一个终端查看进程情况

![](./images/201205101644566463.png)

看到已经启动了一个nginx了

此时在gdb中就可以进行调试了

![](./images/20120510164457509.png)

下面就使用next和step一步一步进行调试吧

##调试中可能遇到的问题

###nginx是master-worker模式，当fork出现子进程的时候如何进入子进程呢？
（gdb）set follow-fork-mode child

这个命令可以是
```sh
set follow-fork-mode parent

set follow-fork-mode child

请参考：Debugging Forks

print（p）能否打印出指针指向的内容呢？
（gdb）p *ptr
```

![](./images/201205101644578524.png)


###当使用attach pid进入某个进程的时候出现错误

```sh
(gdb) attach 30721
```

Attaching to program: /data0/s/bin/s, process 30721

ptrace: Operation not permitted.

这个错误是由于有其他gdb也在调试这个进程

请确定是否有另外的终端或者另外的同事在gdb这个进程

##参考资料：

gdb调试nginx的helloworld

Debugging Forks

Nginx源码阅读笔记