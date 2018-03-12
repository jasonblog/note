---
title: Lua 协程
layout: post
comments: true
language: chinese
category: [program, linux]
keywords: lua,协程,coroutine
description: 协程是另外一种并发方式，相比多进程或者多线程来说，其上下文的切换成本开销更小，这也意味这协程的执行更加高效。Lua 作为一种简单的语言，仍然可以支持闭包、协程等较新的特性，在此介绍一下协程相关的内容。
---

协程是另外一种并发方式，相比多进程或者多线程来说，其上下文的切换成本开销更小，这也意味这协程的执行更加高效。

Lua 作为一种简单的语言，仍然可以支持闭包、协程等较新的特性，在此介绍一下协程相关的内容。

<!-- more -->

## 简介

对于并发任务，通常有两种解决方法：A) 抢占式 (preemptive) 多任务，常见的是通过操作系统决定何时执行哪个任务；B) 协作式 (cooperative) 多任务，有任务自身决定何时放弃任务的执行。

协程 (coroutine) 是一种程序控制机制，实际上在上世纪 60 年代就已经被提出，用来方便地实现协作式的多任务。在很多主流的程序语言中，如 C/C++、Java 等，很少能看到协程，但现在不少脚本语言，如 Python、Lua 等，都提供了协程或与之相似的机制。

Lua 实现的是一种非对称式 (asymmetric) 或者称为半对称式 (semi-symmetric) 协程，之所以被称为非对称的，是因为它提供了两种传递程序控制权的操作：A) 利用 resume() (重)调用协程；B) 通过 yield 挂起协程并将程序控制权返回给协程的调用者。这看起来有点像函数的调用过程。

而 libtask 的实现就更像对称式的协程。

协程类似于线程，包含了自己的栈、本地变量、指针，但是包括了其它的全局变量是共享的。与线程不同的是，线程可以真正的在多个核上并发执行，而协程在具体的时间只能运行其中的一个。

### 常用接口

与协程相关的函数包含在 coroutine table 中，其中的 create() 函数用来创建一个协程，该函数会返回一个 thread 类型的变量，用来表示协程。

{% highlight lua %}
co = coroutine.create(function () print("Hello World") end)
print(co)    --> thread: 0x24a0b20
{% endhighlight %}

Lua 中的协程包含了四种状态：suspended、running、dead 以及 normal，可以通过 satatus() 查看。

{% highlight lua %}
print(coroutine.status(co)) --> suspended
{% endhighlight %}

刚创建的协程处于 suspended 状态，也就是说创建时不会自动运行；可以通过 resume() 函数运行或者重新运行该协程，将状态从 suspended 转换为 running 。如上的函数中，仅仅打印字符串，那么执行完成之后，状态会变为 dead 。

到此为止，协程就像是一个执行起来比较麻烦的函数；不过其真正有用的是 yield() 函数，可以将当前协程转换为 suspended 状态，然后等待协程后续执行。

{% highlight lua %}
co = coroutine.create(function ()
    for i = 1, 3 do
        print("co", i)
        coroutine.yield()
    end
end)
----- OK，开始执行，直到第一次到达yield
coroutine.resume(co)        --> co     1
----- 此时状态再次转换为suspended
print(coroutine.status(co)) --> suspended
----- 再次运行多次
coroutine.resume(co)        --> co     2
coroutine.resume(co)        --> co     3
coroutine.resume(co)        --> prints nothing
print(coroutine.resume(co)) --> false  cannot resume dead coroutine
{% endhighlight %}

当执行了三次之后，在第四次执行时，协程将结束循环并返回；如果再此尝试执行，则会返回 false 以及相关的错误信息。


当在协程 A 中 resume() 另外一个协程 B 时，A 的状态不是 suspended，因为我们不能再通过 resume() 调用执行，而且协程 A 也不是 running 状态，协程 B 才是，因此协程 A 处于 normal 状态。


### resume()

在调用 resume() 执行时，是处于保护状态的，也就是说如果在协程中有异常时，会返回到 resmue() 调用处。正常来说，yield() 只会返回 true，而实际上还可以通过该函数返回值。

{% highlight lua %}
co = coroutine.create(function (a,b)
    coroutine.yield(a + b, a - b)
end)
print(coroutine.resume(co, 20, 10)) --> true  30  10

co = coroutine.create(function ()
    return 6, 7
end)
print(coroutine.resume(co)) --> true   6   7
{% endhighlight %}

如上，通过 return 同样可以返回相应的值。

## 示例

接下来查看几个经典的示例。

### 生产者-消费者

这也许是最经典的示例了，其中生产者一直从文件中读取数据，然后将读取的数据传递给消费者，两个程序的执行过程如下：

{% highlight lua %}
function producer ()
    while true do
        local x = io.read()  -- 从文件中读取新的值
        send(x)              -- 将数据发送给消费者
    end
end
function consumer ()
    while true do
        local x = receive()  -- 从生产者中接收新的值
        io.write(x, "\n")    -- 消费之
    end
end
{% endhighlight %}

从表面看来，两个程序都有自己的循环，而且每个程序都是可以被调用的，但实际上只能有一个是主循环，而另外一个将成为被调用者。通过 resume-yield 刚好可以实现这种逻辑。

当协程调用 yield() 后，将会返回到 resume() 调用；同样，调用 resume() 时会返回 yield() 的结果。正好符合上述的场景，receive() 调用生产者生成新的数据，然后 send() 将新的值返回给消费者。当然，上述的模型中，生产者将成为协程。

如下为完整的示例，生产者将会从 stdin 中读取一行，然后传递给消费者，通过 Ctrl-D 退出。

{% highlight lua %}
function receive ()
    local status, value = coroutine.resume(producer)
    return value
end
function send (x)
    coroutine.yield(x)
end
producer = coroutine.create(function ()
    while true do
        local x = io.read()    -- 从文件中读取新的值
        send(x)                -- 将数据发送给消费者
    end
end)
function consumer ()
    while true do
        local x = receive()    -- 从生产者中接收新的值
        if x == nil then
            return 0
        else
            io.write(x, "\n")  -- 消费之
        end
    end
end
consumer()
{% endhighlight %}

在上述程序中，当消费者需要数据时将会调用生产者，相当于是消费者驱动的；当然，也可以使用生产者驱动，此时消费者将成为协程。

### 生产者-过滤器-消费者

这是上面生产者-消费者模型的扩展，过滤器用来作些数据处理，例如数据转换、过滤等。此时的过滤器同时是生产者也是消费者，将 resume 调用生产者生成数据，将转换后的值 yield 给消费者。

整个函数的调用过程如下：

{% highlight lua %}
function receive (prod)
    local status, value = coroutine.resume(prod)
    return value
end
function send (x)
    coroutine.yield(x)
end
function producer ()
    return coroutine.create(function ()
        while true do
            local x = io.read()    -- 从文件中读取新的值
            send(x)                -- 将数据发送给消费者
        end
    end)
end
function filter (prod)
    return coroutine.create(function ()
        for line = 1, math.huge do
            local x = receive(prod)
            if x ~= nil then
                x = string.format("%5d %s", line, x)
            end
            send(x)
        end
    end)
end
function consumer (prod)
    while true do
        local x = receive(prod)    -- 从生产者中接收新的值
        if x == nil then
            return 0
        else
            io.write(x, "\n")      -- 消费之
        end
    end
end
----- 其调用过程大致如下
p = producer()
f = filter(p)
consumer(f)
----- 也可以简单写为
--consumer(filter(producer()))
{% endhighlight %}

### 非抢占式的多线程

实际上协程更类似于非抢占式的多线程，当一个协程运行时，除非主动通过 yield() 让出执行权，否则不能被动的抢占。很多场景下这一模型都能正常运行，但是当任意协程调用阻塞式的操作时，那么整个程序都将会被阻塞。接下来看看如何解决这一问题。

简单通过下载多个网页作为示例。注意，该示例依赖于 socket 模块，在 CentOS 可以直接通过 yum install lua-socket 进行安装，依赖 EPEL 源。

如下为简单读取 http://www.baidu.com/ 的内容。

{% highlight lua %}
#!/usr/bin/lua
local socket = require("socket")
local host, file = "www.baidu.com", "/"

-- 创建一个TCP连接，连接到HTTP连接的标准80端口上
local sock = assert(socket.connect(host, 80))
sock:send("GET " .. file .. " HTTP/1.0\r\n\r\n")
repeat
    -- 以1K的字节块来接收数据，并把接收到字节块输出来
    local chunk, status, partial = sock:receive(2^10)
    io.write(chunk or partial)
until status == "closed"
-- 关闭 TCP 连接
sock:close()
{% endhighlight %}

如果采用串行的方式，实际上很大一部分时间都耗费在了等待网络数据上，也就是阻塞在了 receive() 函数调用中。所以，通过并发可以提高读取效率。

通过协程可以很容易实现并发，当一个协程没有数据时返回给一个 dispatcher，然后调度另外的协程。

首先需要将上述的程序改写为一个函数，实际上我们对网页的内容并不感兴趣，因此只统计一下网站返回的字节数，而非其内容。

{% highlight lua %}
function receive (connection)
    connection:settimeout(0)    -- 设置为非阻塞模式
    local s, status, partial = connection:receive(2^10)
    if status == "timeout" then
        coroutine.yield(connection)
    end
    return s or partial, status
end

function download (host, file)
    local c = assert(socket.connect(host, 80))
    local count = 0             -- 用于统计读取的字节数
    c:send("GET " .. file .. " HTTP/1.0\r\n\r\n")
    repeat
        local chunk, status = receive(c)
        count = count + #chunk
    until status == "closed"
    c:close()
    print(file, count)
end
{% endhighlight %}

接下来看看 dispatcher 的内容。

{% highlight lua %}
threads = {}
function get (host, file)
    -- 创建协程
    local co = coroutine.create(function ()
        download(host, file)
    end)
    table.insert(threads, co)   -- 将创建的协程添加到列表中，等待被调用
end

function dispatch ()
    local i = 1
    while true do
        if threads[i] == nil then               -- 判断是否还有需要执行的线程
            if threads[1] == nil then break end -- 判断是否已经都执行完
            i = 1                               -- 还有，从头开始执行一次
        end
        local status, res = coroutine.resume(threads[i])
        if not res then   -- 返回false表示该协程已经执行完
            table.remove(threads, i)  -- 删除之
        else
            i = i + 1     -- 然后执行下一个
        end
    end
end
{% endhighlight %}

其中 threads 保存了正在执行的协程，get() 函数用于添加需要下载的页面。dispatch() 则是主要的任务调度过程，其处理也非常简单，就是不断的遍历整个表，查看是否还有需要执行的任务。

然后通过如下方式进行调用。

{% highlight lua %}
host = "www.w3.org"
get(host, "/TR/html401/html40.txt")
get(host, "/TR/2002/REC-xhtml1-20020801/xhtml1.pdf")
get(host, "/TR/REC-html32.html")
get(host, "/TR/2000/REC-DOM-Level-2-Core-20001113/DOM2-Core.txt")
dispatch()
{% endhighlight %}


## 参考

[Coroutines in Lua](http://www.inf.puc-rio.br/~roberto/docs/corosblp.pdf) 。

{% highlight lua %}
{% endhighlight %}
