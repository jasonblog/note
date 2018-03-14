---
title: SaltStack 簡介
layout: post
comments: true
language: chinese
category: [misc,linux,python]
keywords: hello world,示例,sample,markdown
description: SaltStack 是一個輕量級的運維工具，具備配置管理、遠程執行、監控等功能，類似於 Puppet 和 Chef，不過這兩者是用 Ruby 寫的，而 Salt Stack 用的是 Python。
---

SaltStack 是一個輕量級的運維工具，具備配置管理、遠程執行、監控等功能，類似於 Puppet 和 Chef，不過這兩者是用 Ruby 寫的，而 Salt Stack 用的是 Python。

結下來看看具體的使用。

<!-- more -->

## 簡介

SaltStack 基於 Python 寫的經典 C/S 框架的自動化部署平臺，由 Master 和 Minion 構成，使用了一系列成熟的工具，比如網絡模型採用 ZeroMQ、授權加密採用了 AES、串行化採用了 msgpack、配置文件採用 [YAML](http://www.yaml.org/) 標記語言等。

### 安裝測試

在 CentOS7 中可以通過如下方式在單機安裝測試。

{% highlight text %}
----- 直接通過YUM安裝相關的依賴包
# yum --enablerep=epel install salt salt-master salt-minion salt-ssh
# systemctl start salt-master                # 啟動master
# salt-master -d                             # 同上，直接通過命令行啟動，指定後臺運行
# netstat -ltunp | grep python               # 查看打開的端口，默認為4505/4506
# lsof -i:4505                               # 同上，查看使用4505端口的程序

# cat /etc/salt/minion                       # 修改配置文件
master: localhost   # master的IP地址
id: foobar          # 指定minion的ID
# systemctl start salt-minion                # 啟動minion

$ salt --version                             # 查看版本號
$ salt --versions-report                     # 查看其它依賴模塊的版本號

----- 測試，常見的使用方式
# salt-key -A                                # 添加本地節點
# salt "*" test.ping                         # 測試節點
# salt "*" cmd.run 'uptime'                  # 執行命令
# salt -G "os:Centos" test.ping              # 根據被控主機的grains信息過濾
# salt "*" grains.item os                    # 查看操作系統類型
--- 執行代碼，等同於python -c 'import sys; print sys.version'
# salt '*' cmd.exec_code python 'import sys; print sys.version'
{% endhighlight %}

實際上 master/minion 只會安裝少數的文件，主要文件在 salt 安裝包中。

SaltStack master 啟動後默認監聽 4505 和 4506 兩個端口，其中 4505(publish_port) 為 salt 的消息發佈系統，4506 (ret_port) 為 salt 客戶端與服務端通信的端口。

## Master/Minion 的認證和鏈接

SaltStack 主控端是依靠 openssl 證書來與受控端主機認證通訊的，受控端啟動後會發送給主控端一個公鑰證書文件，在主控端用 salt-key 命令來管理證書。

認證過程如下。

1. 在安裝 minion 之後，需要在配置文件中定義參數 master，也就是指定 master 端的 IP 地址；

2. 啟動 minion 服務後會在本地生成一個密鑰對；

3. 接著 minion 會去連接 master，並嘗試把公鑰發送給 master，這時在 master 端可以使用 salt-key 來查看當前已經接收到的認證。

在 master 接受 minion 認證之後，兩者即可通訊了，可以通過 state 模塊來配置管理 minion，也可以通過 cmd.run 遠程對 minion 執行命令。

詳細來說，minion 在第一次啟動時，會在配置文件 (默認為/etc/salt/minion) 中的配置項 pki_dir 指定目錄下（默認為 /etc/salt/pki/minion/）下自動生成 minion.pem (private key) 和 minion.pub (public key)，然後將 minion.pub 發送給 master。

master 在接收到 minion 的 public key 後，通過 salt-key 命令 accept minion public key，這樣在 master 的 /etc/salt/pki/master/minions 下的將會存放以 minion id 命名的 public key，然後 master 就能對 minion 發送指令了。

常見的命令有：

{% highlight text %}
# salt-key -L                                # 查看所有節點，包括未認證的節點
# salt-key -A                                # 添加所有未認證的節點
# salt-key -a node*                          # 支持單個節點、以及通配符
# salt-key -d node                           # 刪除對某個節點的認證
# salt-key -D                                # 刪除所有證書
{% endhighlight %}

## 執行命令

SaltStack 執行命令的格式如下。

![SaltStack Commands Syntax]({{ site.url }}/images/network/saltstack/cmd-syntax.png "SaltStack Commands Syntax"){: .pull-center}

常用的命令如下。

{% highlight text %}
# salt '*' sys.doc                      # 查看所有文檔
# salt '*' sys.doc pkg.install          # 查看pkg.install對應的文檔

# salt '*' cmd.run 'ls -l /bin/ls'      # 直接執行終端命令
# salt '*' disk.usage                   # 查看磁盤使用情況
# salt '*' network.interfaces           # 查看網絡設備

# salt '*' pkg.install coreutils        # 安裝軟件
{% endhighlight %}

在 SaltStack 中，目前有 5 種方式來指定目標機器。

1. 匹配 minion id，可以使用 shell 通配符 (Globbing)、正則表達式(Perl風格)、minion 列表。

2. Grains，包括客戶端配置文件中定義 Grains。

3. 指定列表 (Lists)，節點分組 (Node Groups)，複合匹配 (Compound Matcher)，批量執行 (Batch Size)。

可以參考如下的示例。

{% highlight text %}
# salt '*' test.ping                                    # 所有的minions
# salt '*.example.*' test.ping                          # 可以使用shell中的* ? []通配符
# salt 'web?' test.ping                                 # 匹配web1、web2等
# salt 'web-[x-z]' test.ping                            # 匹配web-x、web-y、web-z

# salt -E 'web1-(prod|devel)' test.ping                 # 使用正則表達式

# salt -L 'web1,web2,web3' test.ping                    # 指定列表

# salt -G 'os:CentOS' test.ping                         # 使用Grains

# salt -N group1 test.ping                              # 指定分組節點node group

# salt -C 'web? and G@os:CentOS or E@web.*' test.ping   # 使用組合模式

# salt '*' -b 10 test.ping                              # 使用批量模式
# salt -G 'os:RedHat' --batch-size 25% test.ping
{% endhighlight %}

在不指定 minion id，則會通過 socket.getfqdn() 獲取，可以通過如下命令查看，同時該值會緩存在 /etc/salt/minion_id 中。

{% highlight text %}
$ python -c 'import socket; print socket.getfqdn(socket.gethostname())'
{% endhighlight %}

節點分組可以在 master 配置文件中進行設置，如下。

{% highlight text %}
nodegroups:
  group1: 'L@foo.domain.com,bar.domain.com,baz.domain.com or bl*.domain.com'
  group2: 'G@os:Debian and foo.domain.com'
{% endhighlight %}

複合匹配是通過布爾操作符連接的多個目標條件，可以使用上述的幾種方式匹配，默認採用 Globbing。另外，not 不能用於第一個條件，需要使用如下的命令 salt -C '* and not G@os:CentOS' test.ping 。

相關匹配規則的語法可以參考 [staltstack docs - Compoud Matchers](https://docs.saltstack.com/en/latest/topics/targeting/compound.html) 。

## Salt State, SLS

State 用於描述系統的目標狀態，SLS 代表 SaLt State 文件，是 Salt State 系統的核心；SLS 描述了系統的目標狀態，採用最簡單易讀的序列化數據格式 YAML。

其中根路徑在 master 的配置文件中通過 file_roots 指定，其中 base 必須存在，而且 base 環境中通常包含 Salt 的入口 top 文件，也即 top.sls 。注意，sls 文件需要同時在 master 和 minion 都存在。

最簡單的配置文件如下，用於檢查 coreutils 是否已經安裝，與 state 相關的源碼保存在 salt/states 目錄下。

{% highlight text %}
# cat top.sls
base:
  '*':
    - coreutils
# cat coreutils.sls
coreutils:                       # ID聲明
   pkg:                          # state聲明
      - installed                # 定義的function
{% endhighlight %}

在配置文件中，可以通過 salt:// 代替配置文件中 file_roots 指定的根路徑，入口文件默認為 top.sls，可以通過 state_top 配置。

<!--
對於機器的匹配方式可以有多種。

<pre style="font-size:0.8em; face:arial;">
base:
  'web1-(prod|devel)':                                 # 匹配minion-id
    - match: pcre                                      # 設置匹配方式
    - webserver

base:
  group1:
    - match: nodegroup                                 # 通過節點分組匹配
    - webserver

base:
  'webserv* and G@os:Debian or E@web-dc1-srv.*':       # 使用組合模式
    - match: compound
    - webserver
</pre>
-->

## Pillar VS. Grains

Pillar 和 Grains 非常容易混淆，其中 Grain 的數據是在 minion 啟動時產生的，通常是類似於 OS、CPU、磁盤的靜態信息；而 Pillar 信息保存在 master 中。

Salt在 0.9.8 版本中引入了 Pillar，Pillar 是一個非常重要的組件，用於給特定的 minion 定義任何需要的數據，這些數據可以被 Salt 的其他組件使用。Pillar 解析後，是一個嵌套的字典結構，最上層的 key 是 minion ID，也就是說 Pillar 數據是與特定 minion 關聯的，每一個 minion 只能看到自己的數據。

Pillar 使用獨立的加密 session，所以 Pillar 可以用來傳遞敏感數據，如 ssh key、加密證書等。

默認情況下，master 配置文件中的所有數據都添加到 Pillar 中，且對所有 minion 可用，如果要禁用這一默認值，可以添加 pillar_opts: False 配置。Pillar 使用與 State 相似的 SLS 文件，其根目錄通過 master 配置文件中 pillar_roots 指定，而且 Pillar 也有 top file。

{% highlight text %}
# salt '*' pillar.data                         # 或者pillar.raw
# salt '*' saltutil.refresh_pillar             # 修改Pillar文件後刷新minion上的數據
{% endhighlight %}

對於 Grains 常見的操作如下。

{% highlight text %}
# salt '*' grains.ls                           # 列出所有grains的名字
# salt '*' grains.items                        # 列出名字以及內容
# salt '*' grains.item os                      # 查看具體選項
{% endhighlight %}

可以在 minion 的配置文件中 (默認為/etc/salt/minion) 設置 Grains 的靜態信息。

{% highlight text %}
grains:
  roles:
    - webserver
    - memcache
  deployment: datacenter4
{% endhighlight %}

另外，可在 master 中定義模塊，可以將腳本放置在 file_roots 下的 _grains 目錄中，當執行 state.highstate、saltutil.sync_grains、saltutil.sync_all 時，會將 _grains 中的文件分發到客戶端上。

{% highlight text %}
# mkdir /srv/salt/_grains
# cat /srv/salt/_grains/custom_grain.py
    def custom_grains():
        grains = {'role' : 'LB'}
        return grains
# salt '*' saltutil.sync_grains
# salt '*' grains.item role
{% endhighlight %}

上述的腳本會作為模塊導入，因此不需要 #!/usr/bin/env python，也不需要有執行權限。

## 模塊

一些常見的模塊會保存在安裝目錄下的 salt/modules 下，如果通過 YUM 安裝，通常為：

{% highlight text %}
$ ls /usr/lib/python2.7/site-packages/salt/modules
{% endhighlight %}

在 Python 中，如果要添加模塊也非常簡單，可以直接在安裝目錄的 modules 子目錄下添加如下文件。

{% highlight text %}
# cat /usr/lib/python2.7/site-packages/salt/modules/foobar.py
def hello():
    return 'hello world'
{% endhighlight %}

然後可以通過 salt '*' foobar.hello 執行。

### 常用模塊介紹


下面看下一些常見的模塊，除此之外還有 grains、pillar、states、modules、returner、runners、reactor 等模塊。

#### cp模塊

實現遠程文件、目錄的複製，以及下載URL文件等操作。

{% highlight text %}
----- 將主服務器file_roots base指定目錄下的文件複製到被控主機
# cat /etc/salt/master
file_roots:
  base:
    - /srv/salt
# echo "This is test file with saltstack module 'cp.get_file'" > /srv/salt/foobar.txt
# salt '*' cp.get_file salt://foobar.txt /tmp/foobar.txt

----- 將目錄下的文件複製到被控主機
# mkdir /srv/salt/dir && mv /srv/salt/foobar.txt /srv/salt/dir/
# salt '*' cp.get_dir salt://dir /tmp

----- 下載指定URL內容到被控主機指定位置
# salt '*' cp.get_url http://justkidding.com/download/files.tgz /tmp/files.tgz

----- 根據grains選擇目標
# salt '*' cp.get_file "salt://{{grains.os}}/vimrc" /etc/vimrc template=test
{% endhighlight %}

該模塊有幾個常用的參數：

* gzip=9，使用gzip的方式進行壓縮，數字越大，壓縮率就越高，9代表最大的壓縮率。

* makedirs=True，當分發的位置在目標主機上不存在時，自動創建該目錄。

模板渲染功能可以同時在源和目標文件中使用。

#### cmd 模塊

實現遠程的命令行調用執行。

{% highlight text %}
# salt '*' cmd.run 'uptime'
{% endhighlight %}

#### file模塊

被控主機文件常見操作，包括文件讀寫、權限、查找、校驗等。

{% highlight text %}
# salt '*' file.get_sum /etc/resolv.conf md5
# salt '*' file.stats /etc/resolv.conf
{% endhighlight %}

#### pkg 包管理模塊

被控主機程序包管理，如yum、apt-get等。

{% highlight text %}
# salt '*' pkg.install coreutils
# salt '*' pkg.file_list coreutils
{% endhighlight %}

#### network模塊

返回被控主機網絡信息。

{% highlight text %}
# salt '*' network.ip_addrs
# salt '*' network.interfaces
{% endhighlight %}

#### service 服務模塊

被控主機程序包服務管理。

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

# 獲取master配置文件
master_ops = salt.config.client_config('/etc/salt/master')#returns a dictionary

# 獲取minion配置文件
minion_opts = salt.config.minion_config('/etc/salt/minion')#returns a dictionary

# 獲取grain值
import salt.loader
opts = salt.config.minion_config('/etc/salt/minion')
grains = salt.loader.grains(opts)
print grains['id']

# 執行salt命令的函數，一般返回字典
import salt.client
local = salt.client.LocalClient()
print local.cmd('*', 'test.ping', [])
{% endhighlight %}
-->

## 配置文件

{% highlight text %}
interface: 0.0.0.0                    # 監聽IP
publish_port: 4505                    # 消息發佈端口
user: root                            # 默認用戶
max_open_files: 100000                # 最大打開文件數
worker_threads: 5                     # 工作線程數，不能小於3
ret_port: 4506                        # master和minion使用的通訊端口
pidfile: /var/run/salt-master.pid     # PID文件

root_dir
pki_dir: /etc/salt/pki/master        # 存放公鑰的目錄
    cachedir
    keep_jobs
    job_cache
    ext_job_cache
    minion_data_cache
    enforce_mine_cache
    sock_dir
{% endhighlight %}

## 參考

相關的文檔可以查看 [docs.saltstack.com](https://docs.saltstack.com/en/latest/contents.html)，也可以直接下載 PDF 文檔，內容比較多，有 2K+ 頁。其它的一些常用網址可以參考： [中文社區(www.saltstack.cn)](http://www.saltstack.cn/)、[官方網址(www.saltstack.com)](http://www.saltstack.com)、[Python官方](https://pypi.python.org/pypi/salt) 。

{% highlight text %}
{% endhighlight %}
