---
title: MySQL 高可用 MHA
layout: post
comments: true
language: chinese
category: [mysql,database]
keywords: mysql,高可用,mha
description: Master High Availability, MHA 在 MySQL 高可用方面是一個相對成熟的解決方案，故障切換過程中，能做到在 0~30 秒之內自動完成數據庫的故障切換操作，而且能最大程度上保證數據的一致性。
---

Master High Availability, MHA 在 MySQL 高可用方面是一個相對成熟的解決方案，能做到在 0~30 秒之內自動完成數據庫的故障切換操作，而且能最大程度上保證數據的一致性。

這裡介紹使用配置方法，以及其原理。

<!-- more -->

## 簡介

該軟件由管理節點 (Manager) 和數據節點 (Node) 組成，其中 Manager 可以單獨部署在一臺機器上，並管理多個 Master-Slave 集群，當然，也可以部署在一臺備節點上。而 Node 節點需要在每個服務器上都部署。

Node 運行在每臺 MySQL 服務器上，Manager 會定時探測集群中的主節點，當主出現故障時，可以自動將最新的備提升為新的主，然後將所有其它的備重新指向新的主。


### 使用條件

1. MHA 是為瞭解決數據一致性，主要支持一主多從，一般一個集群中最少有三臺數據庫服務器，也就是一主二從；從 0.52 開始，就開始支持多主複製架構了，但是多主只允許單點寫入，默認只支持 2 層複製架構。
2. 候選 Master 必須開啟 log-bin ，如果備庫沒有開啟 log-bin 則不會被提升為主，如果所有備庫未開啟則報錯。
3. 主備環境的 binlog、relay-log 必須一致，包括複製過濾規則 binlog-do-db、replicate-ignore-db 等等必須全部一致。
4. 複製用戶必須在候選主上要存在，而且切換完後，所有備庫都需要執行 ```change master``` 命令，因此在 new master 上覆制用戶必須有 ```REPLICATEION SLAVE``` 權限。簡單來說，所有的節點都會創建複製用戶及其權限。
5. SQL 線程默認在執行完 relay-log 後自動刪除，不過這些 relaylog 可能還會使用，所以需要關閉自動刪除，然後自己來刪除，當然必須考慮 repl 延遲問題。
6. 不要使用 ```load data infile```，尤其是 SBR 環境中。

<!--
2. MySQL 版本必須大於等於 5.0
    儘量使用高版本的MySQL

3.4 三層或者多層複制環境

默認情況下，MHA是不支持3層或多層複制架構的（Master1 -> Master2 -> Slave3）
MHA可以恢復Master2，但是不能恢復Slave3，因為Master2，Slave3有不同的master
為了讓MHA支持以上架構，可以參考如下配置：

    在配置文件中，只配置兩層(master1 and master2)
    使用 “multi_tier_slave=1” 參數，然後設置所有hosts

3.6 使用mysqlbinlog 5.1+ 支持MySQL5.1+

    MHA使用mysqlbinlog來應用日誌到目標slave上的
    如果MySQL master設置的是row格式，那麼MySQL必須是大於等於5.1版本，因為5.0不支持row
    mysqlbinlog版本可以這樣被檢測：
[app@slave_host1]$  mysqlbinlog --version
mysqlbinlog Ver 3.3 for unknown-linux-gnu at x86_64

    如果你使用的是MySQL5.1，那麼mysqlbinlog必須大於等於3.3
    如果mysqlbinlog的版本是3.2，而mysql的版本是5.1，那麼mha manager會報錯，且停止monitoring
-->


### 安裝

該軟件實際上是使用 Perl 編寫的，可以直接從 [Github Manager](https://github.com/yoshinorim/mha4mysql-manager) 和 [Github Node](https://github.com/yoshinorim/mha4mysql-node) 下載即可。如下是關於這兩個包中各個文件的介紹。

{% highlight text %}
Manager:
   masterha_check_repl        檢查MySQL複製狀況
   masterha_check_ssh         檢查MHA的SSH配置狀況
   masterha_check_status      檢測當前MHA運行狀態
   masterha_manager           啟動MHA監控進程，執行一次切換之後會退出
   masterha_master_monitor    檢測master是否宕機
   masterha_master_switch     控制故障轉移，可以自動或者手動
   masterha_conf_host         添加或刪除配置的server信息
   masterha_stop              停止manager服務
   masterha_secondary_check   第二次檢查服務器，防止出現腦裂現象

Node: 一般由Manager腳本觸發，無需手動操作
   save_binary_logs           保存和複製宕掉主庫的bilog
   apply_diff_relay_logs      識別差異的中繼日誌事件並將其差異的事件應用於其他的slave
   filter_mysqlbinlog         去除不必要的ROLLBACK事件（MHA已不再使用這個工具）
   purge_relay_logs           清除中繼日誌（不會阻塞SQL線程）
{% endhighlight %}

源碼安裝時，需要安裝如下依賴包；接下來看看如何源碼安裝。

{% highlight text %}
----- 1. 安裝依賴庫
# yum install perl-Module-Install perl-Module-Build  # 源碼安裝編譯
# yum install perl-DBD-MySQL                         # MySQL驅動
# yum install perl-Config-Tiny
# yum install --enablerepo=epel perl-Log-Dispatch perl-Parallel-ForkManager

----- 2. 源碼安裝Node
$ perl Makefile.pl
$ make
# make install

----- 3. 源碼安裝Manager
$ perl Makefile.pl
$ make
# make install
# cp samples/conf/masterha_default.cnf /etc/
{% endhighlight %}

### 配置文件

在 samples/conf 目錄下存在示例配置文件，各個配置項簡單列舉如下。

{% highlight text %}
$ cat << EOF > /tmp/mysql-mha.cnf
[server default]
manager_workdir=/tmp/mha-master/foobar          # Manager工作目錄
manager_log=/tmp/mha-master/foobar/manager.log  # Manager日誌
user=root                                       # 設置監控時使用的用戶名
password=new-password                           # 及其密碼
repl_user=mysync                                # 複製時使用的用戶名
repl_password=kidding                           # 複製用戶的密碼
ssh_user=root                                   # ssh的登錄用戶名
master_binlog_dir=/tmp/mysql-master             # 主庫binlog的保存目錄
log_level=debug                                 # 設置日誌打印級別，包括debug/info/warning/error
ping_interval=1                                 # 向主庫發送ping的時間間隔，默認3秒，三次失敗則自動failover
remote_workdir=/tmp/mha                         # 遠端mysql在發生切換時binlog的保存位置

# Failover時執行的腳本，如果使用了keepalived類似工具，可以配置切換VIP，詳見force_shutdown_internal()函數的實現
#master_ip_failover_script= /usr/local/bin/master_ip_failover
# Failover時執行的腳本，可用來關閉MySQL服務或者服務器，防止發生腦裂，詳見force_shutdown_internal()函數的實現
shutdown_script = ""

master_ip_online_change_script= /usr/local/bin/master_ip_online_change  //設置手動切換時候的切換腳本
report_script=/usr/local/send_report    //設置發生切換後發送的報警的腳本
secondary_check_script= /usr/local/bin/masterha_secondary_check -s server03 -s server02

[server1]
hostname=127.0.0.1
port=3307

[server2]
hostname=127.0.0.1
port=3308
candidate_master=1      # 候選主庫，即使不是最新的，仍會將該庫提升為主庫
check_repl_delay=0      # 默認會檢查relaylog的延遲，超過100M時不會將其提升為主，在此設置為忽略

[server3]
hostname=127.0.0.1
port=3309
EOF
{% endhighlight %}

<!--
二次檢測： 檢查master是否真的掛了，避免腦裂   secondary_network_script
* master_ip_failover: 默認的是空，什麼都不做
    ip漂移
    新master賦予寫
    日誌保留
    報警
等等
-->

### 簡單測試

如果主庫宕了之後，MHA 在完成切換之後會自動停止，所以最好配合 deamontools 工具，在進程停止之後可以自動拉起。切換過程可以查看日誌 [mha-manager.log](/reference/databases/mysql/mha-manager.log) 。

接下來，一步步測試。

##### 1. 設置免登陸

可以通過如下命令查看配置是否正常。

{% highlight text %}
$ masterha_check_ssh --conf=/tmp/mysql-mha.cnf
All SSH connection tests passed successfully.
{% endhighlight %}

<!--
如果slave比較多，實例比較多，最好提高下 /etc/ssh/sshd_config MaxStartups 的值（默認是10）
--conf=/tmp/mysql-mha.cnf
-->

##### 2. 檢查複製配置

{% highlight text %}
$ masterha_check_repl --conf=/tmp/mysql-mha.cnf
{% endhighlight %}

##### 3. 開啟Manager

{% highlight text %}
$ nohup masterha_manager --conf=/tmp/mysql-mha.cnf < /dev/null > /tmp/mha-manager.log 2>&1 &

常用參數：
  --check_only
    只檢查狀態是否正常。
{% endhighlight %}

<!--
nohup masterha_manager --conf=/usr/local/mha/mha.cnf --ignore_last_failover < /dev/null > /home/data/mha/manager.log 2>&1 &
-->

##### 4. 檢查Manager的狀態

{% highlight text %}
$ masterha_check_status --conf=/tmp/mysql-mha.cnf
mysql-mha (pid:17446) is running(0:PING_OK), master:127.0.0.1
{% endhighlight %}

##### 5. 測試關閉Manager

{% highlight text %}
$ masterha_stop --conf=/tmp/mysql-mha.cnf
Stopped mysql-mha successfully.
{% endhighlight %}

注意：binlog-do-db 和 replicate-ignore-db 設置必須相同。 MHA 在啟動時候會檢測過濾規則，如果過濾規則不同，MHA 不啟動監控和故障轉移。


## 主備切換

當主庫故障時，如果沒有自動切換，那麼就只能等待手動切換，當然，時間就不確定了。自動切換就需要找到一個合適的備庫作為主庫，並將主庫指向新主庫，並重新啟動複製。

### 故障場景

當主庫故障時，會有如下的幾種情況，簡單介紹下。

#### 全部同步

![mysql mha all slaves sync]({{ site.url }}/images/databases/mysql/mha-all-slaves-sync.png "mysql mha all slaves sync"){: .pull-center }

所有備庫都從主庫收到了 binlog 事件，那麼此時，任何一個備庫都可以選為新的主庫，只需要執行如下命令即可。

{% highlight text %}
mysql> CHANGE MASTER TO MASTER_HOST='slave1', MASTER_LOG_FILE='file1', MASTER_LOG_POS=pos1;
mysql> START SLAVE;
{% endhighlight %}

當然，這是最簡單的一種方式，大多數情況下不會這麼幸運。


#### 備庫相同，部分事件丟失

![mysql mha all slaves receive same events]({{ site.url }}/images/databases/mysql/mha-all-slaves-same.png "mysql mha all slaves receive same events"){: .pull-center }

也就是所有的備庫都從主庫收到了相同的 binlog 事件，但主庫部分事件沒有發送成功，如果直接切換過去，那麼上述的 ```id=17``` 事件就會丟失了。

如果崩潰的主庫可以通過 SSH 訪問，而且能夠讀取 binlog 文件，那麼在提升一個主庫前，應該將 17 對應的事件保存到 binlog 文件中。

當然，可以通過 semi-sync 減小這種情況發生的概率。

#### 某個備庫事件完整

![mysql mha some slave sync]({{ site.url }}/images/databases/mysql/mha-some-slave-sync.png "mysql mha some slave sync"){: .pull-center }

某個備庫收到了其它備庫未收到的事件，需要從中選出一個備庫，並將事件同步到其它備庫，從而使所有的備庫保持一致。

現在的問題是：1) 如何確認那些 binlog 事件未發送？ 2) 如何使所有的備庫保持最終的一致？

#### 備庫事件不一致，最新備庫有事件丟失

![mysql mha some slaves events lost]({{ site.url }}/images/databases/mysql/mha-some-slaves-events-lost.png "mysql mha some slaves events lost"){: .pull-center }

如果採用異步複製，那麼估計這是最常見的故障場景，也就是備庫收到的事件數不同，其中的某個庫收到的事件比較多，但是仍有部分主庫的事件丟失。

如上圖所示，備庫收到的事件有所不同，Slave2 是最新的備庫，Slave1 和 Slave3 備庫丟失更多事件，而且 ```id=17``` 對應的事件所有備庫都沒有收到。

接下來需要：1) 如果可能需要將 ```id=17``` 對應的事件從主庫複製到備庫；2) 將所有備庫的事件補齊，防止備庫發生數據不一致性。


### 主備複製狀態

可以通過 ```SHOW SLAVE STATUS``` 命令查看當前主備的複製狀態，如下簡單列舉常見狀態。

{% highlight text %}
mysql> SHOW SLAVE STATUS\G
... ...
      Master_Log_File: mysql-bin.000001   # IO線程正在讀取的主庫binlog文件名稱
  Read_Master_Log_Pos: 107                # IO線程已讀取主庫binlog位置
       Relay_Log_File: relay-bin.000002   # SQL線程正在讀取執行的relaylog名稱
        Relay_Log_Pos: 253                # SQL線程已讀取執行的relaylog位置
Relay_Master_Log_File: mysql-bin.000001   # SQL線程執行的主庫binlog文件名稱
  Exec_Master_Log_Pos: 107                # SQL線程執行來自主庫的binlog最後一個事件位置
... ...
{% endhighlight %}

其中一些常見狀態查看方式如下：

1. 備庫同步是否正常。查看 ```Slave_IO_Running```、```Slave_SQL_Running``` 是否正常(Yes)，不正常則可以查看報錯；
2. 從庫是否有事件在 relaylog 中等待執行。查看 IO 線程從主庫的讀取位置 ```Read_Master_Log_Pos``` 和從庫的執行位置 ```Exec_Master_Log_Pos``` 是否一致；
3. 判斷備庫落後主庫多長時間，可以查看 ```Seconds_Behind_Master``` 的值。

其中，```SHOW SLAVE STATUS``` 命令的源碼執行過程如下。

{% highlight text %}
show_slave_status_cmd()
 |-show_slave_status()
   |-show_slave_status_metadata()    發送元數據
   |-show_slave_status_send_data()   真正的監控數據
{% endhighlight %}

在 show_slave_status_send_data() 函數中的相關內容如下。


{% highlight cpp %}
bool show_slave_status_send_data(THD *thd, Master_info *mi,
                                 char* io_gtid_set_buffer,
                                 char* sql_gtid_set_buffer)
{
  // ... ...
  protocol->store(mi->get_master_log_name(), &my_charset_bin);            // Master_Log_File
  protocol->store((ulonglong) mi->get_master_log_pos());                  // Read_Master_Log_Pos
  protocol->store(mi->rli->get_group_relay_log_name() +
                  dirname_length(mi->rli->get_group_relay_log_name()),
                  &my_charset_bin);                                       // Relay_Log_File
  protocol->store((ulonglong) mi->rli->get_group_relay_log_pos());        // Relay_Log_Pos
  protocol->store(mi->rli->get_group_master_log_name(), &my_charset_bin); // Relay_Master_Log_File
  // ... ...
  protocol->store((ulonglong) mi->rli->get_group_master_log_pos());       // Exec_Master_Log_Pos
  // ... ...
}
{% endhighlight %}

### 主備延遲

一般都是通過 ```SHOW SLAVE STATUS``` 命令查看如下的狀態。

{% highlight text %}
mysql> SHOW SLAVE STATUS\G
... ...
    Seconds_Behind_Master: 0  # 備庫延遲時間，單位為秒
{% endhighlight %}

<!--
http://michaelkang.blog.51cto.com/1553154/1201604

http://imysql.com/2014/08/30/mysql-faq-howto-monitor-slave-lag.shtml
-->

## 源碼解析

{% highlight text %}
masterha_manger
 |-MHA::MasterMonitor::main()                        監控是否有異常
 | |-wait_until_master_is_dead()
 |   |-wait_until_master_is_unreachable()
 |     | ###<>###Phase 1: Configuration Check Phase
 |     |-MHA::ManagerUtil::check_node_version()      檢查Manager+Node的版本，要求Node版本>=Manager版本
 |     | |-get_node_version() 通過SSH命令獲取Node版本，可以直接查看該函數
 |     |-MHA::NodeUtil::check_manager_version()
 |     |-MHA::ServerManager::connect_all_and_read_server_status() 確認各個Node節點是否可以連接MySQL服務
 |     |-MHA::ServerManager::get_dead_servers() 再次檢測node節點的狀態
 |     |-MHA::ServerManager::get_alive_servers()
 |     |-MHA::ServerManager::get_alive_slaves()
 |     |-MHA::ServerManager::print_dead_servers()
 |     |
 |     |-MHA::HealthCheck::wait_until_unreachable()
 |
 |-MHA::MasterFailover::main()                         開始執行切換
   |-init_config()                                     初始化配置
   |-do_master_failover()                              真正執行Failover切換操作
   | |
   | | ###<>###Phase 1: Configuration Check Phase
   | |-MHA::ServerManager::init_binlog_server()        檢查每個MySQL服務器，保證SSH+Node版本號
   | | |-MHA::HealthCheck::ssh_check_simple()          檢查SSH是否正常
   | | | |-ssh_check()
   | | |-MHA::ManagerUtil::get_node_version()          同時檢查Node的版本號
   | |-check_settings()
   | | |-MHA::ManagerUtil::check_node_version()
   | | |-connect_all_and_read_server_status()
   | |   |-run_on_start()
   | |   |-run_on_finish()
   | |
   | | ###<>###Phase 2: Dead Master Shutdown Phase
   | |-force_shutdown()
   | | |-stop_io_thread()                              停止所有備庫的IO-thread
   | | |-force_shutdown_internal()                     執行master_ip_failover_script(例如切換VIP)以及
   | | |                                                   shutdown_script(如關閉mysqld進程)指定的腳本
   | |
   | | ###<>###Phase 3: Master Recovery Phase
   | | ###<>###Phase 3.1: Getting Latest Slaves Phase
   | |-check_set_latest_slaves()
   | | |-read_slave_status()
   | | |-identify_latest_slaves()
   | | |-print_latest_slaves()
   | | |-identify_oldest_slaves()
   | | |-print_oldest_slaves()
   | |
   | | ###<>###Phase 3.2: Saving Dead Master's Binlog Phase
   | |-is_gtid_auto_pos_enabled()
   | |-save_master_binlog()                            如果非GTID，會判斷是否有數據丟失
   | | |-MHA::ManagerUtil::check_node_version()        檢查宕機主庫的版本信息
   | | |-save_master_binlog_internal()                 保存binlog
   | |   |-MHA::ManagerUtil::exec_ssh_cmd()            調用Node的save_binary_logs腳本
   | |   |-MHA::NodeUtil::file_copy()                  從宕掉的庫複製差異數據
   | |
   | | ###<>###Phase 3.3: Determining New Master Phase
   | |-is_gtid_auto_pos_enabled()
   | |-get_most_advanced_latest_slave()                是GTID
   | |-find_latest_base_slave()                        非GTID
   | |-select_new_master()
   | |-recover_master()
   | | |-recover_master_gtid_internal()                GTID
   | | |-recover_master_internal()                     NOT GTID
   | | | | ###<>###Phase 3.3: New Master Diff Log Generation Phase
   | | | |-recover_relay_logs()
   | | | | |-generate_diff_from_readpos()              執行Node節點上的apply_diff_relay_logs腳本
   | | | |-send_binlog()
   | | | | ###<>###Phase 3.4: Master Log Apply Phase
   | | | |-recover_slave()                             將獲取的binlog差值恢復到新主庫
   | | | | |-apply_diff()                              執行Node節點上的apply_diff_relay_logs腳本
   | | | |   |-wait_until_relay_log_applied()
   | | | |   |-stop_sql_thread()
   | | | |-get_new_master_binlog_position()
   | | |
   | | |-MHA::ManagerUtil::exec_system()               執行master_ip_failover_script腳本
   | |
   | | ###<>###Phase 4: Slaves Recovery Phase
   | |-recover_slaves()
   | | |-is_gtid_auto_pos_enabled()                判斷是否開啟GTID
   | | |-recover_slaves_gtid_internal()            GTID
   | | |
   | | | ###NOT GTID###BEGIN
   | | |-recover_all_slaves_relay_logs()
   | | |-recover_slaves_internal()
   | | | ###<>###Phase 5: New master cleanup phase
   | | |-reset_slave_on_new_master()
   | | | |-reset_slave_info()
   | | | ###NOT GTID###END
   | |
   | |-cleanup()
   |
   |-finalize_on_error()
{% endhighlight %}


<!--

Identifying the latest slave

通過SHOW SLAVE STATUS中的Master_Log_File、Read_Master_Log_Pos確認那個是最新的SLAVE，注意不能依賴relay log信息，因為不同的備庫信息不同。

Applying diffs to other slaves(不同備庫的Gap不同)
-->


## 參考

關於 MHA 的介紹，可以參考 [Github MHA-Wiki](https://github.com/yoshinorim/mha4mysql-manager/wiki)，另外，還可以查看 [Automated, Non-Stop MySQL Operations and Failover](/reference/databases/mysql/Automated_Non-Stop_MySQL_Operations_and_Failover_Presentation.pdf) 。


<!--
其它

https://www.slideshare.net/matsunobu/myrocks-deep-dive

http://files.cnblogs.com/files/jimingsong/mha-mysql.pdf

MySQL的很多資源

https://huoding.com/2011/04/05/59

http://code.openark.org/forge/openark-kit



https://www.percona.com/blog/2014/07/03/failover-mysql-utilities-part-2-mysqlfailover/

https://scriptingmysql.wordpress.com/2012/12/06/using-the-mysql-script-mysqlfailover-for-automatic-failover-with-mysql-5-6-gtid-replication/

https://github.com/box/Anemometer

https://www.slideshare.net/hoterran/mysqlproxy

[功能測試] 測試場景一、手工操作，手動完成，failover
[功能測試] 測試場景二、手工操作, 自動完成，failover
[功能測試] 測試場景三、手工操作，自動完成，online master switch
[功能測試] 測試場景四、自動監控，自動操作，自動完成，failover
[用例測試] MySQL master 服務 down掉，是否成功自動failover
[用例測試] MySQL master too many connection，無權限，響應慢，是否自動failover
[用例測試] MySQL master 服務器 down掉，且候選master落後的最多， 是否自動failover，知否可以成功的做日誌補償
[用例測試] MySQL slave 服務 down掉，是否自動failover
[用例測試] MySQL 有一臺slave，或者多臺slave服務器延遲很大，是否自動failover
[用例測試] MySQL slave IO/SQL線程 stop，是否自動failover
[用例測試] MySQL slave IO/SQL線程 報錯，是否自動failover
[用例測試] MySQL master 有大事務超過100s再執行，是否可以online master switch
[用例測試] MySQL master 網絡斷掉，是否自動failover
[用例測試] MySQL master 網路瞬斷（1~30秒），是否自動failover
[用例測試] MySQL master 和 候選master 網絡都掛掉的情況，是否自動failover
[用例測試] MHA manager 和 MySQL master之間的網絡斷掉,但是master和slave之間的網絡是好的，是否自動failover
[用例測試] MHA manager 和 MySQL master,slave 網絡都斷掉的情況，是否自動failover
[用例測試] GTID模式下，還需要relay-log嗎？是否能夠成功的補齊日誌
[用例測試] 多線程複製模式下，做failover 和 online master switch，會不會有問題呢？
[用例測試] 在一開始沒有開啟MHA的group中，如何做到日誌補償，然後change master呢？


HealthCheck.pm
（1）從宕機崩潰的master保存二進制日誌事件（binlog events）;
（2）識別含有最新更新的slave；
（3）應用差異的中繼日誌（relay log）到其他的slave；
（4）應用從master保存的二進制日誌事件（binlog events）；
（5）提升一個slave為新的master；
（6）使其他的slave連接新的master進行復制；


通過現有數據搭建備庫。

----- 1. 備份主庫數據
$ mysqldump --master-data=2 --single-transaction -R --triggers -A > all.sql
參數含義：
  --master-data=2
    備份時記錄master的Binlog位置和Position；
  --single-transaction
    備份時為一個事務，獲取一致性快照；
  -R
    備份存儲過程和函數；
  --triggres
    同時在文件末尾生成CHANGE MASTER命令；
  -A
    備份所有的庫。


備庫設置為只讀
SET GLOBAL read_only=1;


MHA在發生切換的過程中，從庫的恢復過程中依賴於relay log的相關信息，所以這裡要將relay log的自動清除設置為OFF，採用手動清除relay log的方式。在默認情況下，從服務器上的中繼日誌會在SQL線程執行完畢後被自動刪除。但是在MHA環境中，這些中繼日誌在恢復其他從服務器時可能會被用到，因此需要禁用中繼日誌的自動刪除功能。定期清除中繼日誌需要考慮到複製延時的問題。在ext3的文件系統下，刪除大的文件需要一定的時間，會導致嚴重的複製延時。為了避免複製延時，需要暫時為中繼日誌創建硬鏈接，因為在linux系統中通過硬鏈接刪除大文件速度會很快。（在mysql數據庫中，刪除大表時，通常也採用建立硬鏈接的方式）


https://docs.oracle.com/cd/E17952_01/mysql-utilities-1.4-en/mysql-utilities-1.4-en.pdf


http://keithlan.github.io/2016/08/05/mha_practice/

http://keithlan.github.io/2016/08/18/mha_source/

http://www.cnblogs.com/gomysql/p/3675429.html

https://huoding.com/2011/12/18/139

-->

{% highlight text %}
{% endhighlight %}
