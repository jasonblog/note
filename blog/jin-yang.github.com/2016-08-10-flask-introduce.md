---
title: Flask 簡介
layout: post
comments: true
language: chinese
category: [python,program,webserver]
keywords: python,flask
description: Flask 是一個使用 Python 編寫的輕量級 Web 應用框架，依賴於兩個外部庫：WSGI 工具箱採用 Werkzeug；模板引擎則使用 Jinja2 。接下來簡單介紹一下相關內容，一步步看看如何使用。
---

Flask 是一個使用 Python 編寫的輕量級 Web 應用框架，依賴於兩個外部庫：WSGI 工具箱採用 Werkzeug；模板引擎則使用 Jinja2 。

接下來簡單介紹一下相關內容，一步步看看如何使用。

<!-- more -->

![flask logo]({{ site.url }}/images/python/flask.svg){: .pull-center}

## 簡介

Flask 是基於 werkzeug 實現的，這是一個基於 wsgi 協議實現的底層函數庫，也就是實現了基本的 HTTP 請求和處理，實際上我們都能在 werkzeug 的基礎上來實現自己的 Web 框架。

### 安裝

可以直接從 [github - flask](https://github.com/pallets/flask) 下載源碼，或者通過 pip 方式安裝。

{% highlight text %}
# pip install flask             # 或者easy_install flask
{% endhighlight %}

當然也可以通過源碼進行安裝，實際上除了上述的兩個核心的功能庫之外，還依賴 click 庫，用來簡化命令行處理；itsdangerous 用來處理 cookie 相關信息。

{% highlight text %}
# python setup.py install       # 直接在源碼目錄下執行安裝命令
{% endhighlight %}

### Hello World !

安裝完成之後可以通過如下方式進行測試，下面是一個簡單的示例。

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

然後瀏覽器訪問 [http://localhost:5000](http://localhost:5000) 或者 curl http://127.1:5000 即可，最終返回如下內容。

![flask]({{ site.url }}/images/webserver/flask_flask.png "flask"){: .pull-center}


## Flask 配置

在每個 Flask 對象中，都有一個 config 屬性，用來保存配置；而 config 實際上是繼承於字典，源碼在 config.py 中實現。

如果要修改配置項，可以直接通過如下的方式進行修改。

{% highlight text %}
----- 1. 在Python中直接配置
app.config['HOST']='www.foobar.com'
print app.config.get('HOST')

----- 2. 通過單獨Python配置文件配置 *****
app.config.from_pyfile('default_config.py')

# FILE: default_config.py
HOST = 'localhost'
PORT = 5000
DEBUG = True

----- 3. 最好設置一個config.py文件，從中讀取該內容
CSRF_ENABLED = True
SECRET_KEY = 'you-will-never-guess'

app.config.from_object('config')

----- 3. 通過環境變量加載配置
export MyAppConfig=/path/to/settings.cfg

app.config.from_envvar('MyAppConfig')
{% endhighlight %}

其中，有一部分的配置項是 Flask 所使用的，例如，SECRET_KEY 是 Flask 中比較重要的一個配置值，包括 Session、Cookies 以及一些第三方擴展都會用到該值，應該儘可能設置為一個很難猜到的隨機值。通常操作系統可以基於一個密碼隨機生成器來生成漂亮的隨機值，該值可以用來做密鑰。

另外，如果需要，還可以自己配置一些應用中將會用到的配置，從而可以全局訪問。

## Blueprint

通過藍圖 (blueprint) 可以把一個應用分解為一個藍圖的集合，一個項目可以實例化一個應用對象，初始化幾個擴展，並註冊一集合的藍圖，從而方便管理。

如下，是一個簡單的示例，其中目錄結構如下所示。

{% highlight text %}
.
|-- asset                      # view資產管理
|   |-- __init__.py
|   `-- views.py
|-- config                     # 配置文件目錄，可能包括nginx、uwsgi等配置文件
|   |-- flask_product.conf     # 生產環境配置文件
|   `-- flask_debug.conf       # 測試用配置文件
|-- README
|-- requirements.txt           # 依賴三方
|-- run.py                     # 啟動腳本，wsgi入口以及調試使用
|-- static                     # 靜態文件，包括了css、js等，可以通過動靜分離提高性能
|   |-- css
|   |   `-- bootstrap.min.css
|   `-- js
|       |-- bootstrap.min.js
|       `-- jquery.min.js
|-- templates                  # Jinja2用的模版，與上述的view對應
|   |-- asset
|   |   `-- index.html
|   |-- user
|   |   `-- index.html
|   `-- index.html
`-- user                       # view用戶管理頁面
    |-- __init__.py
    `-- views.py
{% endhighlight %}

如下是各個文件的內容，可以對號入座。

{% highlight python %}
# -*- coding:utf-8 -*-
# FILE: run.py
from flask import Flask
from asset import asset
from user import user

app = Flask(
    __name__,
    template_folder = 'templates', # 模板路徑，相對路徑或是絕對路徑
    static_folder = 'static',      # 靜態文件前綴
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

如上是 requirements.txt 文件配置，主要作用是記錄需要的依賴包，新環境部署時安裝如下依賴包即可，pip install -r requirements.txt 。


## 三方插件

Flask 一個好處就是其靈活性，很多功能都可以直接通過三方的插件進行擴展，在此列舉一些比較常見的一些插件。

### Flask-WTF

這個是對 WTForms 的封裝，包括了 CSRF、文件上傳、以及 reCAPTCHA，詳細描述可以參考 [Flask-WTF 中文文檔](http://docs.jinkan.org/docs/flask-wtf/quickstart.html)，也可以查看 [Flask-WTF 英文文檔](http://flask-wtf.readthedocs.io/en/latest/)。

直接通過如下的方式進行安裝。

{% highlight text %}
# pip install flask-wtf
{% endhighlight %}

也可以從 [Github - lepture/flask-wtf](https://github.com/lepture/flask-wtf) 上下載，通過源碼安裝。

### Flask-Login

這是一個登陸認證使用的基本插件，可以參考 [Flask-Login 中文文檔](http://docs.jinkan.org/docs/flask-login/)，也可以查看 [Flask-Login 英文文檔](http://flask-login.readthedocs.org/en/latest/)。直接通過如下的方式進行安裝。

{% highlight text %}
# pip install flask-login
{% endhighlight %}

可以從 [Github - maxcountryman/flask-login](https://github.com/maxcountryman/flask-login) 上下載。

### Flask-Security

這一模塊可以支持會話管理、權限分類、密碼加密、基本的 HTTP 鑑權、Token 鑑權、郵箱驗證、密碼重置、用戶註冊、用戶跟蹤等等。

類似，可以通過如下方式安裝。

{% highlight text %}
# pip install flask-security
{% endhighlight %}

可以從 [Flask Security](https://pythonhosted.org/Flask-Security/index.html) 上下載。

### Flask-bcrypt

這個實際上只是簡單的對 bcrypt 的封裝，而 bcrypt 依賴 cffi 依賴 pycparser，這個通常用在用戶的加密認證。

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

Flask 教程 用戶登陸
Flask 的 jinja2 模版引擎 上下文環境
從源碼角度理解nginx和uwsgi的通訊過程

如何查看js(jquery)綁定事件的函數代碼  segmentfault

flask源碼閱讀 github Skycrab/pyython_study

https://bourgeois.me/rest/

http://wsgi.readthedocs.io/en/latest/learn.html
https://spacewander.github.io/explore-flask-zh/index.html


blog.tonyseek.com/post/the-context-mechanism-of-flask/

ELK(ElasticSearch Logstash Kibana)
http://pythonhosted.org/Flask-User/index.html
-->

## 參考

三個模塊的英文文檔可以參考 [flask.pocoo.org](http://flask.pocoo.org/)、[werkzeug.pocoo.org](http://werkzeug.pocoo.org/)、[jinja.pocoo.org](http://jinja.pocoo.org/)，而源碼可以直接查看 [github - flask](https://github.com/pallets/flask)、[github - werkzeug](https://github.com/pallets/werkzeug)、[github - jinja](https://github.com/pallets/jinja)；中文資料可以參考 [Flask 中文文檔](http://docs.jinkan.org/docs/flask/index.html)、[Werkzeug 文檔概覽](http://werkzeug-docs-cn.readthedocs.io/zh_CN/latest/)、[歡迎來到 Jinja2](http://docs.jinkan.org/docs/jinja2/) 。

一個不錯的示例，比較適合大型項目的設計參考，[Flask 代碼模式](http://docs.jinkan.org/docs/flask/patterns/index.html) ，還有一個就是 [歡迎來到 Flask 的世界](http://dormousehole.readthedocs.io/en/latest/) 。

<!--

[flask route設計思路](https://segmentfault.com/a/1190000004213652)


Explore Flask
https://exploreflask.com/en/latest/


https://github.com/ssy341/datatables-cn/blob/gh-pages/index.html
-->

{% highlight python %}
{% endhighlight %}
