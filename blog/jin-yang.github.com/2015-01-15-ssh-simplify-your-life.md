---
title: SSH Simplify Your Life
layout: post
comments: true
category: [linux,misc]
language: chinese
keywords: ssh,sshd
description: 通常在一个 ssh 链接中，需要指定用户名、主机名、主机端口号、配置参数等信息，如果每次登陆一台主机都要输入，那么就会变的非常麻烦，而且一些 IP 地址通常又非常难记。为此，我们可以通过 OpenSSH 的客户端配置文件简化我们的登陆方式，下面简单介绍下。
---

通常在一个 ssh 链接中，需要指定用户名、主机名、主机端口号、配置参数等信息，如果每次登陆一台主机都要输入，那么就会变的非常麻烦，而且一些 IP 地址通常又非常难记。

为此，我们可以通过 OpenSSH 的客户端配置文件简化我们的登陆方式，下面简单介绍下。

<!-- more -->

## 常用操作

通过 SSH 可以在远程主机上执行一些常用的操作，简单列举如下。

{% highlight text %}
$ ssh user@host 'mkdir -p .ssh && cat >> .ssh/authorized_keys' < ~/.ssh/id_rsa.pub
{% endhighlight %}

单引号中间的部分，表示在远程主机上执行的操作，紧跟着的输入重定向，表示数据通过 SSH 传向远程主机。这就是说，SSH 可以在用户和远程主机之间，建立命令和数据的传输通道，因此很多事情都可以通过 SSH 来完成。

下面看几个常用的例子。

{% highlight text %}
----- 1. 将$HOME/src/下的所有文件，复制到远程主机的$HOME/src/目录
$ cd && tar czv src | ssh user@host 'tar xz'

----- 2. 将远程主机$HOME/src/目录下面的所有文件，复制到用户的当前目录
$ ssh user@host 'tar cz src' | tar xzv

----- 3. 查看远程主机是否运行进程httpd
$ ssh user@host 'ps ax | grep [h]ttpd'
{% endhighlight %}

## 简单配置

每次通过 ssh 登陆时，通常需要按照如下方式，指定用户名、主机、端口号，如果拥有多个 ssh 账号，那么要记住每个 ssh 账号的参数会非常麻烦。

{% highlight text %}
$ ssh foobar@192.168.9.123 -p 2222

常用参数:
   -p:  指定端口
   -i:  如果通过公私钥认证，则通过该参数指定私钥
   -v:  调试，用于查看认证过程
   -o:  指定参数，例如-o ServerAliveInterval=60
{% endhighlight %}

实际上，对于 OpenSSH 客户端，可以通过配置文件简化登陆。

OpenSSH 客户端会使用 ```~/.ssh/config``` 用户配置文件，也可使用全局配置 ```/etc/ssh/ssh_config```，如果没有该文件则需要手动创建，该文件中的配置项通过 ```Key = Value``` 格式进行设置。

通过如下的方式指定别名，可以支持通配符 ```*%```，注意，配置项大小写不敏感，值大小写敏感。

{% highlight text %}
Host *                           # 对于所有主机通用配置项
    ServerAliveInterval 60       # 防止因为空闲链接断开，每隔60秒发送一次请求，从而保持连接
    ServerAliveCountMax 3        # 发送请求后，如果服务端连续超过3次没有响应，则自动断开
    StrictHostKeyChecking no     # 默认首次添加到known_hosts时会有提示信息，配置为自动添加
    Protocol 2                   # 使用协议版本V2

Host mysql                       # 登陆时简化MySQL主机的配置
    HostName 192.168.9.123       # 指定服务器的IP，如果与上述的Host参数相同，则可以忽略
    User foobar                  # 登陆用户名
    Port 2222                    # 登陆使用端口号，默认22
    IdentityFile ~/.ssh/mysql    # 如果通过公私钥认证方式，指定私钥文件地址

Host dev backup www* mail
    HostName %h.example.com      # 利用上述的Host作为参数
{% endhighlight %}

例如，上述的 mysql 设置，可以直接通过 ssh mysql 登陆，而且 scp 等，同样可以如此使用。

另外，对于上述的最后一个示例，需要简单说明下，如果是通过 ssh dev 命令登陆，那么实际登陆时对应的主机名为 dev.example.com，以此类推。

### 代理设置

为了安全设置，通常防火墙只允许 80/22 端口访问，假设该服务器同时部署了 MySQL 服务，那么我们就无法通过笔记本进行调试。当然可以通过 ssh 的本地代理实现，设置如下：

{% highlight text %}
$ ssh -f -N -L 9906:127.0.0.1:3306 foobar@database.example.com
{% endhighlight %}

如上，使得本地访问 9906 端口时，会将请求转发到 database.example.com 的 127.0.0.1:3306 ，也就是对应的 MySQL 服务器了。

当然，同样可以通过如下方式简化。

{% highlight text %}
Host tunnel
    HostName database.example.com
    IdentityFile ~/.ssh/foobar.example.key
    LocalForward 9906 127.0.0.1:3306
    User foobar
{% endhighlight %}

然后通过如下方式使用。

{% highlight text %}
$ ssh -f -N tunnel
{% endhighlight %}

## 共享连接

当登陆一台服务器后，可以通过共享链接实现免登陆，也就是说第一次登陆之后，后面的会自动登陆。需要在添加配置文件 ```~/.ssh/config(600)``` 中添加如下内容。

{% highlight text %}
host *
    ControlMaster auto
    ControlPath /tmp/ssh_mux_%r@%h:%p      # 如果不使用该参数只能是最后一个免登
    ControlPersist 4h                      # 默认直接退出，该参数指定4小时后退出
{% endhighlight %}

此时会在 /tmp 目录下创建 ssh_mux 的 sock 文件，它记录了你目前登录到的机器，这样的话，你登录同样的机器就会重用同一个链接了。

这种方法也可以同时避免如下的错误 Shared connection to &lt;hostname&gt; closed.；这个主要是由于使用共享链接，当原链接关闭之后导致共享的链接同样失败，这样共享的链接可以保持打开状态，并且会复用。


## 公私钥认证

如果需要通过自动化脚本定期进行备份，当输入密码并保证密码的安全时，处理起来会比较麻烦。为此，可以通过公钥登录 (RSA/DSA认证)，省去输入密码的步骤。

所谓 "公钥登录" ，原理很简单，就是用户将自己的公钥储存在远程主机上。登录时，远程主机会向用户发送一段随机字符串，用户用自己的私钥加密后，再发回来。远程主机用事先储存的公钥进行解密，如果成功，就证明用户是可信的，直接允许登录 shell ，不再要求密码。

只需要生成一个密钥对，然后将公钥传送到服务器端，并保存在配置文件中的 AuthorizedKeysFile 指定的文件。

### 实践

可以通过如下步骤执行。

#### 1. 服务端设置

在服务器端，首先确认 ```/etc/ssh/sshd_config``` 这个文件，检查下面几行是否有效。

{% highlight text %}
RSAAuthentication yes
PubkeyAuthentication yes
AuthorizedKeysFile .ssh/authorized_keys
{% endhighlight %}

#### 2. 生成公私钥

直接通过 ssh-keygen 生成公私钥密码对即可。

{% highlight text %}
$ ssh-keygen [-t rsa/dsa]
$ ssh-keygen -t rsa -f ~/.ssh/id_rsa -N "" -C "comment"
参数：
    -t:   加密类型
    -f:   指定输出文件
    -N:   通过该密码加密私钥，为空则会自动登陆
    -C:   注释，通常为邮箱名称，会保存在公钥中
{% endhighlight %}

默认采用 rsa 加密，如上，如若想设置无密码登录，则直接回车即可。密钥对默认保存在 .ssh/ 目录下，包括了 id_rsa(私钥) id_rsa.pub(公钥)。

其它常用的加密方式还包括了 RSA, DSA, ECDSA, ED25519 。

#### 3. 将公钥上传到服务器

将公钥上传到服务器，此时会保存到需要免密码登陆用户 (在此为 foobar) 的 $HOME 目录下。

{% highlight text %}
$ scp ~/.ssh/id_rsa.pub foobar@remote-server-ip:~
{% endhighlight %}

#### 4. 公钥保存

配置文件中通过 AuthorizedKeysFile 指定公钥保存的文件名，一般是 ```.ssh/authorized_keys``` 文件，然后直接添加到该文件中即可。

{% highlight text %}
$ mkdir ~/.ssh/ && chmod 700 ~/.ssh/
$ cat id_rsa.pub >> .ssh/authorized_keys
$ chmod 644 .ssh/authorized_keys
{% endhighlight %}

在此，需要明确如下文件的权限。

{% highlight text %}
$ chmod 700 ~/.ssh/
$ chmod 644 ~/.ssh/authorized_keys
$ chmod 600 ~/.ssh/id_rsa
{% endhighlight %}

#### 5. 登陆

如果使用私钥登录多台主机，可以通过 -i 选项指定私钥文件。

{% highlight text %}
$ ssh -i id_rsa user-name@remote-server-ip
{% endhighlight %}

#### 6. 添加到known_hosts

如果是初次登陆时，会提示是否将对应的主机添加到 known_hosts 文件中，可以通过如下命令自动添加，而不需要交互。

{% highlight text %}
$ ssh -T -o StrictHostKeyChecking=no 192.144.51.85 'date'
{% endhighlight %}


### 简化配置

也可以通过如下命令简单地进行设置。

{% highlight text %}
$ ssh-keygen -t dsa -f ~/.ssh/id_dsa -C "foobar@kidding.com" -N ""
$ ssh-copy-id -i /home/foobar/.ssh/id_dsa.pub foobar@remote-host-ip
$ ssh remote-host-ip
{% endhighlight %}

可以通过如下方式在本地进行简单的测试。

{% highlight text %}
$ ssh-keygen -t ed25519
$ cat .ssh/id_rsa.pub >> .ssh/authorized_keys
$ ssh 127.1
{% endhighlight %}

ssh-copy-id 命令会自动将公钥添加到 ```~/.ssh/authorized_keys``` 文件中。

### 安全相关

在生成密钥对时，如果密码没有输入，也就是为空，那么可以自动登陆，不过同样会导致当有人获取了该私钥后，可以登陆你配置的所有服务器。

为此，为了保证私钥的安全，生成密钥对时，最好输入密码，这样即使有人获取了你的私钥文件，他仍然无法登陆。

不过这也导致了每次登陆时需要输入 **私钥保护密码**，会产生与之前相同的问题。

## ssh-agent

为了解决上述的问题，可以使用 ssh-agent，这是一个守护进程，设计它的唯一目的就是对解密用的专用密钥进行高速缓存。

ssh 支持与 ssh-agent 通信，允许 ssh 建立新连接时自动获取解密的专用密钥；你所需要做的就是，使用 ssh-add 命令把密钥添加到 ssh-agent 的高速缓存中即可。

还没有仔细研究过。

<!--
### 工作原理

在 CentOS 中，会默认启动 ssh-agent 服务，当然，也可以手动启动；此是会输出一些信息，包括两个重要的环境变量 SSH_AUTH_SOCK(ssh/scp用来连接的套接字) 和 SSH_AGENT_PID 。

可以通过 ssh-add id_rsa 将私钥添加到 agent 中；另外，如果出现了错误 Agent admitted failure to sign using the key ，则同样需要执行上述命令。

### 常用命令

可以通过 ssh-add 管理私钥，常用命令参数如下：

* -L：列出所缓存的公钥，与服务器端 authorized_keys 文件中保存的值相同；
* -D：清空缓存中的密码，此时如果登陆则需要使用之前设置的私钥保护密码；
* -X/x：对agent加锁，不知道具体的作用，仍然可以免密码登陆。

{% highlight text %}
$ eval $(ssh-keygen)         # 启动并设置环境变量
$ eval $(ssh-keygen -k)      # 退出
{% endhighlight %}

## 缺点

首先，如果在 bash_profile 中使用 eval \`ssh-agent\`，那么每次登录会话都会启动一个新的 ssh-agent 副本；这不仅仅是有一丁点儿浪费，而且还意味着您得使用 ssh-add 向每个新的 ssh-agent 副本添加专用密钥。

如果您只想打开系统上的一个终端或控制台，这没什么大不了的，但是我们中大多数人打开相当多的终端，每次新打开控制台都需要键入密码短语。

从技术角度讲，既然一个 ssh-agent 进程的确应当足够了，要是我们还需这样做，这毫无道理，为此可以使用 keychain ，在 bash_profile 文件里追加 eval \`keychain --eval id_dsa\` 即可实现到其他机器的免登。





## Login to internal lan server at 192.168.0.251 via our public uk office ssh based gateway using ##
## $ ssh uk.gw.lan ##
Host uk.gw.lan uk.lan
     HostName 192.168.0.251
     User nixcraft
     ProxyCommand  ssh nixcraft@gateway.uk.cyberciti.biz nc %h %p 2> /dev/null


    ProxyCommand : Specifies the command to use to connect to the server. The command string extends to the end of the line, and is executed with the user's shell. In the command string, any occurrence of %h will be substituted by the host name to connect, %p by the port, and %r by the remote user name. The command can be basically anything, and should read from its standard input and write to its standard output. This directive is useful in conjunction with nc(1) and its proxy support. For example, the following directive would connect via an HTTP proxy at 192.1.0.253:
    ProxyCommand /usr/bin/nc -X connect -x 192.1.0.253:3128 %h %p
    LocalForward : Specifies that a TCP port on the local machine be forwarded over the secure channel to the specified host and port from the remote machine. The first argument must be [bind_address:]port and the second argument must be host:hostport.
    Protocol : Specifies the protocol versions ssh(1) should support in order of preference. The possible values are 1 and 2.
    ServerAliveInterval : Sets a timeout interval in seconds after which if no data has been received from the server, ssh(1) will send a message through the encrypted channel to request a response from the server. See blogpost "Open SSH Server connection drops out after few or N minutes of inactivity" for more information.
    ServerAliveCountMax : Sets the number of server alive messages which may be sent without ssh(1) receiving any messages back from the server. If this threshold is reached while server alive messages are being sent, ssh will disconnect from the server, terminating the session.

通过本地端口访问服务器<br>
有时可能有些服务，比如数据库或是Web服务器，它们运行在远程服务器上，但是如果有用方式可以直接从本地程序连接它们，那会非常有用，要做到这一点，你需要用到端口转发(port forwarding)，举个例子，如果你的服务器运行Postgres（并且只允许本地访问），那么你就可以在你的SSH配置中加入：
<pre>
Host db
    LocalForward 5433 localhost:5432
</pre>
现在当你连接你的SSH服务器时，它会在你本地电脑打开一个5433端口（我随便挑的），并将所有发送到这个端口的数据转发到服务器的5432端口（Postgres的默认端口），然后，只要你和服务器建立了连接，你就可以通过5433端口来访问服务器的Postgres了。<br>
$ ssh db<br>
现在打开另外一个窗口，你就可以通过下面这条命令在本地连接你的Postgres数据库了：<br>
$ psql -h localhost -p 5443 orders</li><br><li>

# 登陆跳板机的方法

现在假设有 ABC 三台机器，A 是本地机器，B 是跳板服务器（123.456.789.110），C 是工作服务器（192.168.1.111），其中 A 可以访问 B，但不能直接访问 C，B 可以直接访问 C。


### 通过 ssh 端口转发命令

ssh -CfNg -L 8833:192.168.1.111:22 userc@192.168.1.111<br>

句话的意思是当我访问B机器8833端口时，其实是在访问C的22端口。以后就可以直接在A机器命令行输入。<br>
ssh -p 8833 userc@123.456.789.110<br>
就可以登录C机器了，这里要注意的是此处用户名是B登录C的用户名，而ip地址是B的ip地址，要和普通的ssh登录做区分。</li><br><li>

通过proxycommand+netcat<br>
这种方法不需要对B和C进行任何操作，只需要对A机器上的ssh_config文件做些修改就可以了。我们首先修改A机器上的ssh_config文件，添加这么几行。
<pre>
Host Cserver
    HostName 192.168.1.111
    Port 22 【C机器的ssh端口】
    ProxyCommand ssh -p 9022【B机器的ssh端口】 userb@123.456.789.110 nc %h %p
</pre>
然后在A机器的命令行输入<br>
ssh userc@Cserver</li><br><li>

无脑笨方法<br>
我们知道ssh是可以直接执行远程命令的，于是，在A机器命令行输入：<br>
ssh -t -p 9022【B机器ssh端口】 userb@123.456.789.110 "ssh userc@192.168.1.111"<br>
 当然这还是要输入两次ssh命令，没有任何技术含量。无脑懒人可以把这个在.bashrc里面alias一下，记住要source哦。





1. 在服务器间跳转

有些时候，你可能没法直接连接到某台服务器，而需要使用一台中间服务器进行中转，这个过程也可以自动化。首先确保你已经为服务器配置了公钥访问，并开启了agent forwarding，现在你就可以通过2条命令来连接目标服务器，不会有任何提示输入：

$ ssh gateway

gateway $ ssh db

然后在你的本地SSH配置中，添加下面这条配置：

Host db

HostName db.internal.example.com

ProxyCommand ssh gateway netcat -q 600 %h %p

现在你就可以通过一条命令来直接连接目标服务器了:

$ ssh db

这里你可能会需要等待长一点的时间，因为SSH需要进行两次认证，，注意netcat也有可能被写成nc或者ncat或者前面还需要加上g，你需要检查你的中间服务器来确定实际的参数。
 -->






{% highlight text %}
{% endhighlight %}
