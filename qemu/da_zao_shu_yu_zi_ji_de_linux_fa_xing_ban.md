# 打造属于自己的linux发行版


linux的启动过程，包括BIOS的加电自检POST，拷贝MBR的信息（启动BootLoader），加载内核，挂载根文件安系统这几大步熟悉grub的话会知道linux启动时grub中有三项：`root，kernel，initrd`。其三项的作用分别是：

```sh
1.指定内核所在的目录
2.指定内核的名称，以及挂载根目录的方式，还有向内核传递一定的参数
3.initrd实际就是个小的linux系统，在一般的系统中initrd的作用是：启动一个很小的linux用来挂载真实的linux。
```

今天的目的就是从内核开始，打造一个属于自己的linux。



环境：Ubuntu13.04 gcc4.7.3<br>
相关的准备工作：
内核的编译<br> 
qemu的安装<br>

##制作根系统目录

1）创建init程序 

首先创建一个init.c文件，代码如下:

```c
#include<stdio.h>
int main()
{
    printf("Welcome! My student No. is sa*****310.\n");
    return 0;
}
```
静态编译成一个可执行文件。

```sh
gcc -static -o init init.c
```

2）建立引导根目录映像

终端运行：
```sh
dd if=/dev/zero of=initrd.img bs=4096 count=1024
mkfs.ext3 initrd.img
```
有提示，输入y。


###关于dd
dd 是 Linux/UNIX 下的一个非常有用的命令，作用是用指定大小的块拷贝一个文件，并在拷贝的同时进行指定的转换。<br>

```sh
语法：dd [选项] 
if =输入文件（或设备名称）。 
of =输出文件（或设备名称）。 
ibs = bytes 一次读取bytes字节，即读入缓冲区的字节数。 
...
```

3）创建rootfs目录，并挂载

```sh
mkdir rootfs
sudo mount -o loop initrd.img rootfs
```

4）在rootfs中添加一些文件
将init拷贝到initrd4M.img的目标根目录下（因为linux启动后期会在根目录中寻找一个应用程序来运行，在根目录下提供init是一种可选方案）
```sh
cp init rootfs/
```

准备dev目录：

```sh
sudo mkdir rootfs/dev
```

linux启动过程中会启用console设备:
sudo mknod rootfs/dev/console c 5 1
另外需要提供一个linux根设备，我们使用ram:

```sh
sudo mknod rootfs/dev/ram b 1 0
sudo umount rootfs
```
至此，一个包含简单应用程序的根目录initrd4M.img映像就准备好。

mknod 
用于制作字符或块相关文件


用qemu跑一下：
```sh
qemu -kernel ../linux-3.9.2/arch/x86/boot/bzImage -initrd initrd.img -append "root=/dev/ram init=/init"
```



## 更新内核
直接安装的系统内核版本一般不是最新，用

```sh
uname -a  
```
命令可以查看内核的版本号，比如我的就是：

![](./images/20130530105838024)

下面来手动更新内核到最新的稳定版本。

1.获取源码

最新的稳定版本是3.9.4.
下载好之后解压到 /usr/src 文件夹下

```sh
sudo tar -xvf linux-3.9.4.tar.xz -C /usr/src/  
```

2.配置内核
将原来的配置文件拷过来

```sh
cp /usr/src/linux-headers-2.6.32-27-generic/.config .config  
```

首先进行一下配置，进入到 /usr/src//linux-3.9.4 文件夹下，执行

```sh
make menuconfig  
```

报错：
```sh
*** Unable to find the ncurses libraries or the
*** required header files.
```
缺少ncurses库（一个管理应用程序在字符终端显示的函数库），怒装之：

```sh
sudo apt-get install ncurses-dev  
```
重新执行make menuconfig<br>
选Load，然后Ok，然后Save。<br>



![](./images/20130530193054963)


3.编译和安装
终端执行：

```sh
make bzImage #编译kernel  
make modules #编译模块  
make modules_install  #先安装模块
make install #安装内核
```

编译的时间由机器性能决定。

make install之后，grub已经自动更新，不用再手动设置引导。
重启，进入ubuntu，更新后的第一次加载会有些慢。
再次查看内核版本，终端运行

```sh
uname -a  
```

![](./images/20130530193611812)


###添加系统调用

系统调用的原理如下：
![](./images/20130530200110780)


源码目录下涉及内核的三个文件有：

```sh
/kernel/sys.c                                         //定义系统调用
/arch/x86/syscalls/syscall_32.tbl      //设置系统调用号
/include/linux/syscalls.h                     //系统调用的头文件
```

下面来实现一个简单的系统调用。

1）系统调用函数的实现。
在/kernel/sys.c的最后添加下面的代码：

```c
asmlinkage int sys_callquansilang(int num)
{
	printk("Hi,I'm Quansilang. My student No. is sa*****310!");
	return 1;
}
```

2)设置系统调用号

编辑arch/x86/syscalls/syscall_32.tbl
文件，发现已经由350个定义号的系统调用，照葫芦画葫芦娃，在最后添加自己的系统调用：



```c
351 i386    callquansilang      sys_callquansilang  
```

注意要与之前定义的函数对应。

3）添加系统调用的声明到头文件

打开 /include/linux/syscalls.h ，倒数第二行添加

```c
asmlinkage int sys_callquansilang(int num); 
```
4）重新编译内核并安装。


```sh
sudo make mrproper  #清除久的编译文件
sudo make             #编译
```

安装：

```sh
make modules_install  #先安装模块
make install #安装内核
```
重启系统。

5）测试

创建一个main.c

```c
#include <unistd.h>
#include <stdio.h>
int main()
{
    syscall(351, 1);
    return 1;

}
```

编译运行，然后用

```c
sudo dmesg -c
```

查看系统调用log（写系统log会有一点延迟）。


![](./images/20130530215124845)


##参考
向linux内核添加系统调用新老内核比较 - http://www.cnblogs.com/albert1017/archive/2013/05/27/3101760.html
ubuntu 12.10 x64 下编译新内核 + 系统调用方法  -  http://blog.csdn.net/dslovemz/article/details/8744352
Linux添加内核系统调用报告 - http://blog.csdn.net/yming0221/article/details/6559767



