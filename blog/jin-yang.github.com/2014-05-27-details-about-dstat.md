---
title: dstat 使用及其原理
layout: post
comments: true
language: chinese
category: [linux]
keywords: dstat,monitor,监控
description: dstat 是一个不错的系统监控程序，类似于 vmstat、iostat、mpstat，所不同的是，可以监控多个系统指标，如 CPU、网络、内存、中断等，可以将结果显示到终端，也可保存到文件。另外，该程序是通过 Python 实现的，可以动态指定插件，甚至编写自己的插件。
---

dstat 是一个不错的系统监控程序，类似于 vmstat、iostat、mpstat，所不同的是，可以监控多个系统指标，如 CPU、网络、内存、中断等，可以将结果显示到终端，也可保存到文件。

另外，该程序是通过 Python 实现的，可以动态指定插件，甚至编写自己的插件。

在本文中，大致介绍其使用方法，以及实现的原理。

<!-- more -->

## 使用方法

详细的使用方式可以查看 man 帮助，在此只是介绍一些 dstat 的特性。

查看 man 帮助时，就会发现，作者的原意是要替换掉 vmstat、iostat、ifstat 等工具。通过该工具可以将多个监控在一块展示，而且显示比较友好，并且可以直接将统计结果保存。

另外，dstat 支持高亮显示，不过需要注意的是，颜色并非标示系统指标的好或者坏；对于颜色改变，只是说明其单位修改了，或者超过了某个范围 (通常对于 cpu 而言)。

![dstat-sample]{: .pull-center}

其帮助手册可以通过 man 1 dstat 查看，命令的基本格式如下，也可以通过 -h 选项查看。

{% highlight text %}
dstat [-afv] [options..] [delay [count]]

常用选项：
  -c, --cpu
    统计CPU状态，包括 user, system, idle (空闲等待时间百分比), wait (等待磁盘IO),
    hardware interrupt (硬件中断), software interrupt (软件中断) 等；
  -d, --disk
    统计磁盘读写状态，主要包括了读写信息；
  -l, --load
    统计系统负载情况，包括1分钟、5分钟、15分钟平均值；
  -m, --mem
    统计系统物理内存使用情况，包括used, buffers, cache, free；
  -s, --swap
    统计swap已使用和剩余量；
  -n, --net
    统计网络使用情况，包括接收和发送数据；
  -p, --proc
    统计进程信息，包括runnable、uninterruptible、new；
  -N eth1,total
    统计eth1接口汇总流量；
  -r, --io
    统计I/O请求，包括读写请求；
  -y, --sys
    统计系统信息，包括中断、上下文切换；
  -t
    显示统计时时间，对分析历史数据非常有用；
  --fs
    统计文件打开数和inodes数；
{% endhighlight %}

其中最后两个参数表示，每隔 delay 秒显示一行，总计 count 行，默认值是 1s 和空 (无限)。如果 delay 大于 1 ，默认该行还是每隔 1s 更新一次，当超过 delay 秒后，换行。

需要注意的是，当 delay > 1 时，每秒的更新值是显示的之前统计的均值，如 delay=10 ，则会显示 1 秒的均值，2 秒的均值，...，也就是最终结果仍然是秒级的均值。

如果同时设置了 \-\-noupdate ，则确实是每隔 delay 秒更新一次，而不会每秒更新一次。

### 常用参数

如下是常见的使用方法，每隔 3 秒刷新。

{% highlight text %}
$ dstat -clmdngyr 3
{% endhighlight %}

### 使用插件

Dstat 是基于插件的，程序本身提供了定时显示的功能，而显示的内容可以通过插件进行扩展，从而可以很方便的添加自己所需要的插件。

如下的示例是使用 time、cpu、disk 三个插件，其作用是等效的。

{% highlight text %}
$ dstat -tcd
$ dstat --time --cpu --disk
$ dstat -M time,cpu,disk
$ dstat -M time -M cpu -M disk
{% endhighlight %}

可以使用的插件包括了 dstat.py 文件中定义的插件，以及在 plugins 目录下定义的插件，使用时可以直接通过 \-\-PLUG-NAME 指定相应的插件执行。

通过 dstat \-\-list 查看当前支持的所有插件及其保存的目录，其中最简单的是 plugins/dstat_helloworld.py ，也就是一个示例，可以通过 dstat \-\-helloworld 执行。

![dstat-helloworld]{: .pull-center}

如果想要自己定义插件，可以直接参考 plugins/dstat_helloworld.py 的实现。



### 其它

除了直接监控性能外，还可以放到其它的程序中使用，其中在 examples/{read.py,mstat.py}，包含了简单的示例，没有仔细研究过。感兴趣的可以自己检查下。



## 源码分析

dstat 的源码可以通过 yumdownloader 下载，或则直接到 [Dstat 官网][dstat-official] 上下载，当然也可以从 [Github](https://github.com/dagwieers/dstat) 上下载 。在官网，除了 Dstat 之外，还有些其它的工具可供参考。

dstat 中的定时执行模块是通过 sched 模块实现的，因此之前我们先看看该模块的功能。详细文档可以参考官方 [sched — Event scheduler][sched-official]，在此仅简单介绍一下。

### sched 模块

该模块实际是一个调度器，也就是延时处理机制，每次想要定时执行某任务都必须写入一个调度，详细内容可以参考 [Python 官方][sched-official]。

如果查看源码，可以发现其实现非常简单，会通过 timefunc 计算时间，休眠通过 delayfunc 实现，而且通过 queue 保存多个事件。

使用时通常包含了三步：

#### 1. 生成调度器
{% highlight text %}
scheduler = sched.scheduler(timefunc, delayfunc)
{% endhighlight %}
第一个参数是一个可以返回时间戳的函数；第二个参数可以在定时未到达之前的阻塞函数。通常来说，我们会使用 time.time、time.sleep，当然也可以使用自定义的定时函数。<!--第二个也可以是阻塞socket等。-->

#### 2. 加入调度事件

{% highlight text %}
scheduler.enterabs(time, priority, action, argument)
scheduler.enter(delay, priority, action, argument)
{% endhighlight %}
对于 enter() 函数来说，采用的时相对时间，其四个参数分别为 A) 间隔时间；B) 优先级，当多个任务同时到达时使用，数字大的优先级高； C) 被调用回调的函数；D) 传递给回掉函数的参数。

enterabs() 使用的是绝对时间，该参数需要与初始化时的 timefunc 入参格式相同。

#### 3. 运行
{% highlight text %}
scheduler.run()
{% endhighlight %}
运行所有的调度事件，该函数会等待下一个事件，然后执行他直到没有可调度的事件为止。

### 示例

可以参考如下的简单示例。

{% highlight python %}
#!/usr/bin/env python
#-*- coding:utf-8 -*-
import time
import sched

def foobar(msg):         # 被定时调度的函数
    print "Current:", time.time(), 'MSG ', msg

s = sched.scheduler(time.time, time.sleep)     # 初始化
s.enter(2, 2, foobar, ("High Priority.",))     # 添加如下的两个调度任务
s.enter(2, 1, foobar, ("Low  Priority.",))
s.run()                                        # OK, Run it
print "Current:", time.time()
time.sleep(3)
{% endhighlight %}

注意，sched 模块不是循环的，一次调度被执行后就 Over 了，如果想再执行，请再次 enter，也就是如上的函数只会调度一次。

如果要循环执行，可以使用如下的函数。
{% highlight python %}
#!/usr/bin/env python
#-*- coding:utf-8 -*-
import time
import sched

def foobar(msg, act_time):         # 被定时调度的函数
    global s
    print "Current:", time.time(), 'MSG ', msg
    s.enterabs(act_time + 5, 1, foobar, ("Again.", act_time + 5,))

s = sched.scheduler(time.time, time.sleep)                   # 初始化
initime = time.time()
s.enterabs(initime + 1, 1, foobar, ("Begin.", initime + 1,)) # 添加调度任务
s.run()                                                      # OK, Run it
print "Current:", initime
time.sleep(100)
{% endhighlight %}

在 dstat 中，就是使用的后者。

注意，上述程序只为了说明，生产环境的程序中一定不能在定时函数中再次调用 enter() 函数，否则调用栈会不断增加，直到内存上限。


### 框架

其中所有的插件都是继承于 class dstat 类，包括了一些必须的成员变量，以及相应的接口函数，其中部分是必须要包含的，如果没有则会在初始化的时候报错。

#### 插件分析

dstat 的插件包括了 dstat.py 中的类，以及 plugins 目录下的插件。

在内部的代码中，所有的插件必须要以 dstat_ 开头，后面的是插件的名称。在加载的时候，会将命令行传入的 - 转换为 _ ，如 \-\-disk-util 实际对应 plugins/dstat_disk_util.py 文件。

插件查找时，首先会搜索 dstat.py 文件中的实现方法，也就是开头为 dstat 的类，然后搜索 pluginpath 目录下的 dstat_xxx.py 文件，所有的插件可以通过 listplugins() 查看。


#### 循环调度

初始化的操作是在 main() 函数中完成，对于插件，则会通过 execfile()、exec 加载。

对于调度，如上所述，调度任务是通过 sched 实现的。

{% highlight python %}
def main():
    ... ...
    scheduler = sched.scheduler(time.time, time.sleep)
    inittime = time.time()
    ### Let the games begin
    while update <= op.delay * op.count or op.count == -1:
        scheduler.enterabs(inittime + update, 1, perform, (update,))
        scheduler.run()
        sys.stdout.flush()
        update = update + interval
        linecache.clearcache()
    ... ...
{% endhighlight %}

最后执行是 perform() 函数。

#### 执行、显示

如下是主要的执行函数，首先会调用各个插件的 extract() 调用，该函数会对参数进行解析，并将结果保存在 val 中。

然后会调用通用函数 show()、showend() 进行打印，dstat 会通过颜色对状态值进行着色，该功能是通过 cprint() 函数实现，可以参考其细节。

{% highlight python %}
def perform(update):
    ... ...
    ### Calculate all objects (visible, invisible)
    line = newline
    oline = ''
    for o in totlist:
        o.extract()
        if o in vislist:
            line = line + o.show() + o.showend(totlist, vislist)
        if op.output and step == op.delay:
            oline = oline + o.showcsv() + o.showcsvend(totlist, vislist)

    ### Print stats
    sys.stdout.write(line + theme['input'])
    if op.output and step == op.delay:
        outputfile.write(oline + '\n')
    ... ...
{% endhighlight %}


### 总结

介绍到此为止，该工具基本上就是提供了一个通用的框架，可以很好的进行扩展，细节方面可以直接看下源码了。


## 参考

除了文章中列举的链接之外，还可以参考一篇本地保存的 [Dstat: plugin­based real­time monitoring][dstat-ref]，忘记在那里下载的了。

另外，可以参考 [Dstat Offical Site](http://dag.wiee.rs/home-made/dstat/)，以及关于 [Dstat的介绍](http://www.ukuug.org/events/linux2007/2007/papers/Wieers.pdf) 。


[dstat-official]:              http://dag.wiee.rs/home-made/dstat/                  "dstat 官网"
[sched-official]:              https://docs.python.org/2/library/sched.html         "Sched 官方调用"
[dstat-ref]:                   /reference/linux/plugin­based real­time monitoring .pdf

[dstat-sample]:                /images/python/dstat.png                             "dstat 示例"
[dstat-helloworld]:            /images/python/dstat-helloworld.png                  "dstat helloworld 插件示例"

<!--
-->

{% highlight python %}
{% endhighlight %}
