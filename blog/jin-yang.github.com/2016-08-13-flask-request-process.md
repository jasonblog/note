---
title: Flask 请求处理流程
layout: post
comments: true
language: chinese
category: [webserver]
keywords: flask,请求处理
description: 实际上与其它的 web 框架处理流程基本相同，在此介绍下在 Flask 中，一次请求到响应的处理流程。Flask 支持多线程和多进程，通常每个线程或者进程一次只会处理一个请求。当然也可以通过类似 gevent 使用协程、事件库进行优化，对于目前使用的 WebSocket 技术会有较高的优化，不过还没有仔细研究过。OK，接下来看看 Falsk 是如何处理每次的网络请求。
---

实际上与其它的 web 框架处理流程基本相同，在此介绍下在 Flask 中，一次请求到响应的处理流程。Flask 支持多线程和多进程，通常每个线程或者进程一次只会处理一个请求。

当然也可以通过类似 gevent 使用协程、事件库进行优化，对于目前使用的 WebSocket 技术会有较高的优化，不过还没有仔细研究过。

OK，接下来看看 Falsk 是如何处理每次的网络请求。

<!-- more -->

## 简介

Flask 执行时有两种方式，一种是调用 `app.run()` 执行，通常用于调试阶段，默认会在 Python 文件修改之后可以自动加载，所以非常方便；另外一种是通过类似 uwsgi 程序调用执行。

### WSGI

在详细介绍 Flask 处理流程之前，首先说明一下 WSGI 接口定义。

WSGI (Web Server Gateway Interface) 定义了 Web 服务器和 Web 应用程序之间的一种简单而通用的接口，封装了接受请求、解析请求、发送响应等等的这些底层的代码和操作，使开发者可以高效的编写 Web 应用；该接口通常在 Python 中使用，Flask 也是基于此接口的。

如下是从 stackflow 找到的一个不错的图片，一图胜千言阿。

![WSGI Flow(PEP 3333)]({{ site.url }}/images/python/flask-wsgi-interface.png "WSGI Flow(PEP 3333)"){: .pull-center; width='90%'}

如下是一个简单的使用 WSGI 的示例：

{% highlight python %}
def application(environ, start_response):
    start_response('200 OK', [('Content-Type', 'text/html')])
    return [u'<h1>Hello World, This is WSGI!</h1>']
{% endhighlight %}

其中接口非常简单，包含了两个参数：

* environ: 包含全部 HTTP 请求信息的字典，由 WSGI Server 解析 HTTP 请求生成。

* start_response: 一个 WSGI Server 提供的函数，调用可以发送响应的状态码和 HTTP 报文头，函数在返回前必须调用一次 start_response() 进行设置。

另外，application() 应当返回一个可以迭代的对象，也就是 HTTP 正文；该函数由 WSGI Server 直接调用，并提供响应的参数。

![WSGI Application]({{ site.url }}/images/python/flask-wsgi-application.jpg "WSGI Application"){: .pull-center}


Python 内置了一个 WSGIREF 的 WSGI 服务器，不过性能不是很好，一般只用在开发环境；通常开发环境中一般使用 gunicorn、uwsgi 等。

### Flask 调用

在此，我们一步步查看调试时的执行过程（前面说的第一种方式），也就是在此讲解的是 `app.run()` 这种方式的调用；而后者的调用实际也是殊途同归，最终执行路径会到同一代码段，下面会有说明。

接下来从程序初始化、执行程序、处理请求等，一步步查看，实际简化后的调用过程如下。


## 调用 flask 之前

实际上就是在加载程序，然后调用 run() 函数之前。

首先会从 flask 包导入了 Flask 类，该类位于源码的 app.py 文件，基本上就是简单的初始化一些参数；然后，调用该类中的 run() 方法，对于 Flask 类的源码简化后如下。

{% highlight python %}
class Flask(_PackageBoundObject):
    def run(self, host=None, port=None, debug=None, **options):
        from werkzeug.serving import run_simple
        run_simple(host, port, self, **options)
{% endhighlight %}

可以看到，实际执行的是 werkzeug 库中的 run_simple() 函数，该函数在 serving.py 文件中进行定义，该函数非常简单，最终调用的是内嵌的 inner() 函数。

另外，实际上在 Flask 中可以设置为在模块改变之后自动重新加载，这也就是默认值，该功能实际会调用 run_with_reloader() 函数，最终仍然调用 inner() 函数。

接下来看看 inner() 函数，如下。

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

该函数中，通过 make_server() 函数调用，返回 ThreadedWSGIServer()、ForkingWSGIServer()、BaseWSGIServer() 中的一个对象实例，从名称就可以看出来，分别会使用线程、进程做并发。

创建完成之后，就会调用 server_forever() 函数，从名称就可以看出，基本就是一直运行程序，等待请求、处理请求、发送响应；接下来看看其具体的实现。

最终会使用 BaseHTTPServer 中的 HTTPServer 模块，这是一个通用的模块，会调用 WSGIRequestHandler 中的 handle_one_request() 函数，而实际上，最终会调用 run_wsgi() 函数。

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
        # environ是为了给request传递请求的
        # start_response主要是增加响应头和状态码，最后需要werkzeug发送请求
        try:
            for data in application_iter: #根据wsgi规范，app返回的是一个序列
                write(data) #发送结果
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

到此的话，基本就到了 wsgi 规范规定的接口规范，也就是说 app 需要实现一个接口，接受两个参数，也即函数声明是 application(environ, start_response) 。

可以看到，app.run() 模式会调用 BaseHTTPServer 模块处理请求，然后交给实际 wsgi 接口定义的接口。不过，这种方式只适用于调试阶段，BaseHTTPServer 没有进行很好的优化。


## 殊途同归

如上所述，到此为止，实际上就到了 app.run() 和 uwsgi 都调用的接口，也就是 wsgi 的规范中定义的 API 了，最终就到了 Flask 中定义的对象了，也即 Flask 类的定义。

对于 flask 而言，实际是通过定义一个类实现的，也就是 app.py 文件中的 class Flask，而在该类中提供了 \_\_call\_\_() 函数来适配 wsgi 规范。

{% highlight python %}
class Flask(_PackageBoundObject):
    def __call__(self, environ, start_response):   # wsgi定义的入口
        """Shortcut for :attr:`wsgi_app`."""
        return self.wsgi_app(environ, start_response)

    def wsgi_app(self, environ, start_response):
        ctx = self.request_context(environ)    # 通过传入的环境变量构建request信息
        ctx.push()    # 注意：这个ctx就是所说的同时有request,session属性的上下文
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

如山所示，实际上在 wsgi_app() 中就是 Flask 的主要处理流程了，而且代码非常清晰，包括了上下文的生成，请求的分发，返回请求的响应。

其中，比较重要的一个就是请求上下文的生成，也就是 request_context() 以及 push() 到栈中，其中的 class RequestContext 就是保持一个请求的上下文的变量，在请求到来之前 _request_ctx_stack 一直是空的，当请求到来的时候会调用 ctx.push() 向栈中添加上下文信息。

在每次请求调用结束后，也就是在 wsgi_app() 中的 finally 中会调用 ctx.auto_pop(error)，该函数中会根据情况判断是否清除放在 _request_ctx_stack 中的 ctx 。

更加详细的内容可以参考 [Flask 上下文理解](/blog/flask-context.html) 。



## 请求分发

书接上文，接下来是处理请求的分发。

{% highlight python %}
def full_dispatch_request(self):
    # 1. 首先会判断是否为第一次执行，通过before_first_request()注册函数
    self.try_trigger_before_first_request_functions() # 注意只在第一个请求调用
    try:
        request_started.send(self)
        # 2. 一些预处理，包括url_value_preprocessor()、before_request()
        rv = self.preprocess_request()
        if rv is None:
            # 3. 一般上述的函数不会返回值，通常会调用如下函数
            rv = self.dispatch_request()
    except Exception as e:
        rv = self.handle_user_exception(e)
    # 4. 通过返回的值生成响应
    response = self.make_response(rv)
    # 5. 处理after_request()请求
    response = self.process_response(response)
    request_finished.send(self, response=response)
    return response
{% endhighlight %}

在 dispatch_request() 函数中就会根据之前定义的 view 调用相应的函数处理。另外，在该函数之外，也就是弹出上下文的时候会调用 teardown_reques() 注册的函数。

### dispatch_request()

在该函数中会根据 URL 进行实际的请求分发，异常处理在上述的 full_dispatch_request() 中处理。

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


## 钩子函数

在上述的处理流程中实际上可以之间看出来，在此简单总结下可以使用的钩子函数。

* before_first_request<br>
在第一次请求时会调用，后续的所有请求都不会在调用该函数，除非重新初始化。

* before_request<br>
每次处理请求前会调用该函数，包括了第一次请求。

* after_request<br>
如果请求没有异常，每次请求之后会调用的函数。

* teardown_request<br>
即使遇到了异常，每次请求之后也会调用的函数。

* teardown_appcontext<br>
会在应用退出的时候被调用。

结下来我们看看这些钩子函数是如何使用的。


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
### 请求开始

通过 request_started.send(self) 发送请求开始信号，告知 subscriber 请求开始了。

### 预处理

self.preprocess_request()


### 将请求返回值构造响应类

self.make_response(rv)


### 请求结束

通过 request_finished.send(self, response=response) 发送请求结束信号。

### 响应客户端

通过 response(environ, start_response) 函数将响应发送给客户端。
-->



{% highlight python %}
{% endhighlight %}
