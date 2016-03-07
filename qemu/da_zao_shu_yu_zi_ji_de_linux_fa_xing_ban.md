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

![](./images/20130616201444750)


## 整合busybox

busybox简介
BusyBox 是一个集成了一百多个最常用linux命令和工具的软件。BusyBox 包含了一些简单的工具，例如ls、cat和echo等等，还包含了一些更大、更复杂的工具，例如grep、find、mount以及telnet。有些人将 BusyBox 称为 Linux 工具里的瑞士军刀。简单的说BusyBox就好像是个大工具箱，它集成压缩了 Linux 的许多工具和命令，也包含了 Android 系统的自带的shell。
BusyBox 将许多具有共性的小版本的UNIX工具结合到一个单一的可执行文件。这样的集合可以替代大部分常用工具比如的GNU fileutils ， shellutils等工具，BusyBox提供了一个比较完善的环境，可以适用于任何小的或嵌入式系统。

下载源码：http://www.busybox.net/
这里选择1.20稳定版。
解压，终端进入目录执行：
```sh
make menuconfig
```

勾选下面的选项：
```sh
Build Options
Build BusyBox as a static binary (no shared libs)
```

![](./images/20130615122706234)

这个选项是一定要选择的,这样才能把busybox编译成静态链接的可执行文件,运行时才独立于其他函数库.否则必需要其他库文件才能运行,在单一个linux内核不能使他正常工作.

现在直接make的话会报错：`‘RLIMIT_FSIZE’ undeclared`
论坛上的回答是没有包含  sys/resource.h，则在include/libbb.h 中添加：

```c
#include <sys/resource.h>  
```

接下来执行：
```sh
#编译busybox
make 
#安装busybox
make install 
```
安装好之后在文件夹下出现一个_install文件夹，编译完成。

![](./images/20130615124957765)

下面来整合根文件系统。
新建一个文件夹，终端cd进去，将之前的initrd.img拷贝进来。
```sh
#创建文件夹
mkdir rootfs
#挂载镜像
sudo mount -o loop initrd.img rootfs/ 
#将busybox添加进来
cd ../busybox-1.20.2/
sudo make CONFIG_PREFIX=../Opuntu/rootfs/ install
#查看rootfs中结构
cd ../Opuntu
ls rootfs
 #卸载分区
sudo umount rootfs/
```

`运行命令时注意目录结构！`<br>
linux的系统下的目录都是干嘛的？<br>
linux下的文件结构，看看每个文件夹都是干吗用的

```sh
/bin 二进制可执行命令 
/dev 设备特殊文件 
/etc 系统管理和配置文件 
/etc/rc.d 启动的配置文件和脚本 
/home 用户主目录的基点，比如用户user的主目录就是/home/user，可以用~user表示 
/lib 标准程序设计库，又叫动态链接共享库，作用类似windows里的.dll文件 
/sbin 系统管理命令，这里存放的是系统管理员使用的管理程序 
/tmp 公用的临时文件存储点 
/root 系统管理员的主目录（呵呵，特权阶级） 
/mnt 系统提供这个目录是让用户临时挂载其他的文件系统。 
...
```
##qemu测试
```sh
qemu -kernel ../linux-3.9.2/arch/x86/boot/bzImage -initrd initrd.img -append "root=/dev/ram init=/bin/sh"
```

装载好之后可以在qemu中运行busybox的命令，效果如下：

![](./images/20130616205424343)

##整合grub
关于Grub
GNU GRUB（简称“GRUB”）是一个来自GNU项目的多操作系统启动程序。GRUB是多启动规范的实现，它允许用户可以在计算机内同时拥有多个操作系统，并在计算机启动时选择希望运行的操作系统。GRUB可用于选择操作系统分区上的不同内核，也可用于向这些内核传递启动参数。
首先来测试一下grub.
从ftp://alpha.gnu.org/gnu/grub/下载GRUB Legacy的最后一个版本0.97的编译好的文件grub-0.97-i386-pc.tar.gz.
解压之后中端cd进去，执行下面的命令。

```sh
#建立软盘映像：
dd if=/dev/zero of=boot.img bs=512 count=2880
#在boot.img中安装grub：
sudo losetup /dev/loop0 boot.img
sudo dd if=./grub-0.97-i386-pc/boot/grub/stage1 of=/dev/loop0 bs=512 count=1
sudo dd if=./grub-0.97-i386-pc/boot/grub/stage2 of=/dev/loop0 bs=512 seek=1
sudo losetup -d /dev/loop0
```

在qemu中测试是否可以进入grub

```sh
qemu -fda boot.img
```
![](./images/20130616110258062)
OK.
接下来将grub，kernel，busybox一起整合，离胜利只还有一步 - - 。
先你自己发行版取一个库一些的名字，比如Opuntu...(Opensuse+ubuntu),创建一个以它命名的文件夹。

```sh
#拷贝boot.img到当前目录
sudo cp ../grub-0.97-i386-pc/boot.img ./
#创建rootfs文件夹
mkdir rootfs
#创建一个32M的磁盘镜像文件
dd if=/dev/zero of=/dev/zero of=Opuntu.img bs=4096 count=8192
#给磁盘映像分区
fdisk -C 16065 -H 255 -S 63 Opuntu.img
```

解释：设置Opuntu.img的磁头数为255、磁道数为16065、扇区数为63，同时给磁盘分区。
这里我们只分一个区，并设置该分区为引导分区。示意图如下：

![](./images/20130616183140578)
```sh
#格式化分区
sudo losetup -o 1048576 /dev/loop0 Opuntu.img
sudo mkfs.ext3 -m 0 /dev/loop0
```
解释：我们把前面的2048个扇区（0~2047）作为引导扇区使用，格式化分区从第2048个扇区开始，所以1048576=2048*512

```sh
#拷贝之前做好的initrd.img和bzImage.img到rootfs
sudo mount /dev/loop0 rootfs/
sudo cp ../linux-3.9.2/arch/x86/boot/bzImage ./rootfs/
sudo cp ../rootfs/initrd.img ./rootfs/
```

```sh
#添加grub
sudo mkdir rootfs/boot
sudo mkdir rootfs/boot/grub
sudo cp ../grub-0.97-i386-pc/boot/grub/* ./rootfs/boot/grub
sudo vi ./rootfs/boot/grub/menu.lst
```

内容：

```sh
default 0
timeout 30
title linux on 32M.img
root (hd0,0)
kernel (hd0,0)/bzImage root=/dev/ram init=/bin/ash
initrd (hd0,0)/initrd.img
```

```sh
#卸载磁盘镜像
sudo umount rootfs
sudo losetup -d /dev/loop0
```

```sh
#利用grub启动软盘，在硬盘映像上添加grub功能
qemu -boot a -fda boot.img -hda Opuntu.img
```

执行图中的两步（注意空格）：

![](./images/20130616213616781)


运行成功之后，Opuntu.img就是我们的最终成果了，集成了busybox,grub,linux kernel3.92!

```sh
qemu -hda Opuntu.img
```

用qemu跑起来：

![](./images/20130616214002343)
![](./images/20130616214027078)

##参考：
鸟哥私房菜 第二十二章：开关机流程与loader
制作可用grub引导Linux系统的磁盘映像文件 - http://blog.sina.com.cn/s/blog_70dd169101013gcw.html

详细讲解Linux启动流程及启动用到的配置文件及脚本 - http://guodayong.blog.51cto.com/263451/1168731

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



