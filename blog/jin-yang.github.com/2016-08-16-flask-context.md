---
title: Flask 上下文理解
layout: post
comments: true
language: chinese
category: [webserver]
keywords: flask,context,上下文
description: Flask 中定義了兩種上下文，分別為請求上下文和應用上下文，其中前者包括了 current_app (當前的應用對象)、g (處理請求時用作臨時存儲的對象)；後者包括了 request (請求對象，封裝了 HTTP 請求的內容)、session (用於存儲請求之間需要記住的值)。Flask 分發請求之前會激活請求上下文，請求處理完成後再將其刪除，為了支持多個 app，Flask 中的 Context 是通過棧來實現。接下來，我們看看詳細的是如何實現的。
---

Flask 中定義了兩種上下文，分別為請求上下文和應用上下文，其中前者包括了 current_app (當前的應用對象)、g (處理請求時用作臨時存儲的對象)；後者包括了 request (請求對象，封裝了 HTTP 請求的內容)、session (用於存儲請求之間需要記住的值)。

Flask 分發請求之前會激活請求上下文，請求處理完成後再將其刪除，為了支持多個 app，Flask 中的 Context 是通過棧來實現。

接下來，我們看看詳細的是如何實現的。

<!-- more -->

## 簡介

實際上所謂的上下文，在該場景下就是包括了一次請求所包含的信息，包括了從客戶（一般是瀏覽器）發送過來的數據，例如，登陸時使用的用戶名密碼；以及在中間處理過程中生成的數據，例如，每次請求時我們可能會需要新建一個數據庫鏈接。

Flask 會在接收每次請求的時候將參數自動轉換為相應的對象，也就是 request、session，一般來說上下文傳遞可以通過參數進行，這也就意味這每個需要該上下文的函數都要多增加一個入參，為瞭解決這一問題，Flask 提供了一個類似於全局變量的實現方式（如下會講到這一參數是線程安全的）。

在多線程服務器中，通過線程池處理不同客戶的不同請求，當收到請求後，會選一個線程進行處理，請求的臨時對象（也就是上下文）會保存在該線程對應的全局變量中（通過線程 ID 區分），這樣即不幹擾其他線程，又使得所有線程都可以訪問。

### Flask 上下文對象

如上所述，Flask 有兩種上下文，分別是：

##### RequestContext 請求上下文

* Request 請求的對象，會封裝每次的 Http 請求 (environ) 的內容；

* Session 會根據請求中的 cookie，重新載入該訪問者相關的會話信息。

##### AppContext 程序上下文

* g 處理請求時用作臨時存儲的對象，每次請求都會重設這個變量；

* current_app 當前激活程序的程序實例。

##### 生命週期

* current_app 的生命週期最長，只要當前程序實例還在運行，都不會失效。

* request 和 g 的生命週期為一次請求期間，當請求處理完成後，生命週期也就完結了。

* session 就是傳統意義上的 session，只要還未失效（用戶未關閉瀏覽器、沒有超過設定的失效時間），那麼不同的請求會共用同樣的 session。

![Flask Context Logic]({{ site.url }}/images/python/flask-context-logic.jpg "Flask Context Logic"){: .pull-center}

### 應用上下文

關於應用上下文的內容可以參考官方文檔 [flask.pocoo.org/docs/appcontext/](http://flask.pocoo.org/docs/appcontext/) 中的 The Application Context 部分的內容，在此摘抄如下：

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

如上所述，Flask 啟動時會有兩種狀態，開始的狀態可以做些應用級的初始化；當第一個請求過來之後，就進入了另外的一個狀態，詳細的可以參考上面的描述。

在應用上下文中，包括了 current_app、g 兩個對象，一般來說，該對象有如下的兩種創建方式：

* 在處理請求的時候會自動創建；

* 通過 app_context() 手動創建，通常用在一些單元測試的場景中。

在應用上下文中，通過 flask.current_app、g 會返回當前的應用對象上下文，其中包含了當前的應用對象。注意：g 之前是在請求上下文中的，現在被遷移到了應用上下文。

下面是使用應用上下文的使用方式。

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



### 請求上下文

請求上下文中包括了 request、session 變量，包含了一次請求的上下文信息，類似於應用上下文，同樣可以通過兩種方式進行創建。

* 當應用收到請求之後會自動創建；

* 可以通過 app.test_request_context('/route?param=value') 手動創建。

與應用上下文不同的是，請求上下文會接收一個參數代表了一次請求。另外，需要注意的是，在創建請求上下文的時候，如果沒有當前的應用上下文，或者不是本應用的上下文，會自動創建一個應用上下文。

{% highlight text %}
Another thing of note is that the request context will automatically also create
an application context when it’s pushed and there is no application context for
that application so far.
{% endhighlight %}

接下來我們看看在 Flask 中如何使用請求上下文。

無論在什麼地方，如果要獲取當前的 request 對象，直接通過如下的方式使用即可。

{% highlight python %}
from flask import request
request.args
request.forms
request.cookies
{% endhighlight %}


## 線程安全

首先我們看看 request 是如何實現的，實際上之所以有 request 就是為了在多線程（或者協程）的環境下，各個線程可以使用各自的變量，不至於會混亂，接下來我們看看具體是如何實現的。

### 簡介

實際上，Python 提供了同樣類似的線程安全變量保存機制，也就是 threading.local() 方法，而在 flask 中，使用的是 werkzeug 中的 Local 實現的，詳細可以參考 [werkzeug.pocoo.org/docs/local](http://werkzeug.pocoo.org/docs/local/) 。

總體來說，werkzeug 提供了與 threading.local() 相同的機制，不過是 threading 只提供了線程的安全，對於 greenlet 則無效。

### flask 定義

首先，查看源碼目錄下的 \_\_init\_\_.py 文件，可以發現這些變量實際上是在 globals.py 中定義，然後只是在 \_\_init\_\_.py 中引入的，其定義如下。

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

其中，partial() 函數就是函數調用時，如果有多個參數，但其中的一個參數已經知道了，我們可以通過這個函數重新綁定一個新函數，然後去調用這個新函數，如果有默認參數的話，也可以自動對應。

示例如下：

{% highlight text %}
from functools import partial
def add2(a,b,c=2):
    return a+b+c
plus3 = partial(add2, 1)
plus3(2)  # 1+2+2=5
{% endhighlight %}

而上述的代碼，實際就定義了最終調用的是 getattr(top, 'request') 還是 getattr(top, 'session') 。


如上，我們會發現 g、request、session 的調用方式都是一樣的，最後都通過 getattr(top, name) 獲取，這也就意味著有一個上下文對象同時保持了上述的三個對象。

另外，使用時，我們只要一處導入 request，在任何視圖函數中都可以使用 request，關鍵是每次的都是不同的 request 對象，說明獲取 request 對象肯定是一個動態的操作，不然肯定都是相同的 request。

之所以可以做到這樣，主要就是 \_lookup_req_object() 和 LocalProxy 組合完成的。

### LocalProxy

LocalProxy 是 werkzeug/local.py 中定義的一個代理對象，在此處的作用是將所有的請求都發給內部的 _local 對象，如下的 \_\_slots\_\_ 用來表示所有的實例都只有一個實現。

{% highlight text %}
__slots__ = ('__local', '__dict__', '__name__')
class LocalProxy(object):
    def __init__(self, local, name=None):
        # 在上述的request中傳遞的就是_lookup_req_object()
        object.__setattr__(self, '_LocalProxy__local', local)
        object.__setattr__(self, '__name__', name)

    def _get_current_object(self):
        # _lookup_req_object() 中沒有 __release_local__，直接進入該分支
        if not hasattr(self.__local, '__release_local__'):
            return self.__local()
        try:
            return getattr(self.__local, self.__name__)
        except AttributeError:
            raise RuntimeError('no object bound to %s' % self.__name__)

    def __getattr__(self, name):
        return getattr(self._get_current_object(), name)
{% endhighlight %}

當調用 request.method 時會調用 \_\_getattr\_\_() 函數，而實際上 request 對任何方法的調用，最終都會轉化為對 _lookup_req_object() 返回對象的調用。

上述的 request.method 會調用 _request_ctx_stack.top.request.method 。

### LocalStack

既然每次 request 都不同，要麼調用 top = _request_ctx_stack.top 返回的 top 不同，要麼 top.request 屬性不同，在 flask 中每次返回的 top 是不一樣的，所以 request 的各個屬性都是變化的。

現在需要看看 _request_ctx_stack = LocalStack() 了，LocalStack 其實就是簡單的模擬了堆棧的基本操作，包括 push、top、pop，其內部保存了與線程相關聯的本地變量，從而使其在多線程中 request 不混亂。

{% highlight python %}
class Local(object):
    __slots__ = ('__storage__', '__ident_func__')

    def __init__(self):
        object.__setattr__(self, '__storage__', {})
        object.__setattr__(self, '__ident_func__', get_ident)
    def __getattr__(self, name):
        return self.__storage__[self.__ident_func__()][name]
{% endhighlight %}

在 Local 的代碼中，\_\_storage\_\_ 為內部保存的自己，鍵就是 thread.get_ident ，也就是根據線程的標示符返回對應的值。

下面我們來看看整個交互過程，_request_ctx_stack 堆棧是在哪裡設置 push 的，push 的應該是我們上面說的同時具有 request 和 session 屬性的對象。




## 源碼解析

接下來我們看看上下文是如何實現。

### 上下文對象定義

應用上下文和請求上下文在 ctx.py 中定義，內容如下。

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

AppContext 類即是應用上下文，可以看到裡面保存了幾個變量，其中比較重要的有：app (當前應用對象的引用)、g (用來保存需要在每個請求中需要用到的請求內全局變量) 。

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

RequestContext 即請求上下文，其中有我們熟悉的request 和 session，其中 app 和應用上下文中的 app 含義相同。

### 上下文處理流程

如之前文章所述，會在 app.py 中的 wsgi_app() 函數中調用 ctx.push() 函數將上下文信息壓棧。

{% highlight python %}
class RequestContext(object):
    def __init__(self, app, environ, request=None):
        self.app = app
        if request is None:
            request = app.request_class(environ) # 根據環境變量創建request
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

        _request_ctx_stack.push(self) # 將ctx壓入到_request_ctx_stack變量中
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

可以看到 ctx.push() 將 ctx 壓入到 _request_ctx_stack 這個棧中，所以當我們調用 request.METHOD 時將調用 _lookup_req_object() 函數。

top 此時就是上面壓入的 ctx 上下文對象（RequestContext 類的實例），而 getattr(top, "request") 將返回 ctx 實例中的 request 請求，而這個 request 就是在 ctx 的 \_\_init\_\_() 中根據環境變量創建的。

這下應該清楚了，每次請求在調用視圖函數之前，flask 會自動把請求創建好 ctx 上下文，並存放在線程的棧中，當使用時就可以根據線程 id 拿到了所需要的變量。



<!--
    上面是我簡化的代碼，其實在RequestContext push中_app_ctx_stack = LocalStack()是None，也會把app push進去，對應的app上下文對象為AppContext。

    我們知道flask還有一個神祕的對象g，flask從0.10開始g是和app綁定在一起的(http://flask.pocoo.org/docs/0.10/api/#flask.g)，g是AppContext的一個成員變量。雖然說g是和app綁定在一起的，但不同請求的AppContext是不同的，所以g還是不同。也就是說你不能再一個視圖中設置g.name，然後再另一個視圖中使用g.name，會提示AttributeError。

    綁定要app的好處就是可以脫離request使用g，否則就要使用flask._app_ctx_stack.top代替g，
-->

### 為什麼要使用棧

到此，實際上已經大概清除了 request 的工作流程了，但是通常對於多線程來說，一個線程一次只會處理一個請求，也就是說當前的 request 應該是隻有一個，那麼為什麼不是直接使用這個對象，而是要使用棧呢？

這主要是多個應用導致的，這也是 Flask 設計時的標準之一吧，也就是在一個 Python 進程中，可以擁有多個應用。由於一般都只有一個 app，棧頂存放的肯定是當前 request 對象，但是如果是多個 app，那麼棧頂存放的是當前活躍的 request，也就是說使用棧是為了獲取當前的活躍 request 對象。

可以查看如下的代碼。

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

當調用 app1.test_request_context() 時，會將 app1 對應的 context 壓棧，當前活躍 app context 是 app1 的，所以 url_for 使用的是 app1 的 context，當 with app2.test_request_context() 時，會將 app2 對應的 context 壓棧，當前活躍 app context 是 app2 的，當 with 過了作用域，將彈出 app2 的 context，此時活躍的為 app1 的，這就是為什麼 context 使用棧而不是實例變量。



## session 原理

關於 session 使用方式，可以查看 [Flask 常見示例](/blog/python-flask-tips) 中的 session 示例。Flask 中的 session 是通過客戶端的 cookie 實現的，當然也可以使用 [Flask-Session](http://pythonhosted.org/Flask-Session/) 在服務器端實現。

Flask 通過 itsdangerous 這個庫將 session 的內容序列化到瀏覽器的 cookie，當瀏覽器再次請求時會反序列化 cookie 內容，也就得到我們的 session 內容。

假設有如下的代碼，我們保存一個變量。

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

然後可以通過 curl 命令查看或者查看瀏覽器。

### 內容查看

接下來看看這個 cookie 存儲了什麼值。該 cookie 通過 . 分割成了三部分：內容序列化+時間+防篡改值 。

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

通過第一部分可以獲得了 session['username'] 的值；第二部分保存的是時間，itsdangerous 為了減少時間戳的值，之前減掉了 118897263，所以我們要加上。

第三部分是 session 值和時間戳以及我們 SECRET_KEY 的防篡改值，通過 HMAC 算法簽名，也就是說即使你修改了前面的值，由於簽名值有誤，flask 不會使用該 session 。所以一定要保存好 SECRET_KEY，否則洩露後就可以通過構造 cookie 偽造 session 值，這是很恐怖的一件事。

<!--
https://github.com/pallets/itsdangerous
http://pythonhosted.org/itsdangerous/
http://itsdangerous.readthedocs.io/en/latest/
-->



<!--
這個標題有點不太準確，其實應該說：慎用異步 WSGI Server 運行基於 Werkzeug 的應用。或者更寬泛一點——慎用異步 WSGI Server 運行使用 Thread Local 的應用。 Flask 是基於 Werkzeug 的故也在此列。貌似使用 Werkzeug 作為底層 WSGI 庫的框架還有國內 limodou 老師的 Uliweb 。而其他使用 Thread Local 的 WSGI Application 不計其數，Bottle、web.py 都在此列。但我沒有測試過其他的，僅僅掉下過 Flask 的坑。

這個坑問題就出在 Werkzeug 的 Thread Local 上。用過 Flask 的同學應該不會忘記其便捷的 flask.request 、 flask.g 、 flask.session 等對象。引用的時候就像使用單例對象一樣，但實際上對它們的所有賦值操作都只會影響到當前請求（當前線程）。Thread Local 模式的實現一定要有一個 Thread Identity 作為標識，由此產生的 Proxy 對象會根據這個標識創建多個獨立的數據空間。我們訪問代理對象的時候就像訪問全局對象，但代理對象會將消息分發到 Thread Identity 對應的真實對象上。

一般這個實現都會使用系統提供的當前線程 ID 作為 Thread Identity，於是在使用多線程的 Web 服務器上，由於每個請求獨享一個線程，每個 Thread Local 的數據空間也就為一個請求所獨享。這是 Flask（Werkzeug）可以正常工作的假設前提。但異步 WSGI Server 一般不會給每個請求分配一個線程。為了減少內核調度線程的開銷，這類服務器軟件一般使用協同式調度——若干個請求都在同一個進程的主線程中運行，遇到 IO 阻塞（例如等待網絡）則掛起當前幀棧，切換到另一個幀棧執行，整個過程都是協同式的，並沒有時間片輪轉這些搶佔的做法。如此一來，用線程 ID 作為 Thread Identity 的所有 Thread Local 對象就會退化成單例對象，這也是我遇到的靈異現象的原因：在宿舍登錄網站，遠在圖書館的電腦會同步登錄。

說到底還是 Thread Identity 選取導致的問題。其實 Werkzeug 在設計的時候有考慮到這個問題，其默認的 Thread Identity 工廠選取就優先採用 Greenlet 的“當前協程 ID”，這一點從 Werkzeug Local 可以看到。但並非所有異步 WSGI Server 都使用 Greenlet 作為協程庫，我所使用的 uWSGI 就是使用自己實現的 uGreen 作為微線程的。於是上述問題就出現了。

和 Flask 的這個問題相關的是 flask.globals 模塊中的兩個 werkzeug.local.LocalStack 實例，替換它們的 __ident_func__ 屬性應該可以解決問題。但我沒有找到 uWSGI 默認使用的 uGreen 對應的 Thread Identity 獲取方法。或許對於使用 uWSGI 運行的應用來說，不要去使用異步是個更好的選擇吧。當然，不怕麻煩的話也可以給 uWSGI 編譯使用 Greenlet 調度的異步支持插件。
-->


## 結論

如上，在 flask 中包括了兩種上下文，應用上下文和請求上下文。不過從名字上來看，應用上下文一般會被任務是一個應用的全局變量，所有請求都可以訪問修改其中的內容；而請求上下文則是請求內可訪問的內容。

但事實上，這兩者並不是全局與局部的關係，它們都處於一個請求的局部中，每個請求的 g 都是獨立的，並且在整個請求內都是可訪問修改的。

之所以有應用上下文，是因為 flask 設計之初，就是要支持多個應用。

### 其它

還是要重點聲明一下。

g 在每次請求時都會修改，其生命週期只在一個請求中，不能保存類似數據庫鏈接（需要在整個應用存活期間使用）相關的信息，但是可以保存在本次請求中不同函數中都會調用的變量信息。

像類似數據庫的請求可以使用一個全局變量，不過此時對線程是不安全的，因此建議使用鏈接池（線程安全），也就是可以直接使用 Flask-SQLAlchemy 類似的插件。

其時，還有一種信息，就是類似用戶登陸的信息，可以將其保存到 session 中，每次瀏覽器請求都會攜帶該信息，不過實際只能是字符串信息（可以轉換為其它類型數據）。


## 參考

關於請求上下文和應用上下文可以參考官方文檔，[The Request Context](http://flask.pocoo.org/docs/0.11/reqcontext/) 以及 [The Application Context](http://flask.pocoo.org/docs/0.11/appcontext/) 。

[Understanding Contexts in Flask](http://kronosapiens.github.io/blog/2014/08/14/understanding-contexts-in-flask.html) 或者 [本地保存文檔](/reference/python/Understanding Contexts in Flask.mht)；[Charming Python: 從Flask的request說起](http://www.zlovezl.cn/articles/charming-python-start-from-flask-request/)；[Flask 的 Context 機制](https://blog.tonyseek.com/post/the-context-mechanism-of-flask/) 。

<!--
http://blog.csdn.net/sasoritattoo/article/details/9703429
http://blog.csdn.net/barrysj/article/details/51519254
-->

{% highlight python %}
{% endhighlight %}
