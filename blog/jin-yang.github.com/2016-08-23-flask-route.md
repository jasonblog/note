---
title: Flask 路由控制
layout: post
comments: true
language: chinese
category: [webserver]
keywords: flaks,路由控制,route
description: 实际上，flask 框架 (或者以 Werkzeug 为基础构建的应用) 所做的事情就是，将 URL 映射到业务逻辑的处理代码。在这篇文章中，我们看看 flask 框架是如何做映射的。
---

实际上，flask 框架 (或者以 Werkzeug 为基础构建的应用) 所做的事情就是，将 URL 映射到业务逻辑的处理代码。

在这篇文章中，我们看看 flask 框架是如何做映射的。

<!-- more -->

## 简介

flask 中对路由的定义示例如下，也即通过 @app.route('URL') 修饰器来定义路由表。

{% highlight python %}
@app.route('/user/<name>')
def user(name):
    return 'Hello, %s!' % name
{% endhighlight %}

实际上，也可以通过 add_url_rule() 函数进行设置，两者的效果是一样的。

{% highlight python %}
def user(name):
    return 'Hello, %s!' % name
app.add_url_rule('/user/<name>', 'user', user)
{% endhighlight %}

其中，在如上的路由定义时，可以进行类型转换，默认 URL 转换器只支持 string、int、float、path 4 种类型，并不支持正则，默认是 string，其中 path 与 string 类似，区别在于其支持斜线。

### 路由流程

很多人会认为，如果用户访问 http://www.foobar.com/user/andy 这个 URL，flask 会找到相应的函数，然后传递 name='andy'，执行这个函数并返回值。但是，flask 真的是直接根据路由查询视图函数么？

实际上，会有三个元素的的对应关系，也就是 view_func、endpoint、url 之间的对应关系。

在 flask 中，view_func 与 url 并不是直接对应的，当用户发送一个 url 请求过来时，先通过 rule 找到 endpoint(url_map)，然后通过 endpoint 再去找到对应的 view_func。通常，endpoint 和视图函数名一样。

### 路由集中到一起

python 的装饰器其实就是返回函数的函数，外层函数的参数就是装饰器的参数，内层函数的参数是被装饰的函数，所以直接调用 app.route 应该会返回一个函数，直接把 handler 传进去就可以注册它。

{% highlight python %}
app.route(pattern)(handler)
{% endhighlight %}


### endpoint 使用

最主要的使用是使用 url_for() 做反向查找，也就是当你修改 URL 指向时，不会影响到原有的代码。

{% highlight python %}
@app.route('/')
def index():
    print url_for('user', name='Andy') # /user/Andy
@app.route('/user/<name>')
def user(name):
    return 'Hello, %s!' % name
{% endhighlight %}

据说，之所以使用 endpoint，有个原因是 blueprint，如下是一个 blueprint 的简单示例。

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

好像还有其它原因，为什么非要添加一个中间层。


## 源码解析

这个修饰器在 app.py 中定义，实际上也可以等同于 add_url_rule() 方法的调用。

{% highlight python %}
def route(self, rule, **options):
    def decorator(f):
        endpoint = options.pop('endpoint', None)
        self.add_url_rule(rule, endpoint, f, **options)
        return f
    return decorator
{% endhighlight %}

还是以上面的示例为例，view 定义如下。

{% highlight python %}
@app.route('/user/<name>')
def user(name):
    return 'Hello, %s!' % name
{% endhighlight %}

实际上这段代码在去掉修饰符之后，等同于如下代码，也就是最终会调用 add_url_rule() 函数。

{% highlight python %}
user = app.route('/user/<name>')(user)
{% endhighlight %}

最终等价于如下的代码。

{% highlight python %}
def user(name):
    return 'Hello, %s!' % name
app.add_url_rule('/user/<name>', None, user)
{% endhighlight %}

在修饰符 route 的实现中，其中有个 endpoint 参数，这是用于 url_for() 的反向查找，默认为 None，但 flask 在后面会赋值为函数本身的名称。

上述调用，会执行 add_url_rule()，该函数的定义如下：

{% highlight text %}
add_url_rule(self, rule, endpoint=None, view_func=None, **options)
       rule: 匹配的路由地址
  view_func: 视图函数
   endpoint: 接下来会重点讲解的
{% endhighlight %}

其实现为：

{% highlight python %}
def add_url_rule(self, rule, endpoint=None, view_func=None, **options):
    # ...省略对参数的处理
    rule = self.url_rule_class(rule, methods=methods, **options)
    rule.provide_automatic_options = provide_automatic_options
    self.url_map.add(rule)
{% endhighlight %}

再跟进发现 rule 由 url_rule_class 实例化得到，包括后面的 url_map 都是从 werkzeug.routing 导入的，暂时就不深入了。


### 映射关系

在源码中可以发现，每个 app 会有两个主要的变量：

* view_functions，这是一个字典，存储 endpoint-view_func 键值对。

* url_map，它是一个在 werkzeug/routing.py 中实现的 Map 类，包含了一个列表，保存了 role 实例。

在程序运行之前，add_url_rule() 就已经将上述的两个变量设置完成了，在代码中可以通过如下函数测试：

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

从上述代码的输出可以看出，url_map 存储的是 url 与 endpoint 的映射!


<!--
一个 endpoint 只能对应于一个 view_func，在通过 add_url_rule() 函数注册的时候，如果不指定 endpoint，那么 endpoint 就会默认为函数名字，如果同一个 endpoint 于多个 url 注册的话，会有问题，详见代码中，会判断之前已经对应到的跟现在是不是一个，如果不是的话，那么就要抛出异常。然后再去访问这些url当然是肯定不行的啦。有时间会慢慢扩充这部分的内容。


通过阅读这段代码，发现Flask在每个实例里定义了一个Map类（从werkzeug导入）url_map，一个字典view_functions。

注册路由分为两步。

第一步是通过一个Rule类作为入参调用Map类的add方法向url_map里添加一个路由规则，url_map里有一个列表_rules，用来存储实例下所有的路由规则，这个列表的每一个元素都是一个Rule类，其次，url_map中还有一个字典_rules_by_endpoint，这个字典也是存储路由规则的，不过它按照endpoint把它们分开存储了，key值就是endpoint，value是个Rule类。endpoint用来生成URL，可以是字符串、数字甚至是函数，这里使用的是注册的路由处理函数的名字。在向url_map添加路由规则的时候，会触发Rule类实例的绑定方法bind()，这个方法把url_map加入到Rule实例的成员变量中，并触发编译方法compile()，把rule规则和各路由选项解析生成正则表达式并存储进Rule实例中。

第二步是把需要注册的路由处理函数加入到Flask实例的字典view_functions中，key即为函数名。
到这里，flask生成了一个URL route系统，具体如何命中，继续研究代码。
-->






## 转换器

通过运行如下代码，可以获取 Flask 默认支持的转换器。
{% highlight python %}
from flask import Flask
from pprint import pprint
if __name__ == '__main__':
    app = Flask(__name__)
    pprint(app.url_map.converters)
{% endhighlight %}

那如何创建一个新的路由解析转化器呢？ 在源码的 app.py 注释里有个简单的例子，这个例子是创建了一个 list 类型的路由转换器：

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

类似上述的方式，我们可以很容易创建一个正则解析器。

因为之前的转换器最终也是转换成正则的，只需要将获取的值设置为regex属性就可以使用了：

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

Flask（其实是 Werkzeug）使用 Converter 把 URL 中特殊部分 (`<regex("[a-zA-Z0-9]+"):uuid>`) 转换为 Python 变量，通用格式是 `<converter(args):var_name>`，在这个例子中，一个叫 regex 的 converter 把 URL 中相应字段转换为 view() 中的 uuid 变量。

<!--
因此，converter的regex就是用来判断这串字符是否符合转换格式，ok就转换，否则跳过。对于IntegerConverter来说，"abc"显然无能为力。也就是说，其实Werkzeug的路由本来就支持用正则表达式。string、int、float等都是从它派生出来的（可以看看IntegerConverter等built-in Converter的regex）。
-->



{% highlight python %}
{% endhighlight %}
