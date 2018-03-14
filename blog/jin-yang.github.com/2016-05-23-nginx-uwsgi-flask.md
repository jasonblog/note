---
title: Nginx + uWSGI + Flask
layout: post
comments: true
language: chinese
category: [webserver]
keywords: nginx,uwsgi,flask
description: 如前所述，Flask 是一個輕量級的 Web 應用框架，這裡簡單介紹如何通過 nginx、uwsgi 搭建 flask 環境。
---

如前所述，Flask 是一個輕量級的 Web 應用框架，這裡簡單介紹如何通過 nginx、uwsgi 搭建 flask 環境。

<!-- more -->

## 安裝環境

在 CentOS 7 中，我們使用 nginx + uwsgi + flask 搭建一個服務器，可以直接通過如下方式配置環境。

### nginx

首先通過如下方式安裝。

{% highlight text %}
# yum install nginx
# systemctl start nginx
{% endhighlight %}

然後瀏覽器訪問 [http://localhost](http://localhost) 或者 curl http://127.1 即可，最終返回如下內容。

![flask nginx]({{ site.url }}/images/webserver/flask_nginx.png "flask nginx"){: .pull-center}

## uwsgi

這是一個 C 語言編寫的，可以直接參考 [Github - unbit/uwsgi](https://github.com/unbit/uwsgi) 源碼，可以通過 yum 進行安裝，不過安裝的版本比較低，後面測試的時候會有很多的問題。

{% highlight text %}
# yum install python-pip
# pip install uwsgi
{% endhighlight %}

然後新建一個很簡單的腳本，如下。

{% highlight text %}
$ cat foobar.py
def application(env, start-response):
    start-reponse('200 OK','[('Content_Type','text/html')]')
    return [b"Congraduations!!"]

$ uwsgi --http :8001 --wsgi-file foobar.py
{% endhighlight %}

當然，啟動的時候可以通過如下方式啟動。

{% highlight text %}
$ cat foobar.ini
[uwsgi]
chdir=/tmp/test/
wsgi-file=foobar.py
http-socket = 127.0.0.1:8001

$ uwsgi --ini foobar.ini
{% endhighlight %}

然後瀏覽器訪問 [http://localhost:8001](http://localhost:8001) 或者 curl http://127.1:8001 即可，最終返回如下內容。

![flask uwsgi]({{ site.url }}/images/webserver/flask_uwsgi.png "flask uwsgi"){: .pull-center}

上述通過 uwsgi 啟動時，如果啟動報錯，無法識別 \-\-wsgi-file 時，實際是由於版本過低導致的。

<!--
### 調優
調優uwsgi，後臺啟動，熱更改PY，杜絕502

經典配置需要看下

uwsgi-docs-cn.readthedocs.io/zh_CN/latest/Snippets.html
-->


### 配置文件

{% highlight ini %}
[uwsgi]
base = /var/www/uop/
chdir=/tmp/test/
wsgi-file=foobar.py
#http-socket = 127.0.0.1:8001
socket = /tmp/%n.sock
daemonize = %(base)/log/uwsgi.log

# reload uWSGI if the specified file is modified/touched.
touch-reload = %(base)/conf/uwsgi.conf

# fork a thread to scan python file per N seconds.
py-autoreload = 2
{% endhighlight %}

對於監控，可以直接通過 `pip install uwsgitop` 安裝。


## flask

可以直接從 [github - flask](https://github.com/pallets/flask) 下載源碼，或者通過如下方式安裝。

{% highlight text %}
# pip install flask

$ cat hello.py
from flask import Flask
app = Flask(__name__)

@app.route('/')
def hello_world():
    return 'Hello World!'

if __name__ == '__main__':
    app.run(host='127.1', debug=True)

$ python hellp.py
  * Running on http://127.0.0.1:5000/

$ curl http://127.1:5000
{% endhighlight %}

然後瀏覽器訪問 [http://localhost:5000](http://localhost:5000) 或者 curl http://127.1:5000 即可，最終返回如下內容。

![flask]({{ site.url }}/images/webserver/flask_flask.png "flask"){: .pull-center}

## nginx + uwsgi + flask

接著將這三者放到一塊，分別修改如下配置文件。

{% highlight text %}
$ cat /etc/nginx/nginx.conf
... ...
    server {
        location / {
            include uwsgi_params;
            uwsgi_pass 127.0.0.1:8001;
        }
    }
... ...

$ cat foobar.ini
[uwsgi]
master = true
callable = app
chdir = /tmp/test/
wsgi-file = foobar.py
show-config = true
socket = 127.0.0.1:8001

# uwsgi --ini foobar.ini
# systemctl start nginx
{% endhighlight %}

在源碼的 examples/flaskr 目錄下有個示例，可以通過如下方式進行配置、測試。

{% highlight text %}
$ export FLASK_APP=flaskr.py
$ flask initdb
$ flask run
{% endhighlight %}


## 參考

可以參考中文文檔 [歡迎來到 Flask 的世界](http://dormousehole.readthedocs.io/en/latest/) 。

<!--
[flask route設計思路](https://segmentfault.com/a/1190000004213652)
[flask.pocoo.org](http://flask.pocoo.org/)、[werkzeug.pocoo.org](http://werkzeug.pocoo.org/)
-->


{% highlight text %}
{% endhighlight %}
