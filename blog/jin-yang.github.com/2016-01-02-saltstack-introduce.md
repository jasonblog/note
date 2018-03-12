---
title: SaltStack 简介
layout: post
comments: true
language: chinese
category: [misc,linux,python]
keywords: hello world,示例,sample,markdown
description: SaltStack 是一个轻量级的运维工具，具备配置管理、远程执行、监控等功能，类似于 Puppet 和 Chef，不过这两者是用 Ruby 写的，而 Salt Stack 用的是 Python。
---

SaltStack 是一个轻量级的运维工具，具备配置管理、远程执行、监控等功能，类似于 Puppet 和 Chef，不过这两者是用 Ruby 写的，而 Salt Stack 用的是 Python。

结下来看看具体的使用。

<!-- more -->

## 简介

SaltStack 基于 Python 写的经典 C/S 框架的自动化部署平台，由 Master 和 Minion 构成，使用了一系列成熟的工具，比如网络模型采用 ZeroMQ、授权加密采用了 AES、串行化采用了 msgpack、配置文件采用 [YAML](http://www.yaml.org/) 标记语言等。

### 安装测试

在 CentOS7 中可以通过如下方式在单机安装测试。

{% highlight text %}
----- 直接通过YUM安装相关的依赖包
# yum --enablerep=epel install salt salt-master salt-minion salt-ssh
# systemctl start salt-master                # 启动master
# salt-master -d                             # 同上，直接通过命令行启动，指定后台运行
# netstat -ltunp | grep python               # 查看打开的端口，默认为4505/4506
# lsof -i:4505                               # 同上，查看使用4505端口的程序

# cat /etc/salt/minion                       # 修改配置文件
master: localhost   # master的IP地址
id: foobar          # 指定minion的ID
# systemctl start salt-minion                # 启动minion

$ salt --version                             # 查看版本号
$ salt --versions-report                     # 查看其它依赖模块的版本号

----- 测试，常见的使用方式
# salt-key -A                                # 添加本地节点
# salt "*" test.ping                         # 测试节点
# salt "*" cmd.run 'uptime'                  # 执行命令
# salt -G "os:Centos" test.ping              # 根据被控主机的grains信息过滤
# salt "*" grains.item os                    # 查看操作系统类型
--- 执行代码，等同于python -c 'import sys; print sys.version'
# salt '*' cmd.exec_code python 'import sys; print sys.version'
{% endhighlight %}

实际上 master/minion 只会安装少数的文件，主要文件在 salt 安装包中。

SaltStack master 启动后默认监听 4505 和 4506 两个端口，其中 4505(publish_port) 为 salt 的消息发布系统，4506 (ret_port) 为 salt 客户端与服务端通信的端口。

## Master/Minion 的认证和链接

SaltStack 主控端是依靠 openssl 证书来与受控端主机认证通讯的，受控端启动后会发送给主控端一个公钥证书文件，在主控端用 salt-key 命令来管理证书。

认证过程如下。

1. 在安装 minion 之后，需要在配置文件中定义参数 master，也就是指定 master 端的 IP 地址；

2. 启动 minion 服务后会在本地生成一个密钥对；

3. 接着 minion 会去连接 master，并尝试把公钥发送给 master，这时在 master 端可以使用 salt-key 来查看当前已经接收到的认证。

在 master 接受 minion 认证之后，两者即可通讯了，可以通过 state 模块来配置管理 minion，也可以通过 cmd.run 远程对 minion 执行命令。

详细来说，minion 在第一次启动时，会在配置文件 (默认为/etc/salt/minion) 中的配置项 pki_dir 指定目录下（默认为 /etc/salt/pki/minion/）下自动生成 minion.pem (private key) 和 minion.pub (public key)，然后将 minion.pub 发送给 master。

master 在接收到 minion 的 public key 后，通过 salt-key 命令 accept minion public key，这样在 master 的 /etc/salt/pki/master/minions 下的将会存放以 minion id 命名的 public key，然后 master 就能对 minion 发送指令了。

常见的命令有：

{% highlight text %}
# salt-key -L                                # 查看所有节点，包括未认证的节点
# salt-key -A                                # 添加所有未认证的节点
# salt-key -a node*                          # 支持单个节点、以及通配符
# salt-key -d node                           # 删除对某个节点的认证
# salt-key -D                                # 删除所有证书
{% endhighlight %}

## 执行命令

SaltStack 执行命令的格式如下。

![SaltStack Commands Syntax]({{ site.url }}/images/network/saltstack/cmd-syntax.png "SaltStack Commands Syntax"){: .pull-center}

常用的命令如下。

{% highlight text %}
# salt '*' sys.doc                      # 查看所有文档
# salt '*' sys.doc pkg.install          # 查看pkg.install对应的文档

# salt '*' cmd.run 'ls -l /bin/ls'      # 直接执行终端命令
# salt '*' disk.usage                   # 查看磁盘使用情况
# salt '*' network.interfaces           # 查看网络设备

# salt '*' pkg.install coreutils        # 安装软件
{% endhighlight %}

在 SaltStack 中，目前有 5 种方式来指定目标机器。

1. 匹配 minion id，可以使用 shell 通配符 (Globbing)、正则表达式(Perl风格)、minion 列表。

2. Grains，包括客户端配置文件中定义 Grains。

3. 指定列表 (Lists)，节点分组 (Node Groups)，复合匹配 (Compound Matcher)，批量执行 (Batch Size)。

可以参考如下的示例。

{% highlight text %}
# salt '*' test.ping                                    # 所有的minions
# salt '*.example.*' test.ping                          # 可以使用shell中的* ? []通配符
# salt 'web?' test.ping                                 # 匹配web1、web2等
# salt 'web-[x-z]' test.ping                            # 匹配web-x、web-y、web-z

# salt -E 'web1-(prod|devel)' test.ping                 # 使用正则表达式

# salt -L 'web1,web2,web3' test.ping                    # 指定列表

# salt -G 'os:CentOS' test.ping                         # 使用Grains

# salt -N group1 test.ping                              # 指定分组节点node group

# salt -C 'web? and G@os:CentOS or E@web.*' test.ping   # 使用组合模式

# salt '*' -b 10 test.ping                              # 使用批量模式
# salt -G 'os:RedHat' --batch-size 25% test.ping
{% endhighlight %}

在不指定 minion id，则会通过 socket.getfqdn() 获取，可以通过如下命令查看，同时该值会缓存在 /etc/salt/minion_id 中。

{% highlight text %}
$ python -c 'import socket; print socket.getfqdn(socket.gethostname())'
{% endhighlight %}

节点分组可以在 master 配置文件中进行设置，如下。

{% highlight text %}
nodegroups:
  group1: 'L@foo.domain.com,bar.domain.com,baz.domain.com or bl*.domain.com'
  group2: 'G@os:Debian and foo.domain.com'
{% endhighlight %}

复合匹配是通过布尔操作符连接的多个目标条件，可以使用上述的几种方式匹配，默认采用 Globbing。另外，not 不能用于第一个条件，需要使用如下的命令 salt -C '* and not G@os:CentOS' test.ping 。

相关匹配规则的语法可以参考 [staltstack docs - Compoud Matchers](https://docs.saltstack.com/en/latest/topics/targeting/compound.html) 。

## Salt State, SLS

State 用于描述系统的目标状态，SLS 代表 SaLt State 文件，是 Salt State 系统的核心；SLS 描述了系统的目标状态，采用最简单易读的序列化数据格式 YAML。

其中根路径在 master 的配置文件中通过 file_roots 指定，其中 base 必须存在，而且 base 环境中通常包含 Salt 的入口 top 文件，也即 top.sls 。注意，sls 文件需要同时在 master 和 minion 都存在。

最简单的配置文件如下，用于检查 coreutils 是否已经安装，与 state 相关的源码保存在 salt/states 目录下。

{% highlight text %}
# cat top.sls
base:
  '*':
    - coreutils
# cat coreutils.sls
coreutils:                       # ID声明
   pkg:                          # state声明
      - installed                # 定义的function
{% endhighlight %}

在配置文件中，可以通过 salt:// 代替配置文件中 file_roots 指定的根路径，入口文件默认为 top.sls，可以通过 state_top 配置。

<!--
对于机器的匹配方式可以有多种。

<pre style="font-size:0.8em; face:arial;">
base:
  'web1-(prod|devel)':                                 # 匹配minion-id
    - match: pcre                                      # 设置匹配方式
    - webserver

base:
  group1:
    - match: nodegroup                                 # 通过节点分组匹配
    - webserver

base:
  'webserv* and G@os:Debian or E@web-dc1-srv.*':       # 使用组合模式
    - match: compound
    - webserver
</pre>
-->

## Pillar VS. Grains

Pillar 和 Grains 非常容易混淆，其中 Grain 的数据是在 minion 启动时产生的，通常是类似于 OS、CPU、磁盘的静态信息；而 Pillar 信息保存在 master 中。

Salt在 0.9.8 版本中引入了 Pillar，Pillar 是一个非常重要的组件，用于给特定的 minion 定义任何需要的数据，这些数据可以被 Salt 的其他组件使用。Pillar 解析后，是一个嵌套的字典结构，最上层的 key 是 minion ID，也就是说 Pillar 数据是与特定 minion 关联的，每一个 minion 只能看到自己的数据。

Pillar 使用独立的加密 session，所以 Pillar 可以用来传递敏感数据，如 ssh key、加密证书等。

默认情况下，master 配置文件中的所有数据都添加到 Pillar 中，且对所有 minion 可用，如果要禁用这一默认值，可以添加 pillar_opts: False 配置。Pillar 使用与 State 相似的 SLS 文件，其根目录通过 master 配置文件中 pillar_roots 指定，而且 Pillar 也有 top file。

{% highlight text %}
# salt '*' pillar.data                         # 或者pillar.raw
# salt '*' saltutil.refresh_pillar             # 修改Pillar文件后刷新minion上的数据
{% endhighlight %}

对于 Grains 常见的操作如下。

{% highlight text %}
# salt '*' grains.ls                           # 列出所有grains的名字
# salt '*' grains.items                        # 列出名字以及内容
# salt '*' grains.item os                      # 查看具体选项
{% endhighlight %}

可以在 minion 的配置文件中 (默认为/etc/salt/minion) 设置 Grains 的静态信息。

{% highlight text %}
grains:
  roles:
    - webserver
    - memcache
  deployment: datacenter4
{% endhighlight %}

另外，可在 master 中定义模块，可以将脚本放置在 file_roots 下的 _grains 目录中，当执行 state.highstate、saltutil.sync_grains、saltutil.sync_all 时，会将 _grains 中的文件分发到客户端上。

{% highlight text %}
# mkdir /srv/salt/_grains
# cat /srv/salt/_grains/custom_grain.py
    def custom_grains():
        grains = {'role' : 'LB'}
        return grains
# salt '*' saltutil.sync_grains
# salt '*' grains.item role
{% endhighlight %}

上述的脚本会作为模块导入，因此不需要 #!/usr/bin/env python，也不需要有执行权限。

## 模块

一些常见的模块会保存在安装目录下的 salt/modules 下，如果通过 YUM 安装，通常为：

{% highlight text %}
$ ls /usr/lib/python2.7/site-packages/salt/modules
{% endhighlight %}

在 Python 中，如果要添加模块也非常简单，可以直接在安装目录的 modules 子目录下添加如下文件。

{% highlight text %}
# cat /usr/lib/python2.7/site-packages/salt/modules/foobar.py
def hello():
    return 'hello world'
{% endhighlight %}

然后可以通过 salt '*' foobar.hello 执行。

### 常用模块介绍


下面看下一些常见的模块，除此之外还有 grains、pillar、states、modules、returner、runners、reactor 等模块。

#### cp模块

实现远程文件、目录的复制，以及下载URL文件等操作。

{% highlight text %}
----- 将主服务器file_roots base指定目录下的文件复制到被控主机
# cat /etc/salt/master
file_roots:
  base:
    - /srv/salt
# echo "This is test file with saltstack module 'cp.get_file'" > /srv/salt/foobar.txt
# salt '*' cp.get_file salt://foobar.txt /tmp/foobar.txt

----- 将目录下的文件复制到被控主机
# mkdir /srv/salt/dir && mv /srv/salt/foobar.txt /srv/salt/dir/
# salt '*' cp.get_dir salt://dir /tmp

----- 下载指定URL内容到被控主机指定位置
# salt '*' cp.get_url http://justkidding.com/download/files.tgz /tmp/files.tgz

----- 根据grains选择目标
# salt '*' cp.get_file "salt://{{grains.os}}/vimrc" /etc/vimrc template=test
{% endhighlight %}

该模块有几个常用的参数：

* gzip=9，使用gzip的方式进行压缩，数字越大，压缩率就越高，9代表最大的压缩率。

* makedirs=True，当分发的位置在目标主机上不存在时，自动创建该目录。

模板渲染功能可以同时在源和目标文件中使用。

#### cmd 模块

实现远程的命令行调用执行。

{% highlight text %}
# salt '*' cmd.run 'uptime'
{% endhighlight %}

#### file模块

被控主机文件常见操作，包括文件读写、权限、查找、校验等。

{% highlight text %}
# salt '*' file.get_sum /etc/resolv.conf md5
# salt '*' file.stats /etc/resolv.conf
{% endhighlight %}

#### pkg 包管理模块

被控主机程序包管理，如yum、apt-get等。

{% highlight text %}
# salt '*' pkg.install coreutils
# salt '*' pkg.file_list coreutils
{% endhighlight %}

#### network模块

返回被控主机网络信息。

{% highlight text %}
# salt '*' network.ip_addrs
# salt '*' network.interfaces
{% endhighlight %}

#### service 服务模块

被控主机程序包服务管理。

{% highlight text %}
# salt '*' service.start crond
# salt '*' service.status crond
{% endhighlight %}


<!--
## salt-api

https://docs.saltstack.com/en/latest/ref/clients/

{% highlight text %}
# -*- coding:utf-8 -*-
import salt.config

# 获取master配置文件
master_ops = salt.config.client_config('/etc/salt/master')#returns a dictionary

# 获取minion配置文件
minion_opts = salt.config.minion_config('/etc/salt/minion')#returns a dictionary

# 获取grain值
import salt.loader
opts = salt.config.minion_config('/etc/salt/minion')
grains = salt.loader.grains(opts)
print grains['id']

# 执行salt命令的函数，一般返回字典
import salt.client
local = salt.client.LocalClient()
print local.cmd('*', 'test.ping', [])
{% endhighlight %}
-->

## 配置文件

{% highlight text %}
interface: 0.0.0.0                    # 监听IP
publish_port: 4505                    # 消息发布端口
user: root                            # 默认用户
max_open_files: 100000                # 最大打开文件数
worker_threads: 5                     # 工作线程数，不能小于3
ret_port: 4506                        # master和minion使用的通讯端口
pidfile: /var/run/salt-master.pid     # PID文件

root_dir
pki_dir: /etc/salt/pki/master        # 存放公钥的目录
    cachedir
    keep_jobs
    job_cache
    ext_job_cache
    minion_data_cache
    enforce_mine_cache
    sock_dir
{% endhighlight %}

## 参考

相关的文档可以查看 [docs.saltstack.com](https://docs.saltstack.com/en/latest/contents.html)，也可以直接下载 PDF 文档，内容比较多，有 2K+ 页。其它的一些常用网址可以参考： [中文社区(www.saltstack.cn)](http://www.saltstack.cn/)、[官方网址(www.saltstack.com)](http://www.saltstack.com)、[Python官方](https://pypi.python.org/pypi/salt) 。

{% highlight text %}
{% endhighlight %}
