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


