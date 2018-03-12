---
title: 容器之 CGroup
layout: post
comments: true
language: chinese
category: [linux,misc]
keywords: linux,cgroup,container
description:
---

在 CentOS 7 中，已经通过 systemd 替换了之前的 cgroup-tools 工具，为了防止两者冲突，建议只使用 systemd ，只有对于一些不支持的，例如 net_prio ，才使用 cgroup-tools 工具。

在此，简单介绍其使用。

<!-- more -->

## 简介

在系统设计时，经常会遇到类似的需求，就是希望能限制某个或者某些进程的分配资源。

由此，就有了容器的概念，在容器中，有分配好的特定比例的 CPU、IO、内存、网络等资源，这就是 controller group ，简称为 cgroup ，最初由 Google 工程师提出，后来被整合进 Linux 内核中。

cgroup 本身提供了将进程进行分组化管理的功能和接口的基础结构。

## 使用简介

在 CentOS 7 中需要通过 ```yum install libcgroup libcgroup-tools``` 安装额外的 cgroup 工具，对系统来说，默认会挂载到 ```/sys/fs/cgroup/``` 目录下。

{% highlight text %}
----- 查看系统已经存在cgroup子系统及其挂载点
# lssubsys -am
----- 或者通过mount查看cgroup类型的挂载点
# mount -t cgroup

----- 可以命令行挂载和卸载子系统，此时再次执行上述命令将看不到memory挂载点
# umount /sys/fs/cgroup/memory/
----- 挂载cgroup的memory子系统，其中最后的cgroup参数是在/proc/mounts中显示的名称
# mount -t cgroup -o memory cgroup /sys/fs/cgroup/memory/
# mount -t cgroup -o memory none /sys/fs/cgroup/memory/
{% endhighlight %}

另外，在 CentOS 中有 ```/etc/cgconfig.conf``` 配置文件，该文件中可用来配置开机自动启动时挂载的条目：

{% highlight text %}
mount {
    net_prio = /sys/fs/cgroup/net_prio;
}
{% endhighlight %}

然后，通过 ```systemctl restart cgconfig.service``` 重启服务即可，然后可以通过如下方式使用。

### 使用步骤

简单介绍如何通过 ```libcgroup-tools``` 创建分组并设置资源配置参数。

#### 1. 创建控制组群

可以通过如下方式创建以及删除群组，创建后会在 cpu 挂载目录下 ```/sys/fs/cgroup/cpu/``` 目录下看到一个新的目录 test，这个就是新创建的 cpu 子控制组群。

{% highlight text %}
# cgcreate -g cpu:/test
# cgdelete -g cpu:/test
{% endhighlight %}

#### 2. 设置组群参数

```cpu.shares``` 是控制 CPU 的一个属性，更多的属性可以到 ```/sys/fs/cgroup/cpu``` 目录下查看，默认值是 1024，值越大，能获得更多的 CPU 时间。

{% highlight text %}
# cgset -r cpu.shares=512 test
{% endhighlight %}


#### 3. 将进程添加到控制组群

可以直接将需要执行的命令添加到分组中。

{% highlight text %}
----- 直接在cgroup中执行
# cgexec -g cpu:small some-program
----- 将现有的进程添加到cgroup中
# cgclassify -g subsystems:path_to_cgroups pidlist
{% endhighlight %}

例如，想把 sshd 添加到一个分组中，可以通过如下方式操作。

{% highlight text %}
# cgclassify -g cpu:/test `pidof sshd`
# cat /sys/fs/cgroup/cpu/test/tasks
{% endhighlight %}

就会看到相应的进程在这个文件中。

## systemd

CentOS 7 中默认的资源隔离是通过 systemd 进行资源控制的，systemd 内部使用 cgroups 对其下的单元进行资源管理，包括 CPU、BlcokIO 以及 MEM，通过 cgroup 可以 。

systemd 的资源管理主要基于三个单元 service、scope 以及 slice：

* service<br>通过 unit 配置文件定义，包括了一个或者多个进程，可以作为整体启停。
* scope<br>任意进程可以通过 ```fork()``` 方式创建进程，常见的有 session、container 等。
* slice<br>按照层级对 service、scope 组织的运行单元，不单独包含进程资源，进程包含在 service 和 scope 中。

常用的 slice 有 A) ```system.slice```，系统服务进程可能是开机启动或者登陆后手动启动的服务，例如crond、mysqld、nginx等服务；B) ```user.slice``` 用户登陆后的管理，一般为 session；C) ```machine.slice``` 虚机或者容器的管理。

对于 cgroup 默认相关的参数会保存在 ```/sys/fs/cgroup/``` 目录下，当前系统支持的 subsys 可以通过 ```cat /proc/cgroups``` 或者 ```lssubsys``` 查看。

<!--
服务进程限制
    PrivateNetwork=[BOOL] :若服务不需要网络连接可开启本选项，更加安全。
    PrivateTmp=[BOOl] :由于传统/tmp目录是所有本地用户和服务共用，会带来很多安全性问题，开启本选项后，服务将有一个私有的tmp，可防止攻击。
    InaccessibleDirectories= :限制服务进程访问某些目录。
    ReadOnlyDirectories= :设置服务进程对某些目录只读，保证目录下数据不被服务意外撰改。
    OOMScoreAdjust= :调整服务OOM值，从-1000（对该服务进程关闭OOM）到1000(严格)。
    IOSchedulingClass= ：IO调度类型，可设置为0，1,2,3中的某个数值,分配对应none，realtime，betst-effort和idle。
    IOSchedulingPriority= :IO调度优先级，0～7（高到低）。
    CPUSchedulingPriority= :CPU调度优先级，99～1(高到低)
    Nice= :进程调度等级。
-->

### 常见命令

常用命令可以参考如下。

{% highlight text %}
----- 查看slice、scope、service层级关系
# systemd-cgls

----- 当前资源使用情况
# systemd-cgtop

----- 启动一个服务
# systemd-run --unit=name --scope --slice=slice_name command
   unit   用于标示，如果不使用会自动生成一个，通过systemctl会输出；
   scope  默认使用service，该参数指定使用scope ；
   slice  将新启动的service或者scope添加到slice中，默认添加到system.slice，
          也可以添加到已有slice(systemctl -t slice)或者新建一个。
# systemd-run --unit=toptest --slice=test top -b
# systemctl stop toptest

----- 查看当前资源使用状态
$ systemctl show toptest
{% endhighlight %}

各服务配置保存在 ```/usr/lib/systemd/system/``` 目录下，可以通过如下命令设置各个服务的参数。

{% highlight text %}
----- 会自动保存到配置文件中做持久化
# systemctl set-property name parameter=value

----- 只临时修改不做持久化
# systemctl set-property --runtime name property=value

----- 设置CPU和内存使用率
# systemctl set-property httpd.service CPUShares=600 MemoryLimit=500M
{% endhighlight %}

另外，在 systemd 213 版本之后才开始支持 `CPUQuota` 参数，此时可以直接修改 `cpu.cfs_{quota,period}_us` 参数，也就是在 `/sys/fs/cgroup/cpu/` 目录下。

## 参考

关于 systemd 的资源控制，详细可以通过 ```man 5 systemd.resource-control``` 命令查看帮助，或者查看 [systemd.resource-control 中文手册](http://www.jinbuguo.com/systemd/systemd.resource-control.html)；详细的内容可以参考 [Resource Management Guide](https://access.redhat.com/documentation/en-US/Red_Hat_Enterprise_Linux/7/html/Resource_Management_Guide/index.html) 。

<!--
Systemd 进程管理相关
http://fangpeishi.com/systemd_chapter2.html


https://yq.aliyun.com/articles/54458
http://www.cnblogs.com/yanghuahui/p/3751826.html
https://yq.aliyun.com/articles/54483
systemd-cgls



https://github.com/francisbouvier/cgroups
-->

{% highlight text %}
{% endhighlight %}
