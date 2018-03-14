---
title: Lua 簡介
layout: post
comments: true
language: chinese
category: [program,linux]
keywords: lua, introduce, 簡介
description: Lua 在葡萄牙語中是 “月亮” 的意思，是一個小巧的腳本語言，官方版本只包括一個精簡的核心和最基本的庫，使得其體積小、啟動速度快，從而特別適合嵌入到其它程序裡。Lua 歷史可以從 1993 年開始，到現在為止，在很多場景都可以看到 Lua 的影子，如遊戲開發、Web 應用腳本、安全系統插件等，像常見的 nmap、sysbench 等程序。這裡簡單介紹 Lua 的安裝使用。
---

Lua 在葡萄牙語中是 “月亮” 的意思，是一個小巧的腳本語言，官方版本只包括一個精簡的核心和最基本的庫，使得其體積小、啟動速度快，從而特別適合嵌入到其它程序裡。

Lua 歷史可以從 1993 年開始，到現在為止，在很多場景都可以看到 Lua 的影子，如遊戲開發、Web 應用腳本、安全系統插件等，像常見的 nmap、sysbench 等程序。

這裡簡單介紹 Lua 的安裝使用。

<!-- more -->

另外，關於 Lua 的詳細歷史可以參考 [A History of Lua][history] 。

## 安裝使用

在 CentOS 中，Lua 的安裝很簡單，可以通過 yum 或者源碼安裝。
{% highlight text %}
# yum install lua                      # 通過yum安裝
{% endhighlight %}

源碼可以直接從 [www.lua.org][offical] 下載，源碼安裝詳細內容可以參考 doc/readme.html 文件，或者在 CentOS 中通過如下命令安裝。
{% highlight text %}
$ make linux                           # 根據不同的平臺選擇目標；注意，會依賴readline-devel包
$ make test                            # 測試編譯是否正確，簡單輸出版本信息
# make linux install                   # 安裝
{% endhighlight %}
安裝之後，主要包括了 lua、luac 兩個命令，其中前者為解析器，後者為編譯器。

對於 Lua，可以直接運行 Lua 交互界面，輸入命令；也可以通過 lua 命令執行腳本；也可以在運行之前，先通過 luac 編譯為字節碼，然後運行。

{% highlight text %}
$ lua                                  # 1.1 使用交互界面
> print('hello world')

$ lua -e "print('hello world')"        # 2.1 運行單條命令

$ cat foobar.lua                       # 2.2 查看文件，也可以為 #!/usr/bin/env lua
#!/usr/bin/lua
print('hello world')

$ lua foobar.lua                       # 2.2 直接運行
$ ./foobar.lua

$ luac -o foobar foobar.lua            # 3.1 先編譯稱字節碼
$ lua foobar                           # 3.2 然後運行
{% endhighlight %}

另外，可以將 Lua 嵌入到 C/C++ 等語言中，後面詳細介紹。

### 運行

在上述的簡介中已經介紹過了部分執行的方式，可以直接解析執行，也可以 "編譯" 之後執行，除此之外還可以通過 -i 參數在進入交互程序前先執行一個程序。

{% highlight text %}
----- 先執行一段程序打印Hello World
$ lua -i foobar.lua
Lua 5.1.4  Copyright (C) 1994-2008 Lua.org, PUC-Rio
Hello World
>
{% endhighlight %}

另外，在交互程序中，可以通過 dofile() 加載文件，然後可以調用相應的函數。

{% highlight text %}
----- 先加載，然後調用函數
$ cat lib.lua
function trible(x)
    return 3 * x
end
$ lua
Lua 5.1.4  Copyright (C) 1994-2008 Lua.org, PUC-Rio
> dofile("lib.lua")
> print (trible(2))
6
{% endhighlight %}

這兩個功能在調試程序時非常有效。


## 語法簡介

在 Lua 中，一個文件甚至在交互模式中的一行，都被稱為代碼塊 (chunk)。而一個 chunk 是由一系列的命令或者聲明組成，與很多語言不同的是，各個語句之間甚至不需要 ';' 分割，也就是 ';' 是可選的，例如，下面的語句是相同的：

{% highlight text %}
a = 1
b = a*2

a = 1;
b = a*2;

a = 1; b = a*2;

a = 1  b = a*2           # 仍然等效
{% endhighlight %}

### 註釋

在 Lua 中，通過兩個減號作為行註釋；當然，還有塊註釋：

{% highlight lua %}
-- 作為行註釋

--[[
    print("foobar") -- 作為塊註釋
--]]

---[[
    print("foobar") -- 如果有代碼，那麼這裡就不是塊註釋了
--]]
{% endhighlight %}


### 變量

與大多數語言類似，變量名可以為字符串、下劃線、數字 (不能以數字開頭)；不過，需要注意的是，在 Lua 中，以下劃線開頭通常有特殊的含義，例如 \_VERSION、\_ 等。

另外，需要注意的是，lua 中，如果沒有特殊說明，均是全局變量，無論是語句塊還是在函數中，如果需要局部變量則需要使用 local 關鍵字。對於全局變量，在使用之前是不需要初始化的，默認值為 nil 。

而且，Lua 是動態類型的語言 (Dynamically Typed Language)，也就是說定義變量時無需指定類型，每個值都包含其自己的類型。

Lua 中總共有 8 種基本的類型： nil、boolean、number、string、userdata、function、thread、table 。

{% highlight lua %}
print(type(a))                --> nil
print(type(false))            --> boolean
print(type(10.4))             --> number
print(type("Hello world"))    --> string

print(type(print))            --> function
{% endhighlight %}

各種類型的詳細信息如下：

* nil 是一個區別於其它值的值，也就是說用來表明該變量還沒有被賦值。

* Booleans 該類型的值只有兩個 false 和 true，其它類型的值作比較時均為 false。

* Number 數字類型只有 double(64bits)，即使是整形仍然使用 double 表示。

* Strings 也就是用來表示一連串的字符，當然也支持 unicode；而且可以通過 [[ Long Strings ]] 表示比較長的字符串。

* Tables 這是 Lua 中提供高效的表示方法，可表示 arrays、sets 等一系列數據結構。

* Functions 很簡單，就是函數了；Lua 解析器是通過 C 編寫的，Lua 也可以調用 C 函數。

對於 Number 類型，這裡多說幾句。Lua 中的 Number 滿足 IEEE-754 標準，對於整型在比較時是正確的，只有對於像 1/7 這樣的值，對於有限浮點數實際是無法完整表示的。假設採用 6 位表示浮點數，則 1/7 實際表示為 0.1428571，那麼 1/7 * 7 = 0.9999997 。

另外，現在的 CPU 通常是有單獨的浮點運算模塊，而對於沒有浮點運算模塊的 CPU 可以在編譯 Lua 時指定對應 Nubmer 的類型，可以為 float 或者 int 。

{% highlight lua %}
num = 1024
num = 3.0
num = 314.16e-2
num = 0.31416E1
num = 0x56
{% endhighlight %}

### Table

在 Lua 中，所謂 Table 其實就是一個 Key-Value 的數據結構，很像 Python 中的字典，不過其 Key 不僅可以是字符串，也可以是數字甚至是浮點數。

{% highlight lua %}
t = {name="Andy", [10]=100, [3.1415]="PI", ['age']=30}
print(t.name, t[10], t[3.1415], t.age, t["age"])
{% endhighlight %}

如前所述，Lua 中的變量，如果沒有 local 關鍵字，則都是全局變量，所有的全局變量都保存在 _G 的 table 中，可以通過下面的方式來遍歷。

{% highlight lua %}
for k, v in pairs(_G) do
    print(k, v)
end
{% endhighlight %}
<!-- http://www.lua.org/pil/3.6.html -->

### 控制語句

也就是 while、if-else、for 等控制語句。

{% highlight lua %}
----- 使用while循環
count = 1
while count < 6 do
    count = count + 1
end

----- 使用if-else控制語句
if op == "+" then
    r = a + b
elseif op == "-" then
    r = a - b
elseif op == "*" then
    r = a * b
else
    error("invalid operation")
end

----- 使用repeat-until語句
repeat
    line = io.read()
until line ~= ""
print(line)

----- 使用for循環，包括瞭如下的幾種方式
for i = 1, 10, 2 do --打印1到10的奇數
    print(i)
end
days = {"Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday"}
for k, v in pairs(days) do
    print(k, v)
end
{% endhighlight %}

除了上述的示例之外，還包括了 break、return、goto 等語句。


### 函數

在 Lua 中，函數也可以作為一個變量進行賦值：

{% highlight lua %}
v = print
v("Hello World")
v = math.sin
v(1)

----- 下面的兩個函數一樣
function foo(x) return x^2 end
foo = function(x) return x^2 end
{% endhighlight %}

Lua 同樣支持遞歸，最簡單的就是斐波那契數列求和。

{% highlight lua %}
function fib(n)
  if n < 2 then return 1 end
  return fib(n - 2) + fib(n - 1)
end
{% endhighlight %}

Lua 同樣也支持閉包，簡單來說，當有兩個函數嵌套時，被嵌套的函數可以訪問上層函數保存的變量。簡單來說，函數是用來處理數據的，而閉包同時保存了數據。

{% highlight lua %}
----- 使用簡單的計數器
function counter()
    local i = 0
    return function()     -- anonymous function
        i = i + 1
        return i
    end
end
c1 = counter()
print(c1())  --> 1
print(c1())  --> 2

----- 計算N次方
function power(N)
    return function(y) return y^N end
end
power2 = power(2)
power3 = power(3)
print(power2(4))       -- 4的2次方
print(power3(5))       -- 5的3次方
{% endhighlight %}

類似於 Python、Go 語言，可以在一條語句上賦多個值，函數也可以返回多個值：

{% highlight lua %}
name, age = "foobar", 20, "foobar@test.com"
function getUserInfo()
    return "foobar", 20, "foobar@test.com"
end
name, age, email, website = getUserInfo()
{% endhighlight %}

上述的賦值中，最後一個字符串將會丟棄；而函數返回時，其中的 website 為 nil 。


### 其它

~= 表示不等於，而非 != 。

.. 為字符拼接操作符。


## 常用模塊

常見的模塊，以及相關的示例。

### Socket

[LuaSocket](http://w3.impa.br/~diego/software/luasocket/) 是 Lua 的網絡模塊，提供了對 TCP、UDP、DNS、HTTP 等多種網絡協議的訪問操作，包括了兩部分：A) 使用 C 編寫的針對 TCP 和 UDP 傳輸層的訪問；B) 用 Lua 編寫，負責應用功能的網絡接口處理。

#### 安裝

相關的源碼可以從 [GitHub](https://github.com/diegonehab/luasocket) 上下載，在 CentOS 中可以直接通過如下命令安裝，不過需要依賴 EPEL 源。

{% highlight text %}
# yum install lua-socket
{% endhighlight %}

安裝完成之後可以通過如下方式進行測試，直接用 require() 函數加載該模塊，然後輸出當前 LuaSocket 的版本信息。

{% highlight lua %}
local socket = require("socket")
print(socket._VERSION)
{% endhighlight %}

#### 簡單示例

簡單讀取一個網站的網頁。

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


## 參考

官方文檔 [www.lua.org](http://www.lua.org/)，以及相關的參考文檔 [Lua 5.3 Reference Manual](http://www.lua.org/manual/5.3/) 。

Lua 的歷史簡介 [The evolution of an extension language: a history of Lua](http://www.lua.org/history.html)。



[offical]: http://www.lua.org/                     "Lua 官方網站"
[history]: http://www.lua.org/history.html         "The evolution of an extension language: a history of Lua"

{% highlight lua %}
{% endhighlight %}
