---
title: MySQL 高可用 MHA
layout: post
comments: true
language: chinese
category: [mysql,database]
keywords: mysql,高可用,mha
description: Master High Availability, MHA 在 MySQL 高可用方面是一个相对成熟的解决方案，故障切换过程中，能做到在 0~30 秒之内自动完成数据库的故障切换操作，而且能最大程度上保证数据的一致性。
---

Master High Availability, MHA 在 MySQL 高可用方面是一个相对成熟的解决方案，能做到在 0~30 秒之内自动完成数据库的故障切换操作，而且能最大程度上保证数据的一致性。

这里介绍使用配置方法，以及其原理。

<!-- more -->

## 简介

该软件由管理节点 (Manager) 和数据节点 (Node) 组成，其中 Manager 可以单独部署在一台机器上，并管理多个 Master-Slave 集群，当然，也可以部署在一台备节点上。而 Node 节点需要在每个服务器上都部署。

Node 运行在每台 MySQL 服务器上，Manager 会定时探测集群中的主节点，当主出现故障时，可以自动将最新的备提升为新的主，然后将所有其它的备重新指向新的主。


### 使用条件

1. MHA 是为了解决数据一致性，主要支持一主多从，一般一个集群中最少有三台数据库服务器，也就是一主二从；从 0.52 开始，就开始支持多主复制架构了，但是多主只允许单点写入，默认只支持 2 层复制架构。
2. 候选 Master 必须开启 log-bin ，如果备库没有开启 log-bin 则不会被提升为主，如果所有备库未开启则报错。
3. 主备环境的 binlog、relay-log 必须一致，包括复制过滤规则 binlog-do-db、replicate-ignore-db 等等必须全部一致。
4. 复制用户必须在候选主上要存在，而且切换完后，所有备库都需要执行 ```change master``` 命令，因此在 new master 上复制用户必须有 ```REPLICATEION SLAVE``` 权限。简单来说，所有的节点都会创建复制用户及其权限。
5. SQL 线程默认在执行完 relay-log 后自动删除，不过这些 relaylog 可能还会使用，所以需要关闭自动删除，然后自己来删除，当然必须考虑 repl 延迟问题。
6. 不要使用 ```load data infile```，尤其是 SBR 环境中。

<!--
2. MySQL 版本必须大于等于 5.0
    尽量使用高版本的MySQL

3.4 三层或者多层复制环境

默认情况下，MHA是不支持3层或多层复制架构的（Master1 -> Master2 -> Slave3）
MHA可以恢复Master2，但是不能恢复Slave3，因为Master2，Slave3有不同的master
为了让MHA支持以上架构，可以参考如下配置：

    在配置文件中，只配置两层(master1 and master2)
    使用 “multi_tier_slave=1” 参数，然后设置所有hosts

3.6 使用mysqlbinlog 5.1+ 支持MySQL5.1+

    MHA使用mysqlbinlog来应用日志到目标slave上的
    如果MySQL master设置的是row格式，那么MySQL必须是大于等于5.1版本，因为5.0不支持row
    mysqlbinlog版本可以这样被检测：
[app@slave_host1]$  mysqlbinlog --version
mysqlbinlog Ver 3.3 for unknown-linux-gnu at x86_64

    如果你使用的是MySQL5.1，那么mysqlbinlog必须大于等于3.3
    如果mysqlbinlog的版本是3.2，而mysql的版本是5.1，那么mha manager会报错，且停止monitoring
-->


### 安装

该软件实际上是使用 Perl 编写的，可以直接从 [Github Manager](https://github.com/yoshinorim/mha4mysql-manager) 和 [Github Node](https://github.com/yoshinorim/mha4mysql-node) 下载即可。如下是关于这两个包中各个文件的介绍。

{% highlight text %}
Manager:
   masterha_check_repl        检查MySQL复制状况
   masterha_check_ssh         检查MHA的SSH配置状况
   masterha_check_status      检测当前MHA运行状态
   masterha_manager           启动MHA监控进程，执行一次切换之后会退出
   masterha_master_monitor    检测master是否宕机
   masterha_master_switch     控制故障转移，可以自动或者手动
   masterha_conf_host         添加或删除配置的server信息
   masterha_stop              停止manager服务
   masterha_secondary_check   第二次检查服务器，防止出现脑裂现象

Node: 一般由Manager脚本触发，无需手动操作
   save_binary_logs           保存和复制宕掉主库的bilog
   apply_diff_relay_logs      识别差异的中继日志事件并将其差异的事件应用于其他的slave
   filter_mysqlbinlog         去除不必要的ROLLBACK事件（MHA已不再使用这个工具）
   purge_relay_logs           清除中继日志（不会阻塞SQL线程）
{% endhighlight %}

源码安装时，需要安装如下依赖包；接下来看看如何源码安装。

{% highlight text %}
----- 1. 安装依赖库
# yum install perl-Module-Install perl-Module-Build  # 源码安装编译
# yum install perl-DBD-MySQL                         # MySQL驱动
# yum install perl-Config-Tiny
# yum install --enablerepo=epel perl-Log-Dispatch perl-Parallel-ForkManager

----- 2. 源码安装Node
$ perl Makefile.pl
$ make
# make install

----- 3. 源码安装Manager
$ perl Makefile.pl
$ make
# make install
# cp samples/conf/masterha_default.cnf /etc/
{% endhighlight %}

### 配置文件

在 samples/conf 目录下存在示例配置文件，各个配置项简单列举如下。

{% highlight text %}
$ cat << EOF > /tmp/mysql-mha.cnf
[server default]
manager_workdir=/tmp/mha-master/foobar          # Manager工作目录
manager_log=/tmp/mha-master/foobar/manager.log  # Manager日志
user=root                                       # 设置监控时使用的用户名
password=new-password                           # 及其密码
repl_user=mysync                                # 复制时使用的用户名
repl_password=kidding                           # 复制用户的密码
ssh_user=root                                   # ssh的登录用户名
master_binlog_dir=/tmp/mysql-master             # 主库binlog的保存目录
log_level=debug                                 # 设置日志打印级别，包括debug/info/warning/error
ping_interval=1                                 # 向主库发送ping的时间间隔，默认3秒，三次失败则自动failover
remote_workdir=/tmp/mha                         # 远端mysql在发生切换时binlog的保存位置

# Failover时执行的脚本，如果使用了keepalived类似工具，可以配置切换VIP，详见force_shutdown_internal()函数的实现
#master_ip_failover_script= /usr/local/bin/master_ip_failover
# Failover时执行的脚本，可用来关闭MySQL服务或者服务器，防止发生脑裂，详见force_shutdown_internal()函数的实现
shutdown_script = ""

master_ip_online_change_script= /usr/local/bin/master_ip_online_change  //设置手动切换时候的切换脚本
report_script=/usr/local/send_report    //设置发生切换后发送的报警的脚本
secondary_check_script= /usr/local/bin/masterha_secondary_check -s server03 -s server02

[server1]
hostname=127.0.0.1
port=3307

[server2]
hostname=127.0.0.1
port=3308
candidate_master=1      # 候选主库，即使不是最新的，仍会将该库提升为主库
check_repl_delay=0      # 默认会检查relaylog的延迟，超过100M时不会将其提升为主，在此设置为忽略

[server3]
hostname=127.0.0.1
port=3309
EOF
{% endhighlight %}

<!--
二次检测： 检查master是否真的挂了，避免脑裂   secondary_network_script
* master_ip_failover: 默认的是空，什么都不做
    ip漂移
    新master赋予写
    日志保留
    报警
等等
-->

### 简单测试

如果主库宕了之后，MHA 在完成切换之后会自动停止，所以最好配合 deamontools 工具，在进程停止之后可以自动拉起。切换过程可以查看日志 [mha-manager.log](/reference/databases/mysql/mha-manager.log) 。

接下来，一步步测试。

##### 1. 设置免登陆

可以通过如下命令查看配置是否正常。

{% highlight text %}
$ masterha_check_ssh --conf=/tmp/mysql-mha.cnf
All SSH connection tests passed successfully.
{% endhighlight %}

<!--
如果slave比较多，实例比较多，最好提高下 /etc/ssh/sshd_config MaxStartups 的值（默认是10）
--conf=/tmp/mysql-mha.cnf
-->

##### 2. 检查复制配置

{% highlight text %}
$ masterha_check_repl --conf=/tmp/mysql-mha.cnf
{% endhighlight %}

##### 3. 开启Manager

{% highlight text %}
$ nohup masterha_manager --conf=/tmp/mysql-mha.cnf < /dev/null > /tmp/mha-manager.log 2>&1 &

常用参数：
  --check_only
    只检查状态是否正常。
{% endhighlight %}

<!--
nohup masterha_manager --conf=/usr/local/mha/mha.cnf --ignore_last_failover < /dev/null > /home/data/mha/manager.log 2>&1 &
-->

##### 4. 检查Manager的状态

{% highlight text %}
$ masterha_check_status --conf=/tmp/mysql-mha.cnf
mysql-mha (pid:17446) is running(0:PING_OK), master:127.0.0.1
{% endhighlight %}

##### 5. 测试关闭Manager

{% highlight text %}
$ masterha_stop --conf=/tmp/mysql-mha.cnf
Stopped mysql-mha successfully.
{% endhighlight %}

注意：binlog-do-db 和 replicate-ignore-db 设置必须相同。 MHA 在启动时候会检测过滤规则，如果过滤规则不同，MHA 不启动监控和故障转移。


## 主备切换

当主库故障时，如果没有自动切换，那么就只能等待手动切换，当然，时间就不确定了。自动切换就需要找到一个合适的备库作为主库，并将主库指向新主库，并重新启动复制。

### 故障场景

当主库故障时，会有如下的几种情况，简单介绍下。

#### 全部同步

![mysql mha all slaves sync]({{ site.url }}/images/databases/mysql/mha-all-slaves-sync.png "mysql mha all slaves sync"){: .pull-center }

所有备库都从主库收到了 binlog 事件，那么此时，任何一个备库都可以选为新的主库，只需要执行如下命令即可。

{% highlight text %}
mysql> CHANGE MASTER TO MASTER_HOST='slave1', MASTER_LOG_FILE='file1', MASTER_LOG_POS=pos1;
mysql> START SLAVE;
{% endhighlight %}

当然，这是最简单的一种方式，大多数情况下不会这么幸运。


#### 备库相同，部分事件丢失

![mysql mha all slaves receive same events]({{ site.url }}/images/databases/mysql/mha-all-slaves-same.png "mysql mha all slaves receive same events"){: .pull-center }

也就是所有的备库都从主库收到了相同的 binlog 事件，但主库部分事件没有发送成功，如果直接切换过去，那么上述的 ```id=17``` 事件就会丢失了。

如果崩溃的主库可以通过 SSH 访问，而且能够读取 binlog 文件，那么在提升一个主库前，应该将 17 对应的事件保存到 binlog 文件中。

当然，可以通过 semi-sync 减小这种情况发生的概率。

#### 某个备库事件完整

![mysql mha some slave sync]({{ site.url }}/images/databases/mysql/mha-some-slave-sync.png "mysql mha some slave sync"){: .pull-center }

某个备库收到了其它备库未收到的事件，需要从中选出一个备库，并将事件同步到其它备库，从而使所有的备库保持一致。

现在的问题是：1) 如何确认那些 binlog 事件未发送？ 2) 如何使所有的备库保持最终的一致？

#### 备库事件不一致，最新备库有事件丢失

![mysql mha some slaves events lost]({{ site.url }}/images/databases/mysql/mha-some-slaves-events-lost.png "mysql mha some slaves events lost"){: .pull-center }

如果采用异步复制，那么估计这是最常见的故障场景，也就是备库收到的事件数不同，其中的某个库收到的事件比较多，但是仍有部分主库的事件丢失。

如上图所示，备库收到的事件有所不同，Slave2 是最新的备库，Slave1 和 Slave3 备库丢失更多事件，而且 ```id=17``` 对应的事件所有备库都没有收到。

接下来需要：1) 如果可能需要将 ```id=17``` 对应的事件从主库复制到备库；2) 将所有备库的事件补齐，防止备库发生数据不一致性。


### 主备复制状态

可以通过 ```SHOW SLAVE STATUS``` 命令查看当前主备的复制状态，如下简单列举常见状态。

{% highlight text %}
mysql> SHOW SLAVE STATUS\G
... ...
      Master_Log_File: mysql-bin.000001   # IO线程正在读取的主库binlog文件名称
  Read_Master_Log_Pos: 107                # IO线程已读取主库binlog位置
       Relay_Log_File: relay-bin.000002   # SQL线程正在读取执行的relaylog名称
        Relay_Log_Pos: 253                # SQL线程已读取执行的relaylog位置
Relay_Master_Log_File: mysql-bin.000001   # SQL线程执行的主库binlog文件名称
  Exec_Master_Log_Pos: 107                # SQL线程执行来自主库的binlog最后一个事件位置
... ...
{% endhighlight %}

其中一些常见状态查看方式如下：

1. 备库同步是否正常。查看 ```Slave_IO_Running```、```Slave_SQL_Running``` 是否正常(Yes)，不正常则可以查看报错；
2. 从库是否有事件在 relaylog 中等待执行。查看 IO 线程从主库的读取位置 ```Read_Master_Log_Pos``` 和从库的执行位置 ```Exec_Master_Log_Pos``` 是否一致；
3. 判断备库落后主库多长时间，可以查看 ```Seconds_Behind_Master``` 的值。

其中，```SHOW SLAVE STATUS``` 命令的源码执行过程如下。

{% highlight text %}
show_slave_status_cmd()
 |-show_slave_status()
   |-show_slave_status_metadata()    发送元数据
   |-show_slave_status_send_data()   真正的监控数据
{% endhighlight %}

在 show_slave_status_send_data() 函数中的相关内容如下。


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

### 主备延迟

一般都是通过 ```SHOW SLAVE STATUS``` 命令查看如下的状态。

{% highlight text %}
mysql> SHOW SLAVE STATUS\G
... ...
    Seconds_Behind_Master: 0  # 备库延迟时间，单位为秒
{% endhighlight %}

<!--
http://michaelkang.blog.51cto.com/1553154/1201604

http://imysql.com/2014/08/30/mysql-faq-howto-monitor-slave-lag.shtml
-->

## 源码解析

{% highlight text %}
masterha_manger
 |-MHA::MasterMonitor::main()                        监控是否有异常
 | |-wait_until_master_is_dead()
 |   |-wait_until_master_is_unreachable()
 |     | ###<>###Phase 1: Configuration Check Phase
 |     |-MHA::ManagerUtil::check_node_version()      检查Manager+Node的版本，要求Node版本>=Manager版本
 |     | |-get_node_version() 通过SSH命令获取Node版本，可以直接查看该函数
 |     |-MHA::NodeUtil::check_manager_version()
 |     |-MHA::ServerManager::connect_all_and_read_server_status() 确认各个Node节点是否可以连接MySQL服务
 |     |-MHA::ServerManager::get_dead_servers() 再次检测node节点的状态
 |     |-MHA::ServerManager::get_alive_servers()
 |     |-MHA::ServerManager::get_alive_slaves()
 |     |-MHA::ServerManager::print_dead_servers()
 |     |
 |     |-MHA::HealthCheck::wait_until_unreachable()
 |
 |-MHA::MasterFailover::main()                         开始执行切换
   |-init_config()                                     初始化配置
   |-do_master_failover()                              真正执行Failover切换操作
   | |
   | | ###<>###Phase 1: Configuration Check Phase
   | |-MHA::ServerManager::init_binlog_server()        检查每个MySQL服务器，保证SSH+Node版本号
   | | |-MHA::HealthCheck::ssh_check_simple()          检查SSH是否正常
   | | | |-ssh_check()
   | | |-MHA::ManagerUtil::get_node_version()          同时检查Node的版本号
   | |-check_settings()
   | | |-MHA::ManagerUtil::check_node_version()
   | | |-connect_all_and_read_server_status()
   | |   |-run_on_start()
   | |   |-run_on_finish()
   | |
   | | ###<>###Phase 2: Dead Master Shutdown Phase
   | |-force_shutdown()
   | | |-stop_io_thread()                              停止所有备库的IO-thread
   | | |-force_shutdown_internal()                     执行master_ip_failover_script(例如切换VIP)以及
   | | |                                                   shutdown_script(如关闭mysqld进程)指定的脚本
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
   | |-save_master_binlog()                            如果非GTID，会判断是否有数据丢失
   | | |-MHA::ManagerUtil::check_node_version()        检查宕机主库的版本信息
   | | |-save_master_binlog_internal()                 保存binlog
   | |   |-MHA::ManagerUtil::exec_ssh_cmd()            调用Node的save_binary_logs脚本
   | |   |-MHA::NodeUtil::file_copy()                  从宕掉的库复制差异数据
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
   | | | | |-generate_diff_from_readpos()              执行Node节点上的apply_diff_relay_logs脚本
   | | | |-send_binlog()
   | | | | ###<>###Phase 3.4: Master Log Apply Phase
   | | | |-recover_slave()                             将获取的binlog差值恢复到新主库
   | | | | |-apply_diff()                              执行Node节点上的apply_diff_relay_logs脚本
   | | | |   |-wait_until_relay_log_applied()
   | | | |   |-stop_sql_thread()
   | | | |-get_new_master_binlog_position()
   | | |
   | | |-MHA::ManagerUtil::exec_system()               执行master_ip_failover_script脚本
   | |
   | | ###<>###Phase 4: Slaves Recovery Phase
   | |-recover_slaves()
   | | |-is_gtid_auto_pos_enabled()                判断是否开启GTID
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

通过SHOW SLAVE STATUS中的Master_Log_File、Read_Master_Log_Pos确认那个是最新的SLAVE，注意不能依赖relay log信息，因为不同的备库信息不同。

Applying diffs to other slaves(不同备库的Gap不同)
-->


## 参考

关于 MHA 的介绍，可以参考 [Github MHA-Wiki](https://github.com/yoshinorim/mha4mysql-manager/wiki)，另外，还可以查看 [Automated, Non-Stop MySQL Operations and Failover](/reference/databases/mysql/Automated_Non-Stop_MySQL_Operations_and_Failover_Presentation.pdf) 。


<!--
其它

https://www.slideshare.net/matsunobu/myrocks-deep-dive

http://files.cnblogs.com/files/jimingsong/mha-mysql.pdf

MySQL的很多资源

https://huoding.com/2011/04/05/59

http://code.openark.org/forge/openark-kit



https://www.percona.com/blog/2014/07/03/failover-mysql-utilities-part-2-mysqlfailover/

https://scriptingmysql.wordpress.com/2012/12/06/using-the-mysql-script-mysqlfailover-for-automatic-failover-with-mysql-5-6-gtid-replication/

https://github.com/box/Anemometer

https://www.slideshare.net/hoterran/mysqlproxy

[功能测试] 测试场景一、手工操作，手动完成，failover
[功能测试] 测试场景二、手工操作, 自动完成，failover
[功能测试] 测试场景三、手工操作，自动完成，online master switch
[功能测试] 测试场景四、自动监控，自动操作，自动完成，failover
[用例测试] MySQL master 服务 down掉，是否成功自动failover
[用例测试] MySQL master too many connection，无权限，响应慢，是否自动failover
[用例测试] MySQL master 服务器 down掉，且候选master落后的最多， 是否自动failover，知否可以成功的做日志补偿
[用例测试] MySQL slave 服务 down掉，是否自动failover
[用例测试] MySQL 有一台slave，或者多台slave服务器延迟很大，是否自动failover
[用例测试] MySQL slave IO/SQL线程 stop，是否自动failover
[用例测试] MySQL slave IO/SQL线程 报错，是否自动failover
[用例测试] MySQL master 有大事务超过100s再执行，是否可以online master switch
[用例测试] MySQL master 网络断掉，是否自动failover
[用例测试] MySQL master 网路瞬断（1~30秒），是否自动failover
[用例测试] MySQL master 和 候选master 网络都挂掉的情况，是否自动failover
[用例测试] MHA manager 和 MySQL master之间的网络断掉,但是master和slave之间的网络是好的，是否自动failover
[用例测试] MHA manager 和 MySQL master,slave 网络都断掉的情况，是否自动failover
[用例测试] GTID模式下，还需要relay-log吗？是否能够成功的补齐日志
[用例测试] 多线程复制模式下，做failover 和 online master switch，会不会有问题呢？
[用例测试] 在一开始没有开启MHA的group中，如何做到日志补偿，然后change master呢？


HealthCheck.pm
（1）从宕机崩溃的master保存二进制日志事件（binlog events）;
（2）识别含有最新更新的slave；
（3）应用差异的中继日志（relay log）到其他的slave；
（4）应用从master保存的二进制日志事件（binlog events）；
（5）提升一个slave为新的master；
（6）使其他的slave连接新的master进行复制；


通过现有数据搭建备库。

----- 1. 备份主库数据
$ mysqldump --master-data=2 --single-transaction -R --triggers -A > all.sql
参数含义：
  --master-data=2
    备份时记录master的Binlog位置和Position；
  --single-transaction
    备份时为一个事务，获取一致性快照；
  -R
    备份存储过程和函数；
  --triggres
    同时在文件末尾生成CHANGE MASTER命令；
  -A
    备份所有的库。


备库设置为只读
SET GLOBAL read_only=1;


MHA在发生切换的过程中，从库的恢复过程中依赖于relay log的相关信息，所以这里要将relay log的自动清除设置为OFF，采用手动清除relay log的方式。在默认情况下，从服务器上的中继日志会在SQL线程执行完毕后被自动删除。但是在MHA环境中，这些中继日志在恢复其他从服务器时可能会被用到，因此需要禁用中继日志的自动删除功能。定期清除中继日志需要考虑到复制延时的问题。在ext3的文件系统下，删除大的文件需要一定的时间，会导致严重的复制延时。为了避免复制延时，需要暂时为中继日志创建硬链接，因为在linux系统中通过硬链接删除大文件速度会很快。（在mysql数据库中，删除大表时，通常也采用建立硬链接的方式）


https://docs.oracle.com/cd/E17952_01/mysql-utilities-1.4-en/mysql-utilities-1.4-en.pdf


http://keithlan.github.io/2016/08/05/mha_practice/

http://keithlan.github.io/2016/08/18/mha_source/

http://www.cnblogs.com/gomysql/p/3675429.html

https://huoding.com/2011/12/18/139

-->

{% highlight text %}
{% endhighlight %}
