---
title: 容器之 CGroup
layout: post
comments: true
language: chinese
category: [linux,misc]
keywords: linux,cgroup,container
description:
---

在 CentOS 7 中，已經通過 systemd 替換了之前的 cgroup-tools 工具，為了防止兩者衝突，建議只使用 systemd ，只有對於一些不支持的，例如 net_prio ，才使用 cgroup-tools 工具。

在此，簡單介紹其使用。

<!-- more -->

## 簡介

在系統設計時，經常會遇到類似的需求，就是希望能限制某個或者某些進程的分配資源。

由此，就有了容器的概念，在容器中，有分配好的特定比例的 CPU、IO、內存、網絡等資源，這就是 controller group ，簡稱為 cgroup ，最初由 Google 工程師提出，後來被整合進 Linux 內核中。

cgroup 本身提供了將進程進行分組化管理的功能和接口的基礎結構。

## 使用簡介

在 CentOS 7 中需要通過 ```yum install libcgroup libcgroup-tools``` 安裝額外的 cgroup 工具，對系統來說，默認會掛載到 ```/sys/fs/cgroup/``` 目錄下。

{% highlight text %}
----- 查看系統已經存在cgroup子系統及其掛載點
# lssubsys -am
----- 或者通過mount查看cgroup類型的掛載點
# mount -t cgroup

----- 可以命令行掛載和卸載子系統，此時再次執行上述命令將看不到memory掛載點
# umount /sys/fs/cgroup/memory/
----- 掛載cgroup的memory子系統，其中最後的cgroup參數是在/proc/mounts中顯示的名稱
# mount -t cgroup -o memory cgroup /sys/fs/cgroup/memory/
# mount -t cgroup -o memory none /sys/fs/cgroup/memory/
{% endhighlight %}

另外，在 CentOS 中有 ```/etc/cgconfig.conf``` 配置文件，該文件中可用來配置開機自動啟動時掛載的條目：

{% highlight text %}
mount {
    net_prio = /sys/fs/cgroup/net_prio;
}
{% endhighlight %}

然後，通過 ```systemctl restart cgconfig.service``` 重啟服務即可，然後可以通過如下方式使用。

### 使用步驟

簡單介紹如何通過 ```libcgroup-tools``` 創建分組並設置資源配置參數。

#### 1. 創建控制組群

可以通過如下方式創建以及刪除群組，創建後會在 cpu 掛載目錄下 ```/sys/fs/cgroup/cpu/``` 目錄下看到一個新的目錄 test，這個就是新創建的 cpu 子控制組群。

{% highlight text %}
# cgcreate -g cpu:/test
# cgdelete -g cpu:/test
{% endhighlight %}

#### 2. 設置組群參數

```cpu.shares``` 是控制 CPU 的一個屬性，更多的屬性可以到 ```/sys/fs/cgroup/cpu``` 目錄下查看，默認值是 1024，值越大，能獲得更多的 CPU 時間。

{% highlight text %}
# cgset -r cpu.shares=512 test
{% endhighlight %}


#### 3. 將進程添加到控制組群

可以直接將需要執行的命令添加到分組中。

{% highlight text %}
----- 直接在cgroup中執行
# cgexec -g cpu:small some-program
----- 將現有的進程添加到cgroup中
# cgclassify -g subsystems:path_to_cgroups pidlist
{% endhighlight %}

例如，想把 sshd 添加到一個分組中，可以通過如下方式操作。

{% highlight text %}
# cgclassify -g cpu:/test `pidof sshd`
# cat /sys/fs/cgroup/cpu/test/tasks
{% endhighlight %}

就會看到相應的進程在這個文件中。

## systemd

CentOS 7 中默認的資源隔離是通過 systemd 進行資源控制的，systemd 內部使用 cgroups 對其下的單元進行資源管理，包括 CPU、BlcokIO 以及 MEM，通過 cgroup 可以 。

systemd 的資源管理主要基於三個單元 service、scope 以及 slice：

* service<br>通過 unit 配置文件定義，包括了一個或者多個進程，可以作為整體啟停。
* scope<br>任意進程可以通過 ```fork()``` 方式創建進程，常見的有 session、container 等。
* slice<br>按照層級對 service、scope 組織的運行單元，不單獨包含進程資源，進程包含在 service 和 scope 中。

常用的 slice 有 A) ```system.slice```，系統服務進程可能是開機啟動或者登陸後手動啟動的服務，例如crond、mysqld、nginx等服務；B) ```user.slice``` 用戶登陸後的管理，一般為 session；C) ```machine.slice``` 虛機或者容器的管理。

對於 cgroup 默認相關的參數會保存在 ```/sys/fs/cgroup/``` 目錄下，當前系統支持的 subsys 可以通過 ```cat /proc/cgroups``` 或者 ```lssubsys``` 查看。

<!--
服務進程限制
    PrivateNetwork=[BOOL] :若服務不需要網絡連接可開啟本選項，更加安全。
    PrivateTmp=[BOOl] :由於傳統/tmp目錄是所有本地用戶和服務共用，會帶來很多安全性問題，開啟本選項後，服務將有一個私有的tmp，可防止攻擊。
    InaccessibleDirectories= :限制服務進程訪問某些目錄。
    ReadOnlyDirectories= :設置服務進程對某些目錄只讀，保證目錄下數據不被服務意外撰改。
    OOMScoreAdjust= :調整服務OOM值，從-1000（對該服務進程關閉OOM）到1000(嚴格)。
    IOSchedulingClass= ：IO調度類型，可設置為0，1,2,3中的某個數值,分配對應none，realtime，betst-effort和idle。
    IOSchedulingPriority= :IO調度優先級，0～7（高到低）。
    CPUSchedulingPriority= :CPU調度優先級，99～1(高到低)
    Nice= :進程調度等級。
-->

### 常見命令

常用命令可以參考如下。

{% highlight text %}
----- 查看slice、scope、service層級關係
# systemd-cgls

----- 當前資源使用情況
# systemd-cgtop

----- 啟動一個服務
# systemd-run --unit=name --scope --slice=slice_name command
   unit   用於標示，如果不使用會自動生成一個，通過systemctl會輸出；
   scope  默認使用service，該參數指定使用scope ；
   slice  將新啟動的service或者scope添加到slice中，默認添加到system.slice，
          也可以添加到已有slice(systemctl -t slice)或者新建一個。
# systemd-run --unit=toptest --slice=test top -b
# systemctl stop toptest

----- 查看當前資源使用狀態
$ systemctl show toptest
{% endhighlight %}

各服務配置保存在 ```/usr/lib/systemd/system/``` 目錄下，可以通過如下命令設置各個服務的參數。

{% highlight text %}
----- 會自動保存到配置文件中做持久化
# systemctl set-property name parameter=value

----- 只臨時修改不做持久化
# systemctl set-property --runtime name property=value

----- 設置CPU和內存使用率
# systemctl set-property httpd.service CPUShares=600 MemoryLimit=500M
{% endhighlight %}

另外，在 systemd 213 版本之後才開始支持 `CPUQuota` 參數，此時可以直接修改 `cpu.cfs_{quota,period}_us` 參數，也就是在 `/sys/fs/cgroup/cpu/` 目錄下。

## 參考

關於 systemd 的資源控制，詳細可以通過 ```man 5 systemd.resource-control``` 命令查看幫助，或者查看 [systemd.resource-control 中文手冊](http://www.jinbuguo.com/systemd/systemd.resource-control.html)；詳細的內容可以參考 [Resource Management Guide](https://access.redhat.com/documentation/en-US/Red_Hat_Enterprise_Linux/7/html/Resource_Management_Guide/index.html) 。

<!--
Systemd 進程管理相關
http://fangpeishi.com/systemd_chapter2.html


https://yq.aliyun.com/articles/54458
http://www.cnblogs.com/yanghuahui/p/3751826.html
https://yq.aliyun.com/articles/54483
systemd-cgls



https://github.com/francisbouvier/cgroups
-->

{% highlight text %}
{% endhighlight %}
