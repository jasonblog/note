---
title: systemd 使用简介
layout: post
comments: true
language: chinese
category: [linux,misc]
keywords: systemd,简介,linux,init
description: 现在一般新发行的版本会采用新的 init 进程，也就是 systemd ，其中启动过程可以通过 man bootup 查看。在此，简单介绍一下 systemd 。
---

现在一般新发行的版本会采用新的 init 进程，也就是 systemd ，其中启动过程可以通过 man bootup 查看。

在此，简单介绍一下 systemd 。

<!-- more -->

## 简介

Linux 内核通过执行 init 将 CPU 的控制权限，交给其它的任务，在 CentOS 中可以通过如下命令查看 init 来自于那个包。

{% highlight text %}
$ rpm -qif `which init`
{% endhighlight %}

init 进程经历了，SysV、Upstart 以及 systemd，而 systemd 是最具争议的一个项目，因为其不只替换了 init，而且还包括了一整套的系统任务。

作为最新的系统和服务管理器，其设计思路借鉴了 Mac 的启动程序 Launchd，兼容 SysV 和 LSB 的启动脚本。事实上其作用远不仅是启动系统，它还接管了系统服务的启动、结束、状态查询和日志归档等职责，并支持定时任务和通过特定事件，如插入特定 USB 设备和特定端口数据触发的任务。

其有以下特性：支持并行化任务、同时采用 socket 和 D-BUS 总线式激活服务、按需启动相应的守护进程、利用 Linux 的 cgroup 监控进程、支持快照和系统恢复、维护挂载点和自动挂载点，各服务间基于依赖关系进行精密控制。


<!--
Systemd同时也清晰地处理了系统关机过程。它在/usr/lib/systemd/目录下有三个脚本，分别叫systemd-halt.service，systemd-poweroff.service，systemd-reboot.service。这几个脚本会在用户选择关机，重启或待机时执行。在接收到关机事件时，systemd首先卸载所有文件系统并停止所有内存交换设备，断开存储设备，之后停止所有剩下的进程。
-->

可以通过 pstree 查看启动的进程树，接下来查看一下 systemd 的特性。


## Systemd

Systemd 使用 ```target``` 来处理引导和服务管理过程，这些 systemd 里的 ```target``` 文件被用于分组不同的引导单元以及启动同步进程。

如果 A 要求 B 在 A 之前运行，则在 [Unit] 段中添加 Requires=B 和 After=B，如果依赖关系是可选的，可添加 Wants=B 和 After=B；注意 Wants= 和 Requires= 并不意味着 After=，即如果 After= 选项没有设置，这两个单元将被并行启动。

### 执行顺序

执行的第一个目标是 ```/etc/systemd/system/default.target```，对于桌面版本，通常会指向 ```/usr/lib/systemd/system/graphical.target```，该文件为文本，可以查看其实际会依次依赖于 ```multi-user.target => basic.target => sysinit.target``` 。

另外，```local-fs.target``` 单元并不会启动用户相关的服务，它只处理与文件系统相关的底层核心服务，会根据 ```/etc/fstab``` 和 ```/etc/inittab``` 来执行相关操作。

```sysinit.target``` 会启动重要的系统服务例如系统挂载，内存交换空间和设备，内核补充选项等等。```basic.target``` 会启动普通服务特别是图形管理服务，它通过 ```/etc/systemd/system/basic.target.wants``` 目录来决定哪些服务会被启动。

在这个阶段，会启动 ```multi-user.target``` 而这个 target 将自己的子单元放在目录 ```/etc/systemd/system/multi-user.target.wants``` 里。这个 target 为多用户支持设定系统环境，非 root 用户会在这个阶段的引导过程中启用。防火墙相关的服务也会在这个阶段启动。

登陆是通过 ```systemd-logind.service``` 进行，可以通过 ```systemctl help systemd-logind.service``` 查看帮助，通常是针对 XWindow，而终端登陆则通过 ```/usr/lib/systemd/system/getty@.service``` 执行。

依赖关系可以查看 [System bootup process](https://www.freedesktop.org/software/systemd/man/bootup.html) 。


### 启动优化

sysvinit 只能一次一个串行地启动进程，而 Systemd 则并行地启动系统服务进程，并且最初仅启动确实被依赖的那些服务，极大地减少了系统引导的时间。

任何启动项，只要是在系统启动时有被执行到，不论启动成功还是失败，systemd 都能够记录下他们的状态，可以通过 systemctl 查看当前的服务。

详细信息可以通过 ```systemctl status systemd-logind.service``` 查看；启动的结构树可以通过 ```systemd-cgls``` 查看。

{% highlight text %}
# systemd-analyze                         ← 查看系统引导用时
# systemd-analyze time                    ← 同上
# systemd-analyze blame                   ← 查看初始化任务所消耗的时间
# systemd-analyze plot > systemd.svg      ← 将启动过程输出为svg图
# systemd-cgtop                           ← 查看资源的消耗状态
{% endhighlight %}

### 常用 systemctl 命令

通过 systemctl 命令可以管理整个系统。

{% highlight text %}
----- 查看、设置、取消开机启动
# systemctl is-enabled nginx
# systemctl enable nginx
# systemctl disable nginx

----- 启动、停止、kill、重启、查看服务状态
# systemctl start nginx
# systemctl stop nginx
# systemctl kill nginx
# systemctl restart nginx
$ systemctl status nginx

----- 修改service之后重新加载
# systemctl daemon-reload

----- 杀死一个服务的所有进程，传递信号到指定服务的所有进程
# systemctl kill crond.service
----- 指定信号类型，如下两者相同，所有fork的进程都会被杀死
# systemctl kill -s SIGKILL crond.service
# systemctl kill -s 9 crond.service
----- 发送指定信号到服务的主进程
# systemctl kill -s HUP --kill-who=main crond.service

----- 其它操作
# systemctl                               ← 列出正在运行的单元
# systemctl list-units                    ← 同上
# systemctl list-units --type service     ← 同上，只是以service为单位
# systemctl --failed                      ← 查看失败的任务
# systemctl list-unit-files               ← 所有已经安装的任务
# systemctl list-dependencies nginx       ← 查看特定服务的依赖关系
{% endhighlight %}

所有可用的单元文件存放在 ```/usr/lib/systemd/system``` 和 ```/etc/systemd/system``` 目录中，一个单元配置文件可以为，系统服务(.service) 、挂载点(.mount)、sockets(.sockets)、系统设备、交换分区/文件、启动目标(target)、文件系统路径、由 systemd 管理的计时器，详见 ```man 5 systemd.unit``` 。

通过enable 设置为开机启动时，相当于在 ```/etc/systemd/system/default.target``` 符号链接指向的目标对应目录下添加指向 nginx 的符号链接。

### 设置启动级别

在 sysVinit 的 runlevels 大多是以数字分级的，常用的命令如下。

{% highlight text %}
# systemctl isolate graphical.target       // 改变当前目标
# systemctl list-units --type=target       // 列出当前目标
# systemctl get-default                    // 查看默认目标
# systemctl set-default graphical.target   // 改变默认目标
{% endhighlight %}

### 日志管理

journald 是 systemd 独有的日志系统，替换了 sysVinit 中的 syslog 守护进程，通过命令 journalctl 读取日志。

Systemd 统一管理所有 Unit 的启动日志，好处是，只需要 journalctl 一个命令，就可以查看所有日志 (内核日志和应用日志) 的内容；配置文件为 /etc/systemd/journald.conf 。

{% highlight text %}
----- 查看某个Unit的日志
# journalctl -u nginx.service
# journalctl -u nginx.service --since today

----- 实时滚动显示某个Unit的最新日志
# journalctl -u nginx.service -f

# 合并显示多个 Unit 的日志
$ journalctl -u nginx.service -u php-fpm.service --since today
{% endhighlight %}

更多内容可以查看 [How To Use Journalctl to View and Manipulate Systemd Logs](https://www.digitalocean.com/community/tutorials/how-to-use-journalctl-to-view-and-manipulate-systemd-logs) 。

<!--
{% highlight text %}
# journalctl                               // 查看日志
# journalctl -b                            // 启动日志
# journalctl -f                            // 实时显示系统日志
# journalctl /usr/sbin/dnsmasq             // 查看特定任务的日志
{% endhighlight %}
-->

### 电源管理

systemctl 命令也可以用来关机、重启、挂起、休眠。

{% highlight text %}
# systemctl poweroff
# systemctl reboot
# systemctl suspend
# systemctl hibernate
{% endhighlight %}

### 时区设置

systemd 提供了一个 timedatectl 命令行，可用于配置时区信息。

{% highlight text %}
----- 查看当前所支持的时区信息
$ timedatectl list-timezones
----- 选择上述中的时区，然后设置
# timedatectl set-timezone zone
----- 查看当前时区设置的状态
# timedatectl status
{% endhighlight %}

## 管理目标

服务 systemctl 脚本存放在 ```/usr/lib/systemd/``` 目录下，有系统 (system) 和用户 (user) 之分，前者开机后无需登录即可运行，后者则需要在用户登录后才能运行程序。

常见的服务如 nginx 等存放在 ```/usr/lib/systemd/system``` 目录下；下面以 nginx 为例，编写脚本时可以直接参考 nginx 的编写方法。

{% highlight text %}
[Unit]
Description=The nginx HTTP and reverse proxy server
After=network.target remote-fs.target nss-lookup.target

[Service]
Type=forking
PIDFile=/run/nginx.pid
# Nginx will fail to start if /run/nginx.pid already exists but has the wrong
# SELinux context. This might happen when running `nginx -t` from the cmdline.
# https://bugzilla.redhat.com/show_bug.cgi?id=1268621
ExecStartPre=/usr/bin/rm -f /run/nginx.pid
ExecStartPre=/usr/sbin/nginx -t
ExecStart=/usr/sbin/nginx
ExecReload=/bin/kill -s HUP $MAINPID
KillMode=process
KillSignal=SIGQUIT
TimeoutStopSec=5
PrivateTmp=true

[Install]
WantedBy=multi-user.target
{% endhighlight %}

一个服务以 .service 结尾，一般会分为 3 部分：```[Unit]```、```[Service]``` 和 ```[Install]``` 。

服务脚本按照上面编写完成后，以 754 的权限保存在 ```/usr/lib/systemd/system``` 目录下，这时就可以利用 ```systemctl``` 进行配置了。

### Unit

设置描述、帮助文档、启动顺序以及服务的启动依赖条件等，如下是 SSHD 服务的。

{% highlight text %}
Description=OpenSSH server daemon
Documentation=man:sshd(8) man:sshd_config(5)
After=network.target sshd-keygen.service
Wants=sshd-keygen.service
{% endhighlight %}

```After``` 和 ```Before``` 分别设置在哪些服务之后或者之前启动，用于配置各个服务的启动顺序，注意，这里是启动顺序而非依赖关系。

通过 ```Wants``` 和 ```Requires``` 设置弱依赖和强依赖关系，前者表示依赖的服务启停不会影响当前服务，或者表示如果依赖退出，那么该服务同时退出。

### Service

用于配置如何启动服务；注意，命令应该使用绝对路径。

<!--
#  forking：ExecStart字段将以fork()方式启动，此时父进程将会退出，子进程将成为主进程
#  oneshot：类似于simple，但只执行一次，Systemd 会等它执行完，才启动其他服务
#  dbus：类似于simple，但会等待 D-Bus 信号后启动
#  idle：类似于simple，但是要等到其他任务都执行完，才会启动该服务。一种使用场合是为让该服务的输出，不与其他服务的输出相混合
-->

{% highlight bash %}
User=nginx
Group=nginx
PIDFile=/run/nginx.pid

Type=forking                               # 定义启动类型
#  simple : 默认值，通过ExecStart字段启动进程
#  notify : 类似于simple，服务启动结束后会发出通知信号，然后Systemd再启动其他服务

EnvironmentFile=-/etc/sysconfig/nginx      # 依赖环境，可以指定多个
EnvironmentFile=-/etc/default/nginx

ExecStartPre=/usr/bin/rm -f /run/nginx.pid # 启动服务之前执行的命令
ExecStartPre=/usr/sbin/nginx -t
ExecStart=/usr/sbin/nginx                  # 启动时，多个会被最后一个覆盖
ExecStartPost=                             # 启动服务之后执行的命令
ExecReload=/bin/kill -s HUP $MAINPID       # 重启服务时执行的命令
ExecStop=                                  # 停止服务时执行的命令
ExecStopPost=                              # 停止服务之后执行的命令
TimeoutStopSec=5                           # 设置停止超时时间

KillMode=process                           # 重启行为配置，详见如下介绍
#  control-group : 默认值，当前控制组里面的所有子进程，都会被杀掉
#  process       : 只杀主进程，信号可以通过如下方式定义
#  mixed         : 主进程将收到SIGTERM信号，子进程收到SIGKILL信号
#  none          : 没有进程会被杀掉，只是执行服务的stop命令
KillSignal=SIGQUIT

Restart=on-failure                         # 意外失败后重启方式，正常停止不重启
#  no          : 默认值，退出后不重启
#  on-success  : 只有正常退出时(退出状态码为0)，才会重启
#  on-failure  : 非正常退出时 (退出状态码非0)，包括被信号终止和超时，才会重启
#  on-abnormal : 只有被信号终止和超时，才会重启
#  on-abort    : 只有在收到没有捕捉到的信号终止时，才会重启
#  on-watchdog : 超时退出，才会重启
#  always      : 不管是什么退出原因，总是重启
RestartSec=10                              # 重启服务之前，需要等待的秒数，默认100ms

PrivateTmp=True                            # 给服务分配独立的临时空间
{% endhighlight %}

在所有的服务配置之前，都可以加上一个连词号 (```-```)，表示 "抑制错误"，也即即使发生错误也不影响其他命令的执行。

### Install

在通过 enable 设置为开机启动时，添加到那个 target 里面，也即定义如何安装这个配置文件，即怎样做到开机启动。

### 资源隔离

相关内容可以查看 [容器之 CGroup](/post/linux-container-cgroup-introduce.html) 。


## 其它

systemd 带来了一整套与操作系统交互的新途径，如可以用 hostnamectl 获得机器的 hostname 和其它有用的独特信息。

{% highlight text %}
# hostnamectl                              // 查看机器信息
{% endhighlight %}

除了 restart 命令，也可以使用 try-start 选项，它只会在服务已经在运行中的时候重启服务。


<!--

在sysvinit的时代，如果需要结束一个服务及其启动的所有进程，可能会遇到一些糟糕的进程无法正确结束掉，即便是我们使用kill，killall等命令来结束它们，到了systemd的时代一切都变得不一样，systemd号称是第一个能正确的终结一项服务的程序，下面来看看具体如何操作的：
# systemctl kill crond.service
或者指定一个信号发送出去
# systemctl kill -s SIGKILL crond.service
例如可以像这样执行一次reload操作
# systemctl kill -s HUP --kill-who=main crond.service

http://www.csdn.net/article/2015-01-08/2823477/1<br><br>
http://www.linuxidc.com/Linux/2014-11/110023.htm<br><br>
http://www.linuxidc.com/Linux/2014-12/110777.htm<br><br>
http://www.ithov.com/linux/136324.shtml  比较详细介绍了原理

首先，使用systemctl start [服务名（也是文件名）]可测试服务是否可以成功运行，如果不能运行则可以使用systemctl status [服务名（也是文件名）]查看错误信息和其他服务信息，然后根据报错进行修改，直到可以start，如果不放心还可以测试restart和stop命令。

接着，只要使用systemctl enable xxxxx就可以将所编写的服务添加至开机启动即可。

这样看来，虽然systemctl比较陌生，但是其实比init.d那种方式简单不少，而且使用简单，systemctl能简化的操作还有很多，现在也有不少的资料，看来RHEL/CentOS比其他的Linux发行版还是比较先进的，此次更新也终于舍弃了Linux 2.6内核，无论是速度还是稳定性都提升不少。

如果使用 systemd 日志可以直接打印到 STDOUT ，然后由 systemd 统一管理。


bcmp ：比较两个内存中的内容
bcopy : 复制内存中的内容
bzero ： 将一个内存内容全清零
ffs : 在一个整数中查找第一个值为真的位
index : 查找字符串中第一个出现的指定字符
memccpy ：复制内存中的内容
memchr ：在一块内存指定范围内查找一个指定字符
memcmp ：比较内存中存放的内容
memcpy ： 复制一块内存内容到另一块
memfrob ： 对某个内存区重新编码
memmove ： 复制内存内容
memset ：将某值填入到一个内存区域
rindex ：查找字符串中最后一个出现的指定字符
* strcat ：将一个字符串连接到另一个字符串的尾部
* strncat ：将一个字符串的前n个字符连接到另一个字符串的尾部
strchr ；查找字符串中指定字符
strcmp ：比较两个字符串
strcoll ：根据当前环境信息来比较字符串
strcpy ：复制一个字符串的内容到另一个字符串中
strdup ：复制字符串的内容
strfry ：随机重组一个字符串
strlen ： 返回字符串的长度
* strncasecmp ：忽略大小写比较两个字符串
* strcasecmp ：忽略大小写比较字符串
strncmp ：比较字符串前n个字符
strncpy ： 复制字符串的前n个字符
strpbrk ：查找字符串中第一个出现的指定字符
strrchr ：查找字符串中最后一个出现的指定字符
strspn ：计算字符串中由指定字符集字符组成的子字符串的长度
strcspn ：计算字符串中由非指定字符集字符组成的子字符串的长度

https://github.com/jnavila/memtester


    # 查看所有日志（默认情况下 ，只保存本次启动的日志）
    $ sudo journalctl

    # 查看内核日志（不显示应用日志）
    $ sudo journalctl -k

    # 查看系统本次启动的日志
    $ sudo journalctl -b
    $ sudo journalctl -b -0

    # 查看上一次启动的日志（需更改设置）
    $ sudo journalctl -b -1

    # 查看指定时间的日志
    $ sudo journalctl --since="2012-10-30 18:17:16"
    $ sudo journalctl --since "20 min ago"
    $ sudo journalctl --since yesterday
    $ sudo journalctl --since "2015-01-10" --until "2015-01-11 03:00"
    $ sudo journalctl --since 09:00 --until "1 hour ago"

    # 显示尾部的最新10行日志
    $ sudo journalctl -n

    # 显示尾部指定行数的日志
    $ sudo journalctl -n 20

    # 实时滚动显示最新日志
    $ sudo journalctl -f

    # 查看指定服务的日志
    $ sudo journalctl /usr/lib/systemd/systemd

    # 查看指定进程的日志
    $ sudo journalctl _PID=1

    # 查看某个路径的脚本的日志
    $ sudo journalctl /usr/bin/bash

    # 查看指定用户的日志
    $ sudo journalctl _UID=33 --since today



    # 查看指定优先级（及其以上级别）的日志，共有8级
    # 0: emerg
    # 1: alert
    # 2: crit
    # 3: err
    # 4: warning
    # 5: notice
    # 6: info
    # 7: debug
    $ sudo journalctl -p err -b

    # 日志默认分页输出，--no-pager 改为正常的标准输出
    $ sudo journalctl --no-pager

    # 以 JSON 格式（单行）输出
    $ sudo journalctl -b -u nginx.service -o json

    # 以 JSON 格式（多行）输出，可读性更好
    $ sudo journalctl -b -u nginx.serviceqq
     -o json-pretty

    # 显示日志占据的硬盘空间
    $ sudo journalctl --disk-usage

    # 指定日志文件占据的最大空间
    $ sudo journalctl --vacuum-size=1G

    # 指定日志文件保存多久
    $ sudo journalctl --vacuum-time=1years
-->


### 自动登陆

在此查看下如何自动登陆，首先创建一个新的类似于 ```getty@.service``` 的服务。

{% highlight text %}
# cp /lib/systemd/system/getty@.service /etc/systemd/system/autologin@.service
# ln -s /etc/systemd/system/autologin@.service /etc/systemd/system/getty.target.wants/getty@tty8.service
# vi /etc/systemd/system/getty.target.wants/getty@tty8.service
...
ExecStart=-/sbin/mingetty --autologin USERNAME %I
Restart=no
...
Alias=getty.target.wants/getty@tty8.service
{% endhighlight %}

最后重新加载守护进程，运行服务：

{% highlight text %}
# systemctl daemon-reload
# systemctl start getty@tty8.service
{% endhighlight %}

需要注意的是，如果你退出了 tty8 的会话，你需要等到下次重新启动才能使用，除非你给 Restart 的值是 ```'always'```，这样可以使用 systemctl 手动开启，不过，出于安全考虑，强烈建议不要那么做。

### 对比

![SystemD VS. SysVinit]({{ site.url }}/images/linux/systemd-sysvinit.jpg "SystemD VS. SysVinit"){: .pull-center width="90%"}

### 杂项

如果重启过于频繁会报 "uagent.service start request repeated too quickly, refusing to start."

## 参考

[systemd System and Service Manager](http://www.freedesktop.org/wiki/Software/systemd/)，system daemon 官方网站。



<!--
http://www.ruanyifeng.com/blog/2016/03/systemd-tutorial-commands.html
-->


{% highlight text %}
{% endhighlight %}
