---
title: SSH 简介
layout: post
comments: true
category: [linux]
language: chinese
keywords: ssh,openssh
description: OpenSSH 是 SSH (Secure SHell) 协议的免费开源实现，SSH 协议族可以用来进行远程控制，或在计算机之间传送文件。传统方式，如 telnet、rcp ftp、rlogin、rsh 都是极不安全的，并且会使用明文传送密码，OpenSSH 提供了服务端后台程序和客户端工具，用来加密远程控件和文件传输过程中的数据，并由此来代替原来的类似服务。在此，简单介绍下 OpenSSH 。
---

OpenSSH 是 SSH (Secure SHell) 协议的免费开源实现，SSH 协议族可以用来进行远程控制，或在计算机之间传送文件。

传统方式，如 telnet、rcp ftp、rlogin、rsh 都是极不安全的，并且会使用明文传送密码，OpenSSH 提供了服务端后台程序和客户端工具，用来加密远程控件和文件传输过程中的数据，并由此来代替原来的类似服务。

在此，简单介绍下 OpenSSH 。

<!-- more -->

![OpenSSH Logo]({{ site.url }}/images/linux/ssh_logo.png "OpenSSH Logo"){: .pull-center}

## 简介

SSH 采用的是公私钥加密方式，利用公钥进行加密，通过私钥解密，公钥任何人都可以看到，而私钥是私有的。目前 ssh 有 version1 和 version2 两个版本， version2 加入了联机检测功能，可以避免联机过程中插入恶意攻击代码。

OpenSSH Server 在首次执行时会在 /etc/ssh/ 目录下产生公钥和私钥，通常为 ssh_host_* 。如果想要产生新的密钥可以删除 ssh_host_* ，然后重启服务即可。

客户端会将已知服务器的公钥保存在 ~/.ssh/known_hosts 中，当登入远程服务器时，本地主机会主动的用接收到的服务器的 public key 去比对 ~/.ssh/known_hosts 有无相关的公钥，若接收的公钥尚未记录，则询问用户是否记录。若接收到的公钥已有记录，则比对记录是否相同，若相同则继续登入动作；若不相同，则出现警告信息, 且离开登入的动作。


![SSH Tunneling Sock]({{ site.url }}/images/linux/ssh_how_ssh_works.gif){: .pull-center}

服务器工作过程。

1. 服务器建立公钥文件。<br>
每次启动 sshd 服务时，服务器主动寻找 /etc/ssh/ssh_host* 文件，如果没有则 sshd 会计算出这些文件，包括服务器自己的私钥文件。

2. 客户端主动连接请求。<br>
客户端通过 ssh 等软件来连接 SSH 服务器。

3. 服务器将公钥传递给客户端。<br>
接收到用戶端的请求后，服务器的第一步是取得公钥文件传送给客户端，此时是明码传输。

4. 客户端记录/比对服务器的公钥，并随机计算自己的公私钥。<br>
本地主机主动用接收到的服务器的 public key 去比对 ~/.ssh/known_hosts 有无相关的公钥。<br>
A) 若接收的公钥尚未记录，则询问用户是否记录。若要记录 (回答 yes) 则写入 ~/.ssh/known_hosts 且继续登入的后续工作；若不记录 (回答 no) 则不写入该档案，并且离开登入工作。<br>
B) 若接收到的公钥已有记录，则比对记录是否相同，若相同则继续登入动作；若不相同，则出现警告信息, 且离开登入的动作。 这是客户端的自我保护功能，避免你的服务器是被别人伪装的。如果是在测试主机，那么重新安装时服务器的公钥会经常改变，此时会无法正常登入。如果服务器的 Public key 改变，那么，直接删除 ~/.ssh/known_hosts 中对应的记录即可。

5. 回传用户端的公钥资料到服务器端。<br>
用户将自己的公钥传送给服务器，此时服务器具有服务器的私钥和用户端的公钥；而用户端则有服务器的公钥以及用户端的自己的私钥。

6. 开始双向加/解密。<br>
服务器到用户端：服务器在传送资料时，使用用户端的公钥加密后传送出去。用户端接收后用自己的私钥解密。<br>
用户端到服务器：用户端在传送资料时，使用服务器的公钥加密后传送出去。服务器接收后用自己的私钥解密。

~/.ssh/known_hosts 存在于客户端，用于记录已经通过验证的服务器。在第一次登录的时候，会提示是否将服务器添加到该文件中，以后再登录时，会利用该文件检测是否是之前的服务器，从而可以防止“中间人攻击”。

~/.ssh/authorized_keys 是主机保存了用户的公钥，保存在登录后的用户主目录下。正常情况下服务器不会主动保存客户端的公钥，需要用户手动设置，通常用于“无密码登录”。

## 安装使用

OpenSSH 不仅提供了一个 shell，而且还提供了一个较为安全的 ftp-server，也就是客户端和服务器。

{% highlight text %}
# dpkg --list | grep ssh                                  # Ubuntu中查看/安装OpenSSH Server
# apt-get install openssh-server

# yum install openssh-server openssh-clients openssh      # CentOS中安装
{% endhighlight %}

Ubutnu 中安装之后会包括 /etc/init.d/ssh 文件，在 Ubuntu 中可以通过如下方式启动/停止/重启 Openssh Server 。

{% highlight text %}
# /etc/init.d/ssh [start | stop | restart]
# service ssh [start | stop | restart]
{% endhighlight %}

通过如下命令可以查看服务是否已经启动，其中 ssh 使用的是 22 号端口，使用 tcp 协议， ftp 同样使用 22 号端口。

{% highlight text %}
# netstat -vatn | grep 22
{% endhighlight %}

在 CentOS7 中，可以通过如下命令启动。

{% highlight text %}
# systemctl start sshd.service
{% endhighlight %}

## 客户端使用

正常情况下在安装完服务器后，可以通过如下命令进行简单的登录，如果远程主机的用户名和本地用户名一致可以省略 user 。

{% highlight text %}
$ ssh user@host-name
{% endhighlight %}

关于 ssh 客户端的详细参数可以参考 man 命令，常用的参数有：

* -f :需要配合后面的 [command] , 不登入远程主机直接发送一个指令过去而已。

* -o option:主要的参数项目有：<br>
    ConnectTimeout=秒数 : 联机等待的秒数,减少等待的时间；<br>
    StrictHostKeyChecking=[yes|no|ask]:预设是 ask 若要让 public key 主动加入 known_hosts，则可以设定为 no 即可；

* -p :如果你的 sshd 服务启动在非正规的端口 (22),需使用此选项。

### 在服务端执行命令

登入对方主机执行过指令后立刻离开。
{% highlight text %}
$ ssh student@127.0.0.1 find / & > ~/find1.log
{% endhighlight %}
此时你会发现怎么画面卡住了，这是因为上头的指令会造成，已经登入远程主机，但是执行的指令尚未跑完，因此会在等待当中。

与上相似，但是让对方主机自己跑该指令，立刻回到本地主机继续工作。

{% highlight text %}
$ ssh -f student@127.0.0.1 find / &> ~/find1.log
{% endhighlight %}

此时你会立刻注销 127.0.0.1 ，但 find 指令会自己在远程服务器跑喔!


## 配置文件

默认包括了如下的配置文件：

* /etc/ssh/sshd_config ： OpenSSH 服务器配置文件。

* /etc/ssh/ssh_config ： OpenSSH 客户端全局配置文件。

* ~/.ssh/authorized_keys ：已经授权的可以直接登陆的公钥。

### 整体配置

{% highlight text %}
Protocol 2,1    # SSH协议版本，1可能会有中间人攻击，建议只配置为2
{% endhighlight %}

### 安全配置



#### 登入设定部分

{% highlight text %}
PermitRootLogin yes                      # 是否允许root登入
StrictModes yes                          # 严格检查目录权限，权限设置出错则不允许登陆
PubkeyAuthentication yes                 # 允许公钥登陆
AuthorizedKeysFile .ssh/authorized_keys  # 公钥保存文件
HostbasedAuthentication no               # 取消基于host的授权
PasswordAuthentication yes               # 使用密码验证
PermitEmptyPasswords no                  # 是否允许以空的密码登入
AllowUsers list                          # 允许登陆的用户，用空格分开，可以用*?通配符
DenyUsers foobar                         # 禁止某一个用户登入
AllowGroups nosh                         # 允许某一个组登入
DenyGroups nosh                          # 禁止某一个组登入
{% endhighlight %}


#### 登入后设置


{% highlight text %}
ClientAliveInterval 300        # 若300秒内没有收到客户端消息，则发送心跳报文
ClientAliveCountMax 0          # 客户端断开连接之前，能发送保活信息的条数，此时踢出用户
{% endhighlight %}

#### 安全设置

{% highlight text %}
IgnoreRhosts yes              # 取消使用~/.ssh/.rhosts来做为认证
{% endhighlight %}


#### 其它

{% highlight text %}
UseDNS yes                    # 关闭DNS验证，详见后面的介绍
{% endhighlight %}




<!--

You need to firewall ssh port # 22 by updating iptables or pf firewall configurations. Usually, OpenSSH server must only accept connections from your LAN or other remote WAN sites only.
Netfilter (Iptables) Configuration

Update /etc/sysconfig/iptables (Redhat and friends specific file) to accept connection only from 192.168.1.0/24 and 202.54.1.5/29, enter:

-A RH-Firewall-1-INPUT -s 192.168.1.0/24 -m state --state NEW -p tcp --dport 22 -j ACCEPT
-A RH-Firewall-1-INPUT -s 202.54.1.5/29 -m state --state NEW -p tcp --dport 22 -j ACCEPT

If you’ve dual stacked sshd with IPv6, edit /etc/sysconfig/ip6tables (Redhat and friends specific file), enter:

 -A RH-Firewall-1-INPUT -s ipv6network::/ipv6mask -m tcp -p tcp --dport 22 -j ACCEPT

 Replace ipv6network::/ipv6mask with actual IPv6 ranges.
-->





## 杂项

#### 客户端心跳报文

{% highlight text %}
SecureCRT:
   Properties->Terminal->Anti-dile
   设置每隔80秒发送一个字符串或是NO-OP协议包
Putty:
   Properties->Connection->Seconds between keepalives(0 to turn off)
   默认为0，改为80
Xshell:
   File->Properties->Connection->Keep Alive
   设置为80s
Linux:
   在/etc/ssh/ssh_config添加ServerAliveInterval 80
{% endhighlight %}

#### 关闭DNS检查

OpenSSH 服务器有一个 DNS 查找选项 UseDNS 默认是打开的。

当客户端试图登录 OpenSSH 服务器时，服务器端先根据客户端的 IP 地址进行 DNS PTR 反向查询，查询出客户端的 host name，然后根据查询出的客户端 host name 进行 DNS 正向 A 记录查询，验证与其原始 IP 地址是否一致，这是防止客户端欺骗的一种手段，但一般 IP 是动态的，不会有 PTR 记录的，打开这个选项不过是在白白浪费时间而已。

#### 目录权限设置

ssh 对目录的权限有要求，代码中要设置下新生成的 config 文件权限才行。~ 目录权限是750，~/.ssh 的是700， ~/.ssh/* 的是600，~/.ssh/config 是700


#### 减少延迟

如果每次连接服务器都意味着你需要等待几十秒而无所事事，那么你或许应该试试在你的 SSH 配置中加入下面这条，这一配置可以在客户端或者服务端配置，或者在 ssh 登录时加上 "-o GSSAPIAuthentication=no" 。

{% highlight text %}
GSSAPIAuthentication no
{% endhighlight %}

GSSAPI 是 Generic Security Services Application Program Interface (通用安全服务应用程序接口) 是 ITEF 的一个标准，允许不同的安全算法使用一个标准化的 API 来为网络产品提供加密认证。

OpenSSH 使用这个 API 并且底层的 kerberos 5 协议代码提供除了 ssh_keys 的另一种 ssh 认证方式。



<!--
### 加速连接

　　如果你确保你和某个服务器之间的连接是安全的（比如通过公司内网连接），那么你就可以通过选择arcfourencryption算法来让数据传输更快一些：

Host dev
Ciphers arcfour

　　注意这个加速是以牺牲数据的加密性为代价的，所以如果你连接的是位于网上的服务器，千万不要打开这个选项，并且确保你是通过VPN建立的连接。
-->



<!--
tar -cvf - /bin | ssh id@hostname "tar -xvf -"
从服务器复制到其它地方


当主机的私钥修改后，可以通过 ssh-keyscan 进行扫描，列出相应主机对应的公钥信息，其中参数中的 host 信息可以查看 known_hosts 中每行的第一列。

f=$(mktemp) && ssh-keyscan korell > $f && ssh-keygen -l -f $f && rm $f

ssh-agent 会通过一个后台程序，将私钥信息保存在内存中，需要时发送给相应的程序。

当 ssh-agent 启动之后，会自动更新 SSH_AUTH_SOCK 和 SSH_AGENT_PID 环境变量，用来供其它程序使用

介绍最佳实践方式，包括了一个漏洞的链接
https://blog.0xbadc0de.be/archives/300



<h2>中间人攻击</h2>
<p>
	在进行登录时，存在风险：如果有人截获了登录请求，然后冒充远程主机，将伪造的公钥发给用户，那么用户很难辨别真伪。因为不像 https 协议， SSH 协议的公钥是没有证书中心 （CA） 公证的，也就是说，都是自己签发的。<br><br>

	如果攻击者在用户与远程主机之间（如公共的 wifi 区域），用伪造的公钥，获取用户的登录密码。再用这个密码登录远程主机，那么 SSH 的安全机制就荡然无存了。这种风险就是著名的 "中间人攻击" （Man-in-the-middle attack）。<br><br>

	为了解决这一问题，如果你是第一次登录对方主机，系统会出现类似下面的提示：
<pre>
$ ssh user@host
The authenticity of host 'host (12.18.429.21)' can't be established.
RSA key fingerprint is 98:2e:d7:e0:de:9f:ac:67:28:c2:42:2d:37:16:58:4d.
Are you sure you want to continue connecting (yes/no)?</pre>
	所谓 "公钥指纹" ，是指公钥长度较长（这里采用 RSA 算法，长达 1024 位），很难比对，所以对其进行 MD5 计算，将它变成一个 128 位的指纹。上例中是 98:2e:d7:e0:de:9f:ac:67:28:c2:42:2d:37:16:58:4d ，再进行比较，就容易多了。<br><br>


	通常我们可以通过如下的方法来获取公钥对指纹。<ol><li>
		最简单的方法是远程主机在自己的网站上贴出公钥指纹，以便用户自行核对。</li><br><li>

		可以直接通过网络从远程服务器上获取公共密钥，然后通过这个文件生成指纹。
		<pre>$ ssh-keyscan -p 22 -t rsa,dsa REMOTE_HOST > /tmp/ssh_host_rsa_dsa_key.pub<br>$ ssh-keygen -l -f /tmp/ssh_host_rsa_dsa_key.pub</pre></li><br><li>

		想办法通过其他方式登录到真机上，如通过管理控制台，然后通过如下命令直接生成指纹。
		<pre>$ ssh-keygen -lf /etc/ssh/ssh_host_rsa_key</pre>
	</li></ol>
如果指纹不匹配，那么你应该先做一个针对ARP请求的网络扫描，看看有哪个IP地址回应了ARP请求。在ping的时候扫描一下看看有没有ARP请求。如果有两个主机，那么它们会为一个ARP条目而“互掐”起来，你应该能看到两个回应。

一旦你知道了这个神秘主机的以太网地址，就可以通过路由（或交换机）接口上跟踪到ARP流量的去处。

	假定经过风险衡量以后，用户决定接受这个远程主机的公钥，然后，会要求输入密码，如果密码正确，就可以登录了。<br><br>

	当远程主机的公钥被接受以后，它就会被保存在文件 ~/.ssh/known_hosts 之中。下次再连接这台主机，系统就会认出它的公钥已经保存在本地了，从而跳过警告部分，直接提示输入密码。<br><br>

	如果再次登录时，发现 SSH 密钥改变了。这种情况可能是由于中间人攻击导致，但更多的情况下，是因为主机被重建，生成了新的 SSH 密钥，所以在重建服务器的时候要养成保存恢复 SSH 密钥习惯。<br><br>

	其它内容可以参考硕士论文，《SSH 协议的中间人攻击研究》。
	</p>
-->



## 参考

可以参考 [SSH The Secure Shell The Definitive Guide](/reference/linux/SSH_The_Secure_Shell_The_Definitive_Guide.pdf) 文档。

github 上收集的与 SSH 相关的经典内容 [ssh awesome](https://github.com/moul/awesome-ssh) 。


<!--
https://github.com/arthepsy/ssh-audit
https://nvd.nist.gov/
https://zzz.buzz/zh/2016/04/18/hardening-sshd/
https://networkjutsu.com/ssh-brute-force-attack/
封IP可以通过 denyhosts、iptables、Fail2Ban
-->

{% highlight text %}
{% endhighlight %}
