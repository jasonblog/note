---
title: LXC 简介
layout: post
comments: true
language: chinese
category: [linux,misc]
keywords: linux,container,lxc,容器
description: LXC (Linux Containters) 是一种基于内核容器的用户空间接口，提供了一系列创建、配置、管理的接口。其提供了比 chroot 更强的资源隔离，但是与硬件提供的完全虚拟化又不同，其目标为创建一个不需要独立内核，但近可能接近标准 Linux 安装的环境。在此简单介绍其使用方法，以及常用的命令。
---

LXC (Linux Containters) 是一种基于内核容器的用户空间接口，提供了一系列创建、配置、管理的接口。

其提供了比 chroot 更强的资源隔离，但是与硬件提供的完全虚拟化又不同，其目标为创建一个不需要独立内核，但近可能接近标准 Linux 安装的环境。

在此简单介绍其使用方法，以及常用的命令。

<!-- more -->

![linux container logo]({{ site.url }}/images/linux/container/containers-logo.png "linux container logo"){: .pull-center}

## 简介

首先大致介绍一下 LXC 是如何做到的。简单来说就是，一台机器上跑了一个 Linux 内核，然后通过 LXC 跑了 N 个不同的 Linux 发行版本的用户程序，包括 init 。

所谓的 Linux 发行版本，如 RedHat、Ubuntu、CentOS、Debian、Fedora 等等，其实际上都是基于 Linux 内核，有可能打了不同的 patch，但真正不同的只是用户空间程序，如 init 用的是 systemV、upstart 还是饱受争议的 systemd，图形界面用的是 Gnome、KDE、LXDE、Xfce 甚至是 FCWM。

这也要多亏了所谓的 Linux 哲学：提供机制，而非策略（说实话没有找到对应的英语原话）。

### 安装 lxc

在 CentOS 中，如果通过 YUM 安装，则依赖 EPEL 库，可以通过如下方法安装。

{% highlight text %}
# yum install libcgroup lxc lxc-libs lxc-templates --enablerepo=epel
{% endhighlight %}

在 ```/usr/share/lxc/templates``` 目录下保存了常用的模板，可以用非 root 创建容器，当然这会更安全，不过会不方便，如不能挂载文件系统、不能创建设备节点等，可以参考 [Creating unprivileged containers as a user](https://linuxcontainers.org/lxc/getting-started/)，后面再研究，暂时还是用 root 用户。

#### 源码编译

对于源码编译的 lxc，其目录中的内容类似，只是默认会添加 ```/usr/local``` 的 prefix，可以如下使用 ```/usr``` 的 prefix，此时下面讨论的内容会添加 /usr 前缀。

{% highlight text %}
$ ./configure --prefix=/usr && make
# make install
{% endhighlight %}

#### 检查

编译安装完成之后，可以通过如下命令检查环境。

{% highlight text %}
$ lxc-checkconfig
{% endhighlight %}

### 配置文件

通过 yum 安装的 lxc ，常见的配置文件以及目录如下。

* /etc/lxc/default.conf<br>LXC 的默认配置文件，如果在创建容器时不指定配置文件，将默认使用这个配置文件，主要包括了对网络以及命名空间的设置；其它一些配置文件保存在 /usr/share/lxc/config 目录下。

* /var/lib/lxc/<br>保存了容器的相关配置，包括已经创建容器的根文件系统 (rootfs)、配置文件 (config)。

* /usr/share/lxc/templates/<br>保存了当前 LXC 支持的各种发行版的 linux 的模板配置文件，如 lxc-ubuntu、lxc-fedora、lxc-busybox、lxc-sshd 等。

* /var/log/lxc<br>日志默认保存在该目录下，容器名.log 。

注意，在创建一个类似 ubuntu、fedora 相关的容器时，通常会在 ```/var/cache/lxc/``` 目录下保存一份，那么第二次创建时，就不需要再次下载。


## 使用 LXC

在此简单介绍如何创建并使用 LXC，以 CentOS-7 为例，并简单介绍创建 CentOS 的过程。注意，如果安装 ubuntu 镜像，需要安装 debootstrap (用于安装 rootfs) 。

通过 LXC 可以启动一个 Linux 发布版本的镜像，也可以只启动一个进程，下面仍以 root 创建一个 CentOS 7 镜像为例。

### 1. 创建镜像

可以根据现有的模板创建，可以参考 ```/usr/share/lxc/templates/lxc-centos``` ，该文件实际就是一个 bash 脚本，用来创建 rootfs 。

{% highlight text %}
----- 根据模板创建镜像，--后为传入脚本的参数
# lxc-create -t centos -n centos -- --release 7

----- 查看已经安装的rootfs、config等文件，config文件将在lxc-start中使用
# ls /var/lib/lxc/centos/
config  rootfs/  tmp_root_pass

----- 或者通过该命令查看当前已经安装的镜像
# lxc-ls -f
centos

----- 查看设置的密码，创建后会打印该提示
# cat /var/lib/lxc/centos/tmp_root_pass

----- 直接通过chroot修改密码
# chroot /var/lib/lxc/centos/rootfs passwd
{% endhighlight %}

在 ```/var/cache/lxc/centos``` 目录下，会保存临时文件，这样创建时只会在第一次下载所需的软件包，后面如果版本比较旧可能还会 update 。

### 2. 启动容器

启动时，如果不指定 ```-d``` 后台运行，则会将当前的 bash 作为 console 输出，此时会输出容器的启动信息，然后直接进入登陆界面。

{% highlight text %}
----- 后台启动容器，并设置日志级别为DEBUG，日志可以查看/var/log/lxc/centos.log
# lxc-start -n centos -l DEBUG -d

----- 查看容器启动的init对应的PID
# ps faux | grep -A 1 "lxc-start -n centos -d"
{% endhighlight %}

是不是发现启动很快！！！

实际上很大一部分时间是消耗在网络设置上，如果取消网络你会发现启动速度更快。


### 3. 查看状态

容器启动之后实际上是一直有一个进程等待接收命令等操作的，对应的容器实际上是一个子进程。

{% highlight text %}
----- 查看容器的运行状态，包括了其PID、运行状态、IP等信息
# lxc-info -n centos
Name:           centos
State:          RUNNING
PID:            31649                # init线程的PID
IP:             192.168.122.118      # 容器中的IP
CPU use:        0.15 seconds
BlkIO use:      0 bytes
Memory use:     1.18 MiB
KMem use:       0 bytes
Link:           vethXTBOA0           # 容器外的对端网络设备
 TX bytes:      1.76 KiB
 RX bytes:      12.95 KiB
 Total bytes:   14.71 KiB

----- 以行显示当前运行状态
# lxc-ls -f
NAME   STATE   AUTOSTART GROUPS IPV4 IPV6
centos STOPPED 0         -      -    -

----- 查看启动的父进程PID
# ps aux | grep 'lxc-start -n centos' | grep -v grep
root  31639  0.0  0.0  30560  1640 pts/9  S+ 12:27 0:00 lxc-start -n centos

----- OK，查看下父进程派生的子进程
# pstree -p 31639
lxc-start(31639)──systemd(31649)─┬─dbus-daemon(31707)
                                 ├─dhclient(31901)
                                 ├─login(31714)───bash(31958)
                                 ├─rsyslogd(31711)─┬─{rsyslogd}(31718)
                                 │                 └─{rsyslogd}(31719)
                                 ├─sshd(31852)
                                 ├─systemd-journal(31665)
                                 └─systemd-logind(31712)

----- 直接查看容器的ip，实际就是在容器中执行命令
# lxc-attach -n centos -- /sbin/ip address list
{% endhighlight %}

### 4. 连接到容器

如果在启动的时候没有使用 ```-d``` 选项，则默认使用当前的 bash 作为 console ，可以直接在启动后登陆。

{% highlight text %}
----- 通过lxc的命令链接上去
# lxc-attach -n centos

----- 目前链接时还有点问题，直接卡着不动
# lxc-console -n centos

----- 使用util-linux包提供的命令进入设置的namepace空间，需要PID
# nsenter --target PID --mount --uts --ipc --net --pid --root --wd
{% endhighlight %}

注意，attach 只有 3.8 之后的内核才会支持，对于低版本需要通过其它的方法解决。

### 5. 关闭容器

可以在容器中内执行 ```poweroff``` 或 ```shutdown -h now``` 来关闭容器，可以通过如下命令强行关闭，不过此时会花费比较长的时间，实际上就是正常的关闭流程。

{% highlight text %}
# lxc-stop -n centos
{% endhighlight %}

### 6. 销毁容器

如果不需要了，可以通过如下方式删除，包括了 rootfs 等配置，所以 "慎用" 。

{% highlight text %}
# lxc-destroy -n centos
{% endhighlight %}

### 7. 其它常见命令

简单介绍经常会用到的命令。

{% highlight text %}
----- 创建，默认使用/etc/lxc/default.conf，可以通过-f指定配置文件
# lxc-create -n NAME -f FILE

----- 用户获取一个容器的状态，容器的状态有STARTING RUNNING STOPPING STOPPED ABORTING
# lxc-info -n NAME

----- 监控一个容器状态的变换，当一个容器的状态变化时，此命令会在屏幕上打印出容器的状态
# lxc-monitor -n NAME

----- 列出当前系统所有的容器
# lxc-ls

----- 列出特定容器中运行的进程
# lxc-ps -n NAME

----- 用于销毁容器
# lxc-destroy -n NAME
{% endhighlight %}

另外，需要注意的是：通过 ```lxc-create``` 创建后的容器，在停止运行后，不会立即被销毁，要采用 ```lxc-destroy``` 命令才能销毁；容器命令空间是全局的，系统中不允许存在重名的容器，否则会失败。

### 安装 Ubuntu 镜像

只是简单介绍一下如何创建 Ubuntu 的镜像，与镜像相关的大部分的操作与创建 CentOS-7 相同，只是在创建 rootfs 时有些区别。

在此，主要介绍其创建方法，安装 rootfs 时，可能会由于镜像无法访问导致创建失败，可以将 Ubuntu 访问的网站在 ```/usr/share/lxc/templates/lxc-ubuntu``` 开始设置 MIRROR 变量。

{% highlight bash %}
MIRROR=${MIRROR:-http://mirrors.163.com/ubuntu/}
{% endhighlight %}

然后通过如下命令创建 rootfs 。

{% highlight text %}
# lxc-create -t ubuntu -n ubuntu -- -r precise
{% endhighlight %}

实际上，上述的脚本是通过 debootstrap 创建一个 rootfs，也可以通过如下命令自己创建。

{% highlight text %}
# debootstrap --arch amd64 precise ubuntu-precise http://mirrors.163.com/ubuntu/
{% endhighlight %}

## 资源设置

LXC 是通过 cgroup 进行资源管理的，也就是说这部分的设置，都可以通过 lxc-cgroup 命令进行动态调整。在使用前，需要通过如下方式挂载。

{% highlight text %}
----- 挂载cgroup
# mount none -t cgroup /cgroup

----- 让系统每次启动自动挂载cgroup
# echo "none /cgroup cgroup defaults 0 0" >> /etc/fstab
{% endhighlight %}

LXC 在创建容器的时候就在 /group 下创建一个子 group 以实现对容器的资源控制，可以根据需要设定相应子系统的参数来达到目的。

### CPU

对于 CPU 资源配置，主要通过两个变量 cpu.shares、cpuset.cpus 进行设置。

cpu.shares 是一个按照比例计算的 cpu 使用份额，例如，只存在两个 lxc 的 cpu.shares 都是 1024，那么这两个 lxc 使用 cpu 的理论比例就是 50%:50%。

cpuset.cpus 是分配给这个 lxc 使用的具体 cpu，编号从 0 开始；可以通过如下方式进行分配：

{% highlight text %}
# lxc-cgroup -n name cpu.shares 1024
# lxc-cgroup -n name cpuset.cpus 0-1
{% endhighlight %}

另外一个相关的参数是 cpuset.mems，与 cpus 类似，主要用于 NUMA 类型的机器。


### Memory

内存的设置比较简单，主要是关于物理内存以及 swap 。

memory.limit_in_bytes 用户内存的最大量，包括了文件缓存，默认单位是字节。如果 swap 没有关闭的话，即使超过了该限制，仍可能转储到 swap 空间，可以通过 swapoff 关闭。

memory.memsw.limit_in_bytes 设定最大内存与 swap 用量之和，同样默认单位是字节。设置该项前，同样要设置上述的值，而且不能大于该值。

### blkio

也就是用来设置块设备的 IO 访问。

<!--
blkio.weight
指定 cgroup 默认可用访问块 I/O 的相对比例（加权），范围在 100 到 1000。这个值可由具体设备的 blkio.weight_device 参数覆盖。此数值跟cpu.share类似，以访问时间的比例的形式起作用。默认根group的值为1000.如果再根下面建立两group，分别设置weight为500和1000，那么IO时间比值为1:2.更多的说明可以参考内核文档。

 blkio.weight_device
指定对 cgroup 中可用的具体设备 I/O 访问的相对比例（加权），范围是 100 到 1000。这个值的格式为major:minor weight，其中 major 和
minor 是在《Linux 分配的设备》中指定的设备类型和节点数，我们也称之为 Linux 设备列表，您
可以参考 http://www.kernel.org/doc/Documentation/devices.txt

blkio.throttle.read_bps_device

针对特定的 major:minor 设备的读bps的上限限制（bps指byte per second，即每秒的byte数)

blkio.throttle.write_bps_device

针对特定的 major:minor 设备的写bps的上限限制

 blkio.throttle.read_iops_device

针对特定的 major:minor 设备的读ios的上限限制(ios指io per second,即每秒的io次数)

blkio.throttle.write_iops_device

针对特定的 major:minor 设备的写ios的上限限制

此四个参数可以同blkio.weight blkio.weight_device以前使用，系统将同时满足着些限制。



2.network部分

network部分的配置均以lxc.network开头.

lxc.network.type指定用于容器的网络类型：

empty：新的网络空间会为容器建立，但是没有任何网络接口

veth：新的网络空间会为容器建立，容器将链接到lxc.network.link定义的网桥上，从而与外部通信。网桥必须在预先建立。

macvlan：新的网络空间会为容器建立，一个macvlan的接口会链接到lxc.network.link上。

phys：新的网络空间会被建立，然后lxc.network.link指定的物理接口会被分配给容器

lxc.network.flags用于指定网络的状态

up：网络处于可用状态

lxc.network.link用于指定用于和容器接口通信的真实接口，比如一个网桥br0

lxc.network.name用于指定容器的网络接口的名字，与宿主接口不在一个命名空间。如果不指定，lxc为自动分配一个

lxc.network.hwaddr用于指定容器的网络接口的mac地址

lxc.network.ipv4用于指定容器的网络接口的ipv4地址，如果不设置或者设为0.0.0.0则表示，容器将通过dhcp的方式获得ip地址

lxc.network.ipv6用于指定容器的网络接口的ipv6地址
3.rootfs部分

lxc.rootfs用于指定容器的虚拟根目录，设定此项以后，容器内所有进程将会把此目录根目录，不能访问此目录之外的路径，相当于chroot的效果
-->

## 配置文件

LXC 配置项都是以 ```key=value``` 的形式设置，其中以 # 开始的一行为注释；当然，配置也可以在 ```lxc-execute``` 或者 ```lxc-start``` 的命令行以 ```-s key=value``` 设定；如下列举出常见的一些配置。

### 日志设置

容器的启动日志默认只收集 ERROR 级别的信息，默认保存在 ```/var/log/lxc``` 目录下，以容器名开头，以 ```.log``` 结尾的文件中，日志级别和日志路径都是可以在配置文件中修改。

{% highlight text %}
lxc.loglevel=               // 级别，value是0到8的整数，0-trace, 1-debug, 2-info, 3-notice, 4-warn,
                            //         5-error, 6-critical, 7-alert, 8-fatal.
lxc.logfile=                // 路径
{% endhighlight %}

### cgroup 设置

cgroup 部分通常以 ```lxc.cgroup.[subsystem name].key=value``` 的形式进行设置。

### 安全

如果以 root 启动，可以通过如下参数取消其中的部分功能，以 ```CAP_SYS_MODULE``` 为例，实际设置应为该 ```sys_module```，其它详细的参数可以参考 ```man 7 capabilities``` 。

{% highlight text %}
lxc.cap.drop = sys_module mac_admin
{% endhighlight %}

<!--
lxc.mount # fstab文件路径
lxc.mount.entry # fstab文件内容直接写于此，替代lxc.mount指定文件路径方式
-->

## 常用命令

简单列举一下经常使用的命令，想到那写到那，备忘。

### 查看当前容器

可以通过 lxc-ls 命令查看，容器的状态，默认查看的是已经创建的容器（可能没有启动）。

{% highlight text %}
----- 查看当前已经创建的容器
# lxc-ls
{% endhighlight %}

### 查看 LXC 相关的配置

通过如下命令可以查看 LXC 的系统配置信息，主要有两类参数。

{% highlight text %}
----- 查看当前LXC的系统配置
# lxc-config -l
lxc.default_config
lxc.lxcpath
... ...

----- 查看其中的配置项
# lxc-config lxc.lxcpath
{% endhighlight %}

## 参考

官方网站 [linuxcontainers.org](https://linuxcontainers.org/) 。

<!--
用 cgroups 管理 cpu 资源
http://xiezhenye.com/2013/10/%E7%94%A8-cgroups-%E7%AE%A1%E7%90%86-cpu-%E8%B5%84%E6%BA%90.html
-->


{% highlight text %}
{% endhighlight %}
