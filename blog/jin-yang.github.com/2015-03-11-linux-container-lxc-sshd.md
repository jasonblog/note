---
title: LXC sshd 單進程啟動
layout: post
comments: true
language: chinese
category: [linux,misc]
keywords: linux,lxc,sshd
description: 在之前的 [LXC 簡介](/blog/linux-lxc-introduce.html) 中介紹瞭如何搭建 CentOS 7 容器，在此介紹如何單獨啟動 sshd 這一個進程。這樣，實際上我們可以通過這種方式啟動類似 MySQL 之類的服務，從而減小成本。接下來，我們看看是如何啟動的。
---

在之前的 [LXC 簡介](/blog/linux-lxc-introduce.html) 中介紹瞭如何搭建 CentOS 7 容器，在此介紹如何單獨啟動 sshd 這一個進程。這樣，實際上我們可以通過這種方式啟動類似 MySQL 之類的服務，從而減小成本。

接下來，我們看看是如何啟動的。

<!-- more -->

## 啟動 sshd

當通過模板創建容器時，```lxc-create``` 實際會調用在 ```/usr/share/lxc/templates``` 中相應的腳本，在此，其中 sshd 會調用 ```lxc-sshd``` 腳本創建，而 ```lxc-create``` 實際會將命令行進行轉換，然後傳遞給腳本，可以在腳本中通過 ```echo $*``` 查看。

### 創建啟動鏡像

如下是創建和啟動鏡像的過程。

{% highlight text %}
----- 0. 【主機】創建免密碼的公私鑰信息，並將公鑰上傳到容器中
# ssh-keygen -t rsa -f ~/.ssh/id_lxc -N ""

----- 1. 【主機】創建鏡像，其中auth-key可選，可以在後面配置
# lxc-create -t sshd -n sshd-test -- --auth-key ~/.ssh/id_lxc.pub

----- 2. 【主機】後臺啟動鏡像
# lxc-start -n sshd-test -l DEBUG -d
{% endhighlight %}

如上，配置保存在 ```/var/lib/lxc/sshd-test/config``` 文件中；啟動日誌會保存在 ```/var/log/lxc/sshd-test.log``` 中，可以查看詳細的日誌。

注意使用前需要保證 ```lxcbr0``` 存在，如果不存在則通過如下方式啟動。

{% highlight text %}
# brctl addbr lxcbr0
# ifconfig lxcbr0 192.168.2.100
{% endhighlight %}

正常來說，此時應該可以正常提供服務了，不過經常遇到 IP 未設置的情況，此時可以通過如下方式設置。

{% highlight text %}
----- 【主機】查看lxcbr0的IP地址
# ifconfig lxcbr0

----- 【容器】配置IP地址，需要與br在同一個網段
# lxc-attach -n sshd-test
# ifconfig eth0 192.168.2.102

----- 【容器】啟動sshd服務器，注意需要是絕對地址，否則報錯
# /usr/sbin/sshd

----- 【主機】連接，通過-i指定私鑰
# ssh 192.168.2.102 -i ~/.ssh/id_lxc
{% endhighlight %}

在上述設置 IP 時，建議不要在同一個網段，否則路由可能會出現問題，如果這樣，那麼就需要對路由進行調整。

### 設置公私鑰認證

在通過 ```lxc-create``` 創建時，在腳本中實際上可以傳入一個 ```auth_key```，也就是一個公鑰文件，如果沒有進行設置，也可以通過如下方法添加。

{% highlight text %}
----- 1. 生成免密登陸的公私鑰
# ssh-keygen -t rsa -f ~/.ssh/id_lxc -N ""

----- 2. 將公鑰信息保存在authorized_keys文件中
# cd /var/lib/lxc/sshd-test/rootfs/root && mkdir -p .ssh
# cp ~/.ssh/id_lxc.pub .ssh/authorized_keys

----- 3. 更改權限，sshd對權限要求比較嚴格
# chown -R 0:0 .ssh
# chmod 700 .ssh
{% endhighlight %}

以 sshd 為例，首先會通過 install_sshd() 創建目錄。


## 常見錯誤

### unknown capability

出於安全考慮，上述是在啟動時嘗試刪除一些功能，但是卻沒找到，暫時還不太清除原因，可以通過將配置文件中的如下內容註釋掉解決。

{% highlight text %}
lxc.cap.drop = sys_module mac_admin mac_override sys_time
{% endhighlight %}


{% highlight text %}
{% endhighlight %}
