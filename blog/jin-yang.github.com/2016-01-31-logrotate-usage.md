---
title: logrotate 使用方法
layout: post
comments: true
language: chinese
category: [linux, misc]
keywords: linux,log,日志,滚动,logrotate,rotation
description: 通常我们会通过日志记录程序运行时各种信息，如有异常，则通过日志分析用户行为、记录运行轨迹、查找程序问题。但是，磁盘空间有限，为了节省空间和方便整理，可以将日志文件按时间或大小分成多份，删除时间久远的日志文件。这就是通常说的日志滚动 (log rotation)，在本文中，就简单介绍下 Linux 中的 logrotate 程序。
---

通常我们会通过日志记录程序运行时各种信息，如有异常，则通过日志分析用户行为、记录运行轨迹、查找程序问题。但是，磁盘空间有限，为了节省空间和方便整理，可以将日志文件按时间或大小分成多份，删除时间久远的日志文件。

这就是通常说的日志滚动 (log rotation)，在本文中，就简单介绍下 Linux 中的 logrotate 程序。

<!-- more -->

## 简介

logrotate 是默认安装的，一般会定时运行 logrotate，通常是每天一次；会通过 crontab 运行，也就是直接运行 /etc/cron.daily/logrotate 脚本。

其中，CentOS 系统中的脚本内容如下。

{% highlight bash %}
#!/bin/sh

/usr/sbin/logrotate -s /var/lib/logrotate/logrotate.status /etc/logrotate.conf
EXITVALUE=$?
if [ $EXITVALUE != 0 ]; then
    /usr/bin/logger -t logrotate "ALERT exited abnormally with [$EXITVALUE]"
fi
exit 0
{% endhighlight %}

具体执行时间可以查看 /etc/crontab 或者 /etc/anacrontab (CentOS)。

### 配置文件

默认使用 /etc/logrotate.conf 文件，作为全局配置，而不同应用的具体配置则在 /etc/logrotate.d 目录下，通常以应用程序的名称命名，例如 nginx、mysql、syslog、yum 等配置。

如下，是一个配置文件的具体内容。

{% highlight text %}
/var/log/nginx/*.log /var/log/tomcat/*log {   # 可以指定多个路径
    daily                      # 日志轮询周期，weekly,monthly,yearly
    rotate 30                  # 保存30天数据，超过的则删除
    size +100M                 # 超过100M时分割，单位K,M,G，优先级高于daily
    compress                   # 切割后压缩，也可以为nocompress
    delaycompress              # 切割时对上次的日志文件进行压缩
    dateext                    # 日志文件切割时添加日期后缀
    missingok                  # 如果没有日志文件也不报错
    notifempty                 # 日志为空时不进行切换，默认为ifempty
    create 640 nginx nginx     # 使用该模式创建日志文件
    sharedscripts              # 所有的文件切割之后只执行一次下面脚本
    postrotate
        if [ -f /var/run/nginx.pid ]; then
            kill -USR1 `cat /var/run/nginx.pid`
        fi
    endscript
}
{% endhighlight %}

当配置完成后，可以通过如下方式进行测试。

{% highlight text %}
----- 可直接手动执行
$ logrotate --force /etc/logrotate.d/nginx

----- 显示详细的信息；而且--debug/-d实际上不会操作具体文件(Dry Run)
$ logrotate --debug --verbose --force /etc/logrotate.d/nginx
{% endhighlight %}

在 CentOS 中，默认会将日志保存 /var/lib/logrotate.status 文件中，如果需要指定到其它文件，可以通过 ```-s/--state``` 参数指定。

## 日志处理

首先，介绍下在 Linux 系统中是如何处理文件的。

进程每打开一个文件，系统会分配一个新的文件描述符给这个文件；这个文件描述符中保存了文件的状态信息、当前文件位置以及文件的 inode 信息。对文件的操作是通过文件描述符完成，所以文件路径发生改变不会影响文件操作。

### 测试

可以通过 python 脚本进行简单测试，示例如下：

{% highlight python %}
#!/bin/python
import time
with open("/tmp/foobar.txt", "w") as file:
    while 1:
        file.write("test\n")
        file.flush()
        time.sleep(1)
{% endhighlight %}

当通过 ```mv /tmp/{foobar.txt,test.txt}``` 移动文件之后，可以看到 test.txt 文件仍然在写入。原因如上所述，Linux 在执行 mv 操作时，文件描述符是不变的，只修改了文件名。

其中 logrotate 提供了两种方式，分别为 create 和 copytruncate 。

### create

这也就是默认的方案，可以通过 create 命令配置文件的权限和属组设置；这个方案的思路是重命名原日志文件，创建新的日志文件。详细步骤如下：

1. 重命名正在输出日志文件，因为重命名只修改目录以及文件的名称，而进程操作文件使用的是 inode，所以并不影响原程序继续输出日志。

2. 创建新的日志文件，文件名和原日志文件一样，注意，此时只是文件名称一样，而 inode 编号不同，原程序输出的日志还是往原日志文件输出。

3. 最后通过某些方式通知程序，重新打开日志文件；由于重新打开日志文件会用到文件路径而非 inode 编号，所以打开的是新的日志文件。

如上也就是 logrotate 的默认操作方式，也就是 mv+create 执行完之后，通知应用重新在新文件写入即可。mv+create 成本都比较低，几乎是原子操作，如果应用支持重新打开日志文件，如 syslog, nginx, mysql 等，那么这是最好的方式。

不过，有些程序并不支持这种方式，压根没有提供重新打开日志的接口；而如果重启应用程序，必然会降低可用性，为此引入了如下方式。

### copytruncate

该方案是把正在输出的日志拷 (copy) 一份出来，再清空 (trucate) 原来的日志；详细步骤如下：

1. 将当前正在输出的日志文件复制为目标文件，此时程序仍然将日志输出到原来文件中，此时，原文件名也没有变。

2. 清空日志文件，原程序仍然还是输出到预案日志文件中，因为清空文件只把文件的内容删除了，而 inode 并没改变，后续日志的输出仍然写入该文件中。

<!--
文件清空并不影响到输出日志的程序的文件表里的文件位置信息，因为各进程的文件表是独立的。那么文件清空后，程序输出的日志应该接着之前日志的偏移位置输出，这个位置之前会被\0填充才对。但实际上logroate清空日志文件后，程序输出的日志都是从文件开始处开始写的。这是怎么做到的？这个问题让我纠结了很久，直到某天灵光一闪，这不是logrotate做的，而是成熟的写日志的方式，都是用O_APPEND的方式写的。如果程序没有用O_APPEND方式打开日志文件，变会出现copytruncate后日志文件前面会被一堆\0填充的情况。

日志在拷贝完到清空文件这段时间内，程序输出的日志没有备份就清空了，这些日志不是丢了吗？是的，copytruncate有丢失部分日志内容的风险。所以能用create的方案就别用copytruncate。所以很多程序提供了通知我更新打开日志文件的功能来支持create方案，或者自己做了日志滚动，不依赖logrotate。
-->

如上所述，对于 copytruncate 也就是先复制一份文件，然后清空原有文件。

通常来说，清空操作比较快，但是如果日志文件太大，那么复制就会比较耗时，从而可能导致部分日志丢失。不过这种方式不需要应用程序的支持即可。


## FAQ

简单列举下与 logrotate 相关的常见问题。

#### 问题：sharedscripts的作用是什么？

如上配置中，通过通配符 ```'*'``` 指定了多个日志文件，例如 access.log、error.log ，这时会在所有的日志文件都执行完成之后，再统一执行一次脚本；如果没有这条命令，则每个日志文件执行完成后都会执行一次脚本。

#### 问题：rotate和maxage的区别是什么？

都是用来控制保存多少日志文件的，区别在于 rotate 是以个数为单位的，而 maxage 是以天数为单位的。如果是以天来轮转日志，那么二者的差别就不大。


<!--
问题：如何告诉应用程序重新打开日志文件？
Nginx通过postrotate指令发送USR1信号来通知Nginx重新打开日志文件的；MySQL通过flush-logs来重新打开日志文件的。更有甚者，有些应用程序就压根没有提供类似的方法，如果要重新打开日志文件，就必须重启服务。

。还好Logrotate提供了一个名为copytruncate的指令，此方法采用的是先拷贝再清空的方式，整个过程中日志文件的操作句柄没有发生改变，所以不需要通知应用程序重新打开日志文件，但是需要注意的是，在拷贝和清空之间有一个时间差，所以可能会丢失部分日志数据。

BTW：MySQL本身在support-files目录已经包含了一个名为mysql-log-rotate的脚本，不过它比较简单，更详细的日志轮转详见「Rotating MySQL Slow Logs Safely」。

-->

## 参考

关于 MySQL 如何安全地处理日志文件，可以查看 [Rotating MySQL Slow Logs Safely](https://engineering.groupon.com/2013/mysql/rotating-mysql-slow-logs-safely/) 中的相关介绍，否则可能会导致异常。



<!--
logrotate的copytruncate参数导致打开的文件产生空洞

日志文件保存时候使用> 将其保存，对文件进行清除和日志切割(logrotate)时，容易出现文件空洞。

需要使用“>>” 可以避免该问题。修改后问题确实解决了。

在此Mark一下。

同理，在用fopen 打开或创建日志文件时，打开方式使用“w”类似于>，使用"a"类似于>>。如果需要对日志文件进行logrotate备份，最好使用"a"方式创建或打开。（经过测试）

以上情况发生的条件是，日志文件一直处于打开状态，进程没有被重启，如果进程在备份完成之后重新启动或运行，则不会导致文件空洞，因为文件的offset指示器已经到了文件头。




logrotate机制和原理
http://www.lightxue.com/how-logrotate-works
-->


{% highlight text %}
{% endhighlight %}
