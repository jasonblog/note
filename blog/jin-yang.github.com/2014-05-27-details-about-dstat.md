---
title: dstat 使用及其原理
layout: post
comments: true
language: chinese
category: [linux]
keywords: dstat,monitor,監控
description: dstat 是一個不錯的系統監控程序，類似於 vmstat、iostat、mpstat，所不同的是，可以監控多個系統指標，如 CPU、網絡、內存、中斷等，可以將結果顯示到終端，也可保存到文件。另外，該程序是通過 Python 實現的，可以動態指定插件，甚至編寫自己的插件。
---

dstat 是一個不錯的系統監控程序，類似於 vmstat、iostat、mpstat，所不同的是，可以監控多個系統指標，如 CPU、網絡、內存、中斷等，可以將結果顯示到終端，也可保存到文件。

另外，該程序是通過 Python 實現的，可以動態指定插件，甚至編寫自己的插件。

在本文中，大致介紹其使用方法，以及實現的原理。

<!-- more -->

## 使用方法

詳細的使用方式可以查看 man 幫助，在此只是介紹一些 dstat 的特性。

查看 man 幫助時，就會發現，作者的原意是要替換掉 vmstat、iostat、ifstat 等工具。通過該工具可以將多個監控在一塊展示，而且顯示比較友好，並且可以直接將統計結果保存。

另外，dstat 支持高亮顯示，不過需要注意的是，顏色並非標示系統指標的好或者壞；對於顏色改變，只是說明其單位修改了，或者超過了某個範圍 (通常對於 cpu 而言)。

![dstat-sample]{: .pull-center}

其幫助手冊可以通過 man 1 dstat 查看，命令的基本格式如下，也可以通過 -h 選項查看。

{% highlight text %}
dstat [-afv] [options..] [delay [count]]

常用選項：
  -c, --cpu
    統計CPU狀態，包括 user, system, idle (空閒等待時間百分比), wait (等待磁盤IO),
    hardware interrupt (硬件中斷), software interrupt (軟件中斷) 等；
  -d, --disk
    統計磁盤讀寫狀態，主要包括了讀寫信息；
  -l, --load
    統計系統負載情況，包括1分鐘、5分鐘、15分鐘平均值；
  -m, --mem
    統計系統物理內存使用情況，包括used, buffers, cache, free；
  -s, --swap
    統計swap已使用和剩餘量；
  -n, --net
    統計網絡使用情況，包括接收和發送數據；
  -p, --proc
    統計進程信息，包括runnable、uninterruptible、new；
  -N eth1,total
    統計eth1接口彙總流量；
  -r, --io
    統計I/O請求，包括讀寫請求；
  -y, --sys
    統計系統信息，包括中斷、上下文切換；
  -t
    顯示統計時時間，對分析歷史數據非常有用；
  --fs
    統計文件打開數和inodes數；
{% endhighlight %}

其中最後兩個參數表示，每隔 delay 秒顯示一行，總計 count 行，默認值是 1s 和空 (無限)。如果 delay 大於 1 ，默認該行還是每隔 1s 更新一次，當超過 delay 秒後，換行。

需要注意的是，當 delay > 1 時，每秒的更新值是顯示的之前統計的均值，如 delay=10 ，則會顯示 1 秒的均值，2 秒的均值，...，也就是最終結果仍然是秒級的均值。

如果同時設置了 \-\-noupdate ，則確實是每隔 delay 秒更新一次，而不會每秒更新一次。

### 常用參數

如下是常見的使用方法，每隔 3 秒刷新。

{% highlight text %}
$ dstat -clmdngyr 3
{% endhighlight %}

### 使用插件

Dstat 是基於插件的，程序本身提供了定時顯示的功能，而顯示的內容可以通過插件進行擴展，從而可以很方便的添加自己所需要的插件。

如下的示例是使用 time、cpu、disk 三個插件，其作用是等效的。

{% highlight text %}
$ dstat -tcd
$ dstat --time --cpu --disk
$ dstat -M time,cpu,disk
$ dstat -M time -M cpu -M disk
{% endhighlight %}

可以使用的插件包括了 dstat.py 文件中定義的插件，以及在 plugins 目錄下定義的插件，使用時可以直接通過 \-\-PLUG-NAME 指定相應的插件執行。

通過 dstat \-\-list 查看當前支持的所有插件及其保存的目錄，其中最簡單的是 plugins/dstat_helloworld.py ，也就是一個示例，可以通過 dstat \-\-helloworld 執行。

![dstat-helloworld]{: .pull-center}

如果想要自己定義插件，可以直接參考 plugins/dstat_helloworld.py 的實現。



### 其它

除了直接監控性能外，還可以放到其它的程序中使用，其中在 examples/{read.py,mstat.py}，包含了簡單的示例，沒有仔細研究過。感興趣的可以自己檢查下。



## 源碼分析

dstat 的源碼可以通過 yumdownloader 下載，或則直接到 [Dstat 官網][dstat-official] 上下載，當然也可以從 [Github](https://github.com/dagwieers/dstat) 上下載 。在官網，除了 Dstat 之外，還有些其它的工具可供參考。

dstat 中的定時執行模塊是通過 sched 模塊實現的，因此之前我們先看看該模塊的功能。詳細文檔可以參考官方 [sched — Event scheduler][sched-official]，在此僅簡單介紹一下。

### sched 模塊

該模塊實際是一個調度器，也就是延時處理機制，每次想要定時執行某任務都必須寫入一個調度，詳細內容可以參考 [Python 官方][sched-official]。

如果查看源碼，可以發現其實現非常簡單，會通過 timefunc 計算時間，休眠通過 delayfunc 實現，而且通過 queue 保存多個事件。

使用時通常包含了三步：

#### 1. 生成調度器
{% highlight text %}
scheduler = sched.scheduler(timefunc, delayfunc)
{% endhighlight %}
第一個參數是一個可以返回時間戳的函數；第二個參數可以在定時未到達之前的阻塞函數。通常來說，我們會使用 time.time、time.sleep，當然也可以使用自定義的定時函數。<!--第二個也可以是阻塞socket等。-->

#### 2. 加入調度事件

{% highlight text %}
scheduler.enterabs(time, priority, action, argument)
scheduler.enter(delay, priority, action, argument)
{% endhighlight %}
對於 enter() 函數來說，採用的時相對時間，其四個參數分別為 A) 間隔時間；B) 優先級，當多個任務同時到達時使用，數字大的優先級高； C) 被調用回調的函數；D) 傳遞給回掉函數的參數。

enterabs() 使用的是絕對時間，該參數需要與初始化時的 timefunc 入參格式相同。

#### 3. 運行
{% highlight text %}
scheduler.run()
{% endhighlight %}
運行所有的調度事件，該函數會等待下一個事件，然後執行他直到沒有可調度的事件為止。

### 示例

可以參考如下的簡單示例。

{% highlight python %}
#!/usr/bin/env python
#-*- coding:utf-8 -*-
import time
import sched

def foobar(msg):         # 被定時調度的函數
    print "Current:", time.time(), 'MSG ', msg

s = sched.scheduler(time.time, time.sleep)     # 初始化
s.enter(2, 2, foobar, ("High Priority.",))     # 添加如下的兩個調度任務
s.enter(2, 1, foobar, ("Low  Priority.",))
s.run()                                        # OK, Run it
print "Current:", time.time()
time.sleep(3)
{% endhighlight %}

注意，sched 模塊不是循環的，一次調度被執行後就 Over 了，如果想再執行，請再次 enter，也就是如上的函數只會調度一次。

如果要循環執行，可以使用如下的函數。
{% highlight python %}
#!/usr/bin/env python
#-*- coding:utf-8 -*-
import time
import sched

def foobar(msg, act_time):         # 被定時調度的函數
    global s
    print "Current:", time.time(), 'MSG ', msg
    s.enterabs(act_time + 5, 1, foobar, ("Again.", act_time + 5,))

s = sched.scheduler(time.time, time.sleep)                   # 初始化
initime = time.time()
s.enterabs(initime + 1, 1, foobar, ("Begin.", initime + 1,)) # 添加調度任務
s.run()                                                      # OK, Run it
print "Current:", initime
time.sleep(100)
{% endhighlight %}

在 dstat 中，就是使用的後者。

注意，上述程序只為了說明，生產環境的程序中一定不能在定時函數中再次調用 enter() 函數，否則調用棧會不斷增加，直到內存上限。


### 框架

其中所有的插件都是繼承於 class dstat 類，包括了一些必須的成員變量，以及相應的接口函數，其中部分是必須要包含的，如果沒有則會在初始化的時候報錯。

#### 插件分析

dstat 的插件包括了 dstat.py 中的類，以及 plugins 目錄下的插件。

在內部的代碼中，所有的插件必須要以 dstat_ 開頭，後面的是插件的名稱。在加載的時候，會將命令行傳入的 - 轉換為 _ ，如 \-\-disk-util 實際對應 plugins/dstat_disk_util.py 文件。

插件查找時，首先會搜索 dstat.py 文件中的實現方法，也就是開頭為 dstat 的類，然後搜索 pluginpath 目錄下的 dstat_xxx.py 文件，所有的插件可以通過 listplugins() 查看。


#### 循環調度

初始化的操作是在 main() 函數中完成，對於插件，則會通過 execfile()、exec 加載。

對於調度，如上所述，調度任務是通過 sched 實現的。

{% highlight python %}
def main():
    ... ...
    scheduler = sched.scheduler(time.time, time.sleep)
    inittime = time.time()
    ### Let the games begin
    while update <= op.delay * op.count or op.count == -1:
        scheduler.enterabs(inittime + update, 1, perform, (update,))
        scheduler.run()
        sys.stdout.flush()
        update = update + interval
        linecache.clearcache()
    ... ...
{% endhighlight %}

最後執行是 perform() 函數。

#### 執行、顯示

如下是主要的執行函數，首先會調用各個插件的 extract() 調用，該函數會對參數進行解析，並將結果保存在 val 中。

然後會調用通用函數 show()、showend() 進行打印，dstat 會通過顏色對狀態值進行著色，該功能是通過 cprint() 函數實現，可以參考其細節。

{% highlight python %}
def perform(update):
    ... ...
    ### Calculate all objects (visible, invisible)
    line = newline
    oline = ''
    for o in totlist:
        o.extract()
        if o in vislist:
            line = line + o.show() + o.showend(totlist, vislist)
        if op.output and step == op.delay:
            oline = oline + o.showcsv() + o.showcsvend(totlist, vislist)

    ### Print stats
    sys.stdout.write(line + theme['input'])
    if op.output and step == op.delay:
        outputfile.write(oline + '\n')
    ... ...
{% endhighlight %}


### 總結

介紹到此為止，該工具基本上就是提供了一個通用的框架，可以很好的進行擴展，細節方面可以直接看下源碼了。


## 參考

除了文章中列舉的鏈接之外，還可以參考一篇本地保存的 [Dstat: plugin­based real­time monitoring][dstat-ref]，忘記在那裡下載的了。

另外，可以參考 [Dstat Offical Site](http://dag.wiee.rs/home-made/dstat/)，以及關於 [Dstat的介紹](http://www.ukuug.org/events/linux2007/2007/papers/Wieers.pdf) 。


[dstat-official]:              http://dag.wiee.rs/home-made/dstat/                  "dstat 官網"
[sched-official]:              https://docs.python.org/2/library/sched.html         "Sched 官方調用"
[dstat-ref]:                   /reference/linux/plugin­based real­time monitoring .pdf

[dstat-sample]:                /images/python/dstat.png                             "dstat 示例"
[dstat-helloworld]:            /images/python/dstat-helloworld.png                  "dstat helloworld 插件示例"

<!--
-->

{% highlight python %}
{% endhighlight %}
