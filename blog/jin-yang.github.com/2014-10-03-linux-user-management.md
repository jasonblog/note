---
title: Linux 用戶管理
layout: post
comments: true
language: chinese
category: [linux,misc]
keywords: linux,用戶管理
description: Linux 是多用戶系統，可以允許多個用戶登陸，這裡簡單介紹與用戶管理相關的內容。
---

Linux 是多用戶系統，可以允許多個用戶登陸，這裡簡單介紹與用戶管理相關的內容。

<!-- more -->

## 用戶管理

在 CentOS 中，useradd 和 adduser 是相同的；Ubuntu 上可能會有所區別，`/etc/login.defs` 定義了部分在創建用戶時的默認配置選項。

useradd 的操作的一般步驟為：

1. 帳號信息添加到 `/etc/passwd`、`/etc/shadow`、`/etc/group`、`/etc/gshadow` 文件中。
2. 創建 `/home/USERNAME` 目錄。
3. 將 `/etc/skel ` 目錄下的內容複製到 `/home/USERNAME` 目錄下，很多是隱藏文件。
4. 在 `/var/mail` 目錄下創建郵箱帳號。

其中第一步基本上所有的發行版本都會執行，而剩餘的不同的發行版本會有不同的操作。最後還需要通過 `passwd USERNAME` 命令設置用戶的密碼，CentOS 在沒有設置密碼時無法登陸。

在通過 `userdel USERNAME` 刪除用戶時，會刪除 `/etc/passwd`、`/etc/group` 中的內容，但是不會刪除 `/home/user` 目錄以及 `/var/mail` 目錄下文件，可以使用 `-r` 刪除這兩項。

通過 `id user` 命令查看用戶。

### 常見操作

新創建一個用戶，並添加到一個已知的附加用戶組裡，如果用戶組不存在則需要手動創建，也可以通過逗號分隔指定多個用戶組。

{% highlight text %}
# useradd -G admins,ftp,www,developers foobar
{% endhighlight %}

注意，如上的方法會同時創建一個主用戶組。

也可以通過 `-g` 參數將新增加的用戶初始化為指定為登錄組，也就是主要用戶組。

{% highlight text %}
# useradd -g developers foobar
{% endhighlight %}

其它常見操作可以參考如下：

{% highlight text %}
----- 修改主要用戶組為apache
# usermod -g apache foobar
----- 將已經存在的用戶添加到一個組裡面，此時會在/etc/group的apache分組最後一列增加
# usermod -a -G apache foobar
{% endhighlight %}

其中 `-g` 表示 `initial login group`，`-G` 表示 `supplementary groups`。

### UID VS. EUID

Linux 系統中每個進程都有 2 個 ID，分別為用戶 ID(uid) 和有效用戶 ID(euid)；其中，前者一般表示進程的創建者 (表示通過那個用戶創建)，而 EUID 表示進程對於文件和資源的訪問權限 (表示擁有那個用戶的權限)。

一般來說，UID 和 EUID 是相同的，如果用戶設置了 setuid 權限，那麼兩者將會不同。

命令行可以通過 `chmod u+s` 或 `chmod g+s` 來設置二進制的可執行文件的 euid，注意，只對可執行文件設置，一般來說 passwd 文件是具有該權限的。

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

	pw = getpwnam("monitor"); // 注意，入參不能為NULL
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

### 過期設置

通過如下方法設置過期條件。

{% highlight text %}
# useradd USER -e 01/28/12               # 創建用戶時指定過期條件
# grep EXPIRE /etc/default/useradd       # 或者修改模板對應的默認參數
# useradd -D -e 01/19/12                 # 修改默認新建帳戶過期時間
# useradd -D | grep EXPIR                # 查看
# chage -l USER                          # 查看用戶的過期時間
# usermod -e 01/28/12 USER               # 修改用戶屬性
# chage -E 01/28/12 USER                 # 調整賬戶過期時間
{% endhighlight %}

### FAQ

當排查系統用戶是否有訪問權限時，可以通過如下方法：

{% highlight text %}
# su - monitor -s /bin/bash -c 'cat /tmp/mysql.test'
# su - monitor -g monitor -s /bin/bash -c 'cat /tmp/mysql.test'
{% endhighlight %}


## 審計

CentOS 系統上，用戶登錄歷史存儲在以下這些文件中：

* `/var/run/utmp` 記錄當前打開的會話，會被 who 和 w 記錄當前有誰登錄以及他們正在做什麼。
* `/var/log/wtmp` 存儲系統連接歷史記錄，被 last 工具用來記錄最後登錄的用戶的列表。
* `/var/log/btmp` 失敗的登錄嘗試，被 lastb 工具用來記錄最後失敗的登錄嘗試的列表。

實際上，可以直接通過 `utmpdump` 將上述文件中保存的數據 dump 出來，另外，默認的登陸日誌保存在 `/var/log/secure` 。

{% highlight text %}
----- 當前當登錄的用戶的信息
# who
huey     pts/1        2015-05-11 18:29 (192.168.1.105)
sugar    pts/2        2015-05-11 18:29 (192.168.1.105)

----- 登錄的用戶及其當前執行的任務
# w
18:30:51 up 3 min,  2 users,  load average: 0.10, 0.14, 0.06
USER     TTY      FROM              LOGIN@   IDLE   JCPU   PCPU WHAT
huey     pts/1    192.168.1.105    18:29    3.00s  0.52s  0.00s w
sugar    pts/2    192.168.1.105    18:29    1:07   0.47s  0.47s -bash

-----  當前當登錄的用戶的用戶名
# users
huey sugar

-----  當前與過去登錄系統的用戶的信息
# last
root     pts/3        192.168.1.105    Mon May 11 18:33 - 18:33  (00:00)
sugar    pts/2        192.168.1.105    Mon May 11 18:32   still logged in
sugar    pts/2        192.168.1.105    Mon May 11 18:29 - 18:32  (00:02)
huey     pts/1        192.168.1.105    Mon May 11 18:29   still logged in
reboot   system boot  3.5.0-43-generic Mon May 11 18:27 - 18:33  (00:05)
huey     pts/1        192.168.1.105    Sat May  9 10:57 - 17:31  (06:33)

----- 所有登錄系統失敗的用戶的信息
# lastb
btmp begins Sat May  9 09:48:59 2015

----- 用戶最後一次登錄的信息
# lastlog
root             pts/3    192.168.1.105    一  5月 11 18:36:43 +0800 2015
huey             pts/1    192.168.1.105    一  5月 11 18:29:40 +0800 2015
mysql                                      **從未登錄過**
sshd                                       **從未登錄過**

----- 用戶連接時間的統計數據
-----   1. 每天的總的連接時間
# ac -d
May  9  total        6.55
Today   total        0.54
----- 2. 每個用戶的總的連接時間
# ac -p
    huey                                 6.78
    sugar                                0.23
    root                                 0.12
    total        7.13
{% endhighlight %}

<!--
## 安全加固


centos限制登錄失敗次數並鎖定設置

vim /etc/pam.d/login

在#%PAM-1.0下面添加：
auth required pam_tally2.so deny=5 unlock_time=180 #登錄失敗5次鎖定180秒，不包含root
auth required pam_tally2.so deny=5 unlock_time=180 even_deny_root root_unlock_time=180 #包含root

## 每次修改密碼禁止使用前N次用過的密碼

出於安全考慮，要求修改linux密碼策略，每次修改密碼時設置的新密碼不能是前n次使用過的密碼。配置如下：

Debian / Ubuntu：修改文件 # vi /etc/pam.d/common-password
CentOS / RHEL / RedHat / Fedora 修改文件 # vi /etc/pam.d/system-auth

在 password sufficient pam_unix.so use_authtok md5 shadow remember=10
在相應行的後面添加 remember=10，而不是添加一行！

SUSE比較噁心，找了半天才找到。man pw_check
在/etc/security/pam_pwcheck文件中添加remember=5
passwd:     nullok use_cracklib remember=5

http://www.deer-run.com/~hal/sysadmin/pam_cracklib.html
http://zhidao.baidu.com/link?url=xLcMH0cokvN585CPxKf3QVmmN1wDtgESTpAhl1_cxhPQZ0B3D41DhKZCcXr3E0-1nwfBtSKQWQCNKUGPhRvvcq
-->


## 雜項

簡單記錄常用的使用技巧。

### wheel

wheel 組是一特殊用戶組，被一些 Unix 系統用來控制能否通過 su 命令來切換到 root 用戶。

{% highlight text %}
$ grep 'wheel' /etc/group
wheel:x:10:foo,bar,foobar
{% endhighlight %}

可以配置成非 wheel 組的用戶不能通過 su 命令切換到 root 用戶。

{% highlight text %}
$ grep 'pam_wheel' /etc/pam.d/su
auth            required        pam_wheel.so use_uid

$ grep 'WHEEL' /etc/login.defs
SU_WHEEL_ONLY yes
{% endhighlight %}

這時非 wheel 組的成員用 su 命令切換到 root 時提示權限不夠，而用 wheel 組的成員切換沒任何問題。


### 忘記root密碼

啟動進入 Grub 時，通過 e 進入編輯方式，添加 single 參數 (也就是進入單用戶模式)，登陸之後，通過 password 修改密碼即可。

### 登陸提示信息

涉及的有兩個配置文件 `/etc/issue` 以及 `/etc/motd`，其中前者為登陸前的提示，後者為登陸後的提示信息。

如果切換到終端登陸 (注意，是終端，通常為類似 `CTRL+ALT+F2`)，通常會顯示提示信息，該信息是在 `/etc/issue` 中進行設置。

{% highlight text %}
----- 提示信息為
CentOS Linux 7 (Core)
Kernel 3.10.0-327.el7.x86_64 on an X86_64

----- 其中/etc/issue配置為
\S
Kernel \r on an \m

----- 配置文件中各個選項的含義為：
    \d 本地端時間的日期；
    \l 顯示第幾個終端機接口；
    \m 顯示硬件的等級 (i386/i486/i586/i686)；
    \n 顯示主機的網絡名稱；
    \o 顯示 domain name；
    \r 操作系統的版本 (相當於 uname -r)
    \t 顯示本地端時間的時間；
    \s 操作系統的名稱；
    \v 操作系統的版本
{% endhighlight %}

其中 motd 為 `Message Of ToDay` 的簡稱，也就是布告欄信息，每次用戶登陸的時候都會將該信息顯示在終端，可以添加些維護信息之類的。不過缺點是，如果是圖形界面，會看不到這些信息。

一個對於 ROOT 用戶常見的提示如下：

{% highlight text %}
We trust you have received the usual lecture from the local System Administrator.
It usually boils down to these three things:
	#1) Respect the privacy of others.
	#2) Think before you type.
	#3) With great power comes great responsibility.
{% endhighlight %}

{% highlight text %}
{% endhighlight %}
