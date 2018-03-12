---
title: Lua 简介
layout: post
comments: true
language: chinese
category: [program,linux]
keywords: lua, introduce, 简介
description: Lua 在葡萄牙语中是 “月亮” 的意思，是一个小巧的脚本语言，官方版本只包括一个精简的核心和最基本的库，使得其体积小、启动速度快，从而特别适合嵌入到其它程序里。Lua 历史可以从 1993 年开始，到现在为止，在很多场景都可以看到 Lua 的影子，如游戏开发、Web 应用脚本、安全系统插件等，像常见的 nmap、sysbench 等程序。这里简单介绍 Lua 的安装使用。
---

Lua 在葡萄牙语中是 “月亮” 的意思，是一个小巧的脚本语言，官方版本只包括一个精简的核心和最基本的库，使得其体积小、启动速度快，从而特别适合嵌入到其它程序里。

Lua 历史可以从 1993 年开始，到现在为止，在很多场景都可以看到 Lua 的影子，如游戏开发、Web 应用脚本、安全系统插件等，像常见的 nmap、sysbench 等程序。

这里简单介绍 Lua 的安装使用。

<!-- more -->

另外，关于 Lua 的详细历史可以参考 [A History of Lua][history] 。

## 安装使用

在 CentOS 中，Lua 的安装很简单，可以通过 yum 或者源码安装。
{% highlight text %}
# yum install lua                      # 通过yum安装
{% endhighlight %}

源码可以直接从 [www.lua.org][offical] 下载，源码安装详细内容可以参考 doc/readme.html 文件，或者在 CentOS 中通过如下命令安装。
{% highlight text %}
$ make linux                           # 根据不同的平台选择目标；注意，会依赖readline-devel包
$ make test                            # 测试编译是否正确，简单输出版本信息
# make linux install                   # 安装
{% endhighlight %}
安装之后，主要包括了 lua、luac 两个命令，其中前者为解析器，后者为编译器。

对于 Lua，可以直接运行 Lua 交互界面，输入命令；也可以通过 lua 命令执行脚本；也可以在运行之前，先通过 luac 编译为字节码，然后运行。

{% highlight text %}
$ lua                                  # 1.1 使用交互界面
> print('hello world')

$ lua -e "print('hello world')"        # 2.1 运行单条命令

$ cat foobar.lua                       # 2.2 查看文件，也可以为 #!/usr/bin/env lua
#!/usr/bin/lua
print('hello world')

$ lua foobar.lua                       # 2.2 直接运行
$ ./foobar.lua

$ luac -o foobar foobar.lua            # 3.1 先编译称字节码
$ lua foobar                           # 3.2 然后运行
{% endhighlight %}

另外，可以将 Lua 嵌入到 C/C++ 等语言中，后面详细介绍。

### 运行

在上述的简介中已经介绍过了部分执行的方式，可以直接解析执行，也可以 "编译" 之后执行，除此之外还可以通过 -i 参数在进入交互程序前先执行一个程序。

{% highlight text %}
----- 先执行一段程序打印Hello World
$ lua -i foobar.lua
Lua 5.1.4  Copyright (C) 1994-2008 Lua.org, PUC-Rio
Hello World
>
{% endhighlight %}

另外，在交互程序中，可以通过 dofile() 加载文件，然后可以调用相应的函数。

{% highlight text %}
----- 先加载，然后调用函数
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

这两个功能在调试程序时非常有效。


## 语法简介

在 Lua 中，一个文件甚至在交互模式中的一行，都被称为代码块 (chunk)。而一个 chunk 是由一系列的命令或者声明组成，与很多语言不同的是，各个语句之间甚至不需要 ';' 分割，也就是 ';' 是可选的，例如，下面的语句是相同的：

{% highlight text %}
a = 1
b = a*2

a = 1;
b = a*2;

a = 1; b = a*2;

a = 1  b = a*2           # 仍然等效
{% endhighlight %}

### 注释

在 Lua 中，通过两个减号作为行注释；当然，还有块注释：

{% highlight lua %}
-- 作为行注释

--[[
    print("foobar") -- 作为块注释
--]]

---[[
    print("foobar") -- 如果有代码，那么这里就不是块注释了
--]]
{% endhighlight %}


### 变量

与大多数语言类似，变量名可以为字符串、下划线、数字 (不能以数字开头)；不过，需要注意的是，在 Lua 中，以下划线开头通常有特殊的含义，例如 \_VERSION、\_ 等。

另外，需要注意的是，lua 中，如果没有特殊说明，均是全局变量，无论是语句块还是在函数中，如果需要局部变量则需要使用 local 关键字。对于全局变量，在使用之前是不需要初始化的，默认值为 nil 。

而且，Lua 是动态类型的语言 (Dynamically Typed Language)，也就是说定义变量时无需指定类型，每个值都包含其自己的类型。

Lua 中总共有 8 种基本的类型： nil、boolean、number、string、userdata、function、thread、table 。

{% highlight lua %}
print(type(a))                --> nil
print(type(false))            --> boolean
print(type(10.4))             --> number
print(type("Hello world"))    --> string

print(type(print))            --> function
{% endhighlight %}

各种类型的详细信息如下：

* nil 是一个区别于其它值的值，也就是说用来表明该变量还没有被赋值。

* Booleans 该类型的值只有两个 false 和 true，其它类型的值作比较时均为 false。

* Number 数字类型只有 double(64bits)，即使是整形仍然使用 double 表示。

* Strings 也就是用来表示一连串的字符，当然也支持 unicode；而且可以通过 [[ Long Strings ]] 表示比较长的字符串。

* Tables 这是 Lua 中提供高效的表示方法，可表示 arrays、sets 等一系列数据结构。

* Functions 很简单，就是函数了；Lua 解析器是通过 C 编写的，Lua 也可以调用 C 函数。

对于 Number 类型，这里多说几句。Lua 中的 Number 满足 IEEE-754 标准，对于整型在比较时是正确的，只有对于像 1/7 这样的值，对于有限浮点数实际是无法完整表示的。假设采用 6 位表示浮点数，则 1/7 实际表示为 0.1428571，那么 1/7 * 7 = 0.9999997 。

另外，现在的 CPU 通常是有单独的浮点运算模块，而对于没有浮点运算模块的 CPU 可以在编译 Lua 时指定对应 Nubmer 的类型，可以为 float 或者 int 。

{% highlight lua %}
num = 1024
num = 3.0
num = 314.16e-2
num = 0.31416E1
num = 0x56
{% endhighlight %}

### Table

在 Lua 中，所谓 Table 其实就是一个 Key-Value 的数据结构，很像 Python 中的字典，不过其 Key 不仅可以是字符串，也可以是数字甚至是浮点数。

{% highlight lua %}
t = {name="Andy", [10]=100, [3.1415]="PI", ['age']=30}
print(t.name, t[10], t[3.1415], t.age, t["age"])
{% endhighlight %}

如前所述，Lua 中的变量，如果没有 local 关键字，则都是全局变量，所有的全局变量都保存在 _G 的 table 中，可以通过下面的方式来遍历。

{% highlight lua %}
for k, v in pairs(_G) do
    print(k, v)
end
{% endhighlight %}
<!-- http://www.lua.org/pil/3.6.html -->

### 控制语句

也就是 while、if-else、for 等控制语句。

{% highlight lua %}
----- 使用while循环
count = 1
while count < 6 do
    count = count + 1
end

----- 使用if-else控制语句
if op == "+" then
    r = a + b
elseif op == "-" then
    r = a - b
elseif op == "*" then
    r = a * b
else
    error("invalid operation")
end

----- 使用repeat-until语句
repeat
    line = io.read()
until line ~= ""
print(line)

----- 使用for循环，包括了如下的几种方式
for i = 1, 10, 2 do --打印1到10的奇数
    print(i)
end
days = {"Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday"}
for k, v in pairs(days) do
    print(k, v)
end
{% endhighlight %}

除了上述的示例之外，还包括了 break、return、goto 等语句。


### 函数

在 Lua 中，函数也可以作为一个变量进行赋值：

{% highlight lua %}
v = print
v("Hello World")
v = math.sin
v(1)

----- 下面的两个函数一样
function foo(x) return x^2 end
foo = function(x) return x^2 end
{% endhighlight %}

Lua 同样支持递归，最简单的就是斐波那契数列求和。

{% highlight lua %}
function fib(n)
  if n < 2 then return 1 end
  return fib(n - 2) + fib(n - 1)
end
{% endhighlight %}

Lua 同样也支持闭包，简单来说，当有两个函数嵌套时，被嵌套的函数可以访问上层函数保存的变量。简单来说，函数是用来处理数据的，而闭包同时保存了数据。

{% highlight lua %}
----- 使用简单的计数器
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

----- 计算N次方
function power(N)
    return function(y) return y^N end
end
power2 = power(2)
power3 = power(3)
print(power2(4))       -- 4的2次方
print(power3(5))       -- 5的3次方
{% endhighlight %}

类似于 Python、Go 语言，可以在一条语句上赋多个值，函数也可以返回多个值：

{% highlight lua %}
name, age = "foobar", 20, "foobar@test.com"
function getUserInfo()
    return "foobar", 20, "foobar@test.com"
end
name, age, email, website = getUserInfo()
{% endhighlight %}

上述的赋值中，最后一个字符串将会丢弃；而函数返回时，其中的 website 为 nil 。


### 其它

~= 表示不等于，而非 != 。

.. 为字符拼接操作符。


## 常用模块

常见的模块，以及相关的示例。

### Socket

[LuaSocket](http://w3.impa.br/~diego/software/luasocket/) 是 Lua 的网络模块，提供了对 TCP、UDP、DNS、HTTP 等多种网络协议的访问操作，包括了两部分：A) 使用 C 编写的针对 TCP 和 UDP 传输层的访问；B) 用 Lua 编写，负责应用功能的网络接口处理。

#### 安装

相关的源码可以从 [GitHub](https://github.com/diegonehab/luasocket) 上下载，在 CentOS 中可以直接通过如下命令安装，不过需要依赖 EPEL 源。

{% highlight text %}
# yum install lua-socket
{% endhighlight %}

安装完成之后可以通过如下方式进行测试，直接用 require() 函数加载该模块，然后输出当前 LuaSocket 的版本信息。

{% highlight lua %}
local socket = require("socket")
print(socket._VERSION)
{% endhighlight %}

#### 简单示例

简单读取一个网站的网页。

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


## 参考

官方文档 [www.lua.org](http://www.lua.org/)，以及相关的参考文档 [Lua 5.3 Reference Manual](http://www.lua.org/manual/5.3/) 。

Lua 的历史简介 [The evolution of an extension language: a history of Lua](http://www.lua.org/history.html)。



[offical]: http://www.lua.org/                     "Lua 官方网站"
[history]: http://www.lua.org/history.html         "The evolution of an extension language: a history of Lua"

{% highlight lua %}
{% endhighlight %}
