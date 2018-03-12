---
title: LXC sshd 单进程启动
layout: post
comments: true
language: chinese
category: [linux,misc]
keywords: linux,lxc,sshd
description: 在之前的 [LXC 简介](/blog/linux-lxc-introduce.html) 中介绍了如何搭建 CentOS 7 容器，在此介绍如何单独启动 sshd 这一个进程。这样，实际上我们可以通过这种方式启动类似 MySQL 之类的服务，从而减小成本。接下来，我们看看是如何启动的。
---

在之前的 [LXC 简介](/blog/linux-lxc-introduce.html) 中介绍了如何搭建 CentOS 7 容器，在此介绍如何单独启动 sshd 这一个进程。这样，实际上我们可以通过这种方式启动类似 MySQL 之类的服务，从而减小成本。

接下来，我们看看是如何启动的。

<!-- more -->

## 启动 sshd

当通过模板创建容器时，```lxc-create``` 实际会调用在 ```/usr/share/lxc/templates``` 中相应的脚本，在此，其中 sshd 会调用 ```lxc-sshd``` 脚本创建，而 ```lxc-create``` 实际会将命令行进行转换，然后传递给脚本，可以在脚本中通过 ```echo $*``` 查看。

### 创建启动镜像

如下是创建和启动镜像的过程。

{% highlight text %}
----- 0. 【主机】创建免密码的公私钥信息，并将公钥上传到容器中
# ssh-keygen -t rsa -f ~/.ssh/id_lxc -N ""

----- 1. 【主机】创建镜像，其中auth-key可选，可以在后面配置
# lxc-create -t sshd -n sshd-test -- --auth-key ~/.ssh/id_lxc.pub

----- 2. 【主机】后台启动镜像
# lxc-start -n sshd-test -l DEBUG -d
{% endhighlight %}

如上，配置保存在 ```/var/lib/lxc/sshd-test/config``` 文件中；启动日志会保存在 ```/var/log/lxc/sshd-test.log``` 中，可以查看详细的日志。

注意使用前需要保证 ```lxcbr0``` 存在，如果不存在则通过如下方式启动。

{% highlight text %}
# brctl addbr lxcbr0
# ifconfig lxcbr0 192.168.2.100
{% endhighlight %}

正常来说，此时应该可以正常提供服务了，不过经常遇到 IP 未设置的情况，此时可以通过如下方式设置。

{% highlight text %}
----- 【主机】查看lxcbr0的IP地址
# ifconfig lxcbr0

----- 【容器】配置IP地址，需要与br在同一个网段
# lxc-attach -n sshd-test
# ifconfig eth0 192.168.2.102

----- 【容器】启动sshd服务器，注意需要是绝对地址，否则报错
# /usr/sbin/sshd

----- 【主机】连接，通过-i指定私钥
# ssh 192.168.2.102 -i ~/.ssh/id_lxc
{% endhighlight %}

在上述设置 IP 时，建议不要在同一个网段，否则路由可能会出现问题，如果这样，那么就需要对路由进行调整。

### 设置公私钥认证

在通过 ```lxc-create``` 创建时，在脚本中实际上可以传入一个 ```auth_key```，也就是一个公钥文件，如果没有进行设置，也可以通过如下方法添加。

{% highlight text %}
----- 1. 生成免密登陆的公私钥
# ssh-keygen -t rsa -f ~/.ssh/id_lxc -N ""

----- 2. 将公钥信息保存在authorized_keys文件中
# cd /var/lib/lxc/sshd-test/rootfs/root && mkdir -p .ssh
# cp ~/.ssh/id_lxc.pub .ssh/authorized_keys

----- 3. 更改权限，sshd对权限要求比较严格
# chown -R 0:0 .ssh
# chmod 700 .ssh
{% endhighlight %}

以 sshd 为例，首先会通过 install_sshd() 创建目录。


## 常见错误

### unknown capability

出于安全考虑，上述是在启动时尝试删除一些功能，但是却没找到，暂时还不太清除原因，可以通过将配置文件中的如下内容注释掉解决。

{% highlight text %}
lxc.cap.drop = sys_module mac_admin mac_override sys_time
{% endhighlight %}


{% highlight text %}
{% endhighlight %}
