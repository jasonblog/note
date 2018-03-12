---
title: AirFlow 工作流简介
layout: post
comments: true
language: chinese
category: [misc]
keywords: airflow,dac,Directed Acyclical Graphs,编排工具,有向非循环图
description: AirFlow 一个用于编排复杂计算工作流和数据处理流水线的开源工具，通常可以解决一些复杂超长 Cron 脚本任务或者大数据的批量处理任务，其工作流的设计是基于有向非循环图 (Directed Acyclical Graphs, DAG) 。简单来说，在编写工作流时，尽量考虑如何将一个大型的任务拆分为多个可独立执行的原子任务，再将这些任务合并为一个逻辑整体。这里简单介绍一些常见的基本概念及其使用方法。
---

AirFlow 一个用于编排复杂计算工作流和数据处理流水线的开源工具，通常可以解决一些复杂超长 Cron 脚本任务或者大数据的批量处理任务。

其工作流的设计是基于有向非循环图 (Directed Acyclical Graphs, DAG) ，用于设置任务依赖关系和时间调度。

简单来说，在编写工作流时，尽量考虑如何将一个大型的任务拆分为多个可独立执行的原子任务，再将这些任务合并为一个逻辑整体。

这里简单介绍一些常见的基本概念及其使用方法。

<!-- more -->

## 简介

![airflow logo]({{ site.url }}/images/misc/airflow-logo.png "airflow logo"){: .pull-center width="35%" }

图的形状决定了工作流的整体逻辑，可以有多个分支，具体是那个分支在运行时决定，而在设计其中某个结点时，要保证其执行的幂等性，也就是任务可以多次重复执行。

这样，如果发生错误，每个任务可以重试多次，甚至可以完全停止，并通过重新启动最后一个未完成的任务来恢复运行的工作流程。

### 安装

这里通过 virtualenv 进行安装。

{% highlight text %}
----- 通过virtualenv安装
$ mkdir /tmp/project && cd /tmp/project
$ virtualenv --no-site-packages airflow

----- 安装指定版本或者默认
$ pip install airflow==1.8.0
{% endhighlight %}

在安装完一堆的依赖后，就需要配置 `AIRFLOW_HOME` 环境变量，后续的 DAG 和 Plugin 都将以该目录作为根目录查找，如上，可以直接设置为 `/tmp/project` 。

{% highlight text %}
----- 设置环境变量
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

执行了上述的命令后，会生成 `airflow.cfg` 和 `unittests.cfg` 两个文件，其中前者是一个配置文件，关于配置项的详细解释可以参考 [AirFlow Configuration](https://airflow.apache.org/configuration.html) 。

### 启动服务

AirFlow 会将一些信息保存到 DB 中，默认使用 SQLite 可以通过 `airflow initdb` 创建，执行完后会在目录下生成 airflow.db 数据库文件，当然后续也可以修改为 MySQL 。

其提供了多种交互方式，最常用的是 命令行 和 Web UI，其 Web UI 是通过 Flask 编写，启动时直接在上述定义的 `AIRFLOW_HOME` 目录下执行如下的命令。

{% highlight text %}
(airflow) $ airflow webserver
{% endhighlight %}

然后通过浏览器访问 [http://localhost:8080/admin](http://localhost:8080/admin) 即可，启动之后默认会有一堆的示例程序，默认保存在 `/lib/python2.7/site-packages/airflow/example_dags` 目录下。

如果要关闭示例，可以修改配置文件 `airflow.cfg` 中的 `load_examples=False` 配置。

## 编写DAG

在 AirFlow 中，每个节点都是一个任务，可以是一条命令行  (BashOperator)，可以是一段 Python 脚本 (PythonOperator) 等等，然后这些节点根据依赖关系构成了一条流程，一个图，称为一个 DAG 。

默认会到 `${AIRFLOW_HOME}/dags` 目录下查找，可以直接在该目录下创建相应的文件。

如下是一个简单的示例。

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

该文件创建一个简单的 DAG，只有三个运算符，两个 BaseOperator ，也就是执行 Bash 命令分别打印日期以及休眠 5 秒；另一个为 PythonOperator 在执行任务时调用 `print_hello()` 函数。

{% highlight text %}
----- 测试是否正常，如果无报错那么就说明正常
$ python /tmp/project/dags/hello_world.py
{% endhighlight %}


<!--
----- 查看任务
$ airflow list_tasks example_hello_world_dag
-->

## 参考

官方参考文档 [airflow.apache.org](https://airflow.apache.org/) 。

<!--
http://michal.karzynski.pl/blog/2017/03/19/developing-workflows-with-apache-airflow/
https://segmentfault.com/a/1190000005078547
-->

{% highlight text %}
{% endhighlight %}
