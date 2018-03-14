---
title: Flask 請求處理流程
layout: post
comments: true
language: chinese
category: [webserver]
keywords: flask,請求處理
description: 實際上與其它的 web 框架處理流程基本相同，在此介紹下在 Flask 中，一次請求到響應的處理流程。Flask 支持多線程和多進程，通常每個線程或者進程一次只會處理一個請求。當然也可以通過類似 gevent 使用協程、事件庫進行優化，對於目前使用的 WebSocket 技術會有較高的優化，不過還沒有仔細研究過。OK，接下來看看 Falsk 是如何處理每次的網絡請求。
---

實際上與其它的 web 框架處理流程基本相同，在此介紹下在 Flask 中，一次請求到響應的處理流程。Flask 支持多線程和多進程，通常每個線程或者進程一次只會處理一個請求。

當然也可以通過類似 gevent 使用協程、事件庫進行優化，對於目前使用的 WebSocket 技術會有較高的優化，不過還沒有仔細研究過。

OK，接下來看看 Falsk 是如何處理每次的網絡請求。

<!-- more -->

## 簡介

Flask 執行時有兩種方式，一種是調用 `app.run()` 執行，通常用於調試階段，默認會在 Python 文件修改之後可以自動加載，所以非常方便；另外一種是通過類似 uwsgi 程序調用執行。

### WSGI

在詳細介紹 Flask 處理流程之前，首先說明一下 WSGI 接口定義。

WSGI (Web Server Gateway Interface) 定義了 Web 服務器和 Web 應用程序之間的一種簡單而通用的接口，封裝了接受請求、解析請求、發送響應等等的這些底層的代碼和操作，使開發者可以高效的編寫 Web 應用；該接口通常在 Python 中使用，Flask 也是基於此接口的。

如下是從 stackflow 找到的一個不錯的圖片，一圖勝千言阿。

![WSGI Flow(PEP 3333)]({{ site.url }}/images/python/flask-wsgi-interface.png "WSGI Flow(PEP 3333)"){: .pull-center; width='90%'}

如下是一個簡單的使用 WSGI 的示例：

{% highlight python %}
def application(environ, start_response):
    start_response('200 OK', [('Content-Type', 'text/html')])
    return [u'<h1>Hello World, This is WSGI!</h1>']
{% endhighlight %}

其中接口非常簡單，包含了兩個參數：

* environ: 包含全部 HTTP 請求信息的字典，由 WSGI Server 解析 HTTP 請求生成。

* start_response: 一個 WSGI Server 提供的函數，調用可以發送響應的狀態碼和 HTTP 報文頭，函數在返回前必須調用一次 start_response() 進行設置。

另外，application() 應當返回一個可以迭代的對象，也就是 HTTP 正文；該函數由 WSGI Server 直接調用，並提供響應的參數。

![WSGI Application]({{ site.url }}/images/python/flask-wsgi-application.jpg "WSGI Application"){: .pull-center}


Python 內置了一個 WSGIREF 的 WSGI 服務器，不過性能不是很好，一般只用在開發環境；通常開發環境中一般使用 gunicorn、uwsgi 等。

### Flask 調用

在此，我們一步步查看調試時的執行過程（前面說的第一種方式），也就是在此講解的是 `app.run()` 這種方式的調用；而後者的調用實際也是殊途同歸，最終執行路徑會到同一代碼段，下面會有說明。

接下來從程序初始化、執行程序、處理請求等，一步步查看，實際簡化後的調用過程如下。


## 調用 flask 之前

實際上就是在加載程序，然後調用 run() 函數之前。

首先會從 flask 包導入了 Flask 類，該類位於源碼的 app.py 文件，基本上就是簡單的初始化一些參數；然後，調用該類中的 run() 方法，對於 Flask 類的源碼簡化後如下。

{% highlight python %}
class Flask(_PackageBoundObject):
    def run(self, host=None, port=None, debug=None, **options):
        from werkzeug.serving import run_simple
        run_simple(host, port, self, **options)
{% endhighlight %}

可以看到，實際執行的是 werkzeug 庫中的 run_simple() 函數，該函數在 serving.py 文件中進行定義，該函數非常簡單，最終調用的是內嵌的 inner() 函數。

另外，實際上在 Flask 中可以設置為在模塊改變之後自動重新加載，這也就是默認值，該功能實際會調用 run_with_reloader() 函數，最終仍然調用 inner() 函數。

接下來看看 inner() 函數，如下。

{% highlight python %}
def inner():
    try:
        fd = int(os.environ['WERKZEUG_SERVER_FD'])
    except (LookupError, ValueError):
        fd = None
    srv = make_server(hostname, port, application, threaded,
                      processes, request_handler,
                      passthrough_errors, ssl_context,
                      fd=fd)
    if fd is None:
        log_startup(srv.socket)
    srv.serve_forever()
{% endhighlight %}

該函數中，通過 make_server() 函數調用，返回 ThreadedWSGIServer()、ForkingWSGIServer()、BaseWSGIServer() 中的一個對象實例，從名稱就可以看出來，分別會使用線程、進程做併發。

創建完成之後，就會調用 server_forever() 函數，從名稱就可以看出，基本就是一直運行程序，等待請求、處理請求、發送響應；接下來看看其具體的實現。

最終會使用 BaseHTTPServer 中的 HTTPServer 模塊，這是一個通用的模塊，會調用 WSGIRequestHandler 中的 handle_one_request() 函數，而實際上，最終會調用 run_wsgi() 函數。

{% highlight python %}
def run_wsgi(self):
    environ = self.make_environ()
    ... ...
    def start_response(status, response_headers, exc_info=None):
        if exc_info:
            try:
                if headers_sent:
                    reraise(*exc_info)
            finally:
                exc_info = None
        elif headers_set:
            raise AssertionError('Headers already set')
        headers_set[:] = [status, response_headers]
        return write

    def execute(app):
        application_iter = app(environ, start_response)
        # environ是為了給request傳遞請求的
        # start_response主要是增加響應頭和狀態碼，最後需要werkzeug發送請求
        try:
            for data in application_iter: #根據wsgi規範，app返回的是一個序列
                write(data) #發送結果
            if not headers_sent:
                write(b'')
        finally:
            if hasattr(application_iter, 'close'):
                application_iter.close()
            application_iter = None
    try:
        execute(self.server.app)
    except (socket.error, socket.timeout) as e:
        pass
    ... ...
{% endhighlight %}

到此的話，基本就到了 wsgi 規範規定的接口規範，也就是說 app 需要實現一個接口，接受兩個參數，也即函數聲明是 application(environ, start_response) 。

可以看到，app.run() 模式會調用 BaseHTTPServer 模塊處理請求，然後交給實際 wsgi 接口定義的接口。不過，這種方式只適用於調試階段，BaseHTTPServer 沒有進行很好的優化。


## 殊途同歸

如上所述，到此為止，實際上就到了 app.run() 和 uwsgi 都調用的接口，也就是 wsgi 的規範中定義的 API 了，最終就到了 Flask 中定義的對象了，也即 Flask 類的定義。

對於 flask 而言，實際是通過定義一個類實現的，也就是 app.py 文件中的 class Flask，而在該類中提供了 \_\_call\_\_() 函數來適配 wsgi 規範。

{% highlight python %}
class Flask(_PackageBoundObject):
    def __call__(self, environ, start_response):   # wsgi定義的入口
        """Shortcut for :attr:`wsgi_app`."""
        return self.wsgi_app(environ, start_response)

    def wsgi_app(self, environ, start_response):
        ctx = self.request_context(environ)    # 通過傳入的環境變量構建request信息
        ctx.push()    # 注意：這個ctx就是所說的同時有request,session屬性的上下文
        error = None
        try:
            try:
                response = self.full_dispatch_request()
            except Exception as e:
                error = e
                response = self.make_response(self.handle_exception(e))
            return response(environ, start_response)
        finally:
            if self.should_ignore_error(error):
                error = None
            ctx.auto_pop(error)

    def request_context(self, environ):
        return RequestContext(self, environ)
{% endhighlight %}

如山所示，實際上在 wsgi_app() 中就是 Flask 的主要處理流程了，而且代碼非常清晰，包括了上下文的生成，請求的分發，返回請求的響應。

其中，比較重要的一個就是請求上下文的生成，也就是 request_context() 以及 push() 到棧中，其中的 class RequestContext 就是保持一個請求的上下文的變量，在請求到來之前 _request_ctx_stack 一直是空的，當請求到來的時候會調用 ctx.push() 向棧中添加上下文信息。

在每次請求調用結束後，也就是在 wsgi_app() 中的 finally 中會調用 ctx.auto_pop(error)，該函數中會根據情況判斷是否清除放在 _request_ctx_stack 中的 ctx 。

更加詳細的內容可以參考 [Flask 上下文理解](/blog/flask-context.html) 。



## 請求分發

書接上文，接下來是處理請求的分發。

{% highlight python %}
def full_dispatch_request(self):
    # 1. 首先會判斷是否為第一次執行，通過before_first_request()註冊函數
    self.try_trigger_before_first_request_functions() # 注意只在第一個請求調用
    try:
        request_started.send(self)
        # 2. 一些預處理，包括url_value_preprocessor()、before_request()
        rv = self.preprocess_request()
        if rv is None:
            # 3. 一般上述的函數不會返回值，通常會調用如下函數
            rv = self.dispatch_request()
    except Exception as e:
        rv = self.handle_user_exception(e)
    # 4. 通過返回的值生成響應
    response = self.make_response(rv)
    # 5. 處理after_request()請求
    response = self.process_response(response)
    request_finished.send(self, response=response)
    return response
{% endhighlight %}

在 dispatch_request() 函數中就會根據之前定義的 view 調用相應的函數處理。另外，在該函數之外，也就是彈出上下文的時候會調用 teardown_reques() 註冊的函數。

### dispatch_request()

在該函數中會根據 URL 進行實際的請求分發，異常處理在上述的 full_dispatch_request() 中處理。

{% highlight python %}
def dispatch_request(self):
    req = _request_ctx_stack.top.request
    if req.routing_exception is not None:
        self.raise_routing_exception(req)
    rule = req.url_rule
    # if we provide automatic options for this URL and the
    # request came with the OPTIONS method, reply automatically
    if getattr(rule, 'provide_automatic_options', False) \
       and req.method == 'OPTIONS':
        return self.make_default_options_response()
    # otherwise dispatch to the handler for that endpoint
    return self.view_functions[rule.endpoint](**req.view_args)
{% endhighlight %}


## 鉤子函數

在上述的處理流程中實際上可以之間看出來，在此簡單總結下可以使用的鉤子函數。

* before_first_request<br>
在第一次請求時會調用，後續的所有請求都不會在調用該函數，除非重新初始化。

* before_request<br>
每次處理請求前會調用該函數，包括了第一次請求。

* after_request<br>
如果請求沒有異常，每次請求之後會調用的函數。

* teardown_request<br>
即使遇到了異常，每次請求之後也會調用的函數。

* teardown_appcontext<br>
會在應用退出的時候被調用。

結下來我們看看這些鉤子函數是如何使用的。


{% highlight python %}
from flask import Flask, g
app = Flask(__name__)

@app.before_first_request
def foobar_first_request():
    print "first request"

@app.before_request
def foobar_before_request():
    print "before request"

@app.after_request
def foobar_after_request(response):
    print "after request", response
    return response

@app.teardown_request
def foobar_teardown_request(exception):
    print "teardown request", exception

@app.teardown_appcontext
def teardown_db(exception):
    print "teardown appcontext", exception

@app.route('/')
def hello_world():
    return 'Hello World!\n'

if __name__ == '__main__':
    app.run(host='127.1', debug=True)
{% endhighlight %}






<!--
### 請求開始

通過 request_started.send(self) 發送請求開始信號，告知 subscriber 請求開始了。

### 預處理

self.preprocess_request()


### 將請求返回值構造響應類

self.make_response(rv)


### 請求結束

通過 request_finished.send(self, response=response) 發送請求結束信號。

### 響應客戶端

通過 response(environ, start_response) 函數將響應發送給客戶端。
-->



{% highlight python %}
{% endhighlight %}
