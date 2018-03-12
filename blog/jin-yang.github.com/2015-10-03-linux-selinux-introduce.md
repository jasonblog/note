---
title: SELinux 简介
layout: post
comments: true
language: chinese
category: [linux,misc]
keywords: linux,selinux
description: SELinux 给 Linux 带来的最重要价值是：提供了一个灵活的，可配置的 MAC 机制。包括了内核中的模块，以及用户态的工具，对于用户来说是透明的，只有同时满足了 "标准 Linux 访问控制" 和 "SELinux 访问控制" 时，主体才能访问客体。
---

SELinux 给 Linux 带来的最重要价值是：提供了一个灵活的，可配置的 MAC 机制。包括了内核中的模块，以及用户态的工具，对于用户来说是透明的，只有同时满足了 "标准 Linux 访问控制" 和 "SELinux 访问控制" 时，主体才能访问客体。

<!-- more -->

## SELinux (Security-Enhanced Linux)

SELinux 是一种强制存取控制 (Mandatory Access Control, MAC) 的实现，它的作法是以最小权限原则 (Principle of least privilege) 为基础。最初由美国国家安全局 (Natinal Security Agency) 设计，可以直接参考 [www.nsa.gov](https://www.nsa.gov/research/selinux/) 。

最初的 Linux 采用的是自主式存取控制 (Discretionary Access Control, DAC)，基本上就是根据程序的拥有者与文件资源的 rwx 权限来判断存取的能力。这种策略存在着一些问题，如 root 可以读取所有的文档；如果将权限误设置为 777，那么所有的用户都可以读取该文件了。

而 MAC 可以依据条件决定是否有存取权限，可以规范个别细致的项目进行存取控制，提供完整的彻底化规范限制。可以对文件，目录，网络，套接字等进行规范，所有动作必须先得到 DAC 授权，然后得到 MAC 授权才可以存取。

### 运行机制

在 SELinux 中，访问控制属性叫做安全上下文。所有客体（文件、进程间通讯通道、套接字、网络主机等）和主体（进程）都有与其关联的安全上下文，一个安全上下文由三部分组成：用户、角色和类型标识符。

![selinux decision process]({{ site.url }}/images/linux/selinux-decision-process.png){: .pull-center}

当一个 subject (如一个应用) 试图访问一个 object (如一个文件) 时，在 Kernel 中的策略执行服务器将检查 AVC (Access Vector Cache)。如果基于 AVC 中的数据不能做出决定，则请求安全服务器，根据查询结果允许或拒绝访问。

Selinux总共分为三个等级，

* Enforcing<br>默认模式会在系统上启用并实施 SELinux 的安全性政策，包括拒绝访问及记录行动。

* Permissive<br>启用但不会实施安全性政策，而只会发出警告及记录行动，在排除错误时比较有用。

* Disabled<br>已被停用。

SElinux 主要是用来管理程序，因此其主体 (Subject) 等价于程序；目标 (Object) 主要是指文件系统；政策 (Policy) 设置的基本的安全存储策略。

### 配置文件

SELinux 配置文件以及策略文件都位于 ```/etc/selinux``` 目录下，相关的策略可以通过如下方式进行设置。

{% highlight text %}
----- 列出SELinux的所有布尔值
# getsebool -a
----- 设置SELinux布尔值，-P表示持久化，即使reboot之后，仍然有效
# setsebool -P dhcpd_disable_trans=0
{% endhighlight %}

### 查看

通过 ```-Z``` 选项可以查看策略，如 ```ls -Z```、```ps -Z```、```id -Z``` 等，除了常规的参数之外还会有 Security Context ，由 (identify:role:type) 三部分组成；安全性本文 (Security Context) 保存在 inode 中。

![selinux context]({{ site.url }}/images/linux/selinux-context.png){: .pull-center}

字段的含义如下：

* user<br>用户区域，指的是 selinux 环境下的用户，与登陆用户的影射关系可通过 ```semanage login -l``` 查看，常有 user_u(登陆用户)；system_u(Linux启动过程中默认值)；root(也就是root用户)。

* role<br>通常由于安全类型的分组，常见的有object_r(通常为文件，实际只是一个占位符)。

* type<br>与role类型实际决定了那些角色可以执行那些类型，这两种类型实际决定了那些。

* level<br>控制级别，Multi-Category Security(MCS)。

### 常见操作

SELinux 缺省会通过 Linux 审计系统 auditd 将日志写在 ```/var/log/audit/audit.log``` 内。

{% highlight text %}
----- 查看SELinux状态
# sestatus -v                 # 如果SELinux status参数为enabled即为开启状态
# getenforce                  # 也可以用这个命令检查

----- 关闭SELinux
# setenforce 0                # 设置SELinux 成为permissive(0)或者enforcing(1)模式

$ cat /etc/selinux/config     # 可以直接设置配置文件
SELINUX=enforcing/disabled
{% endhighlight %}

当由 Diabled 切换至 Permissive 或 Enforcing 模式时，最好重启，重新标签文件系统。SELinux 包含了很多策略软件包 (policy package)，后缀名为 .pp，一般保存在 ```/etc/selinux``` 目录下，可以通过 find 命令查找。

出现权限问题后可以通过 setroubleshoot 工具包。

{% highlight text %}
$ echo > /var/log/audit/audit.log                        # 清空，防止有过多的信息
$ sealert -a /var/log/audit/audit.log > /tmp/result.txt  # 输出信息
{% endhighlight %}

其中的结果包括了建议执行的命令。


## 配置实例

### Apache 服务标准配置修改

主要查看下如何通过 SELinux 修改 Apache 的权限配置。

#### 1. 让 Apache 可以访问非默认目录

首先，获取默认 ```/var/www``` 目录的 SELinux 上下文。

{% highlight text %}
# semanage fcontext -l | grep '/var/www'
/var/www(/.*)?          all     files   system_u:object_r:httpd_sys_content_t:s0
{% endhighlight %}

也就是 Apache 只能访问包含 ```httpd_sys_content_t``` 标签的文件，假设希望其使用 ```/opt/www``` 作为网站文件目录，就需要给这个目录下的文件增加 ```httpd_sys_content_t``` 标签，分两步实现。

{% highlight text %}
----- 1. 为/opt/www目录下的文件添加默认标签类型
# semanage fcontext -a -t httpd_sys_content_t '/srv/www(/.*)?'

----- 2. 用新的标签类型标注已有文件
# restorecon -Rv /srv/www
{% endhighlight %}

其中 restorecon 命令用于恢复文件默认标签，比较常用，比如从用户主目录下将某个文件复制到 Apache 网站目录下，Apache 默认是无法访问，因为用户主目录的下的文件标签是 ```user_home_t```，此时就需要 restorecon 将其恢复为可被 Apache 访问的 ```httpd_sys_content_t``` 类型。

#### 2. 让 Apache 侦听非标准端口

默认情况下 Apache 只侦听 80 和 443 两个端口，若是直接指定其侦听 888 端口的话，会在通过 systemclt 启动或者重起时报 Permission denied 错误。

这个时候，若是在桌面环境下 SELinux 故障排除工具应该已经弹出来报错了。若是在终端下，可以通过查看 ```/var/log/{messages, audit/audit.log}``` 日志，用 ```sealert -l``` 加编号的方式查看，或者直接使用 ```sealert -b``` 浏览。

<!--
可以看出 SELinux 根据三种不同情况分别给出了对应的解决方法。在这里，第一种情况是我们想要的，于是按照其建议输入：

semanage port -a -t http_port_t -p tcp 888

之后再次启动 Apache 服务就不会有问题了。

这里又可以见到 semanage 这个 SELinux 管理配置工具。它第一个选项代表要更改的类型，然后紧跟所要进行操作。详细内容参考 Man 手册
-->

#### 3. 允许 Apache 访问创建私人网站

若是希望用户可以通过在 ```~/public_html/``` 放置文件的方式创建自己的个人网站的话，那么需要在 Apache 策略中允许该操作执行。使用：

{% highlight text %}
# setsebool httpd_enable_homedirs 1
{% endhighlight %}

setsebool 用来切换由布尔值控制的 SELinux 策略的，当前布尔值策略的状态可以通过 getsebool 来获知。

<!--
默认情况下 setsebool 的设置只保留到下一次重启之前，若是想永久生效的话，需要添加 -P 参数，比如：
setsebool -P httpd_enable_homedirs 1
-->

## capabilities

通常 capabilities 是和 SELinux 配合使用的，以往，如果要运行一个需要 root 权限的程序，那么需要保证有运行权限，且是 root 用户；通过 capabilities 就可以只赋予程序所需要的权限。

以 ping 命令为例，因为需要发送 raw 格式数据，部分发行版本使用了 ```setuid + root```，实际上只需要赋予 ```CAP_NET_RAW``` 权限，然后去除 setuid 即可。

{% highlight text %}
----- 直接复制一个ping命令，然后进行测试
# cp ping anotherping
# chcon -t ping_exec_t anotherping
$ ping -c 1 127.0.0.1
PING 127.0.0.1 (127.0.0.1) 56(84) bytes of data.
64 bytes from 127.0.0.1: icmp_seq=1 ttl=64 time=0.057 ms
$ anotherping -c 1 127.0.0.1
ping: icmp open socket: Operation not permitted

----- 新增CAP_NET_RAW权限，然后用非root用户重新测试
# setcap cap_net_raw+ep anotherping
$ anotherping -c 1 127.0.0.1
PING 127.0.0.1 (127.0.0.1) 56(84) bytes of data.
64 bytes from 127.0.0.1: icmp_seq=1 ttl=64 time=0.054 ms
{% endhighlight %}

## 参考


<!--
chcon 用于修改文件的 SELinux 安全上下文，或者说是修改安全策略。

进程所属的上下文类型称为域，而文件所属的上下文类型称为类型，在 SELinux 开启后，进程只能操作域类型与上下文类型一样的文件。

在 CentOS 中，semanage 命令默认没有安装，可以通过 ```yum install policycoreutils-python``` 安装。

----- 查看哪些服务受SELinux管理，也就是布尔值；包括了如何设置，-P永久生效
# getsebool -a
# setsebool samba_enable_home_dirs=1
# setsebool samba_enable_home_dirs=on
----- 恢复默认上下文
# restorecon /var/www/html
----- 默认上下文类型？保存在那个文件中
# semanage fcontext -l
----- 用户映射关系
# semanage user -l
----- 查看受SELinux控制的端口，以及将指定端口添加到规则中
# semanage port -l
# semanage port -a -t http_port_t -p tcp 8060

chcon [选项]... 环境 文件...
chcon [选项]... [-u 用户] [-r 角色] [-l 范围] [-t 类型] 文件...
chcon [选项]... --reference=参考文件 文件...

二、chcon命令参数：
参数名 描述
-u 用户
--user=用户 设置指定用户的目标安全环境；
 -r 角色
--role=角色 设置指定角色的目标安全环境；
-t 类型
--type=类型 设置指定类型的目标安全环境；
-l 范围
--range=范围 设置指定范围的目标安全环境；
-v
--verbose 处理的所有文件时显示诊断信息；

-R/--recursive
  递归处理目录、文件；


-h
--no-dereference 影响符号连接而非引用的文件；
--reference=file 使用指定参考文件file的安全环境，而非指定值；
-H  如果一个命令行参数是一个目录的符号链接，则遍历它；
-L 遍历每一个符号连接指向的目录；
-P 不遍历任何符号链接（默认）；
--help 显示此帮助信息并退出；
--version 显示版本信息并退出；

三、chcon用法演示：
1、mysql：让selinux

允许mysqld做为数据目录访问“/storage/mysql”：
-----
# chcon -R -t mysqld_db_t /storage/mysql
2、ftp：将共享目录开放给匿名用户：
[root@aiezu.com ~]# chcon -R -t public_content_t /storage/ftp/
#允许匿名用户上传文件：
[root@aiezu.com ~]# chcon -R -t public_content_rw_t /storage/ftp
[root@aiezu.com ~]# setsebool -P allow_ftpd_anon_write=1

3、为网站目录开放httpd访问权限：
chcon -R -t httpd_sys_content_t /storage/web

https://access.redhat.com/documentation/en-US/Red_Hat_Enterprise_Linux/6/html/Security-Enhanced_Linux/chap-Security-Enhanced_Linux-SELinux_Contexts.html
https://debian-handbook.info/browse/zh-CN/stable/sect.selinux.html
https://wiki.centos.org/zh/HowTos/SELinux
http://blog.siphos.be/2013/05/overview-of-linux-capabilities-part-1/
http://www.cis.syr.edu/~wedu/seed/Labs/Documentation/Linux/How_Linux_Capability_Works.pdf
-->


{% highlight text %}
{% endhighlight %}
