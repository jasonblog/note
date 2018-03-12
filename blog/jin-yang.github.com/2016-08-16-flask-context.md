---
title: Flask 上下文理解
layout: post
comments: true
language: chinese
category: [webserver]
keywords: flask,context,上下文
description: Flask 中定义了两种上下文，分别为请求上下文和应用上下文，其中前者包括了 current_app (当前的应用对象)、g (处理请求时用作临时存储的对象)；后者包括了 request (请求对象，封装了 HTTP 请求的内容)、session (用于存储请求之间需要记住的值)。Flask 分发请求之前会激活请求上下文，请求处理完成后再将其删除，为了支持多个 app，Flask 中的 Context 是通过栈来实现。接下来，我们看看详细的是如何实现的。
---

Flask 中定义了两种上下文，分别为请求上下文和应用上下文，其中前者包括了 current_app (当前的应用对象)、g (处理请求时用作临时存储的对象)；后者包括了 request (请求对象，封装了 HTTP 请求的内容)、session (用于存储请求之间需要记住的值)。

Flask 分发请求之前会激活请求上下文，请求处理完成后再将其删除，为了支持多个 app，Flask 中的 Context 是通过栈来实现。

接下来，我们看看详细的是如何实现的。

<!-- more -->

## 简介

实际上所谓的上下文，在该场景下就是包括了一次请求所包含的信息，包括了从客户（一般是浏览器）发送过来的数据，例如，登陆时使用的用户名密码；以及在中间处理过程中生成的数据，例如，每次请求时我们可能会需要新建一个数据库链接。

Flask 会在接收每次请求的时候将参数自动转换为相应的对象，也就是 request、session，一般来说上下文传递可以通过参数进行，这也就意味这每个需要该上下文的函数都要多增加一个入参，为了解决这一问题，Flask 提供了一个类似于全局变量的实现方式（如下会讲到这一参数是线程安全的）。

在多线程服务器中，通过线程池处理不同客户的不同请求，当收到请求后，会选一个线程进行处理，请求的临时对象（也就是上下文）会保存在该线程对应的全局变量中（通过线程 ID 区分），这样即不干扰其他线程，又使得所有线程都可以访问。

### Flask 上下文对象

如上所述，Flask 有两种上下文，分别是：

##### RequestContext 请求上下文

* Request 请求的对象，会封装每次的 Http 请求 (environ) 的内容；

* Session 会根据请求中的 cookie，重新载入该访问者相关的会话信息。

##### AppContext 程序上下文

* g 处理请求时用作临时存储的对象，每次请求都会重设这个变量；

* current_app 当前激活程序的程序实例。

##### 生命周期

* current_app 的生命周期最长，只要当前程序实例还在运行，都不会失效。

* request 和 g 的生命周期为一次请求期间，当请求处理完成后，生命周期也就完结了。

* session 就是传统意义上的 session，只要还未失效（用户未关闭浏览器、没有超过设定的失效时间），那么不同的请求会共用同样的 session。

![Flask Context Logic]({{ site.url }}/images/python/flask-context-logic.jpg "Flask Context Logic"){: .pull-center}

### 应用上下文

关于应用上下文的内容可以参考官方文档 [flask.pocoo.org/docs/appcontext/](http://flask.pocoo.org/docs/appcontext/) 中的 The Application Context 部分的内容，在此摘抄如下：

{% highlight text %}
One of the design ideas behind Flask is that there are two different "states" in which
code is executed. The application setup state in which the application implicitly is on
the module level. It starts when the Flask object is instantiated, and it implicitly ends
when the first request comes in. While the application is in this state a few assumptions
are true:

  * the programmer can modify the application object safely.
  * no request handling happened so far
  * you have to have a reference to the application object in order to modify it, there
    is no magic proxy that can give you a reference to the application object you're
    currently creating or modifying.

In contrast, during request handling, a couple of other rules exist:

  * while a request is active, the context local objects (flask.request and others) point
    to the current request.
  * any code can get hold of these objects at any time.

There is a third state which is sitting in between a little bit. Sometimes you are dealing
with an application in a way that is similar to how you interact with applications during
request handling; just that there is no request active. Consider, for instance, that you're
sitting in an interactive Python shell and interacting with the application, or a command
line application.

The application context is what powers the current_app context local.
{% endhighlight %}

如上所述，Flask 启动时会有两种状态，开始的状态可以做些应用级的初始化；当第一个请求过来之后，就进入了另外的一个状态，详细的可以参考上面的描述。

在应用上下文中，包括了 current_app、g 两个对象，一般来说，该对象有如下的两种创建方式：

* 在处理请求的时候会自动创建；

* 通过 app_context() 手动创建，通常用在一些单元测试的场景中。

在应用上下文中，通过 flask.current_app、g 会返回当前的应用对象上下文，其中包含了当前的应用对象。注意：g 之前是在请求上下文中的，现在被迁移到了应用上下文。

下面是使用应用上下文的使用方式。

{% highlight python %}
import sqlite3
from flask import g

def get_db():
    db = getattr(g, '_database', None)
    if db is None:
        db = g._database = connect_to_database()
    return db

@app.teardown_appcontext
def teardown_db(exception):
    db = getattr(g, '_database', None)
    if db is not None:
        db.close()
{% endhighlight %}



### 请求上下文

请求上下文中包括了 request、session 变量，包含了一次请求的上下文信息，类似于应用上下文，同样可以通过两种方式进行创建。

* 当应用收到请求之后会自动创建；

* 可以通过 app.test_request_context('/route?param=value') 手动创建。

与应用上下文不同的是，请求上下文会接收一个参数代表了一次请求。另外，需要注意的是，在创建请求上下文的时候，如果没有当前的应用上下文，或者不是本应用的上下文，会自动创建一个应用上下文。

{% highlight text %}
Another thing of note is that the request context will automatically also create
an application context when it’s pushed and there is no application context for
that application so far.
{% endhighlight %}

接下来我们看看在 Flask 中如何使用请求上下文。

无论在什么地方，如果要获取当前的 request 对象，直接通过如下的方式使用即可。

{% highlight python %}
from flask import request
request.args
request.forms
request.cookies
{% endhighlight %}


## 线程安全

首先我们看看 request 是如何实现的，实际上之所以有 request 就是为了在多线程（或者协程）的环境下，各个线程可以使用各自的变量，不至于会混乱，接下来我们看看具体是如何实现的。

### 简介

实际上，Python 提供了同样类似的线程安全变量保存机制，也就是 threading.local() 方法，而在 flask 中，使用的是 werkzeug 中的 Local 实现的，详细可以参考 [werkzeug.pocoo.org/docs/local](http://werkzeug.pocoo.org/docs/local/) 。

总体来说，werkzeug 提供了与 threading.local() 相同的机制，不过是 threading 只提供了线程的安全，对于 greenlet 则无效。

### flask 定义

首先，查看源码目录下的 \_\_init\_\_.py 文件，可以发现这些变量实际上是在 globals.py 中定义，然后只是在 \_\_init\_\_.py 中引入的，其定义如下。

{% highlight python %}
def _lookup_req_object(name):
    top = _request_ctx_stack.top
    if top is None:
        raise RuntimeError('working outside of request context')
    return getattr(top, name)

_request_ctx_stack = LocalStack()
_app_ctx_stack = LocalStack()
current_app = LocalProxy(_find_app)
request = LocalProxy(partial(_lookup_req_object, 'request'))
session = LocalProxy(partial(_lookup_req_object, 'session'))
g = LocalProxy(partial(_lookup_app_object, 'g'))
{% endhighlight %}

其中，partial() 函数就是函数调用时，如果有多个参数，但其中的一个参数已经知道了，我们可以通过这个函数重新绑定一个新函数，然后去调用这个新函数，如果有默认参数的话，也可以自动对应。

示例如下：

{% highlight text %}
from functools import partial
def add2(a,b,c=2):
    return a+b+c
plus3 = partial(add2, 1)
plus3(2)  # 1+2+2=5
{% endhighlight %}

而上述的代码，实际就定义了最终调用的是 getattr(top, 'request') 还是 getattr(top, 'session') 。


如上，我们会发现 g、request、session 的调用方式都是一样的，最后都通过 getattr(top, name) 获取，这也就意味着有一个上下文对象同时保持了上述的三个对象。

另外，使用时，我们只要一处导入 request，在任何视图函数中都可以使用 request，关键是每次的都是不同的 request 对象，说明获取 request 对象肯定是一个动态的操作，不然肯定都是相同的 request。

之所以可以做到这样，主要就是 \_lookup_req_object() 和 LocalProxy 组合完成的。

### LocalProxy

LocalProxy 是 werkzeug/local.py 中定义的一个代理对象，在此处的作用是将所有的请求都发给内部的 _local 对象，如下的 \_\_slots\_\_ 用来表示所有的实例都只有一个实现。

{% highlight text %}
__slots__ = ('__local', '__dict__', '__name__')
class LocalProxy(object):
    def __init__(self, local, name=None):
        # 在上述的request中传递的就是_lookup_req_object()
        object.__setattr__(self, '_LocalProxy__local', local)
        object.__setattr__(self, '__name__', name)

    def _get_current_object(self):
        # _lookup_req_object() 中没有 __release_local__，直接进入该分支
        if not hasattr(self.__local, '__release_local__'):
            return self.__local()
        try:
            return getattr(self.__local, self.__name__)
        except AttributeError:
            raise RuntimeError('no object bound to %s' % self.__name__)

    def __getattr__(self, name):
        return getattr(self._get_current_object(), name)
{% endhighlight %}

当调用 request.method 时会调用 \_\_getattr\_\_() 函数，而实际上 request 对任何方法的调用，最终都会转化为对 _lookup_req_object() 返回对象的调用。

上述的 request.method 会调用 _request_ctx_stack.top.request.method 。

### LocalStack

既然每次 request 都不同，要么调用 top = _request_ctx_stack.top 返回的 top 不同，要么 top.request 属性不同，在 flask 中每次返回的 top 是不一样的，所以 request 的各个属性都是变化的。

现在需要看看 _request_ctx_stack = LocalStack() 了，LocalStack 其实就是简单的模拟了堆栈的基本操作，包括 push、top、pop，其内部保存了与线程相关联的本地变量，从而使其在多线程中 request 不混乱。

{% highlight python %}
class Local(object):
    __slots__ = ('__storage__', '__ident_func__')

    def __init__(self):
        object.__setattr__(self, '__storage__', {})
        object.__setattr__(self, '__ident_func__', get_ident)
    def __getattr__(self, name):
        return self.__storage__[self.__ident_func__()][name]
{% endhighlight %}

在 Local 的代码中，\_\_storage\_\_ 为内部保存的自己，键就是 thread.get_ident ，也就是根据线程的标示符返回对应的值。

下面我们来看看整个交互过程，_request_ctx_stack 堆栈是在哪里设置 push 的，push 的应该是我们上面说的同时具有 request 和 session 属性的对象。




## 源码解析

接下来我们看看上下文是如何实现。

### 上下文对象定义

应用上下文和请求上下文在 ctx.py 中定义，内容如下。

{% highlight python %}
class AppContext(object):
    def __init__(self, app):
        self.app = app
        self.url_adapter = app.create_url_adapter(None)
        self.g = app.app_ctx_globals_class()

        # Like request context, app contexts can be pushed multiple times
        # but there a basic "refcount" is enough to track them.
        self._refcnt = 0
{% endhighlight %}

AppContext 类即是应用上下文，可以看到里面保存了几个变量，其中比较重要的有：app (当前应用对象的引用)、g (用来保存需要在每个请求中需要用到的请求内全局变量) 。

{% highlight python %}
class RequestContext(object):
    def __init__(self, app, environ, request=None):
        self.app = app
        if request is None:
            request = app.request_class(environ)
        self.request = request
        self.url_adapter = app.create_url_adapter(self.request)
        self.flashes = None
        self.session = None

        # Request contexts can be pushed multiple times and interleaved with
        # other request contexts.  Now only if the last level is popped we
        # get rid of them.  Additionally if an application context is missing
        # one is created implicitly so for each level we add this information
        self._implicit_app_ctx_stack = []

        # indicator if the context was preserved.  Next time another context
        # is pushed the preserved context is popped.
        self.preserved = False

        # remembers the exception for pop if there is one in case the context
        # preservation kicks in.
        self._preserved_exc = None

        # Functions that should be executed after the request on the response
        # object.  These will be called before the regular "after_request"
        # functions.
        self._after_request_functions = []

        self.match_request()
{% endhighlight %}

RequestContext 即请求上下文，其中有我们熟悉的request 和 session，其中 app 和应用上下文中的 app 含义相同。

### 上下文处理流程

如之前文章所述，会在 app.py 中的 wsgi_app() 函数中调用 ctx.push() 函数将上下文信息压栈。

{% highlight python %}
class RequestContext(object):
    def __init__(self, app, environ, request=None):
        self.app = app
        if request is None:
            request = app.request_class(environ) # 根据环境变量创建request
        self.request = request
        self.session = None

    def push(self):
        top = _request_ctx_stack.top
        if top is not None and top.preserved:
            top.pop(top._preserved_exc)

        # Before we push the request context we have to ensure that there
        # is an application context.
        app_ctx = _app_ctx_stack.top
        if app_ctx is None or app_ctx.app != self.app:
            app_ctx = self.app.app_context()
            app_ctx.push()
            self._implicit_app_ctx_stack.append(app_ctx)
        else:
            self._implicit_app_ctx_stack.append(None)

        if hasattr(sys, 'exc_clear'):
            sys.exc_clear()

        _request_ctx_stack.push(self) # 将ctx压入到_request_ctx_stack变量中
        # Open the session at the moment that the request context is
        # available. This allows a custom open_session method to use the
        # request context (e.g. code that access database information
        # stored on `g` instead of the appcontext).
        self.session = self.app.open_session(self.request)
        if self.session is None:
            self.session = self.app.make_null_session()

    def pop(self, exc=None):
        rv = _request_ctx_stack.pop()

    def auto_pop(self, exc):
        if self.request.environ.get('flask._preserve_context') or \
           (exc is not None and self.app.preserve_context_on_exception):
            self.preserved = True
            self._preserved_exc = exc
        else:
            self.pop(exc)
{% endhighlight %}

可以看到 ctx.push() 将 ctx 压入到 _request_ctx_stack 这个栈中，所以当我们调用 request.METHOD 时将调用 _lookup_req_object() 函数。

top 此时就是上面压入的 ctx 上下文对象（RequestContext 类的实例），而 getattr(top, "request") 将返回 ctx 实例中的 request 请求，而这个 request 就是在 ctx 的 \_\_init\_\_() 中根据环境变量创建的。

这下应该清楚了，每次请求在调用视图函数之前，flask 会自动把请求创建好 ctx 上下文，并存放在线程的栈中，当使用时就可以根据线程 id 拿到了所需要的变量。



<!--
    上面是我简化的代码，其实在RequestContext push中_app_ctx_stack = LocalStack()是None，也会把app push进去，对应的app上下文对象为AppContext。

    我们知道flask还有一个神秘的对象g，flask从0.10开始g是和app绑定在一起的(http://flask.pocoo.org/docs/0.10/api/#flask.g)，g是AppContext的一个成员变量。虽然说g是和app绑定在一起的，但不同请求的AppContext是不同的，所以g还是不同。也就是说你不能再一个视图中设置g.name，然后再另一个视图中使用g.name，会提示AttributeError。

    绑定要app的好处就是可以脱离request使用g，否则就要使用flask._app_ctx_stack.top代替g，
-->

### 为什么要使用栈

到此，实际上已经大概清除了 request 的工作流程了，但是通常对于多线程来说，一个线程一次只会处理一个请求，也就是说当前的 request 应该是只有一个，那么为什么不是直接使用这个对象，而是要使用栈呢？

这主要是多个应用导致的，这也是 Flask 设计时的标准之一吧，也就是在一个 Python 进程中，可以拥有多个应用。由于一般都只有一个 app，栈顶存放的肯定是当前 request 对象，但是如果是多个 app，那么栈顶存放的是当前活跃的 request，也就是说使用栈是为了获取当前的活跃 request 对象。

可以查看如下的代码。

{% highlight python %}
from flask import Flask, url_for
app1 = Flask("app1")
app2 = Flask("app2")

@app1.route("/index1")
def index1():
    return "app1"

@app1.route("/home")
def home():
    return "app1home"

@app2.route("/index2")
def index2():
    return "app2"

with app1.test_request_context():
    print url_for('index1')
    with app2.test_request_context():
        print url_for('index2')
    print url_for('home')

# OUTPUT:
# /index1
# /index2
# /home
{% endhighlight %}

当调用 app1.test_request_context() 时，会将 app1 对应的 context 压栈，当前活跃 app context 是 app1 的，所以 url_for 使用的是 app1 的 context，当 with app2.test_request_context() 时，会将 app2 对应的 context 压栈，当前活跃 app context 是 app2 的，当 with 过了作用域，将弹出 app2 的 context，此时活跃的为 app1 的，这就是为什么 context 使用栈而不是实例变量。



## session 原理

关于 session 使用方式，可以查看 [Flask 常见示例](/blog/python-flask-tips) 中的 session 示例。Flask 中的 session 是通过客户端的 cookie 实现的，当然也可以使用 [Flask-Session](http://pythonhosted.org/Flask-Session/) 在服务器端实现。

Flask 通过 itsdangerous 这个库将 session 的内容序列化到浏览器的 cookie，当浏览器再次请求时会反序列化 cookie 内容，也就得到我们的 session 内容。

假设有如下的代码，我们保存一个变量。

{% highlight python %}
from flask import Flask, session, escape, request
app = Flask(__name__)
app.secret_key = 'key'

@app.route('/')
def index():
    session['username'] = "Andy"
    return "Hello World\n"

if __name__ == '__main__':
    app.run(host='0.0.0.0', port=8080, debug=True)
{% endhighlight %}

然后可以通过 curl 命令查看或者查看浏览器。

### 内容查看

接下来看看这个 cookie 存储了什么值。该 cookie 通过 . 分割成了三部分：内容序列化+时间+防篡改值 。

{% highlight python %}
from sys import argv
from json import loads
from itsdangerous import *
from time import strftime, localtime

cookies = argv[1]
#cookies = "eyJ1c2VybmFtZSI6eyIgYiI6IlFXNWtlUT09In19.CuxlJQ.ljG_CYgC0K3RwRPx0vg20kFA08o"
(data, time, key) = [base64_decode(v) for v in cookies.split('.')]
print "DATE   :", strftime("%Y-%m-%d %H:%M:%S", localtime(bytes_to_int(time)+EPOCH))
#print "KEY    :", key
print "VALUES :"
data = loads(data)
for (d, v) in data.items():
    print '\t', d, v
#'{"username":{" b":"QW5keQ=="}}'
#base64_decode('QW5keQ==')
{% endhighlight %}

通过第一部分可以获得了 session['username'] 的值；第二部分保存的是时间，itsdangerous 为了减少时间戳的值，之前减掉了 118897263，所以我们要加上。

第三部分是 session 值和时间戳以及我们 SECRET_KEY 的防篡改值，通过 HMAC 算法签名，也就是说即使你修改了前面的值，由于签名值有误，flask 不会使用该 session 。所以一定要保存好 SECRET_KEY，否则泄露后就可以通过构造 cookie 伪造 session 值，这是很恐怖的一件事。

<!--
https://github.com/pallets/itsdangerous
http://pythonhosted.org/itsdangerous/
http://itsdangerous.readthedocs.io/en/latest/
-->



<!--
这个标题有点不太准确，其实应该说：慎用异步 WSGI Server 运行基于 Werkzeug 的应用。或者更宽泛一点——慎用异步 WSGI Server 运行使用 Thread Local 的应用。 Flask 是基于 Werkzeug 的故也在此列。貌似使用 Werkzeug 作为底层 WSGI 库的框架还有国内 limodou 老师的 Uliweb 。而其他使用 Thread Local 的 WSGI Application 不计其数，Bottle、web.py 都在此列。但我没有测试过其他的，仅仅掉下过 Flask 的坑。

这个坑问题就出在 Werkzeug 的 Thread Local 上。用过 Flask 的同学应该不会忘记其便捷的 flask.request 、 flask.g 、 flask.session 等对象。引用的时候就像使用单例对象一样，但实际上对它们的所有赋值操作都只会影响到当前请求（当前线程）。Thread Local 模式的实现一定要有一个 Thread Identity 作为标识，由此产生的 Proxy 对象会根据这个标识创建多个独立的数据空间。我们访问代理对象的时候就像访问全局对象，但代理对象会将消息分发到 Thread Identity 对应的真实对象上。

一般这个实现都会使用系统提供的当前线程 ID 作为 Thread Identity，于是在使用多线程的 Web 服务器上，由于每个请求独享一个线程，每个 Thread Local 的数据空间也就为一个请求所独享。这是 Flask（Werkzeug）可以正常工作的假设前提。但异步 WSGI Server 一般不会给每个请求分配一个线程。为了减少内核调度线程的开销，这类服务器软件一般使用协同式调度——若干个请求都在同一个进程的主线程中运行，遇到 IO 阻塞（例如等待网络）则挂起当前帧栈，切换到另一个帧栈执行，整个过程都是协同式的，并没有时间片轮转这些抢占的做法。如此一来，用线程 ID 作为 Thread Identity 的所有 Thread Local 对象就会退化成单例对象，这也是我遇到的灵异现象的原因：在宿舍登录网站，远在图书馆的电脑会同步登录。

说到底还是 Thread Identity 选取导致的问题。其实 Werkzeug 在设计的时候有考虑到这个问题，其默认的 Thread Identity 工厂选取就优先采用 Greenlet 的“当前协程 ID”，这一点从 Werkzeug Local 可以看到。但并非所有异步 WSGI Server 都使用 Greenlet 作为协程库，我所使用的 uWSGI 就是使用自己实现的 uGreen 作为微线程的。于是上述问题就出现了。

和 Flask 的这个问题相关的是 flask.globals 模块中的两个 werkzeug.local.LocalStack 实例，替换它们的 __ident_func__ 属性应该可以解决问题。但我没有找到 uWSGI 默认使用的 uGreen 对应的 Thread Identity 获取方法。或许对于使用 uWSGI 运行的应用来说，不要去使用异步是个更好的选择吧。当然，不怕麻烦的话也可以给 uWSGI 编译使用 Greenlet 调度的异步支持插件。
-->


## 结论

如上，在 flask 中包括了两种上下文，应用上下文和请求上下文。不过从名字上来看，应用上下文一般会被任务是一个应用的全局变量，所有请求都可以访问修改其中的内容；而请求上下文则是请求内可访问的内容。

但事实上，这两者并不是全局与局部的关系，它们都处于一个请求的局部中，每个请求的 g 都是独立的，并且在整个请求内都是可访问修改的。

之所以有应用上下文，是因为 flask 设计之初，就是要支持多个应用。

### 其它

还是要重点声明一下。

g 在每次请求时都会修改，其生命周期只在一个请求中，不能保存类似数据库链接（需要在整个应用存活期间使用）相关的信息，但是可以保存在本次请求中不同函数中都会调用的变量信息。

像类似数据库的请求可以使用一个全局变量，不过此时对线程是不安全的，因此建议使用链接池（线程安全），也就是可以直接使用 Flask-SQLAlchemy 类似的插件。

其时，还有一种信息，就是类似用户登陆的信息，可以将其保存到 session 中，每次浏览器请求都会携带该信息，不过实际只能是字符串信息（可以转换为其它类型数据）。


## 参考

关于请求上下文和应用上下文可以参考官方文档，[The Request Context](http://flask.pocoo.org/docs/0.11/reqcontext/) 以及 [The Application Context](http://flask.pocoo.org/docs/0.11/appcontext/) 。

[Understanding Contexts in Flask](http://kronosapiens.github.io/blog/2014/08/14/understanding-contexts-in-flask.html) 或者 [本地保存文档](/reference/python/Understanding Contexts in Flask.mht)；[Charming Python: 从Flask的request说起](http://www.zlovezl.cn/articles/charming-python-start-from-flask-request/)；[Flask 的 Context 机制](https://blog.tonyseek.com/post/the-context-mechanism-of-flask/) 。

<!--
http://blog.csdn.net/sasoritattoo/article/details/9703429
http://blog.csdn.net/barrysj/article/details/51519254
-->

{% highlight python %}
{% endhighlight %}
