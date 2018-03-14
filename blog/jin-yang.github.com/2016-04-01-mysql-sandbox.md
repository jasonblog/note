---
title: MySQL Sandbox
layout: post
comments: true
language: chinese
category: [mysql,database]
keywords: mysql,sandbox,測試
description: MySQL Sandbox 是個用於在本地快速部署 MySQL 服務的工具，包括了不同版本，不同模式的部署，例如單節點、主備部署、一主多備等等；如果配合 Gearman，甚至可以完成分佈式的部署。該程序通常用於測試，是用 Perl 語言編寫的，在此，僅簡單介紹下如何使用。
---

MySQL Sandbox 是個用於在本地快速部署 MySQL 服務的工具，包括了不同版本，不同模式的部署，例如單節點、主備部署、一主多備等等；如果配合 [Gearman](http://gearman.org/)，甚至可以完成分佈式的部署。

該程序通常用於測試，是用 Perl 語言編寫的，在此，僅簡單介紹下如何使用。

<!-- more -->

![mysql sandbox logo]({{ site.url }}/images/databases/mysql/sandbox-logo.png "mysql sandbox logo"){: .pull-center }

## 簡介

MySQL-Sandbox 會在同一機器上部署不同的實例，實際上也就是安裝時對應的 data-directory、Port、Unix Socket 不同，默認上述的三個參數與版本有關，以 MySQL-5.0.17 為例，對應如下。

{% highlight text %}
Data-Directory: $SANDBOX_HOME/msb_5.0.17/data
          Port: 5017
   Unix Socket: /tmp/mysql_5017.sock
{% endhighlight %}

另外，常見的命令簡介如下。

{% highlight text %}
make_sandbox
  最簡單創建sandbox，也就是一個實例；
low_level_make_sandbox
  創建單個sandbox，可以使用多個選項，不過不會直接使用；
make_replication_sandbox
  創建master-slave架構；
make_multiple_sandbox
  創建相同版本的sandbox；
make_multiple_custom_sandbox
  創建不同版本的sandbox；
make_sandbox_from_source
  從源碼編譯的build目錄創建一個sandbox；
make_sandbox_from_installed
  從已安裝的二進制文件創建一個sandbox；
sbtool
  sandbox管理工具。
{% endhighlight %}

具體用法參考源碼目錄下的 README.md 文檔，非常詳細，在此簡單介紹如下。

### 安裝

對於 Perl 程序可以通過 cpan 安裝，當然需要 root 權限。

{% highlight text %}
# cpan
cpan> install MySQL::Sandbox
{% endhighlight %}

源碼包可以直接從 [github](https://github.com/datacharmer/mysql-sandbox/releases) 上下載，然後通過如下方式安裝。

注意，在如下執行 ```make test``` 時，會依賴 Perl 的測試框架，可通過 ```yum install perl-Test-Simple``` 命令進行安裝。

{% highlight text %}
----- 解壓、編譯、安裝
$ tar -xzf MySQL-Sandbox-x.x.x.tar.gz
$ cd MySQL-Sandbox-x.x.x
$ perl Makefile.PL PREFIX=/opt/mysql-sandbox
$ make
$ make test
$ make install

----- 設置$PERL5LIB以及$PATH變量
$ export PATH=$PATH:/opt/mysql-sandbox/bin
$ export PERL5LIB=/opt/mysql-sandbox/share/perl5
{% endhighlight %}

另外，建議使用普通用戶進行測試，如果要使用 root 用戶，則需要設置 ```export SANDBOX_AS_ROOT=1``` 環境變量。

## 安裝方式

可直接下載二進制包，然後通過如下方式安裝，如上所述，會安裝到 ```$SANDBOX_HOME/msb_X_X_XX``` 目錄下，如果未設置，默認 ```SANDBOX_HOME=$HOME/sandboxes``` 。

可以通過三種方式進行安裝，簡單介紹如下。

### 二進制包

可以直接從 [官網](https://dev.mysql.com/downloads/mysql/) 下載二進制包，例如 [mysql-5.6.35-linux-glibc2.5-x86_64.tar.gz](https://dev.mysql.com/downloads/mysql/5.6.html#downloads) ，包含了已經編譯好的二進制包。

{% highlight text %}
----- 單個MySQL服務
$ make_sandbox /path/mysql-x.x.x.tar.gz

----- 主備方式
$ make_replication_sandbox /path/mysql-x.x.x.tar.gz

----- 主備循環
$ make_replication_sandbox --circular=5 /path/mysql-x.x.x.tar.gz

----- 多個MySQL服務
$ make_multiple_sandbox /path/mysql-x.x.x.tar.gz

----- 多個不同版本的MySQL服務
$ make_multiple_sandbox /path/mysql-x.x.x.tar.gz /path/mysql-x.x.x-other-tar.gz /path/another-tar.gz
{% endhighlight %}

默認會將上述二進制包解壓到當前目錄，並將目錄重命名為版本號，例如 5.6.35 。

### 源碼包

可以直接下載源碼，然後編譯，但是不想安裝，可以通過如下命令搭建。

{% highlight text %}
$ make_sandbox_from_source {SOURCE_DIRECTORY} {sandbox_type} [options]
{% endhighlight %}

其中 ```sandbox_type``` 參數指定了類型，包括了 single、multiple、replication、circular ，示例如下。

{% highlight text %}
$ ./configure && make
$ make_sandbox_from_source $PWD single
{% endhighlight %}

也可以使用如下參數。

{% highlight text %}
$ make_sandbox_from_source $HOME/build/5.6.35 single --export_binaries --check_port 
$ make_sandbox_from_source $HOME/build/5.6.35 replication --how_many_slaves=5
{% endhighlight %}

### 已安裝包

可將不同版本的 MySQL 放在 ```$SANDBOX_BINARY``` 環境變量指定目錄下，然後使用更簡單的方式。

{% highlight text %}
$ export SANDBOX_BINARY=$HOME/mybin
$ ls $SANDBOX_BINARY
5.6.35 5.7.17
$ make_sandbox 5.7.17
{% endhighlight %}

## 使用

在安裝到 ```$SANDBOX_HOME``` 目錄下之後，會生成一列的操作腳本。

### 服務器操作

單個實例對應命令如下：

{% highlight text %}
start
  啟動服務，在數據目錄下創建.pid文件，在/tmp目錄下創建socket文件；如果.pid文件存在則立即退出。
stop
  停止服務，如果.pid文件不存在則立即退出。
clear
  通過stop停止服務，並刪除test數據庫中的文件。
send_kill
  發送SIGTERM信號，如果MySQL服務沒有響應則發送SIGKILL信號。
use
  通過客戶端鏈接到服務器，默認使用my_sandbox.cnf中的配置，用戶名密碼分別為msandbox/msandbox、root/msandbox 。
use "command"
  執行命令。
{% endhighlight %}

而對於多個實例，只需要在上述的操作中添加 ```_all``` 即可，例如 start_all stop_all clear_all use_all send_kill_all 。

在通過客戶端鏈接到服務器時，對於一主多備可以使用 m s1 s2 s3 命令鏈接，而多個節點則可以使用 n1 n2 n3 命令。

### Sandbox Tools

提供了一個 sbtool 工具用於一些常見的操作。

{% highlight text %}
----- 將數據複製到不同環境下
$ make_sandbox 5.1.33
$ make_sandbox 5.1.34
$ sbtool -o copy \
    -s $HOME/sandboxes/msb_5_1_33 \
    -d $HOME/sandboxes/msb_5_1_34

----- 移動到其它目錄下
$ make_sandbox 5.1.33
$ sbtool -o move \
    -s $HOME/sandboxes/msb_5_1_33 \
    -d /some/path/a_fancy_name

----- 新的測試腳本
$ cat << EOF >> check_single_server.sb
shell:
command  = make_sandbox 5.1.30 --no_confirm
expected = sandbox server started
msg = sandbox creation

sql:
path = $SANDBOX_HOME/msb_5_1_30
query = select version()
expected = 5.1.30
msg = checking version

shell:
command = $SANDBOX_HOME/msb_5_1_30/stop
expected = OK
msg = stopped
EOF
$ export TAP_MODE=1
$ perldoc Test::Harness::TAP
$ test_sandbox --tests=user \
    --user_test=t/check_single_server.sb

----- 也可以在一個測試中嵌套測試
$ prove -v t/06_test_user_defined.t

----- 創建複製樹
$ make_multiple_sandbox --how_many_nodes=12 5.0.77
$ sbtool -o tree \
    --tree_dir=$HOME/sandbox/multi_msb_5_0_77 \
 --master_node=1 \
 --mid_nodes='2 3 4' \
 --leaf_nodes='5 6|7 8 9|10 11 12'
$ cd $HOME/sandbox/multi_msb_5_0_77
$ ./n1 -e "show slave hosts"
$ ./n2 -e "show slave hosts"

----- 檢查端口
$ sbtool -o ports
$ sbtool -o ports --only_used

----- 檢查range
$ sbtool -o range --min_range=5130

----- 保存禁止刪除，如下刪除時會報錯
$ sbtool -o preserve \
    -s $HOME/sandboxes/msb_5_1_33/
$ $HOME/sandboxes/msb_5_1_33/clear

----- 允許刪除
$ sbtool -o unpreserve \
    -s $HOME/sandboxes/msb_5_1_33/
$ $HOME/sandboxes/msb_5_1_33/clear

----- 刪除所有，實際可以直接刪除目錄，需要使用絕對路徑
$ sbtool -o delete \
    -s $HOME/sandboxes/msb_5_1_33/
{% endhighlight %}

### 其它

對於 mysqldump、mysqlbinlog、mysqladmin 可以通過如下方式啟動，直接使用 my_sandbox.cnf 中的配置，不需要輸入密碼。

{% highlight text %}
./my sqlbinlog ./data/mysql-bin.000001 | less
./my sqldump dbname > dumpname
./my sqladmin extended-status
{% endhighlight %}

#### 常用示例

{% highlight text %}
----- 搭建不同版本MySQL服務
$ make_multiple_custom_sandbox 5.6.16 5.7.17

----- 也可以添加選項，可以通過--help查看可用參數
$ make_sandbox /path/mysql-x.x.x.tar.gz \
    --my_file={small|medium|large|huge}
$ make_sandbox /path/mysql-x.x.x.tar.gz \
    --interactive

----- 檢查備庫
./check_slaves

----- 所有節點執行命令
./use all "SELECT @@server_id"

----- 重啟時指定參數
./restart --key_buffer=20M
{% endhighlight %}

<!--
--check_port

$SANDBOX_HOME
    指定測試數據目錄所在位置。
-->



## 參考

詳細可以參考官方網站 [mysqlsandbox.net](http://mysqlsandbox.net/index.html)，源碼包可以直接從 [github](https://github.com/datacharmer/mysql-sandbox/releases) 上下載；另外，可以參考不錯的 [PPT](/reference/databases/mysql/mysql_sandbox.pdf) 。

{% highlight text %}
{% endhighlight %}
