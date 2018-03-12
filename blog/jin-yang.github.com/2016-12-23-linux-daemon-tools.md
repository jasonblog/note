---
title: Linux 后台服务管理
layout: post
comments: true
language: chinese
category: [linux,misc]
keywords: 服务管理,monit,supervisor,goreman
description: 在 Linux 中，对于一个后台服务，如果要可靠地在后台运行，最好能监控进程状态，在意外结束时能自动重启。常见的功能全面的主流工具主要有 monit(C)、supervisor(Python)、goreman(Go)，其中 goreman 是对 Ruby 下广泛使用的 foreman 工具，使用 golang 的重写。在此，仅介绍下在 Linux 中如何使用。
---

在 Linux 中，对于一个后台服务，如果要可靠地在后台运行，最好能监控进程状态，在意外结束时能自动重启。

常见的功能全面的主流工具主要有 monit(C)、supervisor(Python)、goreman(Go)，其中 goreman 是对 Ruby 下广泛使用的 foreman 工具，使用 golang 的重写。

在此，仅介绍下在 Linux 中如何使用。

<!-- more -->

## Monit

![monit logo]({{ site.url }}/images/linux/monit-logo.png "monit logo"){: .pull-center width="40%" }

[Monit](http://mmonit.com/monit/) 是一款功能非常丰富的进程、文件、目录和设备的监测软件，通过 C 语言开发，是一款轻量级的后台管理服务，适用于 *nix 平台。

它可以自动修复那些已经停止运作的程序，特使适合处理那些由于多种原因导致的软件错误，同时 Monit 包含一个内嵌的 HTTP(S) Web 界面，你可以使用浏览器方便地查看 Monit 所监视的服务器。

在 CentOS 中，可以直接通过如下命令安装。

{% highlight text %}
# yum --enablerepo=epel install monit
{% endhighlight %}

如果启动时不使用 arguments 参数，则会直接作为服务进程启动，否则尝试链接 127.0.0.1:2812 并执行相应的命令；简单列举下 monit 启动是相关的命令，以及参数。

{% highlight text %}
monit [options] {arguments}

常用参数：
  -I 前端执行，默认会自动切换为后台执行；
  -t 对配置文件执行参数检查；
  -c 指定配置文件；
  -g
  -i 查看monit的ID，第一次会自动生成并保存到~/.monit.id文件中；
  -r 重新生成ID；
  -H [file] 生成指定文件的SHA1和MD5校验值；

常用参数：
  status [name]
    查看某个服务的状态，如果不使用name，则会打印所有服务状态；
  summary [name]
    打印概览；
  quit
    退出服务；

常用命令：
  monit -I -c monitrc
  monit -c monitrc quit
  monit -c monitrc reload
{% endhighlight %}

<!--
      start all
           Start all services listed in the control file and enable monitoring for them. If the group option is set (-g), only start and enable monitoring of services in the named group ("all" is not
           required in this case).

       start name
           Start the named service and enable monitoring for it. The name is a service entry name from the monitrc file.

       stop all
           Stop all services listed in the control file and disable their monitoring. If the group option is set, only stop and disable monitoring of the services in the named group (all" is not required
           in this case).

       stop name
           Stop the named service and disable its monitoring. The name is a service entry name from the monitrc file.

       restart all
           Stop and start all services. If the group option is set, only restart the services in the named group ("all" is not required in this case).

       restart name
           Restart the named service. The name is a service entry name from the monitrc file.

       monitor all
           Enable monitoring of all services listed in the control file. If the group option is set, only start monitoring of services in the named group ("all" is not required in this case).

       monitor name
           Enable monitoring of the named service. The name is a service entry name from the monitrc file. Monit will also enable monitoring of all services this service depends on.

       unmonitor all
           Disable monitoring of all services listed in the control file. If the group option is set, only disable monitoring of services in the named group ("all" is not required in this case).

       unmonitor name
           Disable monitoring of the named service. The name is a service entry name from the monitrc file. Monit will also disable monitoring of all services that depends on this service.

       reload
           Reinitialize a running Monit daemon, the daemon will reread its configuration, close and reopen log files.

       validate
           Check all services listed in the control file. This action is also the default behavior when Monit runs in daemon mode.

       procmatch regex
           Allows for easy testing of pattern for process match check. The command takes regular expression as an argument and displays all running processes matching the pattern.
-->

### 配置

#### 启动内置httpd服务

其中 status、summary、report 指令，需要启动 httpd 服务才可以，否则无法建立链接。

{% highlight text %}
set httpd port 2812 and
    use address localhost  # only accept connection from localhost
    allow localhost        # allow localhost to connect to the server and
    allow admin:monit      # require user 'admin' with password 'monit'
{% endhighlight %}


<!--
### 源码编译

{% highlight text %}
./configure
man -l monit.1 查看帮助信息
{% endhighlight %}

所有的服务都保存在 servicelist 链表中，会在 p.y 做语法解析时进行初始化。

{% highlight text %}
do_action()
 |-do_default()      启动服务
   |
   |                 在while循环中执行
   |-validate()
     |-Event_queue_process()
{% endhighlight %}



## Supervisor
## Goreman
-->

## 参考

[Monit Offical](http://mmonit.com/monit/)，可以查看官方文档 [Monit Reference](https://mmonit.com/monit/documentation/monit.html)，也可以直接查看 man 。

{% highlight text %}
{% endhighlight %}
