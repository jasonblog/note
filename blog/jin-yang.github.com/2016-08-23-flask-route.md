---
title: Flask 路由控制
layout: post
comments: true
language: chinese
category: [webserver]
keywords: flaks,路由控制,route
description: 實際上，flask 框架 (或者以 Werkzeug 為基礎構建的應用) 所做的事情就是，將 URL 映射到業務邏輯的處理代碼。在這篇文章中，我們看看 flask 框架是如何做映射的。
---

實際上，flask 框架 (或者以 Werkzeug 為基礎構建的應用) 所做的事情就是，將 URL 映射到業務邏輯的處理代碼。

在這篇文章中，我們看看 flask 框架是如何做映射的。

<!-- more -->

## 簡介

flask 中對路由的定義示例如下，也即通過 @app.route('URL') 修飾器來定義路由表。

{% highlight python %}
@app.route('/user/<name>')
def user(name):
    return 'Hello, %s!' % name
{% endhighlight %}

實際上，也可以通過 add_url_rule() 函數進行設置，兩者的效果是一樣的。

{% highlight python %}
def user(name):
    return 'Hello, %s!' % name
app.add_url_rule('/user/<name>', 'user', user)
{% endhighlight %}

其中，在如上的路由定義時，可以進行類型轉換，默認 URL 轉換器只支持 string、int、float、path 4 種類型，並不支持正則，默認是 string，其中 path 與 string 類似，區別在於其支持斜線。

### 路由流程

很多人會認為，如果用戶訪問 http://www.foobar.com/user/andy 這個 URL，flask 會找到相應的函數，然後傳遞 name='andy'，執行這個函數並返回值。但是，flask 真的是直接根據路由查詢視圖函數麼？

實際上，會有三個元素的的對應關係，也就是 view_func、endpoint、url 之間的對應關係。

在 flask 中，view_func 與 url 並不是直接對應的，當用戶發送一個 url 請求過來時，先通過 rule 找到 endpoint(url_map)，然後通過 endpoint 再去找到對應的 view_func。通常，endpoint 和視圖函數名一樣。

### 路由集中到一起

python 的裝飾器其實就是返回函數的函數，外層函數的參數就是裝飾器的參數，內層函數的參數是被裝飾的函數，所以直接調用 app.route 應該會返回一個函數，直接把 handler 傳進去就可以註冊它。

{% highlight python %}
app.route(pattern)(handler)
{% endhighlight %}


### endpoint 使用

最主要的使用是使用 url_for() 做反向查找，也就是當你修改 URL 指向時，不會影響到原有的代碼。

{% highlight python %}
@app.route('/')
def index():
    print url_for('user', name='Andy') # /user/Andy
@app.route('/user/<name>')
def user(name):
    return 'Hello, %s!' % name
{% endhighlight %}

據說，之所以使用 endpoint，有個原因是 blueprint，如下是一個 blueprint 的簡單示例。

{% highlight python %}
# FILE: main.py
# -*- coding:utf-8 -*-
from flask import Flask, Blueprint
from admin import admin
from user import user
from pprint import pprint
app = Flask(__name__)
app.register_blueprint(admin, url_prefix='/admin')
app.register_blueprint(user, url_prefix='/user')
pprint(app.view_functions)
pprint(app.url_map)
print url_for('admin.greeting') # /admin/greeting
print url_for('user.greeting')  # /user/greeting
app.run()

# FILE: admin.py
from flask import Blueprint
admin = Blueprint('admin', __name__)
@admin.route('/greeting')
def greeting():
    return 'Hello, administrative user!'

# FILE: user.py
from flask import Blueprint
user = Blueprint('user', __name__)
@user.route('/greeting')
def greeting():
    return 'Hello, lowly normal user!'
{% endhighlight %}

好像還有其它原因，為什麼非要添加一箇中間層。


## 源碼解析

這個修飾器在 app.py 中定義，實際上也可以等同於 add_url_rule() 方法的調用。

{% highlight python %}
def route(self, rule, **options):
    def decorator(f):
        endpoint = options.pop('endpoint', None)
        self.add_url_rule(rule, endpoint, f, **options)
        return f
    return decorator
{% endhighlight %}

還是以上面的示例為例，view 定義如下。

{% highlight python %}
@app.route('/user/<name>')
def user(name):
    return 'Hello, %s!' % name
{% endhighlight %}

實際上這段代碼在去掉修飾符之後，等同於如下代碼，也就是最終會調用 add_url_rule() 函數。

{% highlight python %}
user = app.route('/user/<name>')(user)
{% endhighlight %}

最終等價於如下的代碼。

{% highlight python %}
def user(name):
    return 'Hello, %s!' % name
app.add_url_rule('/user/<name>', None, user)
{% endhighlight %}

在修飾符 route 的實現中，其中有個 endpoint 參數，這是用於 url_for() 的反向查找，默認為 None，但 flask 在後面會賦值為函數本身的名稱。

上述調用，會執行 add_url_rule()，該函數的定義如下：

{% highlight text %}
add_url_rule(self, rule, endpoint=None, view_func=None, **options)
       rule: 匹配的路由地址
  view_func: 視圖函數
   endpoint: 接下來會重點講解的
{% endhighlight %}

其實現為：

{% highlight python %}
def add_url_rule(self, rule, endpoint=None, view_func=None, **options):
    # ...省略對參數的處理
    rule = self.url_rule_class(rule, methods=methods, **options)
    rule.provide_automatic_options = provide_automatic_options
    self.url_map.add(rule)
{% endhighlight %}

再跟進發現 rule 由 url_rule_class 實例化得到，包括後面的 url_map 都是從 werkzeug.routing 導入的，暫時就不深入了。


### 映射關係

在源碼中可以發現，每個 app 會有兩個主要的變量：

* view_functions，這是一個字典，存儲 endpoint-view_func 鍵值對。

* url_map，它是一個在 werkzeug/routing.py 中實現的 Map 類，包含了一個列表，保存了 role 實例。

在程序運行之前，add_url_rule() 就已經將上述的兩個變量設置完成了，在代碼中可以通過如下函數測試：

{% highlight python %}
# -*- coding:utf-8 -*-
from flask import Flask
from pprint import pprint

app = Flask(__name__)
@app.route('/foobar', endpoint='test')
def foobar():
    pass
@app.route('/', endpoint='index')
def hello_world():
    pass
if __name__ == '__main__':
    pprint(app.view_functions)
    pprint(app.url_map)
    app.run()
{% endhighlight %}

從上述代碼的輸出可以看出，url_map 存儲的是 url 與 endpoint 的映射!


<!--
一個 endpoint 只能對應於一個 view_func，在通過 add_url_rule() 函數註冊的時候，如果不指定 endpoint，那麼 endpoint 就會默認為函數名字，如果同一個 endpoint 於多個 url 註冊的話，會有問題，詳見代碼中，會判斷之前已經對應到的跟現在是不是一個，如果不是的話，那麼就要拋出異常。然後再去訪問這些url當然是肯定不行的啦。有時間會慢慢擴充這部分的內容。


通過閱讀這段代碼，發現Flask在每個實例裡定義了一個Map類（從werkzeug導入）url_map，一個字典view_functions。

註冊路由分為兩步。

第一步是通過一個Rule類作為入參調用Map類的add方法向url_map裡添加一個路由規則，url_map裡有一個列表_rules，用來存儲實例下所有的路由規則，這個列表的每一個元素都是一個Rule類，其次，url_map中還有一個字典_rules_by_endpoint，這個字典也是存儲路由規則的，不過它按照endpoint把它們分開存儲了，key值就是endpoint，value是個Rule類。endpoint用來生成URL，可以是字符串、數字甚至是函數，這裡使用的是註冊的路由處理函數的名字。在向url_map添加路由規則的時候，會觸發Rule類實例的綁定方法bind()，這個方法把url_map加入到Rule實例的成員變量中，並觸發編譯方法compile()，把rule規則和各路由選項解析生成正則表達式並存儲進Rule實例中。

第二步是把需要註冊的路由處理函數加入到Flask實例的字典view_functions中，key即為函數名。
到這裡，flask生成了一個URL route系統，具體如何命中，繼續研究代碼。
-->






## 轉換器

通過運行如下代碼，可以獲取 Flask 默認支持的轉換器。
{% highlight python %}
from flask import Flask
from pprint import pprint
if __name__ == '__main__':
    app = Flask(__name__)
    pprint(app.url_map.converters)
{% endhighlight %}

那如何創建一個新的路由解析轉化器呢？ 在源碼的 app.py 註釋裡有個簡單的例子，這個例子是創建了一個 list 類型的路由轉換器：

{% highlight python %}
from werkzeug.routing import BaseConverter

class ListConverter(BaseConverter):
    def to_python(self, value):
        return value.split(',')
    def to_url(self, values):
        return ','.join(BaseConverter.to_url(value)
                        for value in values)

app = Flask(__name__)
app.url_map.converters['list'] = ListConverter
{% endhighlight %}

類似上述的方式，我們可以很容易創建一個正則解析器。

因為之前的轉換器最終也是轉換成正則的，只需要將獲取的值設置為regex屬性就可以使用了：

{% highlight python %}
from flask import Flask
from werkzeug.routing import BaseConverter
class RegexConverter(BaseConverter):
    def __init__(self, map, *args):
        self.map = map
        self.regex = args[0]

app = Flask(__name__)
app.url_map.converters['regex'] = RegexConverter

@app.route('/view/<regex("[a-zA-Z0-9]+"):uuid>/')
def view(uuid):
    """
    url: /view/1010000000125259/
    result: view uuid:1010000000125259
    """
    return "view uuid: %s" % (uuid)

@app.route('/<regex(".*"):url>')
def not_found(url):
    """
    url: /hello
    result: not found: 'hello'
    """
    return "not found: '%s'" % (url)

if __name__ == '__main__':
    app.run()
{% endhighlight %}

Flask（其實是 Werkzeug）使用 Converter 把 URL 中特殊部分 (`<regex("[a-zA-Z0-9]+"):uuid>`) 轉換為 Python 變量，通用格式是 `<converter(args):var_name>`，在這個例子中，一個叫 regex 的 converter 把 URL 中相應字段轉換為 view() 中的 uuid 變量。

<!--
因此，converter的regex就是用來判斷這串字符是否符合轉換格式，ok就轉換，否則跳過。對於IntegerConverter來說，"abc"顯然無能為力。也就是說，其實Werkzeug的路由本來就支持用正則表達式。string、int、float等都是從它派生出來的（可以看看IntegerConverter等built-in Converter的regex）。
-->



{% highlight python %}
{% endhighlight %}
