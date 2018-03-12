---
title: Linux Chroot
layout: post
comments: true
language: chinese
category: [linux,misc]
keywords: linux,chroot
description: chroot 也就是 change root directory，也就是改变程序执行时所参考的根目录位置。在 Linux 系统中，系统默认的目录结构都是以 "/"，即是以根 (root) 开始的，而在使用 chroot 之后，系统的目录结构将以指定的位置作为 "/" 位置。应用场景比较多，如安全中的蜜罐、系统启动时用的 initrd、嵌入式系统挂载的 NFS、Linux From Scratch 的编译过程等等。
---

chroot 也就是 change root directory，也就是改变程序执行时所参考的根目录位置。在 Linux 系统中，系统默认的目录结构都是以 "/"，即是以根 (root) 开始的，而在使用 chroot 之后，系统的目录结构将以指定的位置作为 "/" 位置。

应用场景比较多，如安全中的蜜罐、系统启动时用的 initrd、嵌入式系统挂载的 NFS、Linux From Scratch 的编译过程等等。

在此，简单介绍下 chroot 。

<!-- more -->

![chroot logo]({{ site.url }}/images/linux/chroot-logo.png "chroot logo"){: .pull-center }

## 简介

在经过 chroot 之后，系统读取到的目录和文件将不在是旧系统根下的而是新根下 (即被指定的新的位置) 的目录结构和文件，因此它带来的好处大致有以下的几个:

* 在经过 chroot 之后，在新根下将访问不到旧系统的根目录结构和文件，限制了用户的权力，这样就增强了系统的安全性。

* chroot 后，是一个与原系统根下文件不相关的目录结构。那么，在这个新的环境中，可以用来测试软件的静态编译以及一些与系统不相关的独立开发，例如 LFS 。

* 切换系统的根位置最明显的就是在系统启动时，从初始 RAM 磁盘 (initrd) 切换系统的根位置并执行真正的系统目录的 init；另外的就是急救系统。

在 Linux 中，实际上是有标准的目录结构的，其中包括了目录的命名、属性、权限等，可以参考 [Filesystem Hierarchy Standard, FHS](http://www.pathname.com/fhs/)。FHS 希望可以让使用者了解到已安装软件放置于那个目录下，所以希望软件开发商、操作系统制作者等都能够遵循 FHS 的标准。

上面扯的有点多，实际上构建一个 chroot 环境只需要一些简单的目录以及配置文件等，接下来看看如何构建一个 chroot 环境。



## 搭建环境

注意，我们构建的 chroot 环境是基于当前架构的，比如现在使用的是 X86 机器，那么搭建的运行环境就是基于 X86 机器的。如果想在 X86 上构建一个基于 NFS 的 ARM 测试环境，那么就要用到了交叉编译工具，重新编译所有的二进制文件，目前用的较多的是 busybox；在此就不过多讨论了。

当然，我们仅仅是搭建一个基本的运行环境，然后按需添加写可执行文件。因为架构是一样的，那么我们就可以直接在本机上复制所需要的文件，基本步骤大致如下：

1. 创建所必须的目录，如 /lib、/lib64、/bin、/sbin、/etc 等。
2. 复制基本的可执行文件，如 bash、env、ln、cat、ls、mv、cp、rm、mkdir 等。
3. 复制上述执行文件依赖的库文件，这一步可以自动执行。
4. 创建所必须的文件，以及环境变量，如 /dev/console、/etc/passwd、/etc/shadow 等。

上述的操作比较繁琐，我们直接通过一个简单的脚本 [chroot_create.sh](/reference/linux/chroot_create.sh) 完成上述的操作，需要注意的是，这个脚本只在 CentOS 上测试过，对于像 Debian、Gentoo 等其它的发布版本，可能需要修改。

上述的脚本运行时需要用 root 权限，相关的操作可以直接查看 --help 。

{% highlight text %}
-h, --help
    直接查看帮助
-c, --create
    先搭建环境，然后执行chroot命令；默认操作是直接执行chroot
--root
    指定根目录，默认是在当前目录下创建一个 root 目录
--verbose
    显示操作的详细信息，包括复制了那些文件等。
{% endhighlight %}

常见的命令如下：

{% highlight text %}
# chroot_create.sh                  # 在当前目录下创建root，并切换到该目录
# chroot_chreate.sh -h              # 查看帮助
# chroot_chreate.sh -v dir          # 指定root目录，显示详细信息，下面相同
# chroot_chreate.sh --root dir
# chroot_chreate.sh --root=dir
# chroot_chreate.sh -d dir          # 直接调转到相应的目录
{% endhighlight %}




在脚本的开头包括了一些用户可以修改的配置，如通过 cmdlist 添加执行的命令、user_process() 函数会在创建 root 环境时调用、before_enter_jail() 会在执行 chroot 命令前执行。

chroot 时可以执行命令，在此如果不使用 /usr/bin/env -i 则会将当前的环境便令传入 chroot 目录中，为了提供统一的环境，建议使用上述的参数。


### 调试

通常在打算包含一些命令时，会发现报错，那么可以将 strace 命令添加进去，对命令进行调试。例如，通过如下命令查看 sudo 的运行情况，根据其访问的文件来进行设置，可能需要复制或者创建某些文件。

{% highlight text %}
$ strace sudo
{% endhighlight %}

#### I have no name!

虽然添加了 /etc/passwd(644) /etc/shadow(644) 文件 (此时不需要 /etc/group) ，但是仍有上述的问题。通过 strace \`which whoami\` 可以发现，访问了 /etc/nsswitch.conf 文件，因此需要添加该配置文件。

如果只需要解决上述的问题，根据配置文件中的设置，也就是只设置了 file ，因此只需要添加 /lib/i386-linux-gnu/libnss_files.so.2 即可。

#### 添加 iptables

此时需要添加 /lib/xtables 目录下的内容，也可以根据需要添加，如对于双网卡的设置，只需要添加如下的动态链接库即可 libipt_MASQUERADE.so 。

#### 动态链接库

稍微再啰嗦下"动态链接库"，通过 strace 可以发现，很多命令都会查找动态链接库，通常会按照一定的目录顺序查找。为了加快查找速度可设置 LD_LIBRARY_PATH=/lib 或者使用 ldconfig 命令。

如果使用 ldconfig ，实际调用的是 ldconfig.real，在执行完 ldconfig 命令后，将会生成 /etc/ld.so.cache 文件。



## 源码解析

chroot 在 coreutils 中实现，实际是通过 chdir+chroot+execvp 实现，代码可以参考如下。

{% highlight c %}
int main (int argc, char **argv)
{
    ... ...

    if (chroot (argv[optind]) != 0)
        error (EXIT_CANCELED, errno, _("cannot change root directory to %s"), argv[optind]);

    if (chdir ("/"))
        error (EXIT_CANCELED, errno, _("cannot chdir to root directory"));
    ... ...

    execvp (argv[0], argv); /* Execute the given command.  */
    ... ...
}
{% endhighlight %}

chroot 在内核中的实现是 sys_chroot()，该函数实际上是设置当前进程的 current->fs->root 变量。之所以使用 chdir 是因为，很多系统在实现 chroot 时，并不会改变当前的路径，因此通常需要先通过 chdir 更改路径。

chroot 相关的内容也可以参考 [理解 chroot][understand-chroot]，在此就不过多介绍了。

### 风险

在使用 chroot 时，其中的一个用途是制作一个 Jail 做隔离，限制某些应用的权限，但是实际上如果使用不当，仍有可能突破该 Jail 。

突破的方法也很简单，就是循环利用 chdir("..") 即可，估计内核中的代码是没有做根路经的检查的。当然，解决办法也很简单，就是在 jail 中不要使用 root 用户，那么这样也不会造成太大的危害。

本来是有篇文章介绍的，现在已经成为了死链。


## 其它

在 chroot 环境中，如果要执行 ps 命令将会失败，需要挂载 proc 文件系统，不过会显示所有的进程，包括原用户的进程。

也就是说，采用 chroot 的方法只是对目录做了一些简单的隔离，而对于 CPU、磁盘、网络等资源，实际上是没有隔离的。目前比较流行的是采用类似 LXC 的解决方案，如 systemd ，可以参考 [systemd-nspawn 快速指南](https://linux.cn/article-4678-1.html) ，也就是 systemd 用以替换 chroot 的方案。

最后介绍一个比较奇葩的工具 supermin ，可以用来创建一个很小的内核镜像，不过是使用 OCaml 语言写的。



<!--
[chroot最佳实践](/reference/linux/container/chroot_best_practice.doc) 关于 chroot 的安全设置介绍，包括如何 break jail，可以参考 [How to break out of a chroot() jail](http://www.bpfh.net/simes/computing/chroot-break.html) 或者 [本地文档](/reference/linux/container/Breaking out of a chroot padded cell.mht) ，可将文件 [break_jail.c](/reference/linux/container/break_jail.c) 编译运行。
-->


[understand-chroot]:            https://www.ibm.com/developerworks/cn/linux/l-cn-chroot/            "理解 chroot"
[break-the-jail]:               http://www.bpfh.net/simes/computing/chroot-break.html
