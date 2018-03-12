---
title: 你所不知道的 Linux 定时任务
layout: post
comments: true
language: chinese
category: [linux]
keywords: linux,crontab,cron,定时任务
description: 在 Linux 中，我们经常使用 cron 执行一些定时任务，只需要配置一下时间，它就可以周期的执行一些任务。不知道你是否清楚它的详细用法？是否发现，脚本单独运行时是好好的，放到 cron 任务里却挂了！！！一个部署了 crond 的服务器，系统资源却被莫名其妙的被占满了，Why？？？
---

在 Linux 中，我们经常使用 cron 执行一些定时任务，只需要配置一下时间，它就可以周期的执行一些任务。

不知道你是否清楚它的详细用法？是否发现，脚本单独运行时是好好的，放到 cron 任务里却挂了！！！一个部署了 crond 的服务器，系统资源却被莫名其妙的被占满了，Why？？？

<!-- more -->

## 数学挂钟

在讨论 cron 前，首先看一个很有意思的数学挂钟，这个时钟通过各种符号计算公式表示 1~12 个数字。SO, JUST FOR FUN。

 ![mathematic-clock]{: .pull-center width="300"}

#### 1. 勒让德 (Legendre) 常数

其值为 1 ，可以参考 [维基百科](https://zh.wikipedia.org/zh/%E5%8B%92%E8%AE%A9%E5%BE%B7%E5%B8%B8%E6%95%B0 "勒让德常数")，大致摘抄如下：勒让德在研究素数的分布情况时，发现 pi(x) 满足以下等式：

![legendre-const]{: .pull-center}

其中 B 是一个常数，称为勒让德常数，他估计 B 大约为 1.08366，但不管它的值是什么，只要它存在，就证明了素数定理。该值经过高斯等大佬的努力，最后被数学家 Charles Jean 证明为 1。

#### 2. 无穷递减等比数列

该数列的求和后值为 2 ，其首项为 1，公比为 1/2，也就是 1 + 1/2 + 1/4 + 1/8 + ... 1/(2^i)，当 i 趋于无穷大时，最终和为 2 。

#### 3. 在 XML、HTML 中用于标识

简单来说，也就是说若将上述代码放在浏览器里，显示的就是 3 。其中，&# 后面接十进制字符，&#x 后面接十六进制字符，十六进制的 33 就对应 ASCII 中的 '3'。

可以简单测试下，其中后面就是 ```&#x33;``` 对应的值 &#x33; 。

#### 4. 同余问题

可以参考 [Modular Multiplicative Inverse](https://en.wikipedia.org/wiki/Modular_multiplicative_inverse)，令 x≡2<sup>-1</sup>(mod 7)，2x≡1(mod 7)，则 x=4 。

#### 5. 黄金分割

其中 φ=(√5+1)/2 是黄金分割数，那么将其带进表达式就得 5 。

#### 6. 阶乘

3 的阶乘，3!=3x2x1=6 。

#### 7. 循环小数

用于表示 6.9999... 其中 9 的头上一横，表示循环节是 9 。可以按照无穷递减等比数列查看，该值等于：6.9999... = 6 + 0.9 + 0.09 + 0.009 + 0.0009 + ... = 7 。

#### 8. 二进制

通过二进制表示的 8 ，其中黑色表示 1 ，其它表示 0 ，也即二进制 1000 。

#### 9. 四进值

以四进制表示的 9 ，21(四进制) = 2 * 4 + 1 = 9 。

#### 10. 组合

5 取 2 的组合数，也即 5!/(2!*3!) = 10 。

#### 11. 十六进制

其中 A 是 10，B 是 11，C 是 12 。

#### 12. 立方根

这个很简单，12<sup>3</sup> = 1728，也就是 1728 的立方根就是 12 。

OK，接下来进入正题。

## 简介

在 Linux 平台上如果需要实现定时或者周期执行某一个任务，可以通过编写 cron 脚本来实现。Linux 默认会在开机时启动 crond 进程，该进程负责读取调度任务并执行，用户只需要将相应的调度脚本写入 cron 的调度配置文件中。

另外，需要注意的是，cron 采用的是分钟级的调度，如果要更高精度的，只能用其它方法。

而且，**每次执行时都是并发执行**，而非串行。

### 安装、启动

在很多的发行版本中，cron 是默认安装的，包括了 crond+crontab 两个主要命令。前者是后台任务，用于调度执行；后者用来编译、查看、管理定时任务。

在 CentOS 7 中，该服务是默认安装的，如果没有，则可以安装 cronie 包，然后通过如下命令启动 crond 服务。

{% highlight text %}
# systemctl start crond
{% endhighlight %}

其它的相关操作与 systemctl 指令相同，如 status、restart、stop 等操作。

### 相关配置文件

与 cron 相关的有如下的几个文件，其中前几个是调度相关的文件：

1. /etc/crontab

    全局配置文件；同时每个用户有自己的 crontab 配置文件，这些文件在 /var/spool/cron 目录下。

2. /etc/cron.deny /etc/cron.allow

    分别表示不能/能使用 crontab 命令的用户。如果两个文件同时存在，那么 /etc/cron.allow 优先；如果两个文件都不存在，那么只有超级用户可以安排作业。

3. /etc/cron.d/ /etc/{cron.daily,cron.hourly,cron.monthly,cron.weekly}

    保存的配置文件，后面详解。

4. /var/log/cron

    默认的日志文件。如果配置使用了 syslog，那么日志会发送到 /var/log/messages 文件中。

对于 CentOS 7 来说，有 cron.deny 文件，但是为空，而且没有 cron.allow 文件，这也就意味着所有的用户都可以创建 cron 任务。


### 配置定时任务

定时任务包括了两类：

* 系统级任务 (/etc/crontab)。需要 root 权限，其中第六部分指定了用户名，也就是说能以任意用户执行命令；通常用于系统服务或者一些重要的任务。

* 用户级任务 (/var/spool/cron/)。注意，此时的第六部分为用户需要执行的命令，而且只能以创建任务的用户身份执行。

如果用的任务不是以 hourly monthly weekly 方式执行，则可以将相应的 crontab 写入到 crontab 或 cron.d 目录中。如，可以在 cron.d 目录下新建脚本 echo-date.sh。

{% highlight text %}
# .---------------- minute (0 - 59)
# |  .------------- hour (0 - 23)
# |  |  .---------- day of month (1 - 31)
# |  |  |  .------- month (1 - 12) OR jan,feb,mar,apr ...
# |  |  |  |  .---- day of week (0 - 6) (Sunday=0 or 7) OR sun,mon,tue,wed,thu,fri,sat
# |  |  |  |  |
# *  *  *  *  * user-name  command to be executed
{% endhighlight %}

通常来说，我们是通过 crontab 命令来管理定时任务，常用的参数有：

* -e，编辑 crontab 任务，默认使用当前用户。

* -l，列出用户所有的定时任务；注意，不会显示 /etc 目录下的配置任务。

* -r，删除所有的定时任务。

* -u，指定用户名。

最常见的是通过 crontab -e 编辑 crontab 任务，此时会通过环境变量 $EDITOR 定义的值，调用相应的编辑器，例如 export EDITOR="/usr/bin/vim"，通过 vim 进行编辑。此时会在 /tmp 目录下生成一个临时文件，当编辑完成后替换掉 /var/spool/cron/ 中对应用户的文件。

另外，也可以编辑一个临时文件如 contabs.tmp，编辑后通过 contab contabs.tmp 命令导入新的配置。一般不建议直接修改 /etc/ 下的相关配置文件。

通常来说，需要检查 /etc/crontab 文件、/etc/cron.*/ 目录，以及 /var/spool/cron/ 目录。


## Anacron

其实在官方的源码中，还包括了一个 anacron 指令，而 CentOS 7 中是包含在 anaconda-core 包中的；所以，如果使用 anacron 命令，必须安装该包。

### 简介

像服务器来说，Linux 主机通常是 24 全天全年的处于开机状态，此时只需要 atd 与 crond 这两个服务即可；而对于像我们自己的电脑，经常关机，那么我们就需要 anacron 的帮助了。

anacron 并不能取代 cron，而是以天为单位或者是在启动后立刻进行 anacron 的动作。它会去侦测停机期间该执行但未执行的 crontab 任务，并将该任务运行一遍后，anacron 就会自动停止。

通过 anacron 命令，我们可以选择串行执行（默认）、强制执行（不判断时间戳）、立即执行未执行任务（不延迟等待）等操作。其配置文件是 /etc/anacrontab，每次执行完成会在 /var/spool/anacron/ 目录下保存运行的时间点。

### 任务配置

{% highlight text %}
SHELL=/bin/sh
PATH=/sbin:/bin:/usr/sbin:/usr/bin
MAILTO=root
# the maximal random delay added to the base delay of the jobs
RANDOM_DELAY=45
# the jobs will be started during the following hours only
START_HOURS_RANGE=3-22

#period in days   delay in minutes   job-identifier   command
1                 5                  cron.daily       nice run-parts /etc/cron.daily
7                 25                 cron.weekly      nice run-parts /etc/cron.weekly
@monthly          45                 cron.monthly     nice run-parts /etc/cron.monthly
{% endhighlight %}

其中开头定义了一些环境变量等信息，而配置项的含义如下：

1. period in days：指定指令执行的周期，即指定任务在多少天内执行一次，也可以使用宏定义，如 @day、@weekly、@monthly。

2. delay in minutes：当命令已经就绪后，并非立即执行，而是要延迟等待一段时间。

3. job-identifier：每次启动时都会在 /var/spool/anacron 里面建立一个以 job-identifier 为文件名的文件，记录着任务完成的时间。

4. command：要运行的命令，其中 run-parts 用来运行整个目录的可执行程序。

### 命令详解

实际上，anacron 也是一个 cron 任务，可以通过 ls /etc/cron*/*anacron 查看。通常是通过 anacron -s 执行，以 anacron -s cron.daily 为例，会有如下的步骤：

1. 由配置文件 /etc/anacrontab 解析到 cron.daily 这项工作名称的执行周期为一天。

2. 从 /var/spool/anacron/cron.daily 取出最近一次运行 anacron 的时间戳。

3. 把取出的时间戳与当前的时间戳相比较，如果差异超过了一天，那么就准备进行命令。

4. 若准备执行命令，根据 /etc/anacrontab 的配置，计算延迟的时间。

5. 延迟时间后，开始运行后续命令，也就是 run-parts  /etc/cron.daily 这串命令。

6. 运行完毕后，anacron 程序结束。

另外，需要注意的是，命令执行通常为串行执行。



## 示例

在配置 crontab 任务时，有几个特殊的符号：A) ```"*"``` 所有的取值范围内的数字；B) ```"/"``` 每的意思，如 ```"*/5"``` 表示每 5 个单位；C) ```"-"``` 从某个数字到某个数字；D) ```","``` 用来分开几个离散的数字。

以下举几个例子说明问题：
{% highlight text %}
*/10 * * * *     echo "Ten minutes ago." >> /tmp/foo.txt    // 每十分钟执行一次
0 6 * * *        echo "Good morning." >> /tmp/foo.txt       // 每天早上6点
0 */2 * * *      echo "Have a break now." >> /tmp/foo.txt   // 每两个小时
45 4 1,10,22 * * echo "Restart server." >> /tmp/foo.txt     // 每月1、10、22日的4:45
0 23-7/2,8 * * * echo "Have a good dream." >> /tmp/foo.txt  // 晚上11点到早上8点之间每两个小时，早上八点
0 11 4 * 1-3     echo "Just kidding." >> /tmp/foo.txt       // 每月4号和每周的周一到周三的早上11点
45 11 * * 0,6    echo "Have a good lunch." >> /tmp/foo.txt  // 每周六、周日的11点45分
0 9 * * 1-5      echo "Work hard." >> /tmp/foo.txt          // 从周一到周五的9点
2 8-16/3 * * *   echo "Some examples." >> /tmp/foo.txt      // 8:02、11:02、14:02执行

0,30 18-23 * * *    echo "Same." >> /tmp/foo.txt            // 每天18:00到23:00之间每隔30分钟
0-59/30 18-23 * * * echo "Same." >> /tmp/foo.txt            // 同上
*/30 18-23 * * *    echo "Same." >> /tmp/foo.txt            // 同上
{% endhighlight %}
<!--
at -f test-cron.sh -v 10:25    // -f指定脚本文件，-v指定运行时间
-->

另外，需要注意的几点：

1. 可以在 crontab 的命令中使用环境变量，如：```*/1 * * * * echo $HOME``` 。

2. 第三个域和第五个域是 "或" 操作。

通常，使用 crontab -e 进行的配置是针对某个用户的，而编辑 /etc/crontab 是针对系统的任务。


### 特殊用法

除了上述的写法外，crontab 提供了一些简单的时间定义方法，如：

{% highlight text %}
@daily         echo "Hi" >> /tmp/foo.txt
{% endhighlight %}

除此之外还有如下类似的类型，可以通过 man 5 crontab 查看。

{% highlight text %}
@reboot    :    Run once after reboot.
@yearly    :    Run once a year, ie.  "0 0 1 1 *".
@annually  :    Run once a year, ie.  "0 0 1 1 *".
@monthly   :    Run once a month, ie. "0 0 1 * *".
@weekly    :    Run once a week, ie.  "0 0 * * 0".
@daily     :    Run once a day, ie.   "0 0 * * *".
@hourly    :    Run once an hour, ie. "0 * * * *".
{% endhighlight %}

<!--
 SHELL=/bin/bash
 PATH=/sbin:/bin:/usr/sbin:/usr/bin
 MAILTO=root      //如果出现错误，或者有数据输出，数据作为邮件发给这个帐号
 HOME=/    //使用者运行的路径,这里是根目录

# run-parts
01 * * * * root run-parts /etc/cron.hourly //每小时执行/etc/cron.hourly内的脚本
02 4 * * * root run-parts /etc/cron.daily //每天执行/etc/cron.daily内的脚本
22 4 * * 0 root run-parts /etc/cron.weekly //每星期执行/etc/cron.weekly内的脚本
42 4 1 * * root run-parts /etc/cron.monthly //每月去执行/etc/cron.monthly内的脚本
大家注意"run-parts"这个参数了，如果去掉这个参数的话，后面就可以写要运行的某个脚本名，而不是文件夹名了。
-->


### 常用技巧

可以通过如下命令查看所有用户的 cron 定时任务，注意需要使用 root 权限。

{% highlight bash %}
for user in $(cut -f1 -d: /etc/passwd); do echo "### Crontabs for $user ####"; crontab -u $user -l; done
{% endhighlight %}

需要注意的是，上述脚本只能显示 user 的 crontabs，如果要查看所有的还需要解析 /etc/crontab、/etc/crontab.d、/etc/cron.daily 等配置文件中的任务。



## '%' 导致的问题

例如写个了一个 shell 脚本，其中参数中使用了 '%'，那么在 cron 任务中就可能会执行错误，或者与预期的不符。为简单起见只是将传入的参数保存在 /tmp/foobar.log 中，脚本文件为 /tmp/foobar.sh，其内容如下：

{% highlight bash %}
#!/bin/bash
echo $1 >> /tmp/foobar.log
{% endhighlight %}

然后我们新建一个 cron 任务，内容如下：

{% highlight text %}
*/1 * * * * /tmp/foobar.sh "`date '+%Y-%m-%d %H:%M:%S'`" > /dev/null 2>&1
{% endhighlight %}

正常来说在 /tmp/foobar.log 中会每隔 1 分钟打印一条日志，而实际上却没有。查看 /var/log/cron 日志可以发现，实际执行的命令是 /tmp/foobar.sh `date +，汗 ^_^""

实际上，在 cron 任务中，'%' 是有特殊意义的，在这里需要转义，通过 man 5 crontab 查看帮助，可以看到如下内容：

> A "%" character in the command, unless escaped with a backslash (\\), will be changed into newline characters, and all data after the first % will be sent to the command as standard input.

也就是说，如果 % 没有通过 \ 转义，那么就会被替换成换行，上述命令的正确打开姿势是：
{% highlight text %}
*/1 * * * * /tmp/foobar.sh "`date '+\%Y-\%m-\%d \%H:\%M:\%S'`" > /dev/null 2>&1
{% endhighlight %}


## 输出字符引发的血案

现象是，登陆一台公用的跳板机时，当尝试从个人帐号切换到公用帐号时发现报错，是由于进程数超过了最大限制 (ulimit -u)，然后通过 ps aux 发现有很多进程，其中比较多的是如下的两条命令：
{% highlight text %}
/usr/sbin/postdrop -r
/usr/sbin/sendmail -FCronDaemon -i -odi -oem -oi -t -f root
{% endhighlight %}

基本可以确定是邮件的发送服务导致的，那么是什么程序调用的呢？通过 pstree 发现，其父进程为 crond 。

然后，通过 du -i 查看，发现 /var 的 inode 数目使用了 100%，通过如下命令查看根目录下的文件数目，也就是大约每个子目录中 inode 的数目。
{% highlight text %}
$ for dir in `ls -1Ad /*`; do echo -e "${dir} \t\t `find ${dir} | wc -l`"; done
{% endhighlight %}

然后，可以逐层向下查找，最后可以发现是 /var/spool/postfix/maildrop 目录下有大量的文件。通过 file 命令查看是 data 类型，实际上该目录下的文件可以通过 strings 命令查看，其内容为 crond 发送的邮件。

大概定位到了原因，先恢复掉。kill 掉所有 postdrop、sendmail 进程，然后清空 maildrop 目录下的文件。此时如果直接通过 rm * -f 删除，会由于文件过多而报错，可以通过如下两种方式进行删除。
{% highlight text %}
# find /tmp -type f -exec rm {} \;
# ls | xargs rm -vf
{% endhighlight %}

OK，环境已经恢复，那么具体是什么原因导致的呢？

查看 cronie 的源码可以发现，crond 会 fork 一个子进程去执行任务，而该进程有会再 fork 一个孙子进程执行真正的命令，代码的调用逻辑如下。
{% highlight text %}
main()                       # C入口函数
 |-job_runqueue()            # 执行队列中的命令
   |-do_command()            # 此时会fork一个子进程执行，主进程继续工作
     |-child_process()       # 再fork一个进程，通过execle执行shell命令
       |-execle()            # 执行真正的shell指令，在孙子进程中执行
{% endhighlight %}
在 child_process() 函数中通过 fork() 子进程前，会创建两个管道 (stdin+stdout) 用来与子进程通讯，分别表示输入和输出。默认情况下，crontab 会将命令执行的输出，通过邮件发送给用户。

而在查看 /var/log/maillog 日志发现，是由于 pickup 管道不存在，导致邮件一直在积压。

通常来说，我们不需要发送邮件，为了防止上述问题的发生，可以配置运行脚本输出为 >/dev/null 2>&1，来避免 crontab 运行中有内容输出。
{% highlight text %}
0 * * * * /path/to/script.sh    > /dev/null
0 * * * * /path/to/script.sh    > /dev/null 2>&1
0 * * * * /path/to/command arg1 > /dev/null 2>&1 || true
{% endhighlight %}

或者直接在 crontab 文件的顶部设置 MAILTO 变量为空，也就是 ```MAILTO=""``` 。


当然，如果有必要，可以将输出发送到指定邮箱，但是需要首先确保邮件服务是正常的，然后添加如下配置项：
{% highlight text %}
MAILTO="ooops@foobar.com"
0 3 * * * echo "Helloooo!"
{% endhighlight %}

## 参考

源码可以从 [cronie project][cronie official site] 官方网站下载，相关的帮助可以查看 man 1 crontab (命令行语法相关)、man 5 crontab (配置文件相关)。


<!-- images -->
[mathematic-clock]:    /images/linux/cronie-mathematic-clock.jpg    "数字钟"
[legendre-const]:      /images/linux/cronie-legendre.png            "勒让德常数"

<!-- URLs -->
[cronie official site]: https://fedorahosted.org/cronie/

