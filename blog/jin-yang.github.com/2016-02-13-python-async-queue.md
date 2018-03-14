---
title: Python 異步任務隊列
layout: post
comments: true
language: chinese
category: [linux, network,python]
keywords: python,異步任務隊列,APScheduler,Redis Queue,Celery
description: Python 中有很多的調度系統，這裡簡單介紹一下常用的，例如 APScheduler、Redis Queue、Celery 等。
---

Python 中有很多的調度系統，這裡簡單介紹一下常用的，例如 APScheduler、Redis Queue、Celery 等。

<!-- more -->

## 簡介

Python 中 [APScheduler](https://pypi.python.org/pypi/APScheduler/) 通常用於跨平臺的 cron 操作，可以將任務保存在數據庫中，不過比較適合嵌入的應用程序中執行，沒有提供獨立的執行進程。

## Redis Queue

[Redis Queue, RQ](http://www.python-rq.org) 是一個比 Celery 更加簡單的異步任務隊列，當然他的功能沒有 Celery 多，複雜程度也沒有 Celery 大，但它足夠簡單，其 Broker 只能是 redis 。

{% highlight text %}
# pip install rq

127.0.0.1:6379> type rq:job:fba1419d-2c0a-47a2-83c9-bd614309c92c
hash
127.0.0.1:6379> hgetall rq:job:fba1419d-2c0a-47a2-83c9-bd614309c92c
 1) "status"
     2) "finished"
 3) "origin"
     4) "default"
 5) "description"
     6) "database.tasks.reload.reload(1)"
 7) "created_at"
     8) "2016-10-10T13:54:40Z"
 9) "enqueued_at"
    10) "2016-10-10T13:54:40Z"
11) "timeout"
    12) "180"
13) "data"
    14) "\x80\x02(X\x1c\x00\x00\x00database.tasks.reload.reloadq\x01NK\x01\x85q\x02}q\x03tq\x04."
15) "started_at"
    16) "2016-10-10T13:54:40Z"
17) "ended_at"
    18) "2016-10-10T13:54:41Z"
19) "result"
    20) "\x80\x02U\x05xxxxxq\x01."
21) "ttl"
    22) "-1"

ttl rq:job:88fc4ae9-a9c7-4532-98b7-e0c06ef01dbb


lrange rq:queue:default 1 100              通過list類型存放該隊列中所含有的任務
smembers rq:queues                         通過set保存了所有隊列的信息
{% endhighlight %}

任務會在 job 執行後調用 cleanup() 函數，默認會設置為 result_ttl 值。

<!--
### 任務下發

enqueue_job()

timeout specifies the maximum runtime of the job before it'll be considered 'lost'
result_ttl specifies the expiry time of the key where the job result will be stored
ttl specifies the maximum queued time of the job before it'll be cancelled
depends_on specifies another job (or job id) that must complete before this job will be queued
job_id allows you to manually specify this job's job_id
at_front will place the job at the front of the queue, instead of the back
kwargs and args lets you bypass the auto-pop of these arguments, ie: specify a timeout argument for the underlying job function.
-->

<!--
## Celery

[Celery](http://www.celeryproject.org/)

http://docs.celeryproject.org/en/latest/index.html
-->


{% highlight text %}
{% endhighlight %}
