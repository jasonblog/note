---
title: Python 簡介
layout: post
comments: true
language: chinese
category: [python]
keywords: python,virtualenv
description:
---

<!-- more -->


<!--
# 簡介

![python exception hierarchy]({{ site.url }}/images/python/exception-hierarchy.png "python exception hierarchy"){: .pull-center}


在 ipython 中可以通過 BaseException.&lt;tab&gt; 查看屬性，其中又兩個成員變量 args-tuple 和 message-string(python2.6之後已經去除) 兩個屬性，在繼承時可以覆蓋兩者。<br><br>

觸發異常可以使用 raise ValueError("Some message") 。


# 異常處理方式

如果你在寫程序時遇到異常後想進行如下處理的話請參考我下面寫的對異常處理的方法：

假設有下面的一段程序：
try:
語句1
語句2
.
語句N
except .........:
print .......

但是你並不知道“語句1至語句N”在執行會出什麼樣的異常，但你還要做異常處理，且想把出現的異常打印出來，並不停止程序的運行，
所以在“except ......”這句應怎樣來寫呢？

至少3個方法：

方法一：捕獲所有異常
--------------------------------------------------------------------------------
code:

try:
a=b
b=c
except Exception,ex:
print Exception,":",ex
--------------------------------------------------------------------------------

方法二：採用traceback模塊查看異常
--------------------------------------------------------------------------------
code:

import traceback
try:
a=b
b=c
except:
traceback.print_exc()

----------------------------------------------------------------------------
方法三：採用sys模塊回溯最後的異常
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

但是，如果你還想把這些異常保存到一個日誌文件中，來分析這些異常，那麼請看下面的方法：
把　traceback.print_exc()　打印在屏幕上的信息保存到一個文本文件中
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

一般會使用著兩個工具搭建多 Python 環境，其中 pyenv 用於在多個不同系統 Python 版本之間切換，而 virtualenv 用於創建獨立的開發環境。

### pyenv

這個是一堆的 Bash 腳本，安裝方式可以參考 [github pyenv](https://github.com/pyenv/pyenv) 中的介紹。

### virtualenv

在開發 Python 程序時，經常會遇到要使用多個環境甚至時不同版本的問題，例如 Python 的 3.4、2.7 甚至是 2.6 版本，而通過 pip 安裝時，一般只會安裝到標準路徑下。

virtualenv 就是用來為一個應用創建一套 "隔離" 的 Python 運行環境。

可以直接通過 `pip install virtualenv` 安裝相應的最新版本，假設要開發一個新的項目，需要一套獨立的 Python 運行環境，那麼就可以按照如下步驟操作：

{% highlight text %}
----- 創建目錄
$ mkdir /tmp/project && cd /tmp/project

----- 創建一個獨立的Python運行環境，命名為foobar
$ virtualenv --no-site-packages foobar
New python executable in /tmp/project/foobar/bin/python2
Also creating executable in /tmp/project/foobar/bin/python
Installing setuptools, pip, wheel...done.
{% endhighlight %}

通過命令 `virtualenv` 可以創建一個獨立的 Python 運行環境，參數 `--no-site-packages` 表示已安裝到系統環境中的所有第三方包都不會複製過來，這樣，我們就得到了一個不帶任何第三方包的 "乾淨" 的 Python 運行環境。

新建的 Python 環境被放到當前目錄下的 foobar 目錄，此時可以通過 `source foobar/bin/activate` 切換到這個 Python 環境；當然，不同的終端如 csh、fish 需要執行不同的腳本。

此時的終端提示符會添加 `(foobar)` 前綴，然後可以通過 `pip install jinja2` 類似的命令安裝三方包，需要退出時執行 `deactivate` 命令。


<!--
Fabric
-->


{% highlight python %}
{% endhighlight %}
