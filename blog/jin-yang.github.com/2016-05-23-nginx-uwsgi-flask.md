---
title: Nginx + uWSGI + Flask
layout: post
comments: true
language: chinese
category: [webserver]
keywords: nginx,uwsgi,flask
description: 如前所述，Flask 是一个轻量级的 Web 应用框架，这里简单介绍如何通过 nginx、uwsgi 搭建 flask 环境。
---

如前所述，Flask 是一个轻量级的 Web 应用框架，这里简单介绍如何通过 nginx、uwsgi 搭建 flask 环境。

<!-- more -->

## 安装环境

在 CentOS 7 中，我们使用 nginx + uwsgi + flask 搭建一个服务器，可以直接通过如下方式配置环境。

### nginx

首先通过如下方式安装。

{% highlight text %}
# yum install nginx
# systemctl start nginx
{% endhighlight %}

然后浏览器访问 [http://localhost](http://localhost) 或者 curl http://127.1 即可，最终返回如下内容。

![flask nginx]({{ site.url }}/images/webserver/flask_nginx.png "flask nginx"){: .pull-center}

## uwsgi

这是一个 C 语言编写的，可以直接参考 [Github - unbit/uwsgi](https://github.com/unbit/uwsgi) 源码，可以通过 yum 进行安装，不过安装的版本比较低，后面测试的时候会有很多的问题。

{% highlight text %}
# yum install python-pip
# pip install uwsgi
{% endhighlight %}

然后新建一个很简单的脚本，如下。

{% highlight text %}
$ cat foobar.py
def application(env, start-response):
    start-reponse('200 OK','[('Content_Type','text/html')]')
    return [b"Congraduations!!"]

$ uwsgi --http :8001 --wsgi-file foobar.py
{% endhighlight %}

当然，启动的时候可以通过如下方式启动。

{% highlight text %}
$ cat foobar.ini
[uwsgi]
chdir=/tmp/test/
wsgi-file=foobar.py
http-socket = 127.0.0.1:8001

$ uwsgi --ini foobar.ini
{% endhighlight %}

然后浏览器访问 [http://localhost:8001](http://localhost:8001) 或者 curl http://127.1:8001 即可，最终返回如下内容。

![flask uwsgi]({{ site.url }}/images/webserver/flask_uwsgi.png "flask uwsgi"){: .pull-center}

上述通过 uwsgi 启动时，如果启动报错，无法识别 \-\-wsgi-file 时，实际是由于版本过低导致的。

<!--
### 调优
调优uwsgi，后台启动，热更改PY，杜绝502

经典配置需要看下

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

对于监控，可以直接通过 `pip install uwsgitop` 安装。


## flask

可以直接从 [github - flask](https://github.com/pallets/flask) 下载源码，或者通过如下方式安装。

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

然后浏览器访问 [http://localhost:5000](http://localhost:5000) 或者 curl http://127.1:5000 即可，最终返回如下内容。

![flask]({{ site.url }}/images/webserver/flask_flask.png "flask"){: .pull-center}

## nginx + uwsgi + flask

接着将这三者放到一块，分别修改如下配置文件。

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

在源码的 examples/flaskr 目录下有个示例，可以通过如下方式进行配置、测试。

{% highlight text %}
$ export FLASK_APP=flaskr.py
$ flask initdb
$ flask run
{% endhighlight %}


## 参考

可以参考中文文档 [欢迎来到 Flask 的世界](http://dormousehole.readthedocs.io/en/latest/) 。

<!--
[flask route设计思路](https://segmentfault.com/a/1190000004213652)
[flask.pocoo.org](http://flask.pocoo.org/)、[werkzeug.pocoo.org](http://werkzeug.pocoo.org/)
-->


{% highlight text %}
{% endhighlight %}
