---
title: MySQL 簡介
layout: post
comments: true
language: chinese
category: [mysql,database]
keywords: mysql,introduce,簡介
description: MySQL 是一款最流行的開源關係型數據庫，最初由瑞典的 MySQL AB 公司開發，目前已被 Oracle 收購，現在比較流行的分支包括了 MariaDB 和 Percona。現在來看，MySQL 被收購之後，實際上添加了很多不錯的特性，解掉了很多之前未解的問題，而現在 MariaDB 離主幹的特性越來越遠；而 Percona 據說為了保證質量是不接受社區提供的代碼的，不過其提供的工具卻是極好的^_^ 在此，簡單介紹，二進制安裝、源碼編譯安裝、單實例以及多實例的啟動、常見的調試方法。
---

MySQL 是一款最流行的開源關係型數據庫，最初由瑞典的 MySQL AB 公司開發，目前已被 Oracle 收購，現在比較流行的分支包括了 MariaDB 和 Percona。

現在來看，MySQL 被收購之後，實際上添加了很多不錯的特性，解掉了很多之前未解的問題，而現在 MariaDB 離主幹的特性越來越遠；而 Percona 據說為了保證質量是不接受社區提供的代碼的，不過其提供的工具卻是極好的^_^

在此，簡單介紹，二進制安裝、源碼編譯安裝、單實例以及多實例的啟動、常見的調試方法。

<!-- more -->

![mysql intro logo]({{ site.url }}/images/databases/mysql/mysql-intro-logo.jpg "mysql intro logo"){: .pull-center width="400px"}

## 架構簡介

MySQL 作為一個大型的網絡程序、數據管理系統，架構非常複雜，不過 MySQL 基本採用的是插件模式，下圖大致畫出了其邏輯架構。

![mysql architecture]({{ site.url }}/images/databases/mysql/mysql-architecture.png "mysql architecture"){: .pull-center }

簡單介紹一下如上的結構內容，先大致有個瞭解，詳細的內容後面詳細介紹。

#### Connectors

實際上就是在 TCP 之上定義的 MySQL 應用層協議，比較常用的是調用 Native C API、JDBC、PHP 等各語言 MySQL Connector 。

#### Connection Management

MySQL 為每個連接綁定一個線程，這個連接上的所有操作都在這個線程中執行。為了避免頻繁創建和銷燬線程帶來開銷，通常會緩存線程或者使用線程池，從而避免頻繁的創建和銷燬線程。

客戶端與 MySQL 成功連接後，在正式使用 MySQL 的功能之前，還需要進行認證，默認的認證方式是基於用戶名、主機名、密碼；如果用了 SSL 或者 TLS 的方式進行連接，還會進行證書認證。

#### SQL Interface

MySQL 支持 DML (數據操作語言)、DDL (數據定義語言)、存儲過程、視圖、觸發器、自定義函數等多種 SQL 語言接口。

#### Parser

MySQL 會按照 SQL 語法對客戶端發送的請求進行解析，併為其創建語法樹，並根據數據字典豐富查詢語法樹，會驗證該客戶端是否具有執行該查詢的權限。創建好語法樹後，MySQL 還會對 SQL 查詢進行語法上的優化，進行查詢重寫。

#### Optimizer

語法解析和查詢重寫之後，MySQL 會根據語法樹和數據的統計信息對 SQL 進行優化，包括決定表的讀取順序、選擇合適的索引等，最終生成 SQL 的具體執行步驟。這些具體的執行步驟裡真正的數據操作都是通過預先定義好的存儲引擎 API 來進行的，與具體的存儲引擎實現無關。

#### Caches & Buffers

MySQL 內部維持著一些 Cache 和 Buffer，比如 Query Cache 用來緩存一條 Select 語句的執行結果，如果能夠在其中找到對應的查詢結果，那麼就不必再進行查詢解析、優化和執行的整個過程了。

#### Pluggable Storage Engine

存儲引擎的具體實現，這些存儲引擎都實現了 MySQL 定義好的存儲引擎 API 的部分或者全部。MySQL 可以動態安裝或移除存儲引擎，可以有多種存儲引擎同時存在，可以為每個 Table 設置不同的存儲引擎。

存儲引擎負責在文件系統之上，管理表的數據、索引的實際內容，同時也會管理運行時的 Cache、Buffer、事務、Log 等數據和功能。



## 安裝

對於 Ubuntu、CentOS 這樣的發行版本可以直接通過命令行安裝，從 CentOS 7 之後，實際安裝的是 MariaDB ，也可以通過 Oracle 提供的官方 YUM repository ，然後安裝 MySQL Community Server，下面以 CentOS 為例。

{% highlight text %}
# yum install -y mariadb-server mariadb                                      ← 安裝MariaDB
# systemctl start mariadb                                                    ← 啟動server
# systemctl enable mysqld                                                    ← 設置開機啟動

# rpm -Uvh http://dev.mysql.com/get/mysql-community-release-el7-5.noarch.rpm ← 安裝倉庫
# yum install mysql-community-server                                         ← 安裝社區版
# cat << EOF > /etc/my.cnf                                                   ← 設置配置文件
[client]
password        = new_password
port            = 5506
socket          = /tmp/mysql-5506.sock
[mysqld]
user            = mysql
port            = 5506
socket          = /tmp/mysql-5506.sock
basedir         = /usr
datadir         = /home/foobar/databases/data-5506
pid-file        = /home/foobar/databases/data-5506/mysqld.pid
EOF
# /usr/sbin/mysqld --initialize-insecure --basedir=/usr --user=mysql \       ← 初始化數據，可通過空白密碼登陸
    --datadir=/home/foobar/databases/data-5506
# systemctl start mysqld                                                     ← 啟動server

$ mysql -h 127.1                                                             ← 直接登陸
mysql> ALTER USER 'root'@'localhost' IDENTIFIED BY 'new_password';           ← 修改密碼
mysql> FLUSH PRIVILEGES;
{% endhighlight %}

通過 RPM 包安裝時，默認是保存到 `/var/lib/mysql` 目錄下。另外，需要注意，需要保證上級目錄權限為 755，否則可能會報 Permission denied 錯誤。

當然也可以從 [MySQL 官網](http://dev.mysql.com/downloads/)、[MariaDB 官網](https://downloads.mariadb.org/) 或者 [Percona 官網](https://www.percona.com/) 下載相應的安裝包，或者源碼。對於 CentOS 7 來說，可以從 [MySQL Community Downloads](https://dev.mysql.com/downloads/) 下載 server、client、common、libs 對應的 RPM 包，也可從 [sohu 鏡像](http://mirrors.sohu.com/mysql/) 下載，然後安裝即可。

#### 密碼配置

如果沒有手動初始化數據直接啟動，那麼會將 root 密碼打印到日誌中，然後可以直接登陸；也可以修改配置文件跳過認證，也就是添加 ```skip-grant-tables``` 選項。

{% highlight text %}
----- 不建議使用如下方式修改密碼
mysql> SET PASSWORD=PASSWORD('Root1234@');
mysql> FLUSH PRIVILEGES;

----- 可以使用如下方式修改，且不需要FLUSH，建議時候後者
mysql> SET password='Root1234@';
mysql> ALTER USER 'root'@'localhost' IDENTIFIED BY 'new_password';
{% endhighlight %}

### 簡單配置

MySQL 的配置文件保存在 /etc/my.cnf，包括了 datadir、pid-file 等的設置。另外，安裝完之後 root 的密碼為空，然後可以通過 mysql_secure_installation 來進行一些安全的配置，尤其是線上服務器，包括刪除 test 數據庫和默認創建的匿名用戶。

為了方便登陸，可以設置 ~/.my.cnf，僅為了本地方便登陸，線上生產環境一定不要配置。

{% highlight text %}
[client]
user=root
password=YOUR_PASSWORD
socket=/tmp/mysql.sock
host=127.1
{% endhighlight %}

可以通過 mysqld \-\-verbose \-\-help \| head -15 查看配置文件的默認加載順序，當然，也可以直接通過 mysql \-\-help \| grep my.cnf 過濾進行查看；配置文件中通過 # 表示註釋。

安裝完成之後，可以通過如下命令查看編譯時使用的配置項。

{% highlight text %}
$ mysqld --verbose --help
{% endhighlight %}

### 編譯安裝

還是直接從官網下載源碼即可，假設直接解壓到 /home/foobar/Workspace/mysql 目錄下，並將安裝目錄設置為 /opt/mysql-5.7 。

下面介紹如何通過源碼進行安裝，最簡單的是通過 BUILD/compile-ARCH 腳本進行編譯，當然，可以選擇使用 autotools 或者 CMake 指定相應的參數進行安裝，在此通過 CMake 安裝。

通過 CMake 編譯時，有兩種編譯方式：A) 內部編譯，即在源碼內部編譯，此時會有一些編譯產生的臨時文件，影響代碼結構；B) 外部編譯，不會影響原代碼的結構，在此採用外部編譯。

所謂的外部編譯，就是在一個新的目錄下進行編譯，此時編譯的中間文件不會影響到源碼的目錄結構。源碼可以直接從官網下載，三個發行版本的源碼編譯方式都類似，在此以 MySQL 為例。

{% highlight text %}
----- 安裝依賴包，以及相關的開發庫
# yum install gcc-c++ g++ ncurses-devel readline-devel zlib-devel

----- 源碼安裝
$ tar -xf mysql-boost-x.x.xx.tar.bz2
$ cd mysql-boost-x.x.xx && mkdir build && cd build       ← 新建編譯的目錄
$ cmake ..                                     \
    -DCMAKE_INSTALL_PREFIX=/opt/mysql-5.7      \         ← 安裝根目錄
    -DMYSQL_DATADIR=/opt/data                  \         ← 數據文件的默認路徑
    -DSYSCONFDIR=/etc                          \         ← 配置文件目錄，默認/etc/my.cnf
                                               \
    -DWITH_INNOBASE_STORAGE_ENGINE=ON          \         ← 使用InnoDB存儲引擎
    -DWITH_BLACKHOLE_STORAGE_ENGINE=ON         \         ← 黑洞存儲引擎
    -DWITH_EXAMPLE_STORAGE_ENGINE=ON           \         ← 使用存儲示例
                                               \
    -DWITHOUT_FEDERATED_STORAGE_ENGINE=ON      \         ← 取消一些不常用的存儲引擎
    -DWITHOUT_PARTITION_STORAGE_ENGINE=ON      \
    -DWITHOUT_ARCHIVE_STORAGE_ENGINE=ON        \         ← 日誌記錄和聚合分析，不支持索引
                                               \
    -DMYSQL_TCP_PORT=3306                      \         ← 默認使用的端口號
    -DMYSQL_UNIX_ADDR=/tmp/mysql.sock          \         ← 默認套接字文件路徑
    -DMYSQL_USER=mysql                         \         ← 默認使用的用戶
                                               \
    -DEXTRA_CHARSETS=all                       \         ← 是否支持額外的字符集
    -DDEFAULT_CHARSET=utf8                     \         ← 默認編碼機制
    -DDEFAULT_COLLATION=utf8_general_ci        \         ← 設定默認語言的排序規則
                                               \
    -DWITH_READLINE=ON                         \         ← 使用readline庫，支持類shell的快捷鍵以及歷史命令
    -DENABLED_LOCAL_INFILE=ON                  \         ← 支持批量導入
    -DWITH_LIBWRAP=OFF                         \         ← 是否可以基於WRAP實現訪問控制
    -DWITH_SSL=system                          \         ← 支持ssl會話，實現基於ssl的數據複製
    -DWITH_ZLIB=system                         \         ← 壓縮庫
    -DWITH_EMBEDDED_SERVER=OFF                 \         ← 取消embedded模式
    -DENABLE_PROFILING=ON                      \         ← 啟用性能分析功能
    -DCMAKE_BUILD_TYPE=Debug                   \         ← 打開調試選項
    -DWITH_SAFEMALLOC=OFF                      \         ← 關閉safemalloc，經常會報錯誤，5.7不需要
    -DWITH_BOOST=../boost                                ← 5.7需要該boost庫，直接下載含該庫的包

----- 編譯源碼並安裝
$ make -j`cat /proc/cpuinfo | grep 'processor' | wc -l`  ← 可以簡單使用make並行編譯，指定併發
# make install                                           ← 安裝

----- 設置環境
$ export PATH=$PATH:/opt/mysql-5.7/bin                   ← 設置環境變量
$ vi /etc/ld.so.conf && sudo ldconfig                    ← 設置動態庫目錄
$ mysql_config                                           ← 可用於查看環境配置，很多MySQL軟件依賴該命令

----- 源碼安裝時最好將build目錄中的如下文件保存下
# cat install_manifest.txt | xargs rm                    ← 直接手動刪除
{% endhighlight %}

<!--
#!/bin/bash
cmake ..                                       \
    -DCMAKE_INSTALL_PREFIX=/opt/mysql-5.7      \
    -DCMAKE_BUILD_TYPE=Debug                   \
    -DWITH_EMBEDDED_SERVER=OFF                 \
    -DWITH_EXAMPLE_STORAGE_ENGINE=ON           \
    -DWITH_BOOST=../boost

TODODO: UNIV_LRU_DEBUG 該編輯項如何打開
-->

注意，除了上述列舉的選項外，可通過 ```cmake .. -LH``` 查看當前版本所支持的選項，如要重新編譯可以直接運行 ```make clean; rm CMakeCache.txt; cmake ..``` 即可。

另外，MariaDB 不支持使用 ```-DWITH_DEBUG=1```，應該使用 ```-DCMAKE_BUILD_TYPE=Debug```；更多 MySQL 編譯選項可以參考 [Source Configration Options](http://dev.mysql.com/doc/mysql-sourcebuild-excerpt/en/source-configuration-options.html) 。

在使用 make 進行編譯時，可以通過 -j 選項指定使用多個 Jobs 進行編譯，也就是通過多個 CPU 進行編譯，這樣可以加快編譯速度。

源碼編譯完成後，默認會將程序保存在 sql/mysqld 目錄下，然後即可以通過 make install 將其安裝在 /opt/mysql-5.7 目錄下，為了方便運行，可以將 /opt/mysql-5.7/bin 添加到環境變量 PATH 中。

### 客戶端

在此僅簡單記錄下 mysql 命令行鏈接數據庫時的常用命令，其源碼保存在 client/mysql.cc，當嘗試調試客戶端時可以使用如下所述的 debug 模式排查問題，可以通過如下命令啟動。

{% highlight text %}
$ mysql --debug='d:t:i:o,/tmp/mysql.trace'
{% endhighlight %}

如上所述，在本機測試時，為了方便登陸可以配置 ~/.my.cnf 文件。

{% highlight text %}
$ cat ~/.my.cnf
[client]
user=root
password=YOUR_PASSWD
socket=/tmp/mysql.sock
default-character-set=utf8
{% endhighlight %}

常見的參數選項：

{% highlight text %}
--prompt
    可通過該參數設置 mysql 提示符中顯示當前用戶、數據庫、時間等信息；例如，可以通過如下方式啟動：
    $ mysql -u root -p --prompt="\\u@\\h:\\d >"

--column-type-info
    顯示元數據信息。

-A, --no-auto-rehash
    不預讀數據庫信息（默認操作），切換數據庫時不會預讀，因此切換更快。可以使用反之為--auto-rehash，此時可以補全表明/庫名。

-D, --database
    直接指定數據庫。

-B, --batch
    不會輸出間隔。

反引號
    通過反引號包含的字符串表示非關鍵字。

(NUM)
    int() 表示的是顯示的長度，與保存字節數無關；char() 指定字符串長度，長度固定，varchar 表示可變字符串。
{% endhighlight %}

當數據較多無法一屏顯示時，可以通過 pager more 設置，此時會通過 more 命令進行分頁。

## 運行實例

如下，簡單以源碼安裝時的路徑為例，如果是通過類似 YUM 安裝的，則可以直接替換可執行文件的路徑即可，不再贅述。

### 數據初始化

第一次運行時需要做些初始化操作，主要是需要創建一些所必須的系統數據庫以及相應的表，例如 mysql、preformance_schema、information_schema 等。

{% highlight text %}
----- 5.6以及之前的版本
$ /opt/mysql-5.6/script/mysql_install_db --no-defaults --basedir=/opt/mysql-5.6 \
    --datadir=/home/foobar/Workspace/mysql/data --user=mysql

----- 5.7以及之後的版本，使用--initialize時密碼保存在~/.mysql_secret
$ /opt/mysql-5.7/bin/mysqld --initialize-insecure --basedir=/opt/mysql-5.7 \
    --datadir=/home/foobar/Workspace/mysql/data --user=mysql
{% endhighlight %}

<!--
/opt/mysql-5.7/bin/mysql_install_db --no-defaults --basedir=/opt/mysql-5.7 \
    --datadir=/home/andy/Workspace/databases/mysql/data-mysql
-->

如果不通過 \-\-user 參數指定用戶名，則使用默認的當前用戶，此時不需要設置 datadir 目錄的權限，否則需要設置相應權限。

basedir 指定 MySQL 的安裝目錄；datadir 指定數據的存放位置；可以使用 \-\-defaults-file=my.cnf 指定默認的配置文件，可以使用相對路徑，默認相對於 datadir。在 share 子目錄下，包含了一些配置文件，以及初次安裝運行需要的初始數據庫文件比如 mysql_system_tables.sql。

注意，如果要使用參數指定默認配置文件，則 \-\-defaults-file 選項應該放置在第一個參數，而且 datadir、basedir 必須使用絕對路徑。

### 單實例運行

新建配置文件 data/my.cnf (可以為空)，並通過如下命令啟動/停止/連接服務器。

{% highlight text %}
----- 1. 切換到工作目錄
$ cd /home/foobar/Workspace/mysql

----- 2. 啟動單個MySQL實例，前臺運行，如果要後臺運行直接添加&即可
$ /opt/mysql-5.7/bin/mysqld --defaults-file=data/my.cnf  \
    --datadir=/home/foobar/Workspace/mysql/data          \
    --basedir=/opt/mysql-5.7 --skip-networking --debug
$ /opt/mysql-5.7/bin/mysqld_safe --defaults-file=data/my.cnf  \
    --datadir=/home/foobar/Workspace/mysql/data          \
    --basedir=/opt/mysql-5.7 --skip-networking --debug &

----- 3. 如下命令連接到數據庫，5.6之前為密碼為空，5.7密碼保存在~/.mysql_secret
$ /opt/mysql-5.7/bin/mysql -uroot -p'init-password' -S /tmp/mysql.sock

----- 4. 可以通過如下命令關閉、查看狀態
$ /opt/mysql-5.7/bin/mysqladmin -uroot -p'init-password' shutdown
$ /opt/mysql-5.7/bin/mysqladmin -uroot -p'init-password' status
{% endhighlight %}

<!--
/opt/mysql-5.7/bin/mysqld --defaults-file=data-mysql/my.cnf   \
    --datadir=/home/andy/Workspace/databases/mysql/data-mysql \
    --basedir=/opt/mysql-5.7 --skip-networking --debug
/opt/mysql-5.7/bin/mysqld_safe --defaults-file=data-mysql/my.cnf   \
    --datadir=/home/andy/Workspace/databases/mysql/data-mysql \
    --basedir=/opt/mysql-5.7 --skip-networking --debug
-->

在通過上述命令啟動服務器時，添加了 skip-networking 參數，也就是不啟動 TCP 網絡，因此只能通過 sock 連接。

需要注意的是，如果不使用 \-\-defaults-file 選項，mysqld 會自動讀取 /etc/my.cnf 配置文件，如果默認值與期望值不符，從而可能會導致部分錯誤，當然可以刪除該配置文件，不過不建議。

**推薦** 可以將對應的啟動、關閉命令設置為 alias ，這樣可以簡化啟動方式，同時仍然可以動態添加參數，如直接再在命令的末尾添加 \-\-debug 。

### 多實例運行

多實例主要是端口以及保存的數據目錄不同，通過簡單設置這些參數，可以在同一臺 server 上啟動多個實例，下面啟動一個端口號為 3307 的實例，其中配置文件 mysql3307.cnf 的內容如下。

{% highlight text %}
$ cat mysql3307.cnf
[mysqld]
port = 3307
server-id = 3307
basedir = /opt/mysql-5.7
socket = /tmp/mysql3307.sock
pid-file = /home/foobar/Workspace/mysql/data3307/mysql3307.pid
datadir = /home/foobar/Workspace/mysql/data3307
{% endhighlight %}

然後，可以通過如下命令設置並啟動一個新的 MySQL 實例。

{% highlight text %}
----- 1. 創建目錄
$ cd /home/foobar/Workspace/mysql && mkdir data3307

----- 2. 如上，初始化實例
$ /opt/mysql-5.7/bin/mysqld --initialize-insecure --basedir=/opt/mysql-5.7 \
    --datadir=/home/foobar/Workspace/mysql/data3307 --user=mysql

----- 3. 啟動實例
$ /opt/mariadb/bin/mysqld --defaults-file=data3307/mysql3307.cnf

----- 4. 連接數據庫
$ mysql -uroot -h127.1 -P3307

----- 5. 關閉數據庫
$ /opt/mariadb/bin/mysqladmin -uroot -S /tmp/mysql3307.sock shutdown
{% endhighlight %}

對於多實例，可以通過上述的方法創建實例，不過管理操作還是略微有點複雜，實際上可以直接通過 mysqld_multi 命令管理。

另外，可以通過 mysqld_multi \-\-example 查看示例配置文件，或者使用 multi.cnf 。

{% highlight text %}
$ cat multi.cnf
[mysqld_multi]
mysqld = /opt/mysql-5.7/bin/mysqld
mysqladmin = /opt/mysql-5.7/bin/mysqladmin
user = root

[mysqld3307]
socket = /tmp/mysql3307.sock
port = 3307
pid-file = /home/foobar/Workspace/mysql/data3307/mysql3307.pid
datadir = /home/foobar/Workspace/mysql/data3307
basedir = /opt/mysql-5.7
server-id=3307

[mysqld3308]
socket = /tmp/mysql3308.sock
port = 3308
pid-file = /home/foobar/Workspace/mysql/data3308/mysql3308.pid
datadir = /home/foobar/Workspace/mysql/data3308
basedir = /opt/mysql-5.7
server-id=3308
{% endhighlight %}

如下是一些常見的操作。

{% highlight text %}
$ cd /home/foobar/Workspace/mysql
$ /opt/mysql-5.7/bin/mysqld_multi --defaults-file=multi.cnf report     ← 查看多個實例狀態
$ /opt/mysql-5.7/bin/mysqld_multi --defaults-file=multi.cnf start
$ /opt/mysql-5.7/bin/mysqld_multi --defaults-file=multi.cnf stop
{% endhighlight %}


## 調試程序

如果要調試需要在編譯時添加 -DCMAKE_BUILD_TYPE=Debug 選項，可以通過 gdb 或者使用 dbug 打印調試信息。

### 使用 GDB

可以通過如下的方式加載程序，然後運行。

{% highlight text %}
$ gdb -args /opt/mariadb/bin/mysqld --defaults-file=......
(gdb) run
{% endhighlight %}

如果 MySQL 已經啟動，此時可以通過 gdb 的 attach PID 命令來調試，一般會阻塞在系統調用 poll() 處；調試結束通過 detach 命令退出。注意，要先 **設置斷點** 。

如果通過 CMake 進行編譯，往往會採用絕對路徑，可以通過 readelf -p .debug_str 查看，此時設置/查看斷點時應該使用絕對路徑。可以修改/添加如下文件。

{% highlight text %}
$ cat CMakeLists.txt                         # 添加如下內容
... ...
CONFIGURE_FILE(
    "${PROJECT_SOURCE_DIR}/gcc_debug_fix.sh.in"
    "${PROJECT_BINARY_DIR}/gcc_debug_fix.sh"
    @ONLY)
... ...

$ cat cmake/mysql_add_executable.cmake       # 添加如下內容
ADD_EXECUTABLE(... ...)
SET_TARGET_PROPERTIES(${target} PROPERTIES
    RULE_LAUNCH_COMPILE "${PROJECT_BINARY_DIR}/gcc_debug_fix.sh")
... ...

$ cat gcc_debug_fix.sh.in                    # 添加如下文件，需要設置可執行
#!/bin/sh
PROJECT_BINARY_DIR="@PROJECT_BINARY_DIR@"
PROJECT_SOURCE_DIR="@PROJECT_SOURCE_DIR@"

# shell script invoked with the following arguments
# $(CXX) $(CXX_DEFINES) $(CXX_FLAGS) -o OBJECT_FILE -c SOURCE_FILE

# extract parameters
SOURCE_FILE="${@: -1:1}"
OBJECT_FILE="${@: -3:1}"
COMPILER_AND_FLAGS=${@:1:$#-4}

# make source file path relative to project source dir
SOURCE_FILE_RELATIVE="${SOURCE_FILE:${#PROJECT_SOURCE_DIR} + 1}"

# make object file path absolute
OBJECT_FILE_ABSOLUTE="$PROJECT_BINARY_DIR/$OBJECT_FILE"

cd "$PROJECT_SOURCE_DIR"

# invoke compiler
exec $COMPILER_AND_FLAGS -c "${SOURCE_FILE_RELATIVE}" -o "${OBJECT_FILE_ABSOLUTE}"
{% endhighlight %}

編譯方式仍如上所示，相關內容可以參考 [Make gcc put relative filenames in debug information](http://www.stackoverflow.com/questions/9607155/make-gcc-put-relative-filenames-in-debug-information) 。

### 調試技巧

為了比較好的查看變量內容，可以設置如下。

{% highlight text %}
(gdb) set print vtbl on         ← 用比較規整的格式來顯示虛函數表
(gdb) ptype obj/class/struct    ← 查看obj/class/struct的成員，但是會把基類指針指向的派生類識別為基類
(gdb) set print object on       ← 查看派生對象的真實類名
(gdb) set print pretty on       ← 以樹形打印對象的成員，可以清晰展示繼承關係
{% endhighlight %}

鏈接到 mysqld 後，會阻塞在 poll()，然後設置調試入口，在此簡單列舉一下常見的調試入口點。

* handle_connections_sockets() @ mysqld.cc<br>
    等待接收處理新的鏈接。
* do_handle_one_connection() @ sql_connect.cc<br>
    接下來的處理過程為處理鏈接，注意該處理函數與鏈接的處理方式有關。
* do_command() @ sql_parse.cc; dispatch_command() @ sql_parse.cc<br>
    接著是處理接收到的 SQL，其中有一個變量 THD thd @ sql_class.h 會經常使用，該結構體中包含了當前線程的相關信息。

如下是簡單的在 dispatch_command() 斷點處打印的信息，用 @ 簡單標示斷點信息。

{% highlight text %}
# @ dispatch_command()
(gdb) p thd.net                 ← 查看網絡的信息
(gdb) p packet                  ← 請求的命令
{% endhighlight %}

### 使用 DBUG

對於 MySQL 需要在編譯時添加調試選項 -DCMAKE_BUILD_TYPE=Debug ，然後在啟動時使用 \-\-debug 參數，默認會啟動所有的調試選項，調試的信息會保存在 /tmp/mysqld.trace 文件中。

可以在啟動時使用 \-\-debug[=debug_options] 添加調試選項，默認值是 d:t:i:o,/tmp/mysqld.trace 。選項的格式為 [field1]:[field2]... 每個 field 之間用冒號分割，各個 field 的內容為 flag[,modifier,modifier...] 。

常用的標記有：

{% highlight text %}
d
    允許對當前狀態從DBUG_<N>宏輸出。可能跟著一列關鍵詞，這些關鍵詞僅對那些帶有關鍵詞的DBUG宏選擇輸出。
    一個空的關鍵詞列意味著對所有宏輸出。
t
    允許函數調用/退出跟蹤行。可能跟著一個給出最大跟蹤級別的數字列（只含一個修改量），超過這個數字，
    調試中或跟蹤中的宏不能產生任何輸出。 默認為一個編譯時間選項。
i
    對調試或跟蹤輸出的每一行用PID或線程ID識別進程。
o
    重定向調試器輸出流到指定文件，默認輸出是stderr 文件。
{% endhighlight %}

注意：調試輸出選項是在 mysqld_main() 中的 handle_options() 返回後開始輸出的，之前應該是還沒有初始化，所以 ......

另外，在運行的時候也可以通過，如下的方式修改，日誌的保存位置可以查看該變量。

{% highlight text %}
mysql> SHOW VARIABLES LIKE 'debug';
mysql> SET GLOBAL debug = 'd:t:i:o,/tmp/mysqld.trace';
mysql> SET SESSION debug = 'd:t:i:o,/tmp/mysqld.trace';
{% endhighlight %}

其中，DBUG_RETURN() 後面的數字對應的是行號。

其它與 dbug 相關的內容可以參考 [dbug 源碼](http://sourceforge.net/projects/dbug/)，其中有一個 example.c 文件，可以將其複製到 src 目錄下，最簡單的方式是直接通過 gcc example.c dbug.c 編譯，編譯後運行將直接輸出到終端上；當要取消調試選項時只需要添加宏 #define DBUG_OFF。

注意：MySQL 的 dbug 庫是經過修改的，會有稍微的差別。

## 打印執行的SQL

通常在測試某些功能時，希望可以直接輸出所執行的 SQL，通常可以使用如下的兩種方式。

### general log

可以通過 general_log 打開日誌，記錄所有的執行日誌，當然只用於測試，生產環境不要打開。

{% highlight text %}
mysql> show global variables like 'general%';
mysql> set global general_log=1;                  # 打開查詢日誌
{% endhighlight %}

默認日誌保存在 $datadir/$general_log_file 文件中。

另外，slow query log 用於記錄執行時間超過 long_query_time 的 SQL 。

### tcpdump

可以通過 tcpdump 截取 MySQL 報文，假設使用的時 lo 接口；需要注意的是：使用本地連接可能會直接用 Linux sock 鏈接，通過客戶端創建連接時需要指定 -h 127.1。

{% highlight text %}
# tcpdump -i lo -s 0 -l -w - dst port 3306 | strings

其中的選項為：
-s 0     將截取報文的長度設置為默認值 65536 bytes
-i lo    指定監聽的網絡接口
-l       輸出變為緩衝行形式
-w       直接將包寫入文件中，並不分析和打印出來，在此輸出到標準輸入

# tcpdump -i lo -s 0 -l -w - dst port 3306 | strings | perl -e '
while(<>) { chomp; next if /^[^ ]+[ ]*$/;
  if(/^(SELECT|UPDATE|DELETE|INSERT|SET|COMMIT|ROLLBACK|CREATE|DROP|ALTER)/i) {
    if (defined $q) { print "$q\n"; }
    $q=$_;
  } else {
    $_ =~ s/^[ \t]+//; $q.=" $_";
  }
}'
{% endhighlight %}

## 參考

其中調試時的相對路徑設置可以參考 [stackflow](http://stackoverflow.com/questions/9607155/make-gcc-put-relative-filenames-in-debug-information) 或者 [本地文檔](/reference/databases/mysql/make gcc put relative filenames in debug information.mht) 。


{% highlight text %}
{% endhighlight %}
