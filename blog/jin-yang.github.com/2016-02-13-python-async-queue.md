---
title: Python 异步任务队列
layout: post
comments: true
language: chinese
category: [linux, network,python]
keywords: python,异步任务队列,APScheduler,Redis Queue,Celery
description: Python 中有很多的调度系统，这里简单介绍一下常用的，例如 APScheduler、Redis Queue、Celery 等。
---

Python 中有很多的调度系统，这里简单介绍一下常用的，例如 APScheduler、Redis Queue、Celery 等。

<!-- more -->

## 简介

Python 中 [APScheduler](https://pypi.python.org/pypi/APScheduler/) 通常用于跨平台的 cron 操作，可以将任务保存在数据库中，不过比较适合嵌入的应用程序中执行，没有提供独立的执行进程。

## Redis Queue

[Redis Queue, RQ](http://www.python-rq.org) 是一个比 Celery 更加简单的异步任务队列，当然他的功能没有 Celery 多，复杂程度也没有 Celery 大，但它足够简单，其 Broker 只能是 redis 。

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


lrange rq:queue:default 1 100              通过list类型存放该队列中所含有的任务
smembers rq:queues                         通过set保存了所有队列的信息
{% endhighlight %}

任务会在 job 执行后调用 cleanup() 函数，默认会设置为 result_ttl 值。

<!--
### 任务下发

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
