---
title: Linux 监控杂项
layout: post
comments: true
language: chinese
category: [linux]
keywords: linux,monitor,监控,sar
description: 在此简单列举一些常见的监控工具，以及配置方式等。
---

在此简单列举一些常见的监控工具，以及配置方式等。

<!-- more -->

## SAR

sar (select activity information) 用于采集系统的信息，默认会保存一个月的信息，在 CentOS 中包含在 sysstat软件包中。与此相关的还有：

* sac, system ativity data collector。也就是 sar 的后端，用于收集信息；
* sa1，用于将信息保存为二进制信息，默认位置为 /var/log/sa/saXX (其中XX表示当月的日期)；
* sar2，显示每天的统计信息，**一个 shell 脚本**；
* sadf，用于转换为其它的格式 (如csv，xml) 的程序。

其中 sar1 和 sar2 程序会在 cron 任务中调用执行，也就是在 /etc/cron.d/sysstat 文件中，默认每 10 分钟通过 sa1 采集一次数据；23:53 时通过 sa2 转换一次数据，默认保存 28 天，可以在 /usr/lib64/sa/sa2 中设置。

其中 /etc/cron.d/sysstat 文件内容如下。

{% highlight text %}
# Run system activity accounting tool every 10 minutes
*/10 * * * * root /usr/lib64/sa/sa1 1 1
# Generate a daily summary of process accounting at 23:53
53 23 * * * root /usr/lib64/sa/sa2 -A
{% endhighlight %}

因为使用的是 crontab ，所以最小的粒度是 1min；当然，也可以使用 ```sa1 1 60``` 每秒执行依次，并执行 60 次，进一步减小监控的粒度。

<!-- # 0 * * * * root /usr/lib64/sa/sa1 600 6 & -->

### 保存数据

默认会将监控数据保存在 /var/log/sa/ 目录下，其中保存的数据，例如有一个 sa01 文件，可以直接通过 ```sar -f /var/log/sa/sa01``` 命令查看。

简单列举常用的参数：

{% highlight text %}
sar [interval [count]]
    如果不使用时间参数则会显示到现在为止，所搜集的统计数据；
    -A
    保存所有，等加于-bBdHqrRSuvwWy -I SUM -I XALL -m ALL -n ALL -u ALL -P ALL；
    -f file
    从指定文件读取显示；
    CPU
    -u 统计平均值；-P ALL可以指定cpu号或者显示所有；
{% endhighlight %}

<!--
-o file
默认输出到stdout；用于指定输出的二进制文件，如果不指定文件会输出到/var/log/sa/saXX中；

编译起来还是不错的，有多语言支持。
-->

磁盘建议采用 iostat ，sar -d 采用的设备名称不太好识别，采用的是从设备号。

关于一些常见的监控项，可以参考如下图片：

![monitor sar]({{ site.url }}/images/linux/monitor-misc-sar.png "monitor sar"){: .pull-center width="80%" }


## TOP

top 是查看资源最常用的方式。

### 常用场景

简单列举下经常使用的场景。

#### Batch Mode

默认是交互模式，也就是循环执行，并接收用户的按键，这样对于一些采集的程序就不方便使用，这里可以使用 Batch Mode 。

{% highlight text %}
----- 使用Batch Mode打印指标，此时仍是无限循环
$ top -b

----- 通过-n参数指定只打印一次
$ top -b -n 1

----- 指定时间点运行，需要注意TERM环境变量，top命令需要但是at并没有获取该参数
$ cat ./test.at
TERM=linux top -b -n 1 >/tmp/top-report.txt
$ at -f ./test.at now+1minutes
{% endhighlight %}

#### 监听特定进程

可以通过如下方式指定只监控特定进程或者用户。

{% highlight text %}
----- 指定多个进程
$ top -p 4360,4358
$ top -p 4360 -p 4358

----- 指定用户
$ top -u johndoe
$ top -u 500
$ top -U johndoe
{% endhighlight %}

<!--
The conclusion is, you can either use the plain user name or the numeric UID. "-u, -U? Those two are different?" Yes. Like almost any other GNU tools, options are case sensitive. -U means top will find matching effective, real, saved and filesystem UIDs, while -u just find matching effective user id. Just for reminder, every *nix process runs using effective UID and sometimes it isn't equal with real user ID. Most likely, one is interested in effective UID as filesystem permission and operating system capability are checked against it, not real UID.

While -p is just command-line option only, both -U and -u can be used inside interactive mode. Like you guess, press 'U' or 'u' to filter the processes based on their user name. Same rule is applied, 'u' for effective UID and 'U' for real/effective/saved/filesystem user name. You will be asked to enter the user name or the numeric UID. '

A 显示多个窗口。


-->






## 多线程

在 Linux 下可以通过如下程序查看多线程。

{% highlight text %}
----- 通过-H(Threads toggle)参数，一行显示一个线程
$ top -H -p `pidof mysqld`

----- 查看所有存在的线程，H(Show threads as if they were processes)
$ ps Hp `pidof mysqld`

----- 查看线程调用堆栈
$ pstack `pidof mysqld`

----- 查看线程数
$ pstree -p `pidof mysqld`
{% endhighlight %}

<!--
3、ps -mp <PID>
手册中说：m Show threads after processes
这样可以查看一个进程起的线程数。
-->



{% highlight text %}
{% endhighlight %}
