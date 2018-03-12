---
title: Linux 通讯协议
layout: post
comments: true
language: chinese
category: [linux,misc]
keywords: linux,network,snmp
description: 简单记录下 Linux 常见的通讯协议，如 SNMP 协议。
---

简单记录下 Linux 常见的通讯协议，如 SNMP 协议。

<!-- more -->

## SNMP

简单网络管理协议 (Simple Network Management Protocol, SNMP) 是一种应用层协议，同样也是 TCP/IP 协议族的一部分，用于网络设备之间交换管理信息，所谓的管理也就主要是监控 (get) 和配置 (set) 一些对象。

这里管理的对象通过对象标示符 (Object Identifiers, OID) 标示，不同的采集对象对应不同的 OID，例如系统磁盘的信息用 OID ```"1.3.6.1.4.1.2021.9"``` 标示。这串数字是国际标准化组织协商定义好的，通过管理信息库 (MIB) 保存，可以将其简单视为一个树结构。

### 安装测试

在 CentOS 中，可以通过如下方式安装服务端以及常用命令。

{% highlight text %}
# yum install net-snmp net-snmp-utils
# systemctl start snmpd
{% endhighlight %}

很多人会使用 SNMP V3 的授权机制(V1+V2c不支持加密)，注意，SNMP V1 在性能以及支持的数据类型都比较受限；V2c 和 V3 的数据格式相同，只是 V3 提供了加密功能。

接下来简单介绍下 snmpwalk 命令。

{% highlight text %}
命令参数：
  -c <string>
    指定Community String，类似于用户名或者密码，如果正确则响应，用于 V1+V2c 版本，
    V3 使用用户名+密码，一般默认会有一个只读的 public ；
  -v (1|2c|3)
    指定SNMP的版本号；

----- 查看系统信息，返回结果包括了MIB信息，可以通过该信息指定获取具体的内容
$ snmpwalk -v 2c -c public 127.1 system
$ snmpwalk -v 2c -c public 127.1 system.sysDescr.0
{% endhighlight %}

<!--
https://kb.op5.com/display/HOWTOs/Configure+a+Linux+server+for+SNMP+monitoring
https://www.ibm.com/developerworks/cn/linux/l-cn-snmp/
iReasoning MIB Browser 启用SNMP的网络设备。
-->






{% highlight text %}
{% endhighlight %}
