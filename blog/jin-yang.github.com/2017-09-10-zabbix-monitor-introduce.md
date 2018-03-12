---
title: Zabbix 监控系统
layout: post
comments: true
language: chinese
category: [misc]
keywords: zabbit,monitor,introduce,监控,简介
description:
---

Zabbix 能监视各种网络、系统参数，保证服务器系统的安全运行，同时提供了灵活的 API 接口，例如通知机制，可以让系统管理员及时通过或者当前的异常状态，支持 Agentd、SNMP、JMX 等多种监控方式。

同时，提供了一个 WEB 界面，可以作为分布式系统监控以及网络监控的企业级开源解决方案。

<!-- more -->

![zabbix logo]({{ site.url }}/images/linux/zabbix-logo.png "zabbix logo"){: .pull-center }



<!--
usage: zabbix_sender [-Vhv] {[-zpsI] -ko | [-zpI] -T -i <file> -r} [-c <file>]

  -c --config <file>                   配置文件绝对路径
  -z --zabbix-server <server>          zabbix server的IP地址
  -p --port <server port>              zabbix server端口，默认10051
  -s --host <hostname>                 主机名，需要保证与agent配置文件内容保持一致
  -k --key <key>                       监控项的key
  -o --value <key value>               key值
  
  -I --source-address <IP address>     源IP
  -i --input-file <input file>         从文件里面读取hostname、key、value 一行为一条数据，使用空格作为分隔符，如果主机名带空格，那么请使用双引号包起来
  -T --with-timestamps              一行一条数据，空格作为分隔符: <hostname> <key> <timestamp> <value>，配合 --input-file option，timestamp为unix时间戳
  -r --real-time                      将数据实时提交给服务器
  -v --verbose                         详细模式, -vv 更详细

zabbix_sender -vv -c /etc/zabbix/zabbix_agentd.conf -z 127.0.0.1 -k 'foo.bar' -o 12

 ./zabbix_sender -s 127.0.0.1 -z 127.0.0.1 -k "ttlsa.trapper" -o 1 -r 

/etc/zabbix/zabbix_agentd.conf


https://www.zabbix.com/download
https://repo.zabbix.com/zabbix/3.0/rhel/7/SRPMS/
https://sourceforge.net/projects/zabbix/files/?source=navbar
-->

## 安装

安装详细可以参考 [zabbix manual install](https://www.zabbix.com/documentation/3.0/manual/installation)，二进制代码库下载地址可以参考 [zabbix download](https://www.zabbix.com/download)，也可以参考 [zabbix repos](http://repo.zabbix.com/zabbix/3.0/rhel/7/x86_64/) 。



#### 源码编译

{% highlight text %}
$ ./configure --prefix=/usr/local/zabbix                     \
    --enable-server --enable-agent                           \
    --with-mysql --with-net-snmp --with-libcurl
{% endhighlight %}

### 服务端安装配置

这里同样以 MySQL 作为后端存储为例。

{% highlight text %}
----- 1. 安装依赖包，只列举的部分
# yum install fping OpenIPMI-libs iksemel php-mysql httpd php

----- 2. 安装服务和前端，需要安装一堆的依赖
# rpm -ivh zabbix-server-mysql-3.0.5-1.el7.x86_64.rpm
# rpm -ivh zabbix-web-mysql-3.0.5-1.el7.noarch.rpm

----- 3. 配置MySQL数据库，并创建表
shell> mysql -uroot -p<root_password>
mysql> create database zabbix character set utf8 collate utf8_bin;
mysql> grant all privileges on zabbix.* to zabbix@localhost identified by 'Huawei@123';
mysql> quit;
shell> zcat /usr/share/doc/zabbix-server-mysql-3.0.*/create.sql.gz | mysql -uzabbix -p zabbix

----- 4. 服务端配置
# vi /etc/zabbix/zabbix_server.conf
DBHost=localhost
DBName=zabbix
DBUser=zabbix
DBPassword=Huawei@123

----- 5. 启动服务
# systemctl start zabbix-server

----- 6. 设置时区为某个地区
# vim /etc/php.ini
date.timezone = Asia/Shanghai
{% endhighlight %}

其中服务器使用 httpd，此时需要修改配置文件 `/etc/httpd/conf.d/zabbix.conf` 。 <!-- php配置文件，保存在mod_php5.c中 -->

接着需要配置界面，配置完成之后会保存在 `/etc/zabbix/web/zabbix.conf.php` 文件中，默认的登陆帐号可以使用 `Admin/zabbix` 。

### 客户端安装

这里同样以 RPM 包安装为例。

{% highlight text %}
----- 1. 安装客户端
# rpm -ivh zabbix-agent-3.0.5-1.el7.x86_64.rpm

----- 2. 启动客户端
# systemctl start zabbix-agent

----- 3. 可以查看其启动日志内容
# less /var/log/zabbix/zabbix_agentd.log

----- 4. 修改配置文件
# vim /etc/zabbix/zabbix_agentd.conf
Server=127.0.0.1           被动服务IP，允许这个IP来我这里取数据
ServerActive=127.0.0.1     主动提交数据给这个IP
Hostname=10.120.185.240    需要保证和Zabbix Web中的配置保持相同
{% endhighlight %}

## 特性

### 监控项 Items

也就是添加一个主机监控项，`Configration->Hosts->Items` ，一般会从模板中继承一部分内容。

一般 item 通过 Key 定义，然后其中包括了具体的参数，通常是两个，例如 `system.cpu.load[percpu,avg1]`；在 agent 端包括了很多常用的 key，可以参考 [Zabbix agent](https://www.zabbix.com/documentation/3.0/manual/config/items/itemtypes/zabbix_agent)。

对于自定义的 key 需要在客户端的配置文件 `zabbix_agentd.conf` 中启用 `UnsafeUserParameters=1` 参数，然后在配置文件的最下面来定义 key，如：

{% highlight text %}
UnsafeUserParameters=1       # 并去掉前面的注释符
UserParameter=foo.bar[*], /usr/local/script/monitor.sh $1 $2
{% endhighlight %}

其中，`foo.bar[*]` 定义了 key ，而后面对应了监控程序的绝对路径，通常使用的是通用脚本，然后根据具体的参数决定输出内容，通常参数顺序为 `$1~$9` 。

### 触发器 Triggers

一般用于告警，当 Items 监控项采集值满足 triggers 指定的触发条件时，就会产生 actions 。

同样在 `Configuration->Hosts->Triggers` 中进行配置，在修改时有几条规则：1) template 中的触发值不能单独修改，必须在 template 中修改；2) 可以复制一个同样 trigger 再修改，再禁用模板中的 trigger；3) 也可以通过右上角的 Create trigger 创建一个。

关于 Trigger 的表达式语法信息可以查看 [Zabbix Trigger Expression](https://www.zabbix.com/documentation/3.0/manual/config/triggers/expression) 。

常见示例：

{% highlight text %}
---- 主机宕机检测，持续3min没有ping数据时报警
{test-01:agent.ping.nodata(3m)}=1

---- 连续3分钟的平均值大于80%，或者用max表示一直大于80%
{test-01:system.cpu.util[,idle].avg(3m)}<20
{% endhighlight %}

<!--
手册里边有个"TRIGGER.VALUE"宏，看来得从这里下下手.

TRIGGER.VALUE对应的为Trigger状态，0代表OK, 1代表Problem，分解下需求:

正常情况下连续三分钟CPU使用率超过80%，看起来表达式是:

{TRIGGER.VALUE}=0&{test-01:system.cpu.util[,idle].max(3m)}<20
故障时连续三分钟CPU使用率低于50%恢复正常，即故障时刻CPU使用率持续三分钟高于50%依然为故障，表达式是这个样子的:

{TRIGGER.VALUE}=1&{test-01:system.cpu.util[,idle].min(3m)}<50
然后整合下表达式，就成了下边这个样子:

({TRIGGER.VALUE}=0&{test-01:system.cpu.util[,idle].max(3m)}<20) | ({TRIGGER.VALUE}=1&{test-01:system.cpu.util[,idle].min(3m)}<50)
-->


### 宏定义 Macro

包括了系统内置的宏，一般格式为 `{MACRO}`，可以通过类似 `{HOST.NAME}` 的方式进行引用，例如告警中，详细可以参考 [Macros supported by location](https://www.zabbix.com/documentation/3.0/manual/appendix/macros/supported_by_location) 。

自定义宏通过 `{$MACRO}` 类似方式表示，也就是需要加 `$` 才能引用；*注意* 在调用和定义时都需要加 `$` ，而且 zabbix 还支持在全局、模板或主机级别使用用户自定义宏。

解析时的顺序，首先解析主机，然后按照模板 ID 的顺序依次解析宏定义，最后会解析全局的配置，如果仍然无法找到，那么就不会解析。

常见的使用场景有：

1. `item key` 参数，如 `item.key[server_{HOST.HOST}_local]` 。
2. `trigger` 表达式中。
3. `Low-Level-Discovery` 中。

另外，需要注意的是，宏的名称只能使用 `A-Z, 0-9, _, .`，可以通过如下方式配置。

{% highlight text %}
全局宏定义：
    Administrator-->General-->Macros(右上角选择框) 保存在globalmacro表中
主机/模板宏定义，两者基本类似：
    Configuration-->[Hosts|Templates]-->Macros 保存在hosts以及hostmacro表中
{% endhighlight %}

### 图表展示 Graphs/Screens

Graphs 可用于将多个监控项展示在同一个图表中，然后一个 Screens 中可以用来展示多个 Graphs 。

### 自动发现

随着被监控的主机不断增加，如果纯手工配置，那么工作量将会非常大，这时就可以使用 `Discovery rules` 来配置。通过该模块，可以实现自动发现主机、自动将主机添加到主机组、自动加载模板、自动创建监控项、自动创建图像。

<!--
http://club.oneapm.com/t/zabbix-discovery/515
http://www.ttlsa.com/zabbix/zabbix-low-level-discover/

LLD
1. Configration->Template 新建一个模板
--->


<!--
## 其它

https://www.zabbix.org/wiki/Docs/howto/mysql_partition
http://www.ttlsa.com/zabbix/use-wechat-send-zabbix-msg/
http://www.ttlsa.com/news/google-proxy-latest-2015-08/

select name, key_ from items where key_ like 'system.cpu.load%';

hosts/groups/hosts_groups   用于维护主机和分组的关系。
    hosts        维护主机信息。
 groups       维护分组信息。
    hosts_groups 用于找到组中的主机，以及通过主机ID找到对应的分组。
items/graphs/graphs_items   采集项与展示图之间的关系。


item_condition macro

timer_thread()     main_timer_loop时间相关的处理
 |-process_time_functions()
 | |-DCconfig_get_time_based_triggers() 从缓存中获取trigger表达式
 | |-evaluate_expressions()       trigger expression的主要处理函数
     |-substitute_simple_macros() 宏分为两类，分别是{...} {$...}
  |-substitute_functions()
       |-zbx_evaluate_item_functions()
         |-evaluate_function()
 | |-process_triggers()
 | | |-process_trigger()
 | |   |-add_event()              会保存到内存的events数组中
 | |-process_events()
 |   |-save_events()
 |   |-process_actions()
 |   |-clean_events()
 |-process_maintenance()

substitute_functions()

可以看到一堆的以 zbx_vector_ 开头的函数，其函数声明在 `include/zbxalgo.h` 文件中，而对应的代码实现在 `src/libs/zbxalgo/vectorimpl.h` 文件中。

parse_commandline()   会根据命令行传入的参数设置t.task，一般默认为ZBX_TASK_START
init_metrics()  初始化内置的监控指标
  |-zbx_load_config()  默认会执行switch的default分支，加载配置
  |-load_user_parameters()
  |-START_MAIN_ZABBIX_ENTRY()  一般是设置为daemon，然后设置信号处理句柄
    |-zbx_set_common_signal_handlers()
 |-MAIN_ZABBIX_ENTRY()
   |-load_modules()  没有使用 <del>
   |-zbx_tcp_listen() 后台不使用 <del>
   |-init_collector_data() 初始化采集item
   |-zbx_free_config()
   |-zbx_thread_start()     collector_thread
     |-collector_thread()   每秒采集一次数据，src/zabbix_agent/stats.c
    |-collect_stats_diskdevices() 保存数据到共享内存，通过semop加锁
   |-zbx_thread_start()     listener_thread
     |-listener_thread()    开启监听端口，src/zabbix_agent/listener.c
       |-zbx_tcp_accept()   接收请求
    |-process_listener()
      |-zbx_tcp_recv_to()
   |-init_result()
   |-process()        src/libs/zbxsysinfo/sysinfo.c
   |-zbx_tcp_send_to()
   |-zbx_thread_start()     active_checks_thread
     |-active_checks_thread()
       |-init_active_metrics()

http://www.cnblogs.com/lixiaofei1987/p/3208414.html


## 其它

https://www.zabbix.org/wiki/Docs/howto/mysql_partition
http://www.ttlsa.com/zabbix/use-wechat-send-zabbix-msg/
http://www.ttlsa.com/news/google-proxy-latest-2015-08/

select name, key_ from items where key_ like 'system.cpu.load%';

hosts/groups/hosts_groups   用于维护主机和分组的关系。
    hosts        维护主机信息。
 groups       维护分组信息。
    hosts_groups 用于找到组中的主机，以及通过主机ID找到对应的分组。
items/graphs/graphs_items   采集项与展示图之间的关系。


item_condition macro

timer_thread()     main_timer_loop时间相关的处理
 |-process_time_functions()
 | |-DCconfig_get_time_based_triggers() 从缓存中获取trigger表达式
 | |-evaluate_expressions()       trigger expression的主要处理函数
     |-substitute_simple_macros() 宏分为两类，分别是{...} {$...}
  |-substitute_functions()
       |-zbx_evaluate_item_functions()
         |-evaluate_function()
 | |-process_triggers()
 | | |-process_trigger()
 | |   |-add_event()              会保存到内存的events数组中
 | |-process_events()
 |   |-save_events()
 |   |-process_actions()
 |   |-clean_events()
 |-process_maintenance()

substitute_functions()

可以看到一堆的以 zbx_vector_ 开头的函数，其函数声明在 `include/zbxalgo.h` 文件中，而对应的代码实现在 `src/libs/zbxalgo/vectorimpl.h` 文件中。

parse_commandline()   会根据命令行传入的参数设置t.task，一般默认为ZBX_TASK_START
init_metrics()  初始化内置的监控指标
  |-zbx_load_config()  默认会执行switch的default分支，加载配置
  |-load_user_parameters()
  |-START_MAIN_ZABBIX_ENTRY()  一般是设置为daemon，然后设置信号处理句柄
    |-zbx_set_common_signal_handlers()
 |-MAIN_ZABBIX_ENTRY()
   |-load_modules()  没有使用 <del>
   |-zbx_tcp_listen() 后台不使用 <del>
   |-init_collector_data() 初始化采集item
   |-zbx_free_config()
   |-zbx_thread_start()     collector_thread
     |-collector_thread()   每秒采集一次数据，src/zabbix_agent/stats.c
    |-collect_stats_diskdevices() 保存数据到共享内存，通过semop加锁
   |-zbx_thread_start()     listener_thread
     |-listener_thread()    开启监听端口，src/zabbix_agent/listener.c
       |-zbx_tcp_accept()   接收请求
    |-process_listener()
      |-zbx_tcp_recv_to()
   |-init_result()
   |-process()        src/libs/zbxsysinfo/sysinfo.c
   |-zbx_tcp_send_to()
   |-zbx_thread_start()     active_checks_thread
     |-active_checks_thread()
       |-init_active_metrics()

http://www.cnblogs.com/lixiaofei1987/p/3208414.html

### 添加主机

Configuration -> Hosts -> [Create Host]

1. Host 主要是Agent的接口配置。
2. Templates 选择Template OS Linux作为模板。

## 常见问题

no active checks on server [SVRIP:10051]: host [CLI] not found

一般是 zabbix_agentd.conf 里面的 Hostname 和前端 Zabbix web 里面的配置 (Configuration->Hosts->[Host name]) 不一样所造成的，修改完成后重启 agent 即可。



json_reformat

zabbix配置内容比较多，我们要分为9大块来讲解。分别如下：
1. Host/HostGroup 主机与组。主要用于主机管理，通过主机组可以进行逻辑分组。
2. Item 监控项。主机监控的指标，例如服务器负载可以作为一个监控项。
3. Trigger 触发器。


什么情况下出发什么事情，称之为触发器。例如：定义如果系统负载大于10那么报警，这个东西可以称之为触发器。
4、事件
触发器状态变更、Discovery事件等
5、可视化配置
图表配置，讲监控的数据绘制成曲线图。或者在一个屏幕中可以看到某台主机所有监控图表。
6、模板配置
自定义监控模板。例如Template OS Linux
7、报警配置
配置报警介质：邮箱、sms以及什么情况下发送报警通知。
8、宏变量
用户自定义变量，很有用的一个功能。



配置可以参考如下：
Host groups（主机组）→Hosts（主机）→Applications（监控项组）→Items（监控项）→Triggers（触发器）→Event（事件）→Actions（处理动作）→User groups（用户组）→Users（用户）→Medias（告警方式）→Audit（日志审计）
http://www.xuliangwei.com/xubusi/175.html
http://www.cnblogs.com/ningskyer/articles/5546894.html


https://xiaofandh12.github.io/Zabbix-Server
https://xiaofandh12.github.io/Zabbix-Server-poller
http://blog.csdn.net/liujian0616/article/details/7946492
http://blog.51cto.com/john88wang/1833636
https://www.ibm.com/developerworks/cn/linux/l-lexyac.html
-->

## 参考



{% highlight text %}
{% endhighlight %}
