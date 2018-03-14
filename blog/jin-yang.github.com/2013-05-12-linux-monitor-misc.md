---
title: Linux 監控雜項
layout: post
comments: true
language: chinese
category: [linux]
keywords: linux,monitor,監控,sar
description: 在此簡單列舉一些常見的監控工具，以及配置方式等。
---

在此簡單列舉一些常見的監控工具，以及配置方式等。

<!-- more -->

## SAR

sar (select activity information) 用於採集系統的信息，默認會保存一個月的信息，在 CentOS 中包含在 sysstat軟件包中。與此相關的還有：

* sac, system ativity data collector。也就是 sar 的後端，用於收集信息；
* sa1，用於將信息保存為二進制信息，默認位置為 /var/log/sa/saXX (其中XX表示當月的日期)；
* sar2，顯示每天的統計信息，**一個 shell 腳本**；
* sadf，用於轉換為其它的格式 (如csv，xml) 的程序。

其中 sar1 和 sar2 程序會在 cron 任務中調用執行，也就是在 /etc/cron.d/sysstat 文件中，默認每 10 分鐘通過 sa1 採集一次數據；23:53 時通過 sa2 轉換一次數據，默認保存 28 天，可以在 /usr/lib64/sa/sa2 中設置。

其中 /etc/cron.d/sysstat 文件內容如下。

{% highlight text %}
# Run system activity accounting tool every 10 minutes
*/10 * * * * root /usr/lib64/sa/sa1 1 1
# Generate a daily summary of process accounting at 23:53
53 23 * * * root /usr/lib64/sa/sa2 -A
{% endhighlight %}

因為使用的是 crontab ，所以最小的粒度是 1min；當然，也可以使用 ```sa1 1 60``` 每秒執行依次，並執行 60 次，進一步減小監控的粒度。

<!-- # 0 * * * * root /usr/lib64/sa/sa1 600 6 & -->

### 保存數據

默認會將監控數據保存在 /var/log/sa/ 目錄下，其中保存的數據，例如有一個 sa01 文件，可以直接通過 ```sar -f /var/log/sa/sa01``` 命令查看。

簡單列舉常用的參數：

{% highlight text %}
sar [interval [count]]
    如果不使用時間參數則會顯示到現在為止，所蒐集的統計數據；
    -A
    保存所有，等加於-bBdHqrRSuvwWy -I SUM -I XALL -m ALL -n ALL -u ALL -P ALL；
    -f file
    從指定文件讀取顯示；
    CPU
    -u 統計平均值；-P ALL可以指定cpu號或者顯示所有；
{% endhighlight %}

<!--
-o file
默認輸出到stdout；用於指定輸出的二進制文件，如果不指定文件會輸出到/var/log/sa/saXX中；

編譯起來還是不錯的，有多語言支持。
-->

磁盤建議採用 iostat ，sar -d 採用的設備名稱不太好識別，採用的是從設備號。

關於一些常見的監控項，可以參考如下圖片：

![monitor sar]({{ site.url }}/images/linux/monitor-misc-sar.png "monitor sar"){: .pull-center width="80%" }


## TOP

top 是查看資源最常用的方式。

### 常用場景

簡單列舉下經常使用的場景。

#### Batch Mode

默認是交互模式，也就是循環執行，並接收用戶的按鍵，這樣對於一些採集的程序就不方便使用，這裡可以使用 Batch Mode 。

{% highlight text %}
----- 使用Batch Mode打印指標，此時仍是無限循環
$ top -b

----- 通過-n參數指定只打印一次
$ top -b -n 1

----- 指定時間點運行，需要注意TERM環境變量，top命令需要但是at並沒有獲取該參數
$ cat ./test.at
TERM=linux top -b -n 1 >/tmp/top-report.txt
$ at -f ./test.at now+1minutes
{% endhighlight %}

#### 監聽特定進程

可以通過如下方式指定只監控特定進程或者用戶。

{% highlight text %}
----- 指定多個進程
$ top -p 4360,4358
$ top -p 4360 -p 4358

----- 指定用戶
$ top -u johndoe
$ top -u 500
$ top -U johndoe
{% endhighlight %}

<!--
The conclusion is, you can either use the plain user name or the numeric UID. "-u, -U? Those two are different?" Yes. Like almost any other GNU tools, options are case sensitive. -U means top will find matching effective, real, saved and filesystem UIDs, while -u just find matching effective user id. Just for reminder, every *nix process runs using effective UID and sometimes it isn't equal with real user ID. Most likely, one is interested in effective UID as filesystem permission and operating system capability are checked against it, not real UID.

While -p is just command-line option only, both -U and -u can be used inside interactive mode. Like you guess, press 'U' or 'u' to filter the processes based on their user name. Same rule is applied, 'u' for effective UID and 'U' for real/effective/saved/filesystem user name. You will be asked to enter the user name or the numeric UID. '

A 顯示多個窗口。


-->






## 多線程

在 Linux 下可以通過如下程序查看多線程。

{% highlight text %}
----- 通過-H(Threads toggle)參數，一行顯示一個線程
$ top -H -p `pidof mysqld`

----- 查看所有存在的線程，H(Show threads as if they were processes)
$ ps Hp `pidof mysqld`

----- 查看線程調用堆棧
$ pstack `pidof mysqld`

----- 查看線程數
$ pstree -p `pidof mysqld`
{% endhighlight %}

<!--
3、ps -mp <PID>
手冊中說：m Show threads after processes
這樣可以查看一個進程起的線程數。
-->



{% highlight text %}
{% endhighlight %}
