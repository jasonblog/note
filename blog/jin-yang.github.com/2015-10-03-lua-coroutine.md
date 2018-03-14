---
title: Lua 協程
layout: post
comments: true
language: chinese
category: [program, linux]
keywords: lua,協程,coroutine
description: 協程是另外一種併發方式，相比多進程或者多線程來說，其上下文的切換成本開銷更小，這也意味這協程的執行更加高效。Lua 作為一種簡單的語言，仍然可以支持閉包、協程等較新的特性，在此介紹一下協程相關的內容。
---

協程是另外一種併發方式，相比多進程或者多線程來說，其上下文的切換成本開銷更小，這也意味這協程的執行更加高效。

Lua 作為一種簡單的語言，仍然可以支持閉包、協程等較新的特性，在此介紹一下協程相關的內容。

<!-- more -->

## 簡介

對於併發任務，通常有兩種解決方法：A) 搶佔式 (preemptive) 多任務，常見的是通過操作系統決定何時執行哪個任務；B) 協作式 (cooperative) 多任務，有任務自身決定何時放棄任務的執行。

協程 (coroutine) 是一種程序控制機制，實際上在上世紀 60 年代就已經被提出，用來方便地實現協作式的多任務。在很多主流的程序語言中，如 C/C++、Java 等，很少能看到協程，但現在不少腳本語言，如 Python、Lua 等，都提供了協程或與之相似的機制。

Lua 實現的是一種非對稱式 (asymmetric) 或者稱為半對稱式 (semi-symmetric) 協程，之所以被稱為非對稱的，是因為它提供了兩種傳遞程序控制權的操作：A) 利用 resume() (重)調用協程；B) 通過 yield 掛起協程並將程序控制權返回給協程的調用者。這看起來有點像函數的調用過程。

而 libtask 的實現就更像對稱式的協程。

協程類似於線程，包含了自己的棧、本地變量、指針，但是包括了其它的全局變量是共享的。與線程不同的是，線程可以真正的在多個核上併發執行，而協程在具體的時間只能運行其中的一個。

### 常用接口

與協程相關的函數包含在 coroutine table 中，其中的 create() 函數用來創建一個協程，該函數會返回一個 thread 類型的變量，用來表示協程。

{% highlight lua %}
co = coroutine.create(function () print("Hello World") end)
print(co)    --> thread: 0x24a0b20
{% endhighlight %}

Lua 中的協程包含了四種狀態：suspended、running、dead 以及 normal，可以通過 satatus() 查看。

{% highlight lua %}
print(coroutine.status(co)) --> suspended
{% endhighlight %}

剛創建的協程處於 suspended 狀態，也就是說創建時不會自動運行；可以通過 resume() 函數運行或者重新運行該協程，將狀態從 suspended 轉換為 running 。如上的函數中，僅僅打印字符串，那麼執行完成之後，狀態會變為 dead 。

到此為止，協程就像是一個執行起來比較麻煩的函數；不過其真正有用的是 yield() 函數，可以將當前協程轉換為 suspended 狀態，然後等待協程後續執行。

{% highlight lua %}
co = coroutine.create(function ()
    for i = 1, 3 do
        print("co", i)
        coroutine.yield()
    end
end)
----- OK，開始執行，直到第一次到達yield
coroutine.resume(co)        --> co     1
----- 此時狀態再次轉換為suspended
print(coroutine.status(co)) --> suspended
----- 再次運行多次
coroutine.resume(co)        --> co     2
coroutine.resume(co)        --> co     3
coroutine.resume(co)        --> prints nothing
print(coroutine.resume(co)) --> false  cannot resume dead coroutine
{% endhighlight %}

當執行了三次之後，在第四次執行時，協程將結束循環並返回；如果再此嘗試執行，則會返回 false 以及相關的錯誤信息。


當在協程 A 中 resume() 另外一個協程 B 時，A 的狀態不是 suspended，因為我們不能再通過 resume() 調用執行，而且協程 A 也不是 running 狀態，協程 B 才是，因此協程 A 處於 normal 狀態。


### resume()

在調用 resume() 執行時，是處於保護狀態的，也就是說如果在協程中有異常時，會返回到 resmue() 調用處。正常來說，yield() 只會返回 true，而實際上還可以通過該函數返回值。

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

如上，通過 return 同樣可以返回相應的值。

## 示例

接下來查看幾個經典的示例。

### 生產者-消費者

這也許是最經典的示例了，其中生產者一直從文件中讀取數據，然後將讀取的數據傳遞給消費者，兩個程序的執行過程如下：

{% highlight lua %}
function producer ()
    while true do
        local x = io.read()  -- 從文件中讀取新的值
        send(x)              -- 將數據發送給消費者
    end
end
function consumer ()
    while true do
        local x = receive()  -- 從生產者中接收新的值
        io.write(x, "\n")    -- 消費之
    end
end
{% endhighlight %}

從表面看來，兩個程序都有自己的循環，而且每個程序都是可以被調用的，但實際上只能有一個是主循環，而另外一個將成為被調用者。通過 resume-yield 剛好可以實現這種邏輯。

當協程調用 yield() 後，將會返回到 resume() 調用；同樣，調用 resume() 時會返回 yield() 的結果。正好符合上述的場景，receive() 調用生產者生成新的數據，然後 send() 將新的值返回給消費者。當然，上述的模型中，生產者將成為協程。

如下為完整的示例，生產者將會從 stdin 中讀取一行，然後傳遞給消費者，通過 Ctrl-D 退出。

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
        local x = io.read()    -- 從文件中讀取新的值
        send(x)                -- 將數據發送給消費者
    end
end)
function consumer ()
    while true do
        local x = receive()    -- 從生產者中接收新的值
        if x == nil then
            return 0
        else
            io.write(x, "\n")  -- 消費之
        end
    end
end
consumer()
{% endhighlight %}

在上述程序中，當消費者需要數據時將會調用生產者，相當於是消費者驅動的；當然，也可以使用生產者驅動，此時消費者將成為協程。

### 生產者-過濾器-消費者

這是上面生產者-消費者模型的擴展，過濾器用來作些數據處理，例如數據轉換、過濾等。此時的過濾器同時是生產者也是消費者，將 resume 調用生產者生成數據，將轉換後的值 yield 給消費者。

整個函數的調用過程如下：

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
            local x = io.read()    -- 從文件中讀取新的值
            send(x)                -- 將數據發送給消費者
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
        local x = receive(prod)    -- 從生產者中接收新的值
        if x == nil then
            return 0
        else
            io.write(x, "\n")      -- 消費之
        end
    end
end
----- 其調用過程大致如下
p = producer()
f = filter(p)
consumer(f)
----- 也可以簡單寫為
--consumer(filter(producer()))
{% endhighlight %}

### 非搶佔式的多線程

實際上協程更類似於非搶佔式的多線程，當一個協程運行時，除非主動通過 yield() 讓出執行權，否則不能被動的搶佔。很多場景下這一模型都能正常運行，但是當任意協程調用阻塞式的操作時，那麼整個程序都將會被阻塞。接下來看看如何解決這一問題。

簡單通過下載多個網頁作為示例。注意，該示例依賴於 socket 模塊，在 CentOS 可以直接通過 yum install lua-socket 進行安裝，依賴 EPEL 源。

如下為簡單讀取 http://www.baidu.com/ 的內容。

{% highlight lua %}
#!/usr/bin/lua
local socket = require("socket")
local host, file = "www.baidu.com", "/"

-- 創建一個TCP連接，連接到HTTP連接的標準80端口上
local sock = assert(socket.connect(host, 80))
sock:send("GET " .. file .. " HTTP/1.0\r\n\r\n")
repeat
    -- 以1K的字節塊來接收數據，並把接收到字節塊輸出來
    local chunk, status, partial = sock:receive(2^10)
    io.write(chunk or partial)
until status == "closed"
-- 關閉 TCP 連接
sock:close()
{% endhighlight %}

如果採用串行的方式，實際上很大一部分時間都耗費在了等待網絡數據上，也就是阻塞在了 receive() 函數調用中。所以，通過併發可以提高讀取效率。

通過協程可以很容易實現併發，當一個協程沒有數據時返回給一個 dispatcher，然後調度另外的協程。

首先需要將上述的程序改寫為一個函數，實際上我們對網頁的內容並不感興趣，因此只統計一下網站返回的字節數，而非其內容。

{% highlight lua %}
function receive (connection)
    connection:settimeout(0)    -- 設置為非阻塞模式
    local s, status, partial = connection:receive(2^10)
    if status == "timeout" then
        coroutine.yield(connection)
    end
    return s or partial, status
end

function download (host, file)
    local c = assert(socket.connect(host, 80))
    local count = 0             -- 用於統計讀取的字節數
    c:send("GET " .. file .. " HTTP/1.0\r\n\r\n")
    repeat
        local chunk, status = receive(c)
        count = count + #chunk
    until status == "closed"
    c:close()
    print(file, count)
end
{% endhighlight %}

接下來看看 dispatcher 的內容。

{% highlight lua %}
threads = {}
function get (host, file)
    -- 創建協程
    local co = coroutine.create(function ()
        download(host, file)
    end)
    table.insert(threads, co)   -- 將創建的協程添加到列表中，等待被調用
end

function dispatch ()
    local i = 1
    while true do
        if threads[i] == nil then               -- 判斷是否還有需要執行的線程
            if threads[1] == nil then break end -- 判斷是否已經都執行完
            i = 1                               -- 還有，從頭開始執行一次
        end
        local status, res = coroutine.resume(threads[i])
        if not res then   -- 返回false表示該協程已經執行完
            table.remove(threads, i)  -- 刪除之
        else
            i = i + 1     -- 然後執行下一個
        end
    end
end
{% endhighlight %}

其中 threads 保存了正在執行的協程，get() 函數用於添加需要下載的頁面。dispatch() 則是主要的任務調度過程，其處理也非常簡單，就是不斷的遍歷整個表，查看是否還有需要執行的任務。

然後通過如下方式進行調用。

{% highlight lua %}
host = "www.w3.org"
get(host, "/TR/html401/html40.txt")
get(host, "/TR/2002/REC-xhtml1-20020801/xhtml1.pdf")
get(host, "/TR/REC-html32.html")
get(host, "/TR/2000/REC-DOM-Level-2-Core-20001113/DOM2-Core.txt")
dispatch()
{% endhighlight %}


## 參考

[Coroutines in Lua](http://www.inf.puc-rio.br/~roberto/docs/corosblp.pdf) 。

{% highlight lua %}
{% endhighlight %}
