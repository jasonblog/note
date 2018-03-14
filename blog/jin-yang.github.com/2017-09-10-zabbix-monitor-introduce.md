---
title: Zabbix 監控系統
layout: post
comments: true
language: chinese
category: [misc]
keywords: zabbit,monitor,introduce,監控,簡介
description:
---

Zabbix 能監視各種網絡、系統參數，保證服務器系統的安全運行，同時提供了靈活的 API 接口，例如通知機制，可以讓系統管理員及時通過或者當前的異常狀態，支持 Agentd、SNMP、JMX 等多種監控方式。

同時，提供了一個 WEB 界面，可以作為分佈式系統監控以及網絡監控的企業級開源解決方案。

<!-- more -->

![zabbix logo]({{ site.url }}/images/linux/zabbix-logo.png "zabbix logo"){: .pull-center }



<!--
usage: zabbix_sender [-Vhv] {[-zpsI] -ko | [-zpI] -T -i <file> -r} [-c <file>]

  -c --config <file>                   配置文件絕對路徑
  -z --zabbix-server <server>          zabbix server的IP地址
  -p --port <server port>              zabbix server端口，默認10051
  -s --host <hostname>                 主機名，需要保證與agent配置文件內容保持一致
  -k --key <key>                       監控項的key
  -o --value <key value>               key值
  
  -I --source-address <IP address>     源IP
  -i --input-file <input file>         從文件裡面讀取hostname、key、value 一行為一條數據，使用空格作為分隔符，如果主機名帶空格，那麼請使用雙引號包起來
  -T --with-timestamps              一行一條數據，空格作為分隔符: <hostname> <key> <timestamp> <value>，配合 --input-file option，timestamp為unix時間戳
  -r --real-time                      將數據實時提交給服務器
  -v --verbose                         詳細模式, -vv 更詳細

zabbix_sender -vv -c /etc/zabbix/zabbix_agentd.conf -z 127.0.0.1 -k 'foo.bar' -o 12

 ./zabbix_sender -s 127.0.0.1 -z 127.0.0.1 -k "ttlsa.trapper" -o 1 -r 

/etc/zabbix/zabbix_agentd.conf


https://www.zabbix.com/download
https://repo.zabbix.com/zabbix/3.0/rhel/7/SRPMS/
https://sourceforge.net/projects/zabbix/files/?source=navbar
-->

## 安裝

安裝詳細可以參考 [zabbix manual install](https://www.zabbix.com/documentation/3.0/manual/installation)，二進制代碼庫下載地址可以參考 [zabbix download](https://www.zabbix.com/download)，也可以參考 [zabbix repos](http://repo.zabbix.com/zabbix/3.0/rhel/7/x86_64/) 。



#### 源碼編譯

{% highlight text %}
$ ./configure --prefix=/usr/local/zabbix                     \
    --enable-server --enable-agent                           \
    --with-mysql --with-net-snmp --with-libcurl
{% endhighlight %}

### 服務端安裝配置

這裡同樣以 MySQL 作為後端存儲為例。

{% highlight text %}
----- 1. 安裝依賴包，只列舉的部分
# yum install fping OpenIPMI-libs iksemel php-mysql httpd php

----- 2. 安裝服務和前端，需要安裝一堆的依賴
# rpm -ivh zabbix-server-mysql-3.0.5-1.el7.x86_64.rpm
# rpm -ivh zabbix-web-mysql-3.0.5-1.el7.noarch.rpm

----- 3. 配置MySQL數據庫，並創建表
shell> mysql -uroot -p<root_password>
mysql> create database zabbix character set utf8 collate utf8_bin;
mysql> grant all privileges on zabbix.* to zabbix@localhost identified by 'Huawei@123';
mysql> quit;
shell> zcat /usr/share/doc/zabbix-server-mysql-3.0.*/create.sql.gz | mysql -uzabbix -p zabbix

----- 4. 服務端配置
# vi /etc/zabbix/zabbix_server.conf
DBHost=localhost
DBName=zabbix
DBUser=zabbix
DBPassword=Huawei@123

----- 5. 啟動服務
# systemctl start zabbix-server

----- 6. 設置時區為某個地區
# vim /etc/php.ini
date.timezone = Asia/Shanghai
{% endhighlight %}

其中服務器使用 httpd，此時需要修改配置文件 `/etc/httpd/conf.d/zabbix.conf` 。 <!-- php配置文件，保存在mod_php5.c中 -->

接著需要配置界面，配置完成之後會保存在 `/etc/zabbix/web/zabbix.conf.php` 文件中，默認的登陸帳號可以使用 `Admin/zabbix` 。

### 客戶端安裝

這裡同樣以 RPM 包安裝為例。

{% highlight text %}
----- 1. 安裝客戶端
# rpm -ivh zabbix-agent-3.0.5-1.el7.x86_64.rpm

----- 2. 啟動客戶端
# systemctl start zabbix-agent

----- 3. 可以查看其啟動日誌內容
# less /var/log/zabbix/zabbix_agentd.log

----- 4. 修改配置文件
# vim /etc/zabbix/zabbix_agentd.conf
Server=127.0.0.1           被動服務IP，允許這個IP來我這裡取數據
ServerActive=127.0.0.1     主動提交數據給這個IP
Hostname=10.120.185.240    需要保證和Zabbix Web中的配置保持相同
{% endhighlight %}

## 特性

### 監控項 Items

也就是添加一個主機監控項，`Configration->Hosts->Items` ，一般會從模板中繼承一部分內容。

一般 item 通過 Key 定義，然後其中包括了具體的參數，通常是兩個，例如 `system.cpu.load[percpu,avg1]`；在 agent 端包括了很多常用的 key，可以參考 [Zabbix agent](https://www.zabbix.com/documentation/3.0/manual/config/items/itemtypes/zabbix_agent)。

對於自定義的 key 需要在客戶端的配置文件 `zabbix_agentd.conf` 中啟用 `UnsafeUserParameters=1` 參數，然後在配置文件的最下面來定義 key，如：

{% highlight text %}
UnsafeUserParameters=1       # 並去掉前面的註釋符
UserParameter=foo.bar[*], /usr/local/script/monitor.sh $1 $2
{% endhighlight %}

其中，`foo.bar[*]` 定義了 key ，而後面對應了監控程序的絕對路徑，通常使用的是通用腳本，然後根據具體的參數決定輸出內容，通常參數順序為 `$1~$9` 。

### 觸發器 Triggers

一般用於告警，當 Items 監控項採集值滿足 triggers 指定的觸發條件時，就會產生 actions 。

同樣在 `Configuration->Hosts->Triggers` 中進行配置，在修改時有幾條規則：1) template 中的觸發值不能單獨修改，必須在 template 中修改；2) 可以複製一個同樣 trigger 再修改，再禁用模板中的 trigger；3) 也可以通過右上角的 Create trigger 創建一個。

關於 Trigger 的表達式語法信息可以查看 [Zabbix Trigger Expression](https://www.zabbix.com/documentation/3.0/manual/config/triggers/expression) 。

常見示例：

{% highlight text %}
---- 主機宕機檢測，持續3min沒有ping數據時報警
{test-01:agent.ping.nodata(3m)}=1

---- 連續3分鐘的平均值大於80%，或者用max表示一直大於80%
{test-01:system.cpu.util[,idle].avg(3m)}<20
{% endhighlight %}

<!--
手冊裡邊有個"TRIGGER.VALUE"宏，看來得從這裡下下手.

TRIGGER.VALUE對應的為Trigger狀態，0代表OK, 1代表Problem，分解下需求:

正常情況下連續三分鐘CPU使用率超過80%，看起來表達式是:

{TRIGGER.VALUE}=0&{test-01:system.cpu.util[,idle].max(3m)}<20
故障時連續三分鐘CPU使用率低於50%恢復正常，即故障時刻CPU使用率持續三分鐘高於50%依然為故障，表達式是這個樣子的:

{TRIGGER.VALUE}=1&{test-01:system.cpu.util[,idle].min(3m)}<50
然後整合下表達式，就成了下邊這個樣子:

({TRIGGER.VALUE}=0&{test-01:system.cpu.util[,idle].max(3m)}<20) | ({TRIGGER.VALUE}=1&{test-01:system.cpu.util[,idle].min(3m)}<50)
-->


### 宏定義 Macro

包括了系統內置的宏，一般格式為 `{MACRO}`，可以通過類似 `{HOST.NAME}` 的方式進行引用，例如告警中，詳細可以參考 [Macros supported by location](https://www.zabbix.com/documentation/3.0/manual/appendix/macros/supported_by_location) 。

自定義宏通過 `{$MACRO}` 類似方式表示，也就是需要加 `$` 才能引用；*注意* 在調用和定義時都需要加 `$` ，而且 zabbix 還支持在全局、模板或主機級別使用用戶自定義宏。

解析時的順序，首先解析主機，然後按照模板 ID 的順序依次解析宏定義，最後會解析全局的配置，如果仍然無法找到，那麼就不會解析。

常見的使用場景有：

1. `item key` 參數，如 `item.key[server_{HOST.HOST}_local]` 。
2. `trigger` 表達式中。
3. `Low-Level-Discovery` 中。

另外，需要注意的是，宏的名稱只能使用 `A-Z, 0-9, _, .`，可以通過如下方式配置。

{% highlight text %}
全局宏定義：
    Administrator-->General-->Macros(右上角選擇框) 保存在globalmacro表中
主機/模板宏定義，兩者基本類似：
    Configuration-->[Hosts|Templates]-->Macros 保存在hosts以及hostmacro表中
{% endhighlight %}

### 圖表展示 Graphs/Screens

Graphs 可用於將多個監控項展示在同一個圖表中，然後一個 Screens 中可以用來展示多個 Graphs 。

### 自動發現

隨著被監控的主機不斷增加，如果純手工配置，那麼工作量將會非常大，這時就可以使用 `Discovery rules` 來配置。通過該模塊，可以實現自動發現主機、自動將主機添加到主機組、自動加載模板、自動創建監控項、自動創建圖像。

<!--
http://club.oneapm.com/t/zabbix-discovery/515
http://www.ttlsa.com/zabbix/zabbix-low-level-discover/

LLD
1. Configration->Template 新建一個模板
--->


<!--
## 其它

https://www.zabbix.org/wiki/Docs/howto/mysql_partition
http://www.ttlsa.com/zabbix/use-wechat-send-zabbix-msg/
http://www.ttlsa.com/news/google-proxy-latest-2015-08/

select name, key_ from items where key_ like 'system.cpu.load%';

hosts/groups/hosts_groups   用於維護主機和分組的關係。
    hosts        維護主機信息。
 groups       維護分組信息。
    hosts_groups 用於找到組中的主機，以及通過主機ID找到對應的分組。
items/graphs/graphs_items   採集項與展示圖之間的關係。


item_condition macro

timer_thread()     main_timer_loop時間相關的處理
 |-process_time_functions()
 | |-DCconfig_get_time_based_triggers() 從緩存中獲取trigger表達式
 | |-evaluate_expressions()       trigger expression的主要處理函數
     |-substitute_simple_macros() 宏分為兩類，分別是{...} {$...}
  |-substitute_functions()
       |-zbx_evaluate_item_functions()
         |-evaluate_function()
 | |-process_triggers()
 | | |-process_trigger()
 | |   |-add_event()              會保存到內存的events數組中
 | |-process_events()
 |   |-save_events()
 |   |-process_actions()
 |   |-clean_events()
 |-process_maintenance()

substitute_functions()

可以看到一堆的以 zbx_vector_ 開頭的函數，其函數聲明在 `include/zbxalgo.h` 文件中，而對應的代碼實現在 `src/libs/zbxalgo/vectorimpl.h` 文件中。

parse_commandline()   會根據命令行傳入的參數設置t.task，一般默認為ZBX_TASK_START
init_metrics()  初始化內置的監控指標
  |-zbx_load_config()  默認會執行switch的default分支，加載配置
  |-load_user_parameters()
  |-START_MAIN_ZABBIX_ENTRY()  一般是設置為daemon，然後設置信號處理句柄
    |-zbx_set_common_signal_handlers()
 |-MAIN_ZABBIX_ENTRY()
   |-load_modules()  沒有使用 <del>
   |-zbx_tcp_listen() 後臺不使用 <del>
   |-init_collector_data() 初始化採集item
   |-zbx_free_config()
   |-zbx_thread_start()     collector_thread
     |-collector_thread()   每秒採集一次數據，src/zabbix_agent/stats.c
    |-collect_stats_diskdevices() 保存數據到共享內存，通過semop加鎖
   |-zbx_thread_start()     listener_thread
     |-listener_thread()    開啟監聽端口，src/zabbix_agent/listener.c
       |-zbx_tcp_accept()   接收請求
    |-process_listener()
      |-zbx_tcp_recv_to()
   |-init_result()
   |-process()        src/libs/zbxsysinfo/sysinfo.c
   |-zbx_tcp_send_to()
   |-zbx_thread_start()     active_checks_thread
     |-active_checks_thread()
       |-init_active_metrics()

http://www.cnblogs.com/lixiaofei1987/p/3208414.html


## 其它

https://www.zabbix.org/wiki/Docs/howto/mysql_partition
http://www.ttlsa.com/zabbix/use-wechat-send-zabbix-msg/
http://www.ttlsa.com/news/google-proxy-latest-2015-08/

select name, key_ from items where key_ like 'system.cpu.load%';

hosts/groups/hosts_groups   用於維護主機和分組的關係。
    hosts        維護主機信息。
 groups       維護分組信息。
    hosts_groups 用於找到組中的主機，以及通過主機ID找到對應的分組。
items/graphs/graphs_items   採集項與展示圖之間的關係。


item_condition macro

timer_thread()     main_timer_loop時間相關的處理
 |-process_time_functions()
 | |-DCconfig_get_time_based_triggers() 從緩存中獲取trigger表達式
 | |-evaluate_expressions()       trigger expression的主要處理函數
     |-substitute_simple_macros() 宏分為兩類，分別是{...} {$...}
  |-substitute_functions()
       |-zbx_evaluate_item_functions()
         |-evaluate_function()
 | |-process_triggers()
 | | |-process_trigger()
 | |   |-add_event()              會保存到內存的events數組中
 | |-process_events()
 |   |-save_events()
 |   |-process_actions()
 |   |-clean_events()
 |-process_maintenance()

substitute_functions()

可以看到一堆的以 zbx_vector_ 開頭的函數，其函數聲明在 `include/zbxalgo.h` 文件中，而對應的代碼實現在 `src/libs/zbxalgo/vectorimpl.h` 文件中。

parse_commandline()   會根據命令行傳入的參數設置t.task，一般默認為ZBX_TASK_START
init_metrics()  初始化內置的監控指標
  |-zbx_load_config()  默認會執行switch的default分支，加載配置
  |-load_user_parameters()
  |-START_MAIN_ZABBIX_ENTRY()  一般是設置為daemon，然後設置信號處理句柄
    |-zbx_set_common_signal_handlers()
 |-MAIN_ZABBIX_ENTRY()
   |-load_modules()  沒有使用 <del>
   |-zbx_tcp_listen() 後臺不使用 <del>
   |-init_collector_data() 初始化採集item
   |-zbx_free_config()
   |-zbx_thread_start()     collector_thread
     |-collector_thread()   每秒採集一次數據，src/zabbix_agent/stats.c
    |-collect_stats_diskdevices() 保存數據到共享內存，通過semop加鎖
   |-zbx_thread_start()     listener_thread
     |-listener_thread()    開啟監聽端口，src/zabbix_agent/listener.c
       |-zbx_tcp_accept()   接收請求
    |-process_listener()
      |-zbx_tcp_recv_to()
   |-init_result()
   |-process()        src/libs/zbxsysinfo/sysinfo.c
   |-zbx_tcp_send_to()
   |-zbx_thread_start()     active_checks_thread
     |-active_checks_thread()
       |-init_active_metrics()

http://www.cnblogs.com/lixiaofei1987/p/3208414.html

### 添加主機

Configuration -> Hosts -> [Create Host]

1. Host 主要是Agent的接口配置。
2. Templates 選擇Template OS Linux作為模板。

## 常見問題

no active checks on server [SVRIP:10051]: host [CLI] not found

一般是 zabbix_agentd.conf 裡面的 Hostname 和前端 Zabbix web 裡面的配置 (Configuration->Hosts->[Host name]) 不一樣所造成的，修改完成後重啟 agent 即可。



json_reformat

zabbix配置內容比較多，我們要分為9大塊來講解。分別如下：
1. Host/HostGroup 主機與組。主要用於主機管理，通過主機組可以進行邏輯分組。
2. Item 監控項。主機監控的指標，例如服務器負載可以作為一個監控項。
3. Trigger 觸發器。


什麼情況下出發什麼事情，稱之為觸發器。例如：定義如果系統負載大於10那麼報警，這個東西可以稱之為觸發器。
4、事件
觸發器狀態變更、Discovery事件等
5、可視化配置
圖表配置，講監控的數據繪製成曲線圖。或者在一個屏幕中可以看到某臺主機所有監控圖表。
6、模板配置
自定義監控模板。例如Template OS Linux
7、報警配置
配置報警介質：郵箱、sms以及什麼情況下發送報警通知。
8、宏變量
用戶自定義變量，很有用的一個功能。



配置可以參考如下：
Host groups（主機組）→Hosts（主機）→Applications（監控項組）→Items（監控項）→Triggers（觸發器）→Event（事件）→Actions（處理動作）→User groups（用戶組）→Users（用戶）→Medias（告警方式）→Audit（日誌審計）
http://www.xuliangwei.com/xubusi/175.html
http://www.cnblogs.com/ningskyer/articles/5546894.html


https://xiaofandh12.github.io/Zabbix-Server
https://xiaofandh12.github.io/Zabbix-Server-poller
http://blog.csdn.net/liujian0616/article/details/7946492
http://blog.51cto.com/john88wang/1833636
https://www.ibm.com/developerworks/cn/linux/l-lexyac.html
-->

## 參考



{% highlight text %}
{% endhighlight %}
