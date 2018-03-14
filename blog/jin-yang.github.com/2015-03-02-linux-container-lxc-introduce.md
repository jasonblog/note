---
title: LXC 簡介
layout: post
comments: true
language: chinese
category: [linux,misc]
keywords: linux,container,lxc,容器
description: LXC (Linux Containters) 是一種基於內核容器的用戶空間接口，提供了一系列創建、配置、管理的接口。其提供了比 chroot 更強的資源隔離，但是與硬件提供的完全虛擬化又不同，其目標為創建一個不需要獨立內核，但近可能接近標準 Linux 安裝的環境。在此簡單介紹其使用方法，以及常用的命令。
---

LXC (Linux Containters) 是一種基於內核容器的用戶空間接口，提供了一系列創建、配置、管理的接口。

其提供了比 chroot 更強的資源隔離，但是與硬件提供的完全虛擬化又不同，其目標為創建一個不需要獨立內核，但近可能接近標準 Linux 安裝的環境。

在此簡單介紹其使用方法，以及常用的命令。

<!-- more -->

![linux container logo]({{ site.url }}/images/linux/container/containers-logo.png "linux container logo"){: .pull-center}

## 簡介

首先大致介紹一下 LXC 是如何做到的。簡單來說就是，一臺機器上跑了一個 Linux 內核，然後通過 LXC 跑了 N 個不同的 Linux 發行版本的用戶程序，包括 init 。

所謂的 Linux 發行版本，如 RedHat、Ubuntu、CentOS、Debian、Fedora 等等，其實際上都是基於 Linux 內核，有可能打了不同的 patch，但真正不同的只是用戶空間程序，如 init 用的是 systemV、upstart 還是飽受爭議的 systemd，圖形界面用的是 Gnome、KDE、LXDE、Xfce 甚至是 FCWM。

這也要多虧了所謂的 Linux 哲學：提供機制，而非策略（說實話沒有找到對應的英語原話）。

### 安裝 lxc

在 CentOS 中，如果通過 YUM 安裝，則依賴 EPEL 庫，可以通過如下方法安裝。

{% highlight text %}
# yum install libcgroup lxc lxc-libs lxc-templates --enablerepo=epel
{% endhighlight %}

在 ```/usr/share/lxc/templates``` 目錄下保存了常用的模板，可以用非 root 創建容器，當然這會更安全，不過會不方便，如不能掛載文件系統、不能創建設備節點等，可以參考 [Creating unprivileged containers as a user](https://linuxcontainers.org/lxc/getting-started/)，後面再研究，暫時還是用 root 用戶。

#### 源碼編譯

對於源碼編譯的 lxc，其目錄中的內容類似，只是默認會添加 ```/usr/local``` 的 prefix，可以如下使用 ```/usr``` 的 prefix，此時下面討論的內容會添加 /usr 前綴。

{% highlight text %}
$ ./configure --prefix=/usr && make
# make install
{% endhighlight %}

#### 檢查

編譯安裝完成之後，可以通過如下命令檢查環境。

{% highlight text %}
$ lxc-checkconfig
{% endhighlight %}

### 配置文件

通過 yum 安裝的 lxc ，常見的配置文件以及目錄如下。

* /etc/lxc/default.conf<br>LXC 的默認配置文件，如果在創建容器時不指定配置文件，將默認使用這個配置文件，主要包括了對網絡以及命名空間的設置；其它一些配置文件保存在 /usr/share/lxc/config 目錄下。

* /var/lib/lxc/<br>保存了容器的相關配置，包括已經創建容器的根文件系統 (rootfs)、配置文件 (config)。

* /usr/share/lxc/templates/<br>保存了當前 LXC 支持的各種發行版的 linux 的模板配置文件，如 lxc-ubuntu、lxc-fedora、lxc-busybox、lxc-sshd 等。

* /var/log/lxc<br>日誌默認保存在該目錄下，容器名.log 。

注意，在創建一個類似 ubuntu、fedora 相關的容器時，通常會在 ```/var/cache/lxc/``` 目錄下保存一份，那麼第二次創建時，就不需要再次下載。


## 使用 LXC

在此簡單介紹如何創建並使用 LXC，以 CentOS-7 為例，並簡單介紹創建 CentOS 的過程。注意，如果安裝 ubuntu 鏡像，需要安裝 debootstrap (用於安裝 rootfs) 。

通過 LXC 可以啟動一個 Linux 發佈版本的鏡像，也可以只啟動一個進程，下面仍以 root 創建一個 CentOS 7 鏡像為例。

### 1. 創建鏡像

可以根據現有的模板創建，可以參考 ```/usr/share/lxc/templates/lxc-centos``` ，該文件實際就是一個 bash 腳本，用來創建 rootfs 。

{% highlight text %}
----- 根據模板創建鏡像，--後為傳入腳本的參數
# lxc-create -t centos -n centos -- --release 7

----- 查看已經安裝的rootfs、config等文件，config文件將在lxc-start中使用
# ls /var/lib/lxc/centos/
config  rootfs/  tmp_root_pass

----- 或者通過該命令查看當前已經安裝的鏡像
# lxc-ls -f
centos

----- 查看設置的密碼，創建後會打印該提示
# cat /var/lib/lxc/centos/tmp_root_pass

----- 直接通過chroot修改密碼
# chroot /var/lib/lxc/centos/rootfs passwd
{% endhighlight %}

在 ```/var/cache/lxc/centos``` 目錄下，會保存臨時文件，這樣創建時只會在第一次下載所需的軟件包，後面如果版本比較舊可能還會 update 。

### 2. 啟動容器

啟動時，如果不指定 ```-d``` 後臺運行，則會將當前的 bash 作為 console 輸出，此時會輸出容器的啟動信息，然後直接進入登陸界面。

{% highlight text %}
----- 後臺啟動容器，並設置日誌級別為DEBUG，日誌可以查看/var/log/lxc/centos.log
# lxc-start -n centos -l DEBUG -d

----- 查看容器啟動的init對應的PID
# ps faux | grep -A 1 "lxc-start -n centos -d"
{% endhighlight %}

是不是發現啟動很快！！！

實際上很大一部分時間是消耗在網絡設置上，如果取消網絡你會發現啟動速度更快。


### 3. 查看狀態

容器啟動之後實際上是一直有一個進程等待接收命令等操作的，對應的容器實際上是一個子進程。

{% highlight text %}
----- 查看容器的運行狀態，包括了其PID、運行狀態、IP等信息
# lxc-info -n centos
Name:           centos
State:          RUNNING
PID:            31649                # init線程的PID
IP:             192.168.122.118      # 容器中的IP
CPU use:        0.15 seconds
BlkIO use:      0 bytes
Memory use:     1.18 MiB
KMem use:       0 bytes
Link:           vethXTBOA0           # 容器外的對端網絡設備
 TX bytes:      1.76 KiB
 RX bytes:      12.95 KiB
 Total bytes:   14.71 KiB

----- 以行顯示當前運行狀態
# lxc-ls -f
NAME   STATE   AUTOSTART GROUPS IPV4 IPV6
centos STOPPED 0         -      -    -

----- 查看啟動的父進程PID
# ps aux | grep 'lxc-start -n centos' | grep -v grep
root  31639  0.0  0.0  30560  1640 pts/9  S+ 12:27 0:00 lxc-start -n centos

----- OK，查看下父進程派生的子進程
# pstree -p 31639
lxc-start(31639)──systemd(31649)─┬─dbus-daemon(31707)
                                 ├─dhclient(31901)
                                 ├─login(31714)───bash(31958)
                                 ├─rsyslogd(31711)─┬─{rsyslogd}(31718)
                                 │                 └─{rsyslogd}(31719)
                                 ├─sshd(31852)
                                 ├─systemd-journal(31665)
                                 └─systemd-logind(31712)

----- 直接查看容器的ip，實際就是在容器中執行命令
# lxc-attach -n centos -- /sbin/ip address list
{% endhighlight %}

### 4. 連接到容器

如果在啟動的時候沒有使用 ```-d``` 選項，則默認使用當前的 bash 作為 console ，可以直接在啟動後登陸。

{% highlight text %}
----- 通過lxc的命令鏈接上去
# lxc-attach -n centos

----- 目前鏈接時還有點問題，直接卡著不動
# lxc-console -n centos

----- 使用util-linux包提供的命令進入設置的namepace空間，需要PID
# nsenter --target PID --mount --uts --ipc --net --pid --root --wd
{% endhighlight %}

注意，attach 只有 3.8 之後的內核才會支持，對於低版本需要通過其它的方法解決。

### 5. 關閉容器

可以在容器中內執行 ```poweroff``` 或 ```shutdown -h now``` 來關閉容器，可以通過如下命令強行關閉，不過此時會花費比較長的時間，實際上就是正常的關閉流程。

{% highlight text %}
# lxc-stop -n centos
{% endhighlight %}

### 6. 銷燬容器

如果不需要了，可以通過如下方式刪除，包括了 rootfs 等配置，所以 "慎用" 。

{% highlight text %}
# lxc-destroy -n centos
{% endhighlight %}

### 7. 其它常見命令

簡單介紹經常會用到的命令。

{% highlight text %}
----- 創建，默認使用/etc/lxc/default.conf，可以通過-f指定配置文件
# lxc-create -n NAME -f FILE

----- 用戶獲取一個容器的狀態，容器的狀態有STARTING RUNNING STOPPING STOPPED ABORTING
# lxc-info -n NAME

----- 監控一個容器狀態的變換，當一個容器的狀態變化時，此命令會在屏幕上打印出容器的狀態
# lxc-monitor -n NAME

----- 列出當前系統所有的容器
# lxc-ls

----- 列出特定容器中運行的進程
# lxc-ps -n NAME

----- 用於銷燬容器
# lxc-destroy -n NAME
{% endhighlight %}

另外，需要注意的是：通過 ```lxc-create``` 創建後的容器，在停止運行後，不會立即被銷燬，要採用 ```lxc-destroy``` 命令才能銷燬；容器命令空間是全局的，系統中不允許存在重名的容器，否則會失敗。

### 安裝 Ubuntu 鏡像

只是簡單介紹一下如何創建 Ubuntu 的鏡像，與鏡像相關的大部分的操作與創建 CentOS-7 相同，只是在創建 rootfs 時有些區別。

在此，主要介紹其創建方法，安裝 rootfs 時，可能會由於鏡像無法訪問導致創建失敗，可以將 Ubuntu 訪問的網站在 ```/usr/share/lxc/templates/lxc-ubuntu``` 開始設置 MIRROR 變量。

{% highlight bash %}
MIRROR=${MIRROR:-http://mirrors.163.com/ubuntu/}
{% endhighlight %}

然後通過如下命令創建 rootfs 。

{% highlight text %}
# lxc-create -t ubuntu -n ubuntu -- -r precise
{% endhighlight %}

實際上，上述的腳本是通過 debootstrap 創建一個 rootfs，也可以通過如下命令自己創建。

{% highlight text %}
# debootstrap --arch amd64 precise ubuntu-precise http://mirrors.163.com/ubuntu/
{% endhighlight %}

## 資源設置

LXC 是通過 cgroup 進行資源管理的，也就是說這部分的設置，都可以通過 lxc-cgroup 命令進行動態調整。在使用前，需要通過如下方式掛載。

{% highlight text %}
----- 掛載cgroup
# mount none -t cgroup /cgroup

----- 讓系統每次啟動自動掛載cgroup
# echo "none /cgroup cgroup defaults 0 0" >> /etc/fstab
{% endhighlight %}

LXC 在創建容器的時候就在 /group 下創建一個子 group 以實現對容器的資源控制，可以根據需要設定相應子系統的參數來達到目的。

### CPU

對於 CPU 資源配置，主要通過兩個變量 cpu.shares、cpuset.cpus 進行設置。

cpu.shares 是一個按照比例計算的 cpu 使用份額，例如，只存在兩個 lxc 的 cpu.shares 都是 1024，那麼這兩個 lxc 使用 cpu 的理論比例就是 50%:50%。

cpuset.cpus 是分配給這個 lxc 使用的具體 cpu，編號從 0 開始；可以通過如下方式進行分配：

{% highlight text %}
# lxc-cgroup -n name cpu.shares 1024
# lxc-cgroup -n name cpuset.cpus 0-1
{% endhighlight %}

另外一個相關的參數是 cpuset.mems，與 cpus 類似，主要用於 NUMA 類型的機器。


### Memory

內存的設置比較簡單，主要是關於物理內存以及 swap 。

memory.limit_in_bytes 用戶內存的最大量，包括了文件緩存，默認單位是字節。如果 swap 沒有關閉的話，即使超過了該限制，仍可能轉儲到 swap 空間，可以通過 swapoff 關閉。

memory.memsw.limit_in_bytes 設定最大內存與 swap 用量之和，同樣默認單位是字節。設置該項前，同樣要設置上述的值，而且不能大於該值。

### blkio

也就是用來設置塊設備的 IO 訪問。

<!--
blkio.weight
指定 cgroup 默認可用訪問塊 I/O 的相對比例（加權），範圍在 100 到 1000。這個值可由具體設備的 blkio.weight_device 參數覆蓋。此數值跟cpu.share類似，以訪問時間的比例的形式起作用。默認根group的值為1000.如果再根下面建立兩group，分別設置weight為500和1000，那麼IO時間比值為1:2.更多的說明可以參考內核文檔。

 blkio.weight_device
指定對 cgroup 中可用的具體設備 I/O 訪問的相對比例（加權），範圍是 100 到 1000。這個值的格式為major:minor weight，其中 major 和
minor 是在《Linux 分配的設備》中指定的設備類型和節點數，我們也稱之為 Linux 設備列表，您
可以參考 http://www.kernel.org/doc/Documentation/devices.txt

blkio.throttle.read_bps_device

針對特定的 major:minor 設備的讀bps的上限限制（bps指byte per second，即每秒的byte數)

blkio.throttle.write_bps_device

針對特定的 major:minor 設備的寫bps的上限限制

 blkio.throttle.read_iops_device

針對特定的 major:minor 設備的讀ios的上限限制(ios指io per second,即每秒的io次數)

blkio.throttle.write_iops_device

針對特定的 major:minor 設備的寫ios的上限限制

此四個參數可以同blkio.weight blkio.weight_device以前使用，系統將同時滿足著些限制。



2.network部分

network部分的配置均以lxc.network開頭.

lxc.network.type指定用於容器的網絡類型：

empty：新的網絡空間會為容器建立，但是沒有任何網絡接口

veth：新的網絡空間會為容器建立，容器將鏈接到lxc.network.link定義的網橋上，從而與外部通信。網橋必須在預先建立。

macvlan：新的網絡空間會為容器建立，一個macvlan的接口會鏈接到lxc.network.link上。

phys：新的網絡空間會被建立，然後lxc.network.link指定的物理接口會被分配給容器

lxc.network.flags用於指定網絡的狀態

up：網絡處於可用狀態

lxc.network.link用於指定用於和容器接口通信的真實接口，比如一個網橋br0

lxc.network.name用於指定容器的網絡接口的名字，與宿主接口不在一個命名空間。如果不指定，lxc為自動分配一個

lxc.network.hwaddr用於指定容器的網絡接口的mac地址

lxc.network.ipv4用於指定容器的網絡接口的ipv4地址，如果不設置或者設為0.0.0.0則表示，容器將通過dhcp的方式獲得ip地址

lxc.network.ipv6用於指定容器的網絡接口的ipv6地址
3.rootfs部分

lxc.rootfs用於指定容器的虛擬根目錄，設定此項以後，容器內所有進程將會把此目錄根目錄，不能訪問此目錄之外的路徑，相當於chroot的效果
-->

## 配置文件

LXC 配置項都是以 ```key=value``` 的形式設置，其中以 # 開始的一行為註釋；當然，配置也可以在 ```lxc-execute``` 或者 ```lxc-start``` 的命令行以 ```-s key=value``` 設定；如下列舉出常見的一些配置。

### 日誌設置

容器的啟動日誌默認只收集 ERROR 級別的信息，默認保存在 ```/var/log/lxc``` 目錄下，以容器名開頭，以 ```.log``` 結尾的文件中，日誌級別和日誌路徑都是可以在配置文件中修改。

{% highlight text %}
lxc.loglevel=               // 級別，value是0到8的整數，0-trace, 1-debug, 2-info, 3-notice, 4-warn,
                            //         5-error, 6-critical, 7-alert, 8-fatal.
lxc.logfile=                // 路徑
{% endhighlight %}

### cgroup 設置

cgroup 部分通常以 ```lxc.cgroup.[subsystem name].key=value``` 的形式進行設置。

### 安全

如果以 root 啟動，可以通過如下參數取消其中的部分功能，以 ```CAP_SYS_MODULE``` 為例，實際設置應為該 ```sys_module```，其它詳細的參數可以參考 ```man 7 capabilities``` 。

{% highlight text %}
lxc.cap.drop = sys_module mac_admin
{% endhighlight %}

<!--
lxc.mount # fstab文件路徑
lxc.mount.entry # fstab文件內容直接寫於此，替代lxc.mount指定文件路徑方式
-->

## 常用命令

簡單列舉一下經常使用的命令，想到那寫到那，備忘。

### 查看當前容器

可以通過 lxc-ls 命令查看，容器的狀態，默認查看的是已經創建的容器（可能沒有啟動）。

{% highlight text %}
----- 查看當前已經創建的容器
# lxc-ls
{% endhighlight %}

### 查看 LXC 相關的配置

通過如下命令可以查看 LXC 的系統配置信息，主要有兩類參數。

{% highlight text %}
----- 查看當前LXC的系統配置
# lxc-config -l
lxc.default_config
lxc.lxcpath
... ...

----- 查看其中的配置項
# lxc-config lxc.lxcpath
{% endhighlight %}

## 參考

官方網站 [linuxcontainers.org](https://linuxcontainers.org/) 。

<!--
用 cgroups 管理 cpu 資源
http://xiezhenye.com/2013/10/%E7%94%A8-cgroups-%E7%AE%A1%E7%90%86-cpu-%E8%B5%84%E6%BA%90.html
-->


{% highlight text %}
{% endhighlight %}
