---
title: Flask 简介
layout: post
comments: true
language: chinese
category: [python,program,webserver]
keywords: python,flask
description: Flask 是一个使用 Python 编写的轻量级 Web 应用框架，依赖于两个外部库：WSGI 工具箱采用 Werkzeug；模板引擎则使用 Jinja2 。接下来简单介绍一下相关内容，一步步看看如何使用。
---

Flask 是一个使用 Python 编写的轻量级 Web 应用框架，依赖于两个外部库：WSGI 工具箱采用 Werkzeug；模板引擎则使用 Jinja2 。

接下来简单介绍一下相关内容，一步步看看如何使用。

<!-- more -->

![flask logo]({{ site.url }}/images/python/flask.svg){: .pull-center}

## 简介

Flask 是基于 werkzeug 实现的，这是一个基于 wsgi 协议实现的底层函数库，也就是实现了基本的 HTTP 请求和处理，实际上我们都能在 werkzeug 的基础上来实现自己的 Web 框架。

### 安装

可以直接从 [github - flask](https://github.com/pallets/flask) 下载源码，或者通过 pip 方式安装。

{% highlight text %}
# pip install flask             # 或者easy_install flask
{% endhighlight %}

当然也可以通过源码进行安装，实际上除了上述的两个核心的功能库之外，还依赖 click 库，用来简化命令行处理；itsdangerous 用来处理 cookie 相关信息。

{% highlight text %}
# python setup.py install       # 直接在源码目录下执行安装命令
{% endhighlight %}

### Hello World !

安装完成之后可以通过如下方式进行测试，下面是一个简单的示例。

{% highlight text %}
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


## Flask 配置

在每个 Flask 对象中，都有一个 config 属性，用来保存配置；而 config 实际上是继承于字典，源码在 config.py 中实现。

如果要修改配置项，可以直接通过如下的方式进行修改。

{% highlight text %}
----- 1. 在Python中直接配置
app.config['HOST']='www.foobar.com'
print app.config.get('HOST')

----- 2. 通过单独Python配置文件配置 *****
app.config.from_pyfile('default_config.py')

# FILE: default_config.py
HOST = 'localhost'
PORT = 5000
DEBUG = True

----- 3. 最好设置一个config.py文件，从中读取该内容
CSRF_ENABLED = True
SECRET_KEY = 'you-will-never-guess'

app.config.from_object('config')

----- 3. 通过环境变量加载配置
export MyAppConfig=/path/to/settings.cfg

app.config.from_envvar('MyAppConfig')
{% endhighlight %}

其中，有一部分的配置项是 Flask 所使用的，例如，SECRET_KEY 是 Flask 中比较重要的一个配置值，包括 Session、Cookies 以及一些第三方扩展都会用到该值，应该尽可能设置为一个很难猜到的随机值。通常操作系统可以基于一个密码随机生成器来生成漂亮的随机值，该值可以用来做密钥。

另外，如果需要，还可以自己配置一些应用中将会用到的配置，从而可以全局访问。

## Blueprint

通过蓝图 (blueprint) 可以把一个应用分解为一个蓝图的集合，一个项目可以实例化一个应用对象，初始化几个扩展，并注册一集合的蓝图，从而方便管理。

如下，是一个简单的示例，其中目录结构如下所示。

{% highlight text %}
.
|-- asset                      # view资产管理
|   |-- __init__.py
|   `-- views.py
|-- config                     # 配置文件目录，可能包括nginx、uwsgi等配置文件
|   |-- flask_product.conf     # 生产环境配置文件
|   `-- flask_debug.conf       # 测试用配置文件
|-- README
|-- requirements.txt           # 依赖三方
|-- run.py                     # 启动脚本，wsgi入口以及调试使用
|-- static                     # 静态文件，包括了css、js等，可以通过动静分离提高性能
|   |-- css
|   |   `-- bootstrap.min.css
|   `-- js
|       |-- bootstrap.min.js
|       `-- jquery.min.js
|-- templates                  # Jinja2用的模版，与上述的view对应
|   |-- asset
|   |   `-- index.html
|   |-- user
|   |   `-- index.html
|   `-- index.html
`-- user                       # view用户管理页面
    |-- __init__.py
    `-- views.py
{% endhighlight %}

如下是各个文件的内容，可以对号入座。

{% highlight python %}
# -*- coding:utf-8 -*-
# FILE: run.py
from flask import Flask
from asset import asset
from user import user

app = Flask(
    __name__,
    template_folder = 'templates', # 模板路径，相对路径或是绝对路径
    static_folder = 'static',      # 静态文件前缀
)

app.register_blueprint(asset, url_prefix='/asset')
app.register_blueprint(user)

if __name__ == '__main__':
    app.run(host='0.0.0.0', port=8000, debug=True)
{% endhighlight %}

{% highlight python %}
# FILE: asset/__init__.py
from flask import Blueprint
asset = Blueprint(
    'asset',
    __name__,
    #template_folder = '/opt/templates/',
    #static_folder = '/opt/static/',
)
import views
{% endhighlight %}

{% highlight python %}
# FILE: asset/views.py
from flask import render_template
from asset import asset
@asset.route('/')
def index():
    print'__name__', __name__
    return render_template('asset/index.html')
{% endhighlight %}

{% highlight python %}
echo 'This is asset index page...'
{% endhighlight %}

{% highlight python %}
# FILE: user/__init__.py
from flask import Blueprint
user = Blueprint(
    'user',
    __name__,
)
import views
{% endhighlight %}

{% highlight python %}
# FILE: user/views.py
from flask import  render_template
from user import user

@user.route('/')
def index():
    print '__name__', __name__
    return render_template('user/index.html')
{% endhighlight %}

{% highlight python %}
echo 'This is user index page...'
{% endhighlight %}

{% highlight python %}
Flask==0.10.1
Flask-Bootstrap==3.3.5.6
Flask-Login==0.2.11
Flask-SQLAlchemy==2.0
Flask-WTF==0.12
{% endhighlight %}

如上是 requirements.txt 文件配置，主要作用是记录需要的依赖包，新环境部署时安装如下依赖包即可，pip install -r requirements.txt 。


## 三方插件

Flask 一个好处就是其灵活性，很多功能都可以直接通过三方的插件进行扩展，在此列举一些比较常见的一些插件。

### Flask-WTF

这个是对 WTForms 的封装，包括了 CSRF、文件上传、以及 reCAPTCHA，详细描述可以参考 [Flask-WTF 中文文档](http://docs.jinkan.org/docs/flask-wtf/quickstart.html)，也可以查看 [Flask-WTF 英文文档](http://flask-wtf.readthedocs.io/en/latest/)。

直接通过如下的方式进行安装。

{% highlight text %}
# pip install flask-wtf
{% endhighlight %}

也可以从 [Github - lepture/flask-wtf](https://github.com/lepture/flask-wtf) 上下载，通过源码安装。

### Flask-Login

这是一个登陆认证使用的基本插件，可以参考 [Flask-Login 中文文档](http://docs.jinkan.org/docs/flask-login/)，也可以查看 [Flask-Login 英文文档](http://flask-login.readthedocs.org/en/latest/)。直接通过如下的方式进行安装。

{% highlight text %}
# pip install flask-login
{% endhighlight %}

可以从 [Github - maxcountryman/flask-login](https://github.com/maxcountryman/flask-login) 上下载。

### Flask-Security

这一模块可以支持会话管理、权限分类、密码加密、基本的 HTTP 鉴权、Token 鉴权、邮箱验证、密码重置、用户注册、用户跟踪等等。

类似，可以通过如下方式安装。

{% highlight text %}
# pip install flask-security
{% endhighlight %}

可以从 [Flask Security](https://pythonhosted.org/Flask-Security/index.html) 上下载。

### Flask-bcrypt

这个实际上只是简单的对 bcrypt 的封装，而 bcrypt 依赖 cffi 依赖 pycparser，这个通常用在用户的加密认证。

{% highlight text %}
# pip install flask-bcrypt
{% endhighlight %}

<!--

flask-admin|
Flask-DebugToolbar|
Flask-Login|
Flask-Cache|
flask-sqlalchemy|
Flask-mako|
Flask-Genshi|
WTForms





## Flask-Restful

{% highlight text %}
# pip install flask-restful
{% endhighlight %}

## Flask-SQLAlchemy

# pip install flask-sqlalchemy

http://flask-sqlalchemy.pocoo.org

https://github.com/mitsuhiko/flask-sqlalchemy

http://www.pythondoc.com/flask-sqlalchemy/index.html



# Flask-Admin

https://github.com/flask-admin/flask-admin


flask-script

Flask 教程 用户登陆
Flask 的 jinja2 模版引擎 上下文环境
从源码角度理解nginx和uwsgi的通讯过程

如何查看js(jquery)绑定事件的函数代码  segmentfault

flask源码阅读 github Skycrab/pyython_study

https://bourgeois.me/rest/

http://wsgi.readthedocs.io/en/latest/learn.html
https://spacewander.github.io/explore-flask-zh/index.html


blog.tonyseek.com/post/the-context-mechanism-of-flask/

ELK(ElasticSearch Logstash Kibana)
http://pythonhosted.org/Flask-User/index.html
-->

## 参考

三个模块的英文文档可以参考 [flask.pocoo.org](http://flask.pocoo.org/)、[werkzeug.pocoo.org](http://werkzeug.pocoo.org/)、[jinja.pocoo.org](http://jinja.pocoo.org/)，而源码可以直接查看 [github - flask](https://github.com/pallets/flask)、[github - werkzeug](https://github.com/pallets/werkzeug)、[github - jinja](https://github.com/pallets/jinja)；中文资料可以参考 [Flask 中文文档](http://docs.jinkan.org/docs/flask/index.html)、[Werkzeug 文档概览](http://werkzeug-docs-cn.readthedocs.io/zh_CN/latest/)、[欢迎来到 Jinja2](http://docs.jinkan.org/docs/jinja2/) 。

一个不错的示例，比较适合大型项目的设计参考，[Flask 代码模式](http://docs.jinkan.org/docs/flask/patterns/index.html) ，还有一个就是 [欢迎来到 Flask 的世界](http://dormousehole.readthedocs.io/en/latest/) 。

<!--

[flask route设计思路](https://segmentfault.com/a/1190000004213652)


Explore Flask
https://exploreflask.com/en/latest/


https://github.com/ssy341/datatables-cn/blob/gh-pages/index.html
-->

{% highlight python %}
{% endhighlight %}
