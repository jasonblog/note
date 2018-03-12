---
title: Linux 用户管理
layout: post
comments: true
language: chinese
category: [linux,misc]
keywords: linux,用户管理
description: Linux 是多用户系统，可以允许多个用户登陆，这里简单介绍与用户管理相关的内容。
---

Linux 是多用户系统，可以允许多个用户登陆，这里简单介绍与用户管理相关的内容。

<!-- more -->

## 用户管理

在 CentOS 中，useradd 和 adduser 是相同的；Ubuntu 上可能会有所区别，`/etc/login.defs` 定义了部分在创建用户时的默认配置选项。

useradd 的操作的一般步骤为：

1. 帐号信息添加到 `/etc/passwd`、`/etc/shadow`、`/etc/group`、`/etc/gshadow` 文件中。
2. 创建 `/home/USERNAME` 目录。
3. 将 `/etc/skel ` 目录下的内容复制到 `/home/USERNAME` 目录下，很多是隐藏文件。
4. 在 `/var/mail` 目录下创建邮箱帐号。

其中第一步基本上所有的发行版本都会执行，而剩余的不同的发行版本会有不同的操作。最后还需要通过 `passwd USERNAME` 命令设置用户的密码，CentOS 在没有设置密码时无法登陆。

在通过 `userdel USERNAME` 删除用户时，会删除 `/etc/passwd`、`/etc/group` 中的内容，但是不会删除 `/home/user` 目录以及 `/var/mail` 目录下文件，可以使用 `-r` 删除这两项。

通过 `id user` 命令查看用户。

### 常见操作

新创建一个用户，并添加到一个已知的附加用户组里，如果用户组不存在则需要手动创建，也可以通过逗号分隔指定多个用户组。

{% highlight text %}
# useradd -G admins,ftp,www,developers foobar
{% endhighlight %}

注意，如上的方法会同时创建一个主用户组。

也可以通过 `-g` 参数将新增加的用户初始化为指定为登录组，也就是主要用户组。

{% highlight text %}
# useradd -g developers foobar
{% endhighlight %}

其它常见操作可以参考如下：

{% highlight text %}
----- 修改主要用户组为apache
# usermod -g apache foobar
----- 将已经存在的用户添加到一个组里面，此时会在/etc/group的apache分组最后一列增加
# usermod -a -G apache foobar
{% endhighlight %}

其中 `-g` 表示 `initial login group`，`-G` 表示 `supplementary groups`。

### UID VS. EUID

Linux 系统中每个进程都有 2 个 ID，分别为用户 ID(uid) 和有效用户 ID(euid)；其中，前者一般表示进程的创建者 (表示通过那个用户创建)，而 EUID 表示进程对于文件和资源的访问权限 (表示拥有那个用户的权限)。

一般来说，UID 和 EUID 是相同的，如果用户设置了 setuid 权限，那么两者将会不同。

命令行可以通过 `chmod u+s` 或 `chmod g+s` 来设置二进制的可执行文件的 euid，注意，只对可执行文件设置，一般来说 passwd 文件是具有该权限的。

{% highlight c %}
#include <pwd.h>
#include <stdio.h>
#include <errno.h>
#include <fcntl.h>
#include <string.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>

int main(void)
{
	struct passwd * pw;

	pw = getpwnam("monitor"); // 注意，入参不能为NULL
	if (pw == NULL) {
		printf("User is not exist\n");
		return -1;
	}

	printf("Password information:\n");
	printf("    pw->pw_name = %s\n", pw->pw_name);
	printf("    pw->pw_passwd = %s\n", pw->pw_passwd);
	printf("    pw->pw_uid = %d\n", pw->pw_uid);
	printf("    pw->pw_gid = %d\n", pw->pw_gid);
	printf("    pw->pw_gecos = %s\n", pw->pw_gecos);
	printf("    pw->pw_dir = %s\n", pw->pw_dir);
	printf("    pw->pw_shell = %s\n", pw->pw_shell);

	if (setgid(pw->pw_gid) == -1) {
		fprintf(stderr, "Failed to set group id(%d): %s\n",
				pw->pw_gid, strerror(errno));
	}

	if (setuid(pw->pw_uid) == -1) {
		fprintf(stderr, "Failed to set user id(%d): %s\n",
				pw->pw_uid, strerror(errno));
	}

	fprintf(stdout, "Current user/group info: UID=%d, EUID=%d, GID=%d, EGID=%d\n",
				getuid(), geteuid(), getgid(), getegid());

	int fd = open("/tmp/mysql.test", 0);
	if (fd < 0) {
		fprintf(stderr, "Failed to openfile: %s\n", strerror(errno));
	}
	if (fd > 0)
		close(fd);

	return 0;
}
{% endhighlight %}

### 过期设置

通过如下方法设置过期条件。

{% highlight text %}
# useradd USER -e 01/28/12               # 创建用户时指定过期条件
# grep EXPIRE /etc/default/useradd       # 或者修改模板对应的默认参数
# useradd -D -e 01/19/12                 # 修改默认新建帐户过期时间
# useradd -D | grep EXPIR                # 查看
# chage -l USER                          # 查看用户的过期时间
# usermod -e 01/28/12 USER               # 修改用户属性
# chage -E 01/28/12 USER                 # 调整账户过期时间
{% endhighlight %}

### FAQ

当排查系统用户是否有访问权限时，可以通过如下方法：

{% highlight text %}
# su - monitor -s /bin/bash -c 'cat /tmp/mysql.test'
# su - monitor -g monitor -s /bin/bash -c 'cat /tmp/mysql.test'
{% endhighlight %}


## 审计

CentOS 系统上，用户登录历史存储在以下这些文件中：

* `/var/run/utmp` 记录当前打开的会话，会被 who 和 w 记录当前有谁登录以及他们正在做什么。
* `/var/log/wtmp` 存储系统连接历史记录，被 last 工具用来记录最后登录的用户的列表。
* `/var/log/btmp` 失败的登录尝试，被 lastb 工具用来记录最后失败的登录尝试的列表。

实际上，可以直接通过 `utmpdump` 将上述文件中保存的数据 dump 出来，另外，默认的登陆日志保存在 `/var/log/secure` 。

{% highlight text %}
----- 当前当登录的用户的信息
# who
huey     pts/1        2015-05-11 18:29 (192.168.1.105)
sugar    pts/2        2015-05-11 18:29 (192.168.1.105)

----- 登录的用户及其当前执行的任务
# w
18:30:51 up 3 min,  2 users,  load average: 0.10, 0.14, 0.06
USER     TTY      FROM              LOGIN@   IDLE   JCPU   PCPU WHAT
huey     pts/1    192.168.1.105    18:29    3.00s  0.52s  0.00s w
sugar    pts/2    192.168.1.105    18:29    1:07   0.47s  0.47s -bash

-----  当前当登录的用户的用户名
# users
huey sugar

-----  当前与过去登录系统的用户的信息
# last
root     pts/3        192.168.1.105    Mon May 11 18:33 - 18:33  (00:00)
sugar    pts/2        192.168.1.105    Mon May 11 18:32   still logged in
sugar    pts/2        192.168.1.105    Mon May 11 18:29 - 18:32  (00:02)
huey     pts/1        192.168.1.105    Mon May 11 18:29   still logged in
reboot   system boot  3.5.0-43-generic Mon May 11 18:27 - 18:33  (00:05)
huey     pts/1        192.168.1.105    Sat May  9 10:57 - 17:31  (06:33)

----- 所有登录系统失败的用户的信息
# lastb
btmp begins Sat May  9 09:48:59 2015

----- 用户最后一次登录的信息
# lastlog
root             pts/3    192.168.1.105    一  5月 11 18:36:43 +0800 2015
huey             pts/1    192.168.1.105    一  5月 11 18:29:40 +0800 2015
mysql                                      **从未登录过**
sshd                                       **从未登录过**

----- 用户连接时间的统计数据
-----   1. 每天的总的连接时间
# ac -d
May  9  total        6.55
Today   total        0.54
----- 2. 每个用户的总的连接时间
# ac -p
    huey                                 6.78
    sugar                                0.23
    root                                 0.12
    total        7.13
{% endhighlight %}

<!--
## 安全加固


centos限制登录失败次数并锁定设置

vim /etc/pam.d/login

在#%PAM-1.0下面添加：
auth required pam_tally2.so deny=5 unlock_time=180 #登录失败5次锁定180秒，不包含root
auth required pam_tally2.so deny=5 unlock_time=180 even_deny_root root_unlock_time=180 #包含root

## 每次修改密码禁止使用前N次用过的密码

出于安全考虑，要求修改linux密码策略，每次修改密码时设置的新密码不能是前n次使用过的密码。配置如下：

Debian / Ubuntu：修改文件 # vi /etc/pam.d/common-password
CentOS / RHEL / RedHat / Fedora 修改文件 # vi /etc/pam.d/system-auth

在 password sufficient pam_unix.so use_authtok md5 shadow remember=10
在相应行的后面添加 remember=10，而不是添加一行！

SUSE比较恶心，找了半天才找到。man pw_check
在/etc/security/pam_pwcheck文件中添加remember=5
passwd:     nullok use_cracklib remember=5

http://www.deer-run.com/~hal/sysadmin/pam_cracklib.html
http://zhidao.baidu.com/link?url=xLcMH0cokvN585CPxKf3QVmmN1wDtgESTpAhl1_cxhPQZ0B3D41DhKZCcXr3E0-1nwfBtSKQWQCNKUGPhRvvcq
-->


## 杂项

简单记录常用的使用技巧。

### wheel

wheel 组是一特殊用户组，被一些 Unix 系统用来控制能否通过 su 命令来切换到 root 用户。

{% highlight text %}
$ grep 'wheel' /etc/group
wheel:x:10:foo,bar,foobar
{% endhighlight %}

可以配置成非 wheel 组的用户不能通过 su 命令切换到 root 用户。

{% highlight text %}
$ grep 'pam_wheel' /etc/pam.d/su
auth            required        pam_wheel.so use_uid

$ grep 'WHEEL' /etc/login.defs
SU_WHEEL_ONLY yes
{% endhighlight %}

这时非 wheel 组的成员用 su 命令切换到 root 时提示权限不够，而用 wheel 组的成员切换没任何问题。


### 忘记root密码

启动进入 Grub 时，通过 e 进入编辑方式，添加 single 参数 (也就是进入单用户模式)，登陆之后，通过 password 修改密码即可。

### 登陆提示信息

涉及的有两个配置文件 `/etc/issue` 以及 `/etc/motd`，其中前者为登陆前的提示，后者为登陆后的提示信息。

如果切换到终端登陆 (注意，是终端，通常为类似 `CTRL+ALT+F2`)，通常会显示提示信息，该信息是在 `/etc/issue` 中进行设置。

{% highlight text %}
----- 提示信息为
CentOS Linux 7 (Core)
Kernel 3.10.0-327.el7.x86_64 on an X86_64

----- 其中/etc/issue配置为
\S
Kernel \r on an \m

----- 配置文件中各个选项的含义为：
    \d 本地端时间的日期；
    \l 显示第几个终端机接口；
    \m 显示硬件的等级 (i386/i486/i586/i686)；
    \n 显示主机的网络名称；
    \o 显示 domain name；
    \r 操作系统的版本 (相当于 uname -r)
    \t 显示本地端时间的时间；
    \s 操作系统的名称；
    \v 操作系统的版本
{% endhighlight %}

其中 motd 为 `Message Of ToDay` 的简称，也就是布告栏信息，每次用户登陆的时候都会将该信息显示在终端，可以添加些维护信息之类的。不过缺点是，如果是图形界面，会看不到这些信息。

一个对于 ROOT 用户常见的提示如下：

{% highlight text %}
We trust you have received the usual lecture from the local System Administrator.
It usually boils down to these three things:
	#1) Respect the privacy of others.
	#2) Think before you type.
	#3) With great power comes great responsibility.
{% endhighlight %}

{% highlight text %}
{% endhighlight %}
