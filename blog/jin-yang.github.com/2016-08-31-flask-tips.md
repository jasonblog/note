---
title: Flask 常见示例
layout: post
comments: true
language: chinese
category: [python,program,webserver]
keywords: flask,示例
description: 记录 Flask 常见的示例，可以用来作为参考使用。
---

记录 Flask 常见的示例，可以用来作为参考使用。

<!-- more -->

## 最简单的应用

{% highlight python %}
from flask import Flask
app = Flask(__name__)

@app.route('/')                                  # 指定多个URL
@app.route('/index',  methods=['GET', 'POST'])   # 指定多个方法
def hello_world():
    return 'Hello World!'

if __name__ == '__main__':
    app.run(host='0.0.0.0', port=8080, debug=True, processes=10)
{% endhighlight %}

指定监听地址，端口号，并使用调试模式，当然只用于调试。

## 获取参数

{% highlight python %}
from flask import Flask,request
app = Flask(__name__)

@app.route('/')
def hello_world():
    return {"param":request.args.get('foobar')}  #111
    #return request.args.items().__str__()       #222

@app.route('/page/<int:page>/')
def index(page=1):
    pass

if __name__ == '__main__':
    app.run(host='0.0.0.0', port=8080, debug=True)

#111
# curl 'http://127.1:8080/?foobar=hello'
# {"param": "hello"}
#222
# curl 'http://127.1:8080/?foobar=hello&foo=world&bar=hi'
[('foobar', u'hello'), ('foo', u'world'), ('bar', u'hi')]
{% endhighlight %}



## POST 请求处理

假设以用户注册为例，需要向服务器发送用户名和密码，其中服务器侧的程序如下：

{% highlight python %}
from flask import Flask, request

app = Flask(__name__)

@app.route('/register', methods=['POST'])
def register():
    print request.headers                              # 头部信息
    print request.form                                 # 所有表单内容
    print request.form['name']                         # 获取name参数
    print request.form.get('name')                     # 如上
    print request.form.getlist('name')                 # 获取name的列表
    print request.form.get('nickname', default='fool') # 参数无则返回默认值
    return 'welcome'

if __name__ == '__main__':
    app.run(host='0.0.0.0', port=8080, debug=True)
{% endhighlight %}

如上的 request.form 是个 ImmutableMultiDict 对象，详细可查看源码中的 flask.Request.form 。

其中客户端 client.py 的内容如下：

{% highlight python %}
import requests
user_info = {'name': 'foobar', 'password': 'kidding'}
r = requests.post("http://127.1:8080/register", data=user_info)
print r.text
{% endhighlight %}


## 路由、变量转换

可以给 URL 添加变量部分，&lt;converter:variable_name&gt; 分别指定了转换器，以及转换后变量的名称。

{% highlight python %}
from flask import Flask
app = Flask(__name__)

@app.route('/user/<username>')
def show_user_profile(username):
    # show the user profile for that user
    return 'User %s' % username

@app.route('/post/<int:post_id>')
def show_post(post_id):
    # show the post with the given id, the id is an integer
    return 'Post %d' % post_id

if __name__ == '__main__':
    app.run(host='0.0.0.0', port=8080, debug=True)
{% endhighlight %}

默认采用的是 string(字符串，无斜线)、int(整数)、float(浮点型)、path(类似字符串，但是可以接受斜线) 。


## 重定向

Flask 中有两种重定向的行为，分别介绍如下。

### 唯一 URL / 重定向

Flask 的 URL 规则基于 Werkzeug 的路由模块，其中有个默认的重定向规则，也就是唯一 URL / 的重定向行为，如下所示。

{% highlight python %}
from flask import Flask
app = Flask(__name__)

@app.route('/projects/')
def projects():
    return 'The project page'

@app.route('/about')
def about():
    return 'The about page'

if __name__ == '__main__':
    app.run(host='0.0.0.0', port=8080, debug=True)
{% endhighlight %}

上述的方式中，当尝试访问 projects 时会重定向到 projects/，而对于第二种尝试访问 about/ 时会产生 404 "Not Found" 错误。

{% highlight text %}
$ curl -L http://127.1:8080/projects
{% endhighlight %}

### 用户重定向

另外一种，在 Flask 中通常是通过 redirect() 执行重定向操作，有如下的几种方式。

{% highlight text %}
redirect("http://www.foobar.com", code=302)   # 直接调转到某一个网站
redirect("/login", code=302)                  # 在本网站内调转，通常用于多个APP调转
redirect(url_for("login.user_login"))         # 用于同一个APP的调转
{% endhighlight %}

code 可以选择 301 (永久性跳转)、302 (临时性跳转)，其它的还有 303、305、307 等操作。


## 构造 URL

可以通过 url_for() 来给指定的函数构造 URL，第一个参数是函数名，并接受规则变量部分的命名参数，如果是未知变量部分则会添加到 URL 末尾作为查询参数。

{% highlight python %}
from flask import Flask, url_for
app = Flask(__name__)

@app.route('/')
def index(): pass

@app.route('/login')
def login(): pass

@app.route('/user/<username>')
def profile(username): pass

with app.test_request_context():
    print url_for('index')
    print url_for('login')
    print url_for('login', next='/')
    print url_for('profile', username='John Doe')

# OUTPUT:
# /
# /login
# /login?next=%2F
# /user/John%20Doe
{% endhighlight %}

通过 url_for() 函数，URL 构建会转义特殊字符和 Unicode 数据，免去你很多麻烦。


## Cookies

可以通过如下方法设置 cookies，如果要使用会话可以查看下部分的内容。

{% highlight python %}
from flask import Flask, request, make_response
app = Flask(__name__)

@app.route('/set')
def set_username():
    resp = make_response("Hello World\n")
    resp.set_cookie('username', 'Andy Dufresne\n')
    return resp

@app.route('/get')
def index():
    # use cookies.get(key) instead of cookies[key] to not get a
    # KeyError if the cookie is missing.
    username = request.cookies.get('username')
    return username

if __name__ == '__main__':
    app.run(host='0.0.0.0', port=8080, debug=True)
{% endhighlight %}

如上，Cookies 是设置在响应对象上的，而通常视图函数只是返回字符串，之后由 Flask 将字符串转换为响应对象，如果要显式地转换，可以使用 make_response() 函数然后再进行修改。

然后通过如下方式进行测试。

{% highlight text %}
$ curl http://127.1:8080/set -c /tmp/cookie
Hello World
$ curl http://127.1:8080/get -b /tmp/cookie
Andy Dufresne
$ cat /tmp/cookis
127.1   FALSE   /       FALSE   0       username        "the username"
{% endhighlight %}

注意，上述的 cookies 中是明文保存的。


## 会话 session

会话允许你在不同请求间存储特定用户的信息，是在 Cookies 的基础上实现的，并且对 Cookies 进行密钥签名。这意味着用户可以查看你 Cookie 的内容，但却不能修改它，除非用户知道签名的密钥。

{% highlight python %}
from flask import Flask, session, escape, request
app = Flask(__name__)

@app.route('/')
def index():
    if 'username' in session:
        return 'Logged in as %s\n' % escape(session['username'])
    return 'You are not logged in\n'

@app.route('/login/<user>', methods=['GET', 'POST'])
def login(user):
    session['username'] = user
    return "Hello World\n"

@app.route('/logout')
def logout():
    # remove the username from the session if it's there
    session.pop('username', None)
    return "Finished\n"

# NOTE: set the secret key.  keep this really secret
app.secret_key = 'A0Zr98j/3yX R~HH!jmN]LWX/,?RT'

if __name__ == '__main__':
    app.run(host='0.0.0.0', port=8080, debug=True)
{% endhighlight %}

然后通过如下方式进行测试。

{% highlight text %}
$ curl http://127.1:8080/login/Andy -c /tmp/cookie
Hello World

# 即使重新加载，只要密钥没有改变就不会有问题
$ curl http://127.1:8080/ -b /tmp/cookie
Logged in as Andy

Andy Dufresne
$ cat /tmp/cookis
127.1   FALSE   /       FALSE   0       username        "the username"
{% endhighlight %}

上述的密钥可以通过 os.urandom(24) 生成随机秘密。如果发现某些请求并没有持久化，这时需要检查你的页面响应中的 Cookies 的大小，并与 Web 浏览器所支持的大小对比。


## 执行命令

可以在 flask 中设置一些初始化操作命令，直接通过 flask 命令执行。

{% highlight python %}
from flask import Flask
app = Flask(__name__)

@app.cli.command('init')
def init_command():
    print('Initialized the environment.')

@app.route('/')
def show_entries():
    return "Hello World!\n"
{% endhighlight %}

假设上述文件文 foobar.py，则可以通过如下命令执行。

{% highlight text %}
$ export FLASK_APP=foobar.py
$ flask init                      # 执行初始化命令
Initialized the environment.
$ flask run                       # 可以直接运行
 * Serving Flask app "foobar"
 * Running on http://127.0.0.1:5000/ (Press CTRL+C to quit)
{% endhighlight %}


## 消息闪现

Flask 提供了消息闪现系统，可以简单地给用户反馈，增加用户体验。 消息闪现系统通常会在请求结束时记录信息，并在下一个（且仅在下一个）请求中访问记录的信息，展现这些消息通常结合要模板布局。

{% highlight python %}
# -*- coding:utf-8 -*-
from flask import Flask, render_template, flash
app = Flask(__name__)
app.secret_key = 'key'
@app.route('/')
def show_entry():
    return render_template('layout.html')

@app.route('/flash')
def show_flash():
    flash('flash message\n')
    return render_template('layout.html')

if __name__ == '__main__':
    app.run(host='0.0.0.0', port=8080, debug=True)
{% endhighlight %}

其中 templates/layout.html 模版如下：

{% highlight python %}
{ % for message in get_flashed_messages() % } { { message } } { % endfor % }
{% endhighlight %}

该方法会在 session 中保存 session['_flashes'] = flashes 值，所以需要配置 secret_key，使用 flash() 方法可以闪现一条消息，要操作消息本身，使用 get_flashed_messages() 函数，并且只能在模板中也可以使用。

## 工厂模式

您可以像下面展示的这样，从一个函数里启动这个应用:

{% highlight python %}
def create_app(config_filename):
    app = Flask(__name__)
    app.config.from_pyfile(config_filename)
    from yourapplication.views.admin import admin
    from yourapplication.views.frontend import frontend
    app.register_blueprint(admin)
    app.register_blueprint(frontend)
    return app
{% endhighlight %}

此时如果要获取当前配置下的应用程序，可以使用 current_app 。

{% highlight python %}
from flask import current_app, Blueprint, render_template
admin = Blueprint('admin', __name__, url_prefix='/admin')

@admin.route('/')
def index():
    return render_template(current_app.config['INDEX_TEMPLATE'])
{% endhighlight %}

所以，要使用这样的一个应用，你必须先创建这个应用对象，这里是一个运行此类程序的 run.py 文件的例子。

{% highlight python %}
from yourapplication import create_app
app = create_app('/path/to/config.cfg')
app.run()
{% endhighlight %}



## 处理JSON

实际上包括了两种，一个是如何接收 json 数据，还有就是如何返回 json 数据。

### 接收JSON请求

首先是前端通过 jQuery 发送数据请求。

{% highlight javascript %}
$.ajax({
     type        : 'post',
     contentType : 'application/json; charset=UTF-8',
     url         : SCRIPT_ROOT_URL,
     dataType    : 'json',   // 注意，这里是指期望接收到数据的格式
     data        : JSON.stringify({
                  'username': $('input[name=username]').val(),
                  'address': $('input[name=address]').val()
     }),
     error       : function(xhr, err) {
         console.log('Failure: ' + err)
     },
     success     : function(data, textStatus) {
         $('#info').text(data.info);
     }
});
// NOTE: 如果是表单，可以使用jquery的serialize()获取表单的全部数据
// data = $('#some-form').serialize();
{% endhighlight %}

注意，如果上述的请求中，如果不使用 JSON.stringify() 函数处理，实际上发送的请求是在请求 URL 后面添加参数，例如 /your/url/?username=foobar&address=china 。

也就是说，请求头中的 Content-Tpye 默认是 application/x-www-form-urlencoded，所以参数会被编码上述的格式，提交到后端，后端会当作表单数据处理。

{% highlight python %}
# -*- coding:utf-8 -*-
from flask import Flask, request, Response
import json

app = Flask(__name__)

@app.route('/json', methods=['POST'])
def foobar1():
    print request.get_data()
    print request.get_json()
    return json.dumps({'info': 'hello world'})

if __name__ == '__main__':
    app.run(host='0.0.0.0', port=8080, debug=True)
{% endhighlight %}

### 返回JSON数据

处理 JSON 需要把请求头和响应头的 Content-Type 设置为 application/json 。

{% highlight python %}
# -*- coding:utf-8 -*-
from flask import Flask, request, Response
import json

app = Flask(__name__)

# 1. 直接返回JSON格式
@app.route('/json1', methods=['POST'])   #
def foobar1():
    return json.dumps({'info': 'hello world'})

# 2. 默认返回text/html格式，可以设置为返回json格式
@app.route('/json2', methods=['POST'])
def foobar2():
    print request.headers, request.json, request.get_data()
    rt = {'info':'hello '+request.json['name']}
    response = Response(json.dumps(rt),  mimetype='application/json')
    response.headers.add('Server', 'python flask')
    return response

# 3. 使用Flask库中的jsonify
@app.route('/json3', methods=['POST'])
def foobar3():
    return jsonify({'info': 'hello world'})

if __name__ == '__main__':
    app.run(host='0.0.0.0', port=8080, debug=True)
{% endhighlight %}

客户端内容如下。

{% highlight python %}
import requests, json

user_info = {'name': 'foobar'}
headers = {'content-type': 'application/json'}
r = requests.post("http://127.1:8080/json", data=json.dumps(user_info), headers=headers)
print r.headers
print r.json()
{% endhighlight %}

简单来说，jsonify() 函数返回了 flask.Response() 对象，而且其中返回消息的 content-type 头信息为 'application/json'，而 json.dumps() 则是返回一个 encoded 字符串。

另外，需要注意的是 jsonify() 的参数是 kwargs 或者 dictionaries；而 json.dumps() 还支持 lists 以及其它数据类型。

也可以使用如下的修饰符，当报错时，会直接返回相应的报错信息。

{% highlight python %}
from functools import wraps
def jsonify(f):
    @wraps(f)
    def _wrapped(*args, **kwargs):
        from flask import jsonify as flask_jsonify
        try:
            result_dict = f(*args, **kwargs)
        except Exception as e:
            result_dict = dict(status='error')
            if current_app.config['DEBUG']:
                result_dict['reason'] = str(e)
                from traceback import format_exc
                result_dict['exc_info'] = format_exc()
        return flask_jsonify(**result_dict)
    return _wrapped

@app.route('/json', methods=['POST'])
@jsonify
def foobar():
    return dict(info='hello world')
{% endhighlight %}

### 源码解析

其中 get_json() 函数在源码的 flask/wrappers.py 文件中，其代码也很简单，源码如下。

{% highlight python %}
def get_json(self, force=False, silent=False, cache=True):
    if not (force or self.is_json):     # 如果不是json格式，则返回None
        return None

    request_charset = self.mimetype_params.get('charset')
    try:                                # 然后尝试通过json.loads()加载JSON格式数据
        data = _get_data(self, cache)
        if request_charset is not None:
            rv = json.loads(data, encoding=request_charset)
        else:
            rv = json.loads(data)
    except ValueError as e:
        if silent:
            rv = None
        else:
            rv = self.on_json_loading_failed(e)
    if cache:
        self._cached_json = rv
    return rv
{% endhighlight %}

而返回的 json 数据处理如下，在 flask/json.py 文件中，代码如下。

{% highlight python %}
def jsonify(*args, **kwargs):
    indent = None
    separators = (',', ':')

    if current_app.config['JSONIFY_PRETTYPRINT_REGULAR'] and not request.is_xhr:
        indent = 2
        separators = (', ', ': ')

    if args and kwargs:
        raise TypeError('jsonify() behavior undefined when passed both args and kwargs')
    elif len(args) == 1:  # single args are passed directly to dumps()
        data = args[0]
    else:
        data = args or kwargs

    return current_app.response_class(
        (dumps(data, indent=indent, separators=separators), '\n'),
        mimetype=current_app.config['JSONIFY_MIMETYPE']
    )

{% endhighlight %}




## 参考

关于一些技巧，可以直接参考 [快速入门](http://docs.jinkan.org/docs/flask/quickstart.html) 所展示的示例。


{% highlight python %}
{% endhighlight %}
