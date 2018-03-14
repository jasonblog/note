---
title: Nginx 監控
layout: post
comments: true
language: chinese
category: [linux,webserver]
keywords: nginx,monitor,監控
description: Nginx 服務器在生產環境中運行時，可以通過類似 Nagios、Zabbix、Munin 之類的網絡監控軟件。然而，很多時候處理問題時，只需要快速簡單的查看當前 Nginx 服務器的狀態請求。可以使用一個 Python 編寫的工具 ngxtop，可以直接從 [www.github.com](https://github.com/lebinh/ngxtop) 上下載相應的源碼。接下來介紹一下該工具的使用方法。
---

Nginx 服務器在生產環境中運行時，可以通過類似 Nagios、Zabbix、Munin 之類的網絡監控軟件。然而，很多時候處理問題時，只需要快速簡單的查看當前 Nginx 服務器的狀態請求。

可以使用一個 Python 編寫的工具 ngxtop，可以直接從 [www.github.com](https://github.com/lebinh/ngxtop) 上下載相應的源碼。

接下來介紹一下該工具的使用方法。

<!-- more -->

## ngxtop

ngxtop 會實時解析 nginx 的訪問日誌 (access log)，並且會將處理結果輸出到終端，其功能類似於系統命令 top，通過 ngxtop 再也不需要 tail 日誌看屏幕刷新了。

### 安裝

ngxtop 是 Python 源碼編寫的程序，可以直接通過源碼安裝，也可以通過 pip 安裝。

{% highlight text %}
$ wget https://github.com/lebinh/ngxtop/archive/master.zip -O ngxtop-master.zip
$ unzip ngxtop-master.zip
$ cd ngxtop-master
# python setup.py install
... ...
Finished processing dependencies for ngxtop==0.0.1
{% endhighlight %}

或者通過 pip 安裝。

{% highlight text %}
# pip install ngxtop
{% endhighlight %}

其中詳細的使用方法可以參考 [https://github.com/lebinh/ngxtop](https://github.com/lebinh/ngxtop) 中的幫助文檔。

### 使用

ngxtop 默認會從其配置文件 ```/etc/nginx/nginx.conf``` 中查找 Nginx 日誌的地址，如果使用的是該文件，那麼可以直接運行 ngxtop 命令，這將會列出 10 個 Nginx 服務，按請求數量排序。

首先會解析配置文件的內容，解析後的結果可以通過 info 子命令查看。

{% highlight text %}
----- 查看解析配置文件的基本信息
$ ngxtop info

----- 顯示前 20 個最頻繁的請求，按照順序排列
$ ngxtop -n 20

----- 顯示自定義的請求變量
$ ngxtop print request http_user_agent remote_addr

----- 顯示請求最多的客戶端IP地址
$ ngxtop top remote_addr

----- 顯示狀態碼是404的請求
$ ngxtop -i 'status == 404' print request status

----- 404前十的請求
$ ngxtop top request_path --filter 'status == 404'

----- 總流量前十的請求
$ ngxtop --order-by 'avg(bytes_sent) * count'

----- 輸出400以上狀態嗎的請求以及請求來源
$ ngxtop -i 'status >= 400' print request status http_referer

----- 返回為200的請求，且請求地址為foo，打印消息體的平均長度
$ ngxtop avg bytes_sent --filter 'status == 200 and request_path.startswith("foo")'
{% endhighlight %}


## http_stub_status_module

另外，可以通過 Nginx 模塊監控其狀態，不過需要在編譯時添加 ```--with-http_stub_status_module``` 選項，默認該選項不會包含在內。

然後在配置文件中添加如下內容。

{% highlight text %}
http {
    ... ...
    server {
        ... ...
        location /status {
            stub_status on;
            access_log off;
            allow 127.0.0.1;
            deny all;
        }
    }
}
{% endhighlight %}

修改完之後重新加載配置即可，然後可以通過 [http://localhost/status](http://localhost/status) 查看，此時會顯示如下信息。

{% highlight text %}
Active connections: 557             # 對後端發起的活動連接數
server accepts handled requests     # 總共處理的鏈接數；成功握手次數；總共處理的請求
35 35 65
Reading: 3 Writing: 16 Waiting: 8   # 讀取到客戶端Header信息數；返回給客戶端的Header信息數；等待下次請求的駐留連接
{% endhighlight %}

注意，對於最後一行 Waiting 在開啟 keep-alive 時，該值等於 ```active-(reading+writing)```，意思就是說已經處理完正在等候下一次請求指令的駐留連接。

<!--
https://linux.cn/article-5970-1.html
http://www.ttlsa.com/zabbix/zabbix-monitor-nginx-performance/
-->


{% highlight text %}
{% endhighlight %}
