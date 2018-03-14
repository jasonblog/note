---
title: AirFlow 工作流簡介
layout: post
comments: true
language: chinese
category: [misc]
keywords: airflow,dac,Directed Acyclical Graphs,編排工具,有向非循環圖
description: AirFlow 一個用於編排複雜計算工作流和數據處理流水線的開源工具，通常可以解決一些複雜超長 Cron 腳本任務或者大數據的批量處理任務，其工作流的設計是基於有向非循環圖 (Directed Acyclical Graphs, DAG) 。簡單來說，在編寫工作流時，儘量考慮如何將一個大型的任務拆分為多個可獨立執行的原子任務，再將這些任務合併為一個邏輯整體。這裡簡單介紹一些常見的基本概念及其使用方法。
---

AirFlow 一個用於編排複雜計算工作流和數據處理流水線的開源工具，通常可以解決一些複雜超長 Cron 腳本任務或者大數據的批量處理任務。

其工作流的設計是基於有向非循環圖 (Directed Acyclical Graphs, DAG) ，用於設置任務依賴關係和時間調度。

簡單來說，在編寫工作流時，儘量考慮如何將一個大型的任務拆分為多個可獨立執行的原子任務，再將這些任務合併為一個邏輯整體。

這裡簡單介紹一些常見的基本概念及其使用方法。

<!-- more -->

## 簡介

![airflow logo]({{ site.url }}/images/misc/airflow-logo.png "airflow logo"){: .pull-center width="35%" }

圖的形狀決定了工作流的整體邏輯，可以有多個分支，具體是那個分支在運行時決定，而在設計其中某個結點時，要保證其執行的冪等性，也就是任務可以多次重複執行。

這樣，如果發生錯誤，每個任務可以重試多次，甚至可以完全停止，並通過重新啟動最後一個未完成的任務來恢復運行的工作流程。

### 安裝

這裡通過 virtualenv 進行安裝。

{% highlight text %}
----- 通過virtualenv安裝
$ mkdir /tmp/project && cd /tmp/project
$ virtualenv --no-site-packages airflow

----- 安裝指定版本或者默認
$ pip install airflow==1.8.0
{% endhighlight %}

在安裝完一堆的依賴後，就需要配置 `AIRFLOW_HOME` 環境變量，後續的 DAG 和 Plugin 都將以該目錄作為根目錄查找，如上，可以直接設置為 `/tmp/project` 。

{% highlight text %}
----- 設置環境變量
(airflow) $ export AIRFLOW_HOME=/tmp/project

----- 查看其版本信息
(airflow) $ airflow version
  ____________       _____________
 ____    |__( )_________  __/__  /________      __
____  /| |_  /__  ___/_  /_ __  /_  __ \_ | /| / /
___  ___ |  / _  /   _  __/ _  / / /_/ /_ |/ |/ /
 _/_/  |_/_/  /_/    /_/    /_/  \____/____/|__/
   v1.8.0
{% endhighlight %}

執行了上述的命令後，會生成 `airflow.cfg` 和 `unittests.cfg` 兩個文件，其中前者是一個配置文件，關於配置項的詳細解釋可以參考 [AirFlow Configuration](https://airflow.apache.org/configuration.html) 。

### 啟動服務

AirFlow 會將一些信息保存到 DB 中，默認使用 SQLite 可以通過 `airflow initdb` 創建，執行完後會在目錄下生成 airflow.db 數據庫文件，當然後續也可以修改為 MySQL 。

其提供了多種交互方式，最常用的是 命令行 和 Web UI，其 Web UI 是通過 Flask 編寫，啟動時直接在上述定義的 `AIRFLOW_HOME` 目錄下執行如下的命令。

{% highlight text %}
(airflow) $ airflow webserver
{% endhighlight %}

然後通過瀏覽器訪問 [http://localhost:8080/admin](http://localhost:8080/admin) 即可，啟動之後默認會有一堆的示例程序，默認保存在 `/lib/python2.7/site-packages/airflow/example_dags` 目錄下。

如果要關閉示例，可以修改配置文件 `airflow.cfg` 中的 `load_examples=False` 配置。

## 編寫DAG

在 AirFlow 中，每個節點都是一個任務，可以是一條命令行  (BashOperator)，可以是一段 Python 腳本 (PythonOperator) 等等，然後這些節點根據依賴關係構成了一條流程，一個圖，稱為一個 DAG 。

默認會到 `${AIRFLOW_HOME}/dags` 目錄下查找，可以直接在該目錄下創建相應的文件。

如下是一個簡單的示例。

{% highlight python %}
# -*- coding: utf-8 -*-

import airflow
from airflow import DAG
from airflow.operators.bash_operator import BashOperator
from airflow.operators.python_operator import PythonOperator
from datetime import timedelta

#-------------------------------------------------------------------------------
# these args will get passed on to each operator
# you can override them on a per-task basis during operator initialization

default_args = {
    'owner': 'foobar',
    'depends_on_past': False,
    'start_date': airflow.utils.dates.days_ago(2),
    'email': ['foobar@kidding.com'],
    'email_on_failure': False,
    'email_on_retry': False,
    'retries': 1,
    'retry_delay': timedelta(minutes=5),
    # 'queue': 'bash_queue',
    # 'pool': 'backfill',
    # 'priority_weight': 10,
    # 'end_date': datetime(2016, 1, 1),
    # 'wait_for_downstream': False,
    # 'dag': dag,
    # 'adhoc':False,
    # 'sla': timedelta(hours=2),
    # 'execution_timeout': timedelta(seconds=300),
    # 'on_failure_callback': some_function,
    # 'on_success_callback': some_other_function,
    # 'on_retry_callback': another_function,
    # 'trigger_rule': u'all_success'
}

#-------------------------------------------------------------------------------
# dag

dag = DAG(
    'example_hello_world_dag',
    default_args=default_args,
    description='my first DAG',
    schedule_interval=timedelta(days=1))

#-------------------------------------------------------------------------------
# first operator

date_operator = BashOperator(
    task_id='date_task',
    bash_command='date',
    dag=dag)

#-------------------------------------------------------------------------------
# second operator

sleep_operator = BashOperator(
    task_id='sleep_task',
    depends_on_past=False,
    bash_command='sleep 5',
    dag=dag)

#-------------------------------------------------------------------------------
# third operator

def print_hello():
    return 'Hello world!'

hello_operator = PythonOperator(
    task_id='hello_task',
    python_callable=print_hello,
    dag=dag)

#-------------------------------------------------------------------------------
# dependencies

sleep_operator.set_upstream(date_operator)
hello_operator.set_upstream(date_operator)
{% endhighlight %}

該文件創建一個簡單的 DAG，只有三個運算符，兩個 BaseOperator ，也就是執行 Bash 命令分別打印日期以及休眠 5 秒；另一個為 PythonOperator 在執行任務時調用 `print_hello()` 函數。

{% highlight text %}
----- 測試是否正常，如果無報錯那麼就說明正常
$ python /tmp/project/dags/hello_world.py
{% endhighlight %}


<!--
----- 查看任務
$ airflow list_tasks example_hello_world_dag
-->

## 參考

官方參考文檔 [airflow.apache.org](https://airflow.apache.org/) 。

<!--
http://michal.karzynski.pl/blog/2017/03/19/developing-workflows-with-apache-airflow/
https://segmentfault.com/a/1190000005078547
-->

{% highlight text %}
{% endhighlight %}
