---
title: RESTful
layout: post
comments: true
language: chinese
category: [network,linux]
keywords: hello world,示例,sample,markdown
description: 简单记录一下一些与 Markdown 相关的内容，包括了一些使用模版。
---

RESTful 架构是一种互联网软件架构，它结构清晰、符合标准、易于理解、扩展方便，所以正得到越来越多网站的采用。

<!-- more -->

<!--
出来好多文章都是说怎么在RESTful Uri里使用动词等等，这些人除了一部分是把文章拿来抄一抄的，其他的其实搞混了REST和RPC的概念了，REST强调资源，RPC强调动作，所以REST的Uri组成为名词，RPC多为动词短语。然而它们也并不是完全不相关的两个东西，本文中的实现就会参考一部分JSON-RPC的设计思想。
-->

![restful api logo]({{ site.url }}/images/network/restful-api-logo.gif "restful api logo"){: .pull-center }

## 简介

REST 这个词是 [Roy Thomas Fielding](https://en.wikipedia.org/wiki/Roy_Fielding) 在 2000 年的 [博士论文](http://www.ics.uci.edu/~fielding/pubs/dissertation/top.htm) 中提出，也就是他对互联网软件的架构原则，全称为 Representational State Transfer 。

他在论文中提到："我这篇文章的写作目的，就是想在符合架构原理的前提下，理解和评估以网络为基础的应用软件的架构设计，得到一个功能强、性能好、适宜通信的架构。REST 指的是一组架构约束条件和原则。" 如果一个架构符合 REST 的约束条件和原则，我们就称它为 RESTful 架构。

要理解 RESTful 架构，最好的方法就是去理解 `Representational State Transfer` 这个词组所代表的具体含义，进而可以体会 REST 是一种什么样的设计。

### 资源, Resources

REST 中其实省略了主语 "资源"，这也就是网络上的一个事物，可以是实体，如手机号、文本、图片；也可以只是一个抽象概念，如服务。

可以用一个 URI（统一资源定位符）指向它，每种资源对应一个特定的 URI，URI 是每个资源的地址的标示符。

### 表现层, Representation

"资源" 是一种信息实体，它可以有多种外在表现形式，把 "资源" 具体呈现出来的形式，叫做它的 "表现层"。

比如，文本可以用 txt 格式、HTML 格式、XML 格式、JSON 格式表现，甚至可以采用二进制格式；图片可以用 JPG 格式表现，也可以用 PNG 格式表现。

URI 只代表资源的实体，不代表它的形式，严格地说，有些网址最后的 `.html` 后缀名是不必要的，因为这个后缀名表示格式，属于表现层范畴，而 URI 应该只代表资源的位置；它的具体表现形式，应该在 HTTP 请求的头信息中用 `Accept` 和 `Content-Type` 字段指定。

### 状态转化, State Transfer

访问一个网站，就代表了客户端和服务器的一个互动过程，在这个过程中，就会涉及到数据和状态的变化。

HTTP 协议是一个无状态协议，这也就意味着，所有的状态都保存在服务器端；因此，如果客户端想要操作服务器，必须通过某种手段，让服务器端发生"状态转化"，而这种转化是建立在表现层之上的，所以就是"表现层状态转化"。

具体来说，就是通过 HTTP 协议里面的四个表示操作方式的动词：GET (获取资源)，POST (新建资源或者更新资源)，PUT (更新资源)，DELETE (删除资源)。





<!--
安全性与幂等性

关于HTTP请求采用的这些个方法，具有两个基本的特性，即“安全性”和“幂等性”。对于上述7种HTTP方法，GET、HEAD和OPTIONS均被认为是安全的方法，因为它们旨在实现对数据的获取，并不具有“边界效应（Side Effect[1]）”。至于其它4个HTTP方法，由于它们会导致服务端资源的变化，所以被认为是不安全的方法。

幂等性（Idempotent）是一个数学上的概念，在这里表示发送一次和多次请求引起的边界效应是一致的。在网速不够快的情况下，客户端发送一个请求后不能立即得到响应，由于不能确定是否请求是否被成功提交，所以它有可能会再次发送另一个相同的请求，幂等性决定了第二个请求是否有效。

上述3种安全的HTTP方法（GET、HEAD和OPTIONS）均是幂等方法。由于DELETE和PATCH请求操作的是现有的某个资源，所以它们是幂等方法。对于PUT请求，只有在对应资源不存在的情况下服务器才会进行添加操作，否则只作修改操作，所以它也是幂等方法。至于最后一种POST，由于它总是进行添加操作，如果服务器接收到两次相同的POST操作，将导致两个相同的资源被创建，所以这是一个非幂等的方法。

当我们在设计Web API的时候，应该尽量根据请求HTTP方法的幂等型来决定处理的逻辑。由于PUT是一个幂等方法，所以携带相同资源的PUT请求不应该引起资源的状态变化，如果我们在资源上附加一个自增长的计数器表示被修改的次数，这实际上就破坏了幂等型。

不过就我个人的观点来说，在有的场合下针对幂等型要求可以不需要那么严格。举个例子，我对于我们开发的发部分应用来说，数据表基本上都有一个名为LastUpdatedTime的字段表示记录最后一次被修改的时间，因为这是为了数据安全审核（Auditing）的需要。在这种情况下，如果接收到一个基于数据修改的PUT请求，我们总是会用提交数据去覆盖现有的数据，并将当前服务端时间（客户端时间不可靠）作为字段LastUpdatedTime的值，这实际上也破坏了幂等性。

可能有人说我们可以在真正修改数据之前检查提交的数据是否与现有数据一致，但是在涉及多个表链接的时候这个“预检”操作会带来性能损失，而且针对每个字段的逐一比较也是一个很繁琐的事情，所以我们一般不作这样的预检操作。

七、无状态性

RESTful只要维护资源的状态，而不需要维护客户端的状态。对于它来说，每次请求都是全新的，它只需要针对本次请求作相应的操作，不需要将本次请求的相关信息记录下来以便用于后续来自相同客户端请求的处理。

对于上面我们介绍的RESTful的这些个特性，它们都是要求我们为了满足这些特征做点什么，唯有这个无状态却是要求我们不要做什么，因为HTTP本身就是无状态的。举个例子，一个网页通过调用Web API分页获取符合查询条件的记录。一般情况下，页面导航均具有“上一页”和“下一页”链接用于呈现当前页的前一页和后一页的记录。那么现在有两种实现方式返回上下页的记录。

    Web API不仅仅会定义根据具体页码的数据查询定义相关的操作，还会针对“上一页”和“下一页”这样的请求定义单独的操作。它自身会根据客户端的Session ID对每次数据返回的页面在本地进行保存，以便能够知道上一页和下一页具体是哪一页。
    Web API只会定义根据具体页码的数据查询定义相关的操作，当前返回数据的页码由客户端来维护。

第一种貌似很“智能”，其实就是一种画蛇添足的作法，因为它破坏了Web API的无状态性。设计无状态的Web API不仅仅使Web API自身显得简单而精炼，还因减除了针对客户端的“亲和度（Affinty）”使我们可以有效地实施负载均衡，因为只有这样集群中的每一台服务器对于每个客户端才是等效的。

[1] 大部分计算机书籍都将Side Effect翻译成“副作用”，而我们一般将“副（负）作用”理解为负面的作用，其实计算机领域Side Effect表示的作用无所谓正负，所以我们觉得还是还原其字面的含义“边界效用”。除此之外，对于GET、HEAD和OPTIONS请求来说，如果服务端需要对它们作日志、缓存甚至计数操作，严格来说这也算是一种Side Effect，但是请求的发送者不对此负责。

[2] 这里的“兼容”不是指支持由浏览器发送的请求，因为通过执行JavaScript脚本可以让作为宿主的浏览器发送任何我们希望的请求，这里的兼容体现在尽可能地支持浏览器访问我们在地址栏中输入的URI默认发送的HTTP-GET请求。
-->










## 最佳实践

### 1. 资源命名

资源 (也就是 URI) 应该使用名词而不是动词，因为 "资源" 表示一种实体，所以应该是名词，URI 不应该有动词，动词应该放在 HTTP 协议中，而且资源一般使用复数。

例如，某个 URI 是 `/posts/show/cars`，其中 `show` 是动词，这个 URI 就设计错了，正确的写法应该是 `/posts/cars` ，然后用 GET 方法表示 `show` 。

<!--
使用子资源表达关系
如果一个资源与另外一个资源有关系，使用子资源：
GET /cars/711/drivers/ 返回 car 711的所有司机
GET /cars/711/drivers/4 返回 car 711的4号司机
-->

### 2. 修改状态

GET 方法和查询参数不应该涉及状态改变，如果要修改状态，应该使用 PUT、POST、DELETE 方法，而不是 GET 方法来改变状态。

GET /users/711?activate
GET /users/711/activate

### 3. HTTP 头

使用 HTTP 头声明序列化格式，在客户端和服务端，双方都要知道通讯的格式，格式在 HTTP 头部中指定。

{% highlight text %}
Content-Type    请求格式
Accept          系列可接受的响应格式
{% endhighlight %}

### 4. 集合操作

可以为集合提供过滤、排序、选择和分页等功能。

{% highlight text %}
----- filtering 过滤，使用唯一的查询参数进行过滤
GET /cars?color=red                    返回红色的cars
GET /cars?seats<=2                     返回小于两座位的cars集合

----- Sorting 排序，允许针对多个字段排序
GET /cars?sort=-manufactorer,+model    根据生产者降序和模型升序排列的car集合

----- Paging 分页，通过limit和offset实现分页，可以提供默认值
GET /cars?offset=10&limit=5            为了将总数发给客户端，使用订制的HTTP头 X-Total-Count

----- Field Selection 段选择，只显示资源的部分所需要字段，降低网络流量，提高API可用性
GET /cars?fields=manufacturer,model,id,color
{% endhighlight %}

### 5. 返回码

使用 HTTP 状态码处理错误，HTTP 状态码提供了 70 个出错，实际上可以只使用其中的 10 个左右。

{% highlight text %}
200 – OK – 一切正常
201 – OK – 新的资源已经成功创建
204 – OK – 资源已经成功擅长

304 – Not Modified – 客户端使用缓存数据

400 – Bad Request – 请求无效，需要附加细节解释如 "JSON无效"
401 – Unauthorized – 请求需要用户验证
403 – Forbidden – 服务器已经理解了请求，但是拒绝服务或这种请求的访问是不允许的。
404 – Not found – 没有发现该资源
422 – Unprocessable Entity – 只有服务器不能处理实体时使用，比如图像不能被格式化，或者重要字段丢失。

500 – Internal Server Error – API开发者应该避免这种错误。

使用详细的错误包装错误：

{
  "errors": [
   {
    "userMessage": "Sorry, the requested resource does not exist",
    "internalMessage": "No car found in the database",
    "code": 34,
    "more info": "http://dev.mwaysolutions.com/blog/api/v1/errors/12345"
   }
  ]
}
{% endhighlight %}



### 6. 版本管理

可以在 URI 中加入版本号，也可以在 HTTP 请求头或者消息体中添加。







<!--
RESTful API 设计最佳实践
http://blog.jobbole.com/41233/


http://www.runoob.com/w3cnote/restful-architecture.html
-->

{% highlight text %}
{% endhighlight %}
