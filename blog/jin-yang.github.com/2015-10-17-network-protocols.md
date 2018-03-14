---
title: Linux 通訊協議
layout: post
comments: true
language: chinese
category: [linux,misc]
keywords: linux,network,snmp
description: 簡單記錄下 Linux 常見的通訊協議，如 SNMP 協議。
---

簡單記錄下 Linux 常見的通訊協議，如 SNMP 協議。

<!-- more -->

## SNMP

簡單網絡管理協議 (Simple Network Management Protocol, SNMP) 是一種應用層協議，同樣也是 TCP/IP 協議族的一部分，用於網絡設備之間交換管理信息，所謂的管理也就主要是監控 (get) 和配置 (set) 一些對象。

這裡管理的對象通過對象標示符 (Object Identifiers, OID) 標示，不同的採集對象對應不同的 OID，例如系統磁盤的信息用 OID ```"1.3.6.1.4.1.2021.9"``` 標示。這串數字是國際標準化組織協商定義好的，通過管理信息庫 (MIB) 保存，可以將其簡單視為一個樹結構。

### 安裝測試

在 CentOS 中，可以通過如下方式安裝服務端以及常用命令。

{% highlight text %}
# yum install net-snmp net-snmp-utils
# systemctl start snmpd
{% endhighlight %}

很多人會使用 SNMP V3 的授權機制(V1+V2c不支持加密)，注意，SNMP V1 在性能以及支持的數據類型都比較受限；V2c 和 V3 的數據格式相同，只是 V3 提供了加密功能。

接下來簡單介紹下 snmpwalk 命令。

{% highlight text %}
命令參數：
  -c <string>
    指定Community String，類似於用戶名或者密碼，如果正確則響應，用於 V1+V2c 版本，
    V3 使用用戶名+密碼，一般默認會有一個只讀的 public ；
  -v (1|2c|3)
    指定SNMP的版本號；

----- 查看系統信息，返回結果包括了MIB信息，可以通過該信息指定獲取具體的內容
$ snmpwalk -v 2c -c public 127.1 system
$ snmpwalk -v 2c -c public 127.1 system.sysDescr.0
{% endhighlight %}

<!--
https://kb.op5.com/display/HOWTOs/Configure+a+Linux+server+for+SNMP+monitoring
https://www.ibm.com/developerworks/cn/linux/l-cn-snmp/
iReasoning MIB Browser 啟用SNMP的網絡設備。
-->






{% highlight text %}
{% endhighlight %}
