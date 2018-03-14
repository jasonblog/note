---
title: Flask 常見示例
layout: post
comments: true
language: chinese
category: [python,program,webserver]
keywords: flask,示例
description: 記錄 Flask 常見的示例，可以用來作為參考使用。
---

記錄 Flask 常見的示例，可以用來作為參考使用。

<!-- more -->

## 最簡單的應用

{% highlight python %}
from flask import Flask
app = Flask(__name__)

@app.route('/')                                  # 指定多個URL
@app.route('/index',  methods=['GET', 'POST'])   # 指定多個方法
def hello_world():
    return 'Hello World!'

if __name__ == '__main__':
    app.run(host='0.0.0.0', port=8080, debug=True, processes=10)
{% endhighlight %}

指定監聽地址，端口號，並使用調試模式，當然只用於調試。

## 獲取參數

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



## POST 請求處理

假設以用戶註冊為例，需要向服務器發送用戶名和密碼，其中服務器側的程序如下：

{% highlight python %}
from flask import Flask, request

app = Flask(__name__)

@app.route('/register', methods=['POST'])
def register():
    print request.headers                              # 頭部信息
    print request.form                                 # 所有表單內容
    print request.form['name']                         # 獲取name參數
    print request.form.get('name')                     # 如上
    print request.form.getlist('name')                 # 獲取name的列表
    print request.form.get('nickname', default='fool') # 參數無則返回默認值
    return 'welcome'

if __name__ == '__main__':
    app.run(host='0.0.0.0', port=8080, debug=True)
{% endhighlight %}

如上的 request.form 是個 ImmutableMultiDict 對象，詳細可查看源碼中的 flask.Request.form 。

其中客戶端 client.py 的內容如下：

{% highlight python %}
import requests
user_info = {'name': 'foobar', 'password': 'kidding'}
r = requests.post("http://127.1:8080/register", data=user_info)
print r.text
{% endhighlight %}


## 路由、變量轉換

可以給 URL 添加變量部分，&lt;converter:variable_name&gt; 分別指定了轉換器，以及轉換後變量的名稱。

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

默認採用的是 string(字符串，無斜線)、int(整數)、float(浮點型)、path(類似字符串，但是可以接受斜線) 。


## 重定向

Flask 中有兩種重定向的行為，分別介紹如下。

### 唯一 URL / 重定向

Flask 的 URL 規則基於 Werkzeug 的路由模塊，其中有個默認的重定向規則，也就是唯一 URL / 的重定向行為，如下所示。

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

上述的方式中，當嘗試訪問 projects 時會重定向到 projects/，而對於第二種嘗試訪問 about/ 時會產生 404 "Not Found" 錯誤。

{% highlight text %}
$ curl -L http://127.1:8080/projects
{% endhighlight %}

### 用戶重定向

另外一種，在 Flask 中通常是通過 redirect() 執行重定向操作，有如下的幾種方式。

{% highlight text %}
redirect("http://www.foobar.com", code=302)   # 直接調轉到某一個網站
redirect("/login", code=302)                  # 在本網站內調轉，通常用於多個APP調轉
redirect(url_for("login.user_login"))         # 用於同一個APP的調轉
{% endhighlight %}

code 可以選擇 301 (永久性跳轉)、302 (臨時性跳轉)，其它的還有 303、305、307 等操作。


## 構造 URL

可以通過 url_for() 來給指定的函數構造 URL，第一個參數是函數名，並接受規則變量部分的命名參數，如果是未知變量部分則會添加到 URL 末尾作為查詢參數。

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

通過 url_for() 函數，URL 構建會轉義特殊字符和 Unicode 數據，免去你很多麻煩。


## Cookies

可以通過如下方法設置 cookies，如果要使用會話可以查看下部分的內容。

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

如上，Cookies 是設置在響應對象上的，而通常視圖函數只是返回字符串，之後由 Flask 將字符串轉換為響應對象，如果要顯式地轉換，可以使用 make_response() 函數然後再進行修改。

然後通過如下方式進行測試。

{% highlight text %}
$ curl http://127.1:8080/set -c /tmp/cookie
Hello World
$ curl http://127.1:8080/get -b /tmp/cookie
Andy Dufresne
$ cat /tmp/cookis
127.1   FALSE   /       FALSE   0       username        "the username"
{% endhighlight %}

注意，上述的 cookies 中是明文保存的。


## 會話 session

會話允許你在不同請求間存儲特定用戶的信息，是在 Cookies 的基礎上實現的，並且對 Cookies 進行密鑰簽名。這意味著用戶可以查看你 Cookie 的內容，但卻不能修改它，除非用戶知道簽名的密鑰。

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

然後通過如下方式進行測試。

{% highlight text %}
$ curl http://127.1:8080/login/Andy -c /tmp/cookie
Hello World

# 即使重新加載，只要密鑰沒有改變就不會有問題
$ curl http://127.1:8080/ -b /tmp/cookie
Logged in as Andy

Andy Dufresne
$ cat /tmp/cookis
127.1   FALSE   /       FALSE   0       username        "the username"
{% endhighlight %}

上述的密鑰可以通過 os.urandom(24) 生成隨機祕密。如果發現某些請求並沒有持久化，這時需要檢查你的頁面響應中的 Cookies 的大小，並與 Web 瀏覽器所支持的大小對比。


## 執行命令

可以在 flask 中設置一些初始化操作命令，直接通過 flask 命令執行。

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

假設上述文件文 foobar.py，則可以通過如下命令執行。

{% highlight text %}
$ export FLASK_APP=foobar.py
$ flask init                      # 執行初始化命令
Initialized the environment.
$ flask run                       # 可以直接運行
 * Serving Flask app "foobar"
 * Running on http://127.0.0.1:5000/ (Press CTRL+C to quit)
{% endhighlight %}


## 消息閃現

Flask 提供了消息閃現系統，可以簡單地給用戶反饋，增加用戶體驗。 消息閃現系統通常會在請求結束時記錄信息，並在下一個（且僅在下一個）請求中訪問記錄的信息，展現這些消息通常結合要模板佈局。

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

該方法會在 session 中保存 session['_flashes'] = flashes 值，所以需要配置 secret_key，使用 flash() 方法可以閃現一條消息，要操作消息本身，使用 get_flashed_messages() 函數，並且只能在模板中也可以使用。

## 工廠模式

您可以像下面展示的這樣，從一個函數裡啟動這個應用:

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

此時如果要獲取當前配置下的應用程序，可以使用 current_app 。

{% highlight python %}
from flask import current_app, Blueprint, render_template
admin = Blueprint('admin', __name__, url_prefix='/admin')

@admin.route('/')
def index():
    return render_template(current_app.config['INDEX_TEMPLATE'])
{% endhighlight %}

所以，要使用這樣的一個應用，你必須先創建這個應用對象，這裡是一個運行此類程序的 run.py 文件的例子。

{% highlight python %}
from yourapplication import create_app
app = create_app('/path/to/config.cfg')
app.run()
{% endhighlight %}



## 處理JSON

實際上包括了兩種，一個是如何接收 json 數據，還有就是如何返回 json 數據。

### 接收JSON請求

首先是前端通過 jQuery 發送數據請求。

{% highlight javascript %}
$.ajax({
     type        : 'post',
     contentType : 'application/json; charset=UTF-8',
     url         : SCRIPT_ROOT_URL,
     dataType    : 'json',   // 注意，這裡是指期望接收到數據的格式
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
// NOTE: 如果是表單，可以使用jquery的serialize()獲取表單的全部數據
// data = $('#some-form').serialize();
{% endhighlight %}

注意，如果上述的請求中，如果不使用 JSON.stringify() 函數處理，實際上發送的請求是在請求 URL 後面添加參數，例如 /your/url/?username=foobar&address=china 。

也就是說，請求頭中的 Content-Tpye 默認是 application/x-www-form-urlencoded，所以參數會被編碼上述的格式，提交到後端，後端會當作表單數據處理。

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

### 返回JSON數據

處理 JSON 需要把請求頭和響應頭的 Content-Type 設置為 application/json 。

{% highlight python %}
# -*- coding:utf-8 -*-
from flask import Flask, request, Response
import json

app = Flask(__name__)

# 1. 直接返回JSON格式
@app.route('/json1', methods=['POST'])   #
def foobar1():
    return json.dumps({'info': 'hello world'})

# 2. 默認返回text/html格式，可以設置為返回json格式
@app.route('/json2', methods=['POST'])
def foobar2():
    print request.headers, request.json, request.get_data()
    rt = {'info':'hello '+request.json['name']}
    response = Response(json.dumps(rt),  mimetype='application/json')
    response.headers.add('Server', 'python flask')
    return response

# 3. 使用Flask庫中的jsonify
@app.route('/json3', methods=['POST'])
def foobar3():
    return jsonify({'info': 'hello world'})

if __name__ == '__main__':
    app.run(host='0.0.0.0', port=8080, debug=True)
{% endhighlight %}

客戶端內容如下。

{% highlight python %}
import requests, json

user_info = {'name': 'foobar'}
headers = {'content-type': 'application/json'}
r = requests.post("http://127.1:8080/json", data=json.dumps(user_info), headers=headers)
print r.headers
print r.json()
{% endhighlight %}

簡單來說，jsonify() 函數返回了 flask.Response() 對象，而且其中返回消息的 content-type 頭信息為 'application/json'，而 json.dumps() 則是返回一個 encoded 字符串。

另外，需要注意的是 jsonify() 的參數是 kwargs 或者 dictionaries；而 json.dumps() 還支持 lists 以及其它數據類型。

也可以使用如下的修飾符，當報錯時，會直接返回相應的報錯信息。

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

### 源碼解析

其中 get_json() 函數在源碼的 flask/wrappers.py 文件中，其代碼也很簡單，源碼如下。

{% highlight python %}
def get_json(self, force=False, silent=False, cache=True):
    if not (force or self.is_json):     # 如果不是json格式，則返回None
        return None

    request_charset = self.mimetype_params.get('charset')
    try:                                # 然後嘗試通過json.loads()加載JSON格式數據
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

而返回的 json 數據處理如下，在 flask/json.py 文件中，代碼如下。

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




## 參考

關於一些技巧，可以直接參考 [快速入門](http://docs.jinkan.org/docs/flask/quickstart.html) 所展示的示例。


{% highlight python %}
{% endhighlight %}
