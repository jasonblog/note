---
title: Linux 网络设置
layout: post
comments: true
category: [linux, network]
language: chinese
keywords: linux,网络设置
description: 主要记录下在 Linux 中，一些常见的网络配置，例如 IP 地址、路由、MAC 地址、主机名等设置方式。
---


<!-- more -->

![ftp logo]({{ site.url }}/images/network/ftp-logo.jpg "ftp logo"){: .pull-center width="70%" }

## 简介

FTP 服务器登录通常有三种不同的身份，分别是: 1) 实体账号 real user；2) 访客 guest；3) 匿名用户 anonymous。

通常实体帐号拥有较多的权限，而匿名用户通常只有上传和下载的功能。

### 安装使用

可以通过如下步骤安装 FTP 服务器。

{% highlight text %}
----- 安装服务器以及客户端
# yum install vsftpd ftp

----- 启动服务器
# systemctl start vsftpd

----- 使用客户端
$ ftp user@hostname
ftp> open 127.1         # 打开链接，需要用户登陆
ftp> pwd                # 查看当前目录，一般默认是$HOME
ftp> ls                 # 查看目录以及文件
ftp> cd /some/dir       # 切换目录
ftp> lcd /some/dir      # 切换本地目录
ftp> get /some/file     # 下载文件
ftp> mget *.txt         # 同时下载多个文件

ftp> put file           # 上传文件
ftp> mput *.txt
{% endhighlight %}

也可以使用 `anonymous` 以及空密码登陆。

<!--
ascii: 使用ascii类型传输方式。
bin: 使用二进制文件传输方式。
close: 中断与远程服务器的ftp会话(与open对应)。
delete remote-file: 删除远程主机文件。
mkdir: 创建目录
mput local-files: 上传多个文件
open host[port]: 建立指定ftp服务器连接，可指定连接端口。
put local-file[remote-file]: 上传文件
$ macro-ame[args]： 执行宏定义macro-name。
account[password]： 提供登录远程系统成功后访问系统资源所需的补充口令。
append local-file[remote-file]：将本地文件追加到远程系统主机，若未指定远程系统文件名，则使用本地文件名。
bell：每个命令执行完毕后计算机响铃一次。
bye：退出ftp会话过程。
case：在使用mget时，将远程主机文件名中的大写转为小写字母。
cd remote-dir：进入远程主机目录。
cdup：进入远程主机目录的父目录。
chmod mode file-name：将远程主机文件file-name的存取方式设置为mode，如：chmod 777 a.out。

cr：使用asscii方式传输文件时，将回车换行转换为回行。
debug[debug-value]：设置调试方式， 显示发送至远程主机的每条命令，如：deb up 3，若设为0，表示取消debug。
dir[remote-dir][local-file]：显示远程主机目录，并将结果存入本地文件
disconnection：同close。
form format：将文件传输方式设置为format，缺省为file方式。
get remote-file[local-file]： 将远程主机的文件remote-file传至本地硬盘的local-file。
glob：设置mdelete，mget，mput的文件名扩展，缺省时不扩展文件名，同命令行的-g参数。
hash：每传输1024字节，显示一个hash符号(#)。
help[cmd]：显示ftp内部命令cmd的帮助信息，如：help get。
idle[seconds]：将远程服务器的休眠计时器设为[seconds]秒。
image：设置二进制传输方式(同binary)。
lcd[dir]：将本地工作目录切换至dir。
ls[remote-dir][local-file]：显示远程目录remote-dir， 并存入本地文件local-file。
macdef macro-name：定义一个宏，遇到macdef下的空行时，宏定义结束。
mdelete[remote-file]：删除远程主机文件。
mdir remote-files local-file：与dir类似，但可指定多个远程文件，如 ：mdir *.o.*.zipoutfile 。
mget remote-files：传输多个远程文件。
mkdir dir-name：在远程主机中建一目录。
mls remote-file local-file：同nlist，但可指定多个文件名。
mode[modename]：将文件传输方式设置为modename， 缺省为stream方式。
modtime file-name：显示远程主机文件的最后修改时间。
mput local-file：将多个文件传输至远程主机。
newer file-name： 如果远程机中file-name的修改时间比本地硬盘同名文件的时间更近，则重传该文件。
nlist[remote-dir][local-file]：显示远程主机目录的文件清单，并存入本地硬盘的local-file。
nmap[inpattern outpattern]：设置文件名映射机制， 使得文件传输时，文件中的某些字符相互转换， 如：nmap $1.$2.$3[$1，$2].[$2，$3]，则传输文件a1.a2.a3时，文件名变为a1，a2。 该命令特别适用于远程主机为非UNIX机的情况。
ntrans[inchars[outchars]]：设置文件名字符的翻译机制，如ntrans1R，则文件名LLL将变为RRR。
passive：进入被动传输方式。
prompt：设置多个文件传输时的交互提示。
proxy ftp-cmd：在次要控制连接中，执行一条ftp命令， 该命令允许连接两个ftp服务器，以在两个服务器间传输文件。第一条ftp命令必须为open，以首先建立两个服务器间的连接。
put local-file[remote-file]：将本地文件local-file传送至远程主机。
pwd：显示远程主机的当前工作目录。
quit：同bye，退出ftp会话。
quote arg1，arg2...：将参数逐字发至远程ftp服务器，如：quote syst.
recv remote-file[local-file]：同get。
reget remote-file[local-file]：类似于get， 但若local-file存在，则从上次传输中断处续传。
rhelp[cmd-name]：请求获得远程主机的帮助。
rstatus[file-name]：若未指定文件名，则显示远程主机的状态， 否则显示文件状态。
rename[from][to]：更改远程主机文件名。
reset：清除回答队列。
restart marker：从指定的标志marker处，重新开始get或put，如：restart 130。
rmdir dir-name：删除远程主机目录。
runique：设置文件名只一性存储，若文件存在，则在原文件后加后缀.1， .2等。
send local-file[remote-file]：同put。
sendport：设置PORT命令的使用。
site arg1，arg2...：将参数作为SITE命令逐字发送至远程ftp主机。
size file-name：显示远程主机文件大小，如：site idle 7200。
status：显示当前ftp状态。
struct[struct-name]：将文件传输结构设置为struct-name， 缺省时使用stream结构。
sunique：将远程主机文件名存储设置为只一(与runique对应)。
system：显示远程主机的操作系统类型。
tenex：将文件传输类型设置为TENEX机的所需的类型。
tick：设置传输时的字节计数器。
trace：设置包跟踪。
type[type-name]：设置文件传输类型为type-name，缺省为ascii，如:type binary，设置二进制传输方式。
umask[newmask]：将远程服务器的缺省umask设置为newmask，如：umask 3
user user-name[password][account]：向远程主机表明自己的身份，需要口令时，必须输入口令，如：user anonymous my@email。
verbose：同命令行的-v参数，即设置详尽报告方式，ftp 服务器的所有响 应都将显示给用户，缺省为on.
?[cmd]：同help.
下载cmd  中登入后，用get 文件名即可下载，下载文件放在才C:\Documents and Settings\Administrator
上传put e:\linghongli.txt即可上传了
-->

## 链接模式

FTP 在建立链接时需要两个通道，分别为命令通道和数据通道，命令通道通常为 21 号端口，数据通道通常为 22 号端口。FTP 在建立链接时分为主动和被动链接，注意所谓主动被动是在建立数据通道时对于服务器而言。

### 主动模式

![ftp connect active]({{ site.url }}/images/network/ftp-connect-active.gif  "ftp connect active"){: .pull-center }

FTP 默认是主动链接，链接过程如下。

1. 建立命令通道，客户端与 FTP 服务器端的 port 21 通过三次握手链接，建立链接后客户端便可以通过这个通道来对 FTP 服务器下达指令，例如包括查询文件名、下载、上传等。

<!--
2. 客户端发出Active链接请求且告知端口号
	通知 FTP 服务器端使用 active 且告知连接的端口， FTP 服务器的 21 埠号主要用在命令的下达，当牵涉到数据流时，就不是使用这个通道了。客户端在需要数据的情况下，会告知服务器端要用什么方式来联机，如果是主动式 (active) 联机时，客户端会先随机启用一个端口 (port BB) ，且透过命令通道告知 FTP 服务器这两个信息，并等待 FTP 服务器的联机。</li><li>

<font color="blue">FTP 服务器『主动』向客户端联机</font><br />
FTP 服务器由命令通道了解客户端的需求后，会主动的由 20 这个端口向客户端的 port BB 联机。此时 FTP 的客户端与服务器端共会建立两条联机，分别用在命令的下达与数据的传递。</li></ol>
-->





{% highlight text %}
{% endhighlight %}
