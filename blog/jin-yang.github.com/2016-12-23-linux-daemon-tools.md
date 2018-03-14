---
title: Linux 後臺服務管理
layout: post
comments: true
language: chinese
category: [linux,misc]
keywords: 服務管理,monit,supervisor,goreman
description: 在 Linux 中，對於一個後臺服務，如果要可靠地在後臺運行，最好能監控進程狀態，在意外結束時能自動重啟。常見的功能全面的主流工具主要有 monit(C)、supervisor(Python)、goreman(Go)，其中 goreman 是對 Ruby 下廣泛使用的 foreman 工具，使用 golang 的重寫。在此，僅介紹下在 Linux 中如何使用。
---

在 Linux 中，對於一個後臺服務，如果要可靠地在後臺運行，最好能監控進程狀態，在意外結束時能自動重啟。

常見的功能全面的主流工具主要有 monit(C)、supervisor(Python)、goreman(Go)，其中 goreman 是對 Ruby 下廣泛使用的 foreman 工具，使用 golang 的重寫。

在此，僅介紹下在 Linux 中如何使用。

<!-- more -->

## Monit

![monit logo]({{ site.url }}/images/linux/monit-logo.png "monit logo"){: .pull-center width="40%" }

[Monit](http://mmonit.com/monit/) 是一款功能非常豐富的進程、文件、目錄和設備的監測軟件，通過 C 語言開發，是一款輕量級的後臺管理服務，適用於 *nix 平臺。

它可以自動修復那些已經停止運作的程序，特使適合處理那些由於多種原因導致的軟件錯誤，同時 Monit 包含一個內嵌的 HTTP(S) Web 界面，你可以使用瀏覽器方便地查看 Monit 所監視的服務器。

在 CentOS 中，可以直接通過如下命令安裝。

{% highlight text %}
# yum --enablerepo=epel install monit
{% endhighlight %}

如果啟動時不使用 arguments 參數，則會直接作為服務進程啟動，否則嘗試鏈接 127.0.0.1:2812 並執行相應的命令；簡單列舉下 monit 啟動是相關的命令，以及參數。

{% highlight text %}
monit [options] {arguments}

常用參數：
  -I 前端執行，默認會自動切換為後臺執行；
  -t 對配置文件執行參數檢查；
  -c 指定配置文件；
  -g
  -i 查看monit的ID，第一次會自動生成並保存到~/.monit.id文件中；
  -r 重新生成ID；
  -H [file] 生成指定文件的SHA1和MD5校驗值；

常用參數：
  status [name]
    查看某個服務的狀態，如果不使用name，則會打印所有服務狀態；
  summary [name]
    打印概覽；
  quit
    退出服務；

常用命令：
  monit -I -c monitrc
  monit -c monitrc quit
  monit -c monitrc reload
{% endhighlight %}

<!--
      start all
           Start all services listed in the control file and enable monitoring for them. If the group option is set (-g), only start and enable monitoring of services in the named group ("all" is not
           required in this case).

       start name
           Start the named service and enable monitoring for it. The name is a service entry name from the monitrc file.

       stop all
           Stop all services listed in the control file and disable their monitoring. If the group option is set, only stop and disable monitoring of the services in the named group (all" is not required
           in this case).

       stop name
           Stop the named service and disable its monitoring. The name is a service entry name from the monitrc file.

       restart all
           Stop and start all services. If the group option is set, only restart the services in the named group ("all" is not required in this case).

       restart name
           Restart the named service. The name is a service entry name from the monitrc file.

       monitor all
           Enable monitoring of all services listed in the control file. If the group option is set, only start monitoring of services in the named group ("all" is not required in this case).

       monitor name
           Enable monitoring of the named service. The name is a service entry name from the monitrc file. Monit will also enable monitoring of all services this service depends on.

       unmonitor all
           Disable monitoring of all services listed in the control file. If the group option is set, only disable monitoring of services in the named group ("all" is not required in this case).

       unmonitor name
           Disable monitoring of the named service. The name is a service entry name from the monitrc file. Monit will also disable monitoring of all services that depends on this service.

       reload
           Reinitialize a running Monit daemon, the daemon will reread its configuration, close and reopen log files.

       validate
           Check all services listed in the control file. This action is also the default behavior when Monit runs in daemon mode.

       procmatch regex
           Allows for easy testing of pattern for process match check. The command takes regular expression as an argument and displays all running processes matching the pattern.
-->

### 配置

#### 啟動內置httpd服務

其中 status、summary、report 指令，需要啟動 httpd 服務才可以，否則無法建立鏈接。

{% highlight text %}
set httpd port 2812 and
    use address localhost  # only accept connection from localhost
    allow localhost        # allow localhost to connect to the server and
    allow admin:monit      # require user 'admin' with password 'monit'
{% endhighlight %}


<!--
### 源碼編譯

{% highlight text %}
./configure
man -l monit.1 查看幫助信息
{% endhighlight %}

所有的服務都保存在 servicelist 鏈表中，會在 p.y 做語法解析時進行初始化。

{% highlight text %}
do_action()
 |-do_default()      啟動服務
   |
   |                 在while循環中執行
   |-validate()
     |-Event_queue_process()
{% endhighlight %}



## Supervisor
## Goreman
-->

## 參考

[Monit Offical](http://mmonit.com/monit/)，可以查看官方文檔 [Monit Reference](https://mmonit.com/monit/documentation/monit.html)，也可以直接查看 man 。

{% highlight text %}
{% endhighlight %}
