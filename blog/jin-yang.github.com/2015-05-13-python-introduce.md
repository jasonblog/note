---
title: Python 简介
layout: post
comments: true
language: chinese
category: [python]
keywords: python,virtualenv
description:
---

<!-- more -->


<!--
# 简介

![python exception hierarchy]({{ site.url }}/images/python/exception-hierarchy.png "python exception hierarchy"){: .pull-center}


在 ipython 中可以通过 BaseException.&lt;tab&gt; 查看属性，其中又两个成员变量 args-tuple 和 message-string(python2.6之后已经去除) 两个属性，在继承时可以覆盖两者。<br><br>

触发异常可以使用 raise ValueError("Some message") 。


# 异常处理方式

如果你在写程序时遇到异常后想进行如下处理的话请参考我下面写的对异常处理的方法：

假设有下面的一段程序：
try:
语句1
语句2
.
语句N
except .........:
print .......

但是你并不知道“语句1至语句N”在执行会出什么样的异常，但你还要做异常处理，且想把出现的异常打印出来，并不停止程序的运行，
所以在“except ......”这句应怎样来写呢？

至少3个方法：

方法一：捕获所有异常
--------------------------------------------------------------------------------
code:

try:
a=b
b=c
except Exception,ex:
print Exception,":",ex
--------------------------------------------------------------------------------

方法二：采用traceback模块查看异常
--------------------------------------------------------------------------------
code:

import traceback
try:
a=b
b=c
except:
traceback.print_exc()

----------------------------------------------------------------------------
方法三：采用sys模块回溯最后的异常
----------------------------------------------------------------------------
code:

import sys
try:
a=b
b=c
except:
info=sys.exc_info()
print info[0],":",info[1]

--------------------------------------------------------------------------------

但是，如果你还想把这些异常保存到一个日志文件中，来分析这些异常，那么请看下面的方法：
把　traceback.print_exc()　打印在屏幕上的信息保存到一个文本文件中
code:

try:
a=b
b=c
except:
f=open("c:\\log.txt",'a')
traceback.print_exc(file=f)
f.flush()
f.close()

-->




## pyenv & virtualenv

一般会使用着两个工具搭建多 Python 环境，其中 pyenv 用于在多个不同系统 Python 版本之间切换，而 virtualenv 用于创建独立的开发环境。

### pyenv

这个是一堆的 Bash 脚本，安装方式可以参考 [github pyenv](https://github.com/pyenv/pyenv) 中的介绍。

### virtualenv

在开发 Python 程序时，经常会遇到要使用多个环境甚至时不同版本的问题，例如 Python 的 3.4、2.7 甚至是 2.6 版本，而通过 pip 安装时，一般只会安装到标准路径下。

virtualenv 就是用来为一个应用创建一套 "隔离" 的 Python 运行环境。

可以直接通过 `pip install virtualenv` 安装相应的最新版本，假设要开发一个新的项目，需要一套独立的 Python 运行环境，那么就可以按照如下步骤操作：

{% highlight text %}
----- 创建目录
$ mkdir /tmp/project && cd /tmp/project

----- 创建一个独立的Python运行环境，命名为foobar
$ virtualenv --no-site-packages foobar
New python executable in /tmp/project/foobar/bin/python2
Also creating executable in /tmp/project/foobar/bin/python
Installing setuptools, pip, wheel...done.
{% endhighlight %}

通过命令 `virtualenv` 可以创建一个独立的 Python 运行环境，参数 `--no-site-packages` 表示已安装到系统环境中的所有第三方包都不会复制过来，这样，我们就得到了一个不带任何第三方包的 "干净" 的 Python 运行环境。

新建的 Python 环境被放到当前目录下的 foobar 目录，此时可以通过 `source foobar/bin/activate` 切换到这个 Python 环境；当然，不同的终端如 csh、fish 需要执行不同的脚本。

此时的终端提示符会添加 `(foobar)` 前缀，然后可以通过 `pip install jinja2` 类似的命令安装三方包，需要退出时执行 `deactivate` 命令。


<!--
Fabric
-->


{% highlight python %}
{% endhighlight %}
