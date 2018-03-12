---
title: WebSocket 协议简介
layout: post
comments: true
language: chinese
category: [linux,network]
keywords: websocket,protocol,http,tcp
description: 对于 HTTP 协议，通信只能由客户端发起，服务端无法主动向客户端推送信息，通过轮询方式就很消耗资源。为了达到实时响应以及良好的用户体验，通常通过 AJAX 实现异步无刷新请求功能，不过只能在浏览器异步请求服务器，也就是拉取；而推送一般通过类似 Comet 的技术，略显复杂。而 WebSocket 是一种全新的协议，将 TCP 的 Socket 应用在了 web page 上，从而使通信双方建立起一个保持在活动状态连接通道，并且属于全双工工作模式。
---

对于 HTTP 协议，通信只能由客户端发起，服务端无法主动向客户端推送信息，通过轮询方式就很消耗资源。

为了达到实时响应以及良好的用户体验，通常通过 AJAX 实现异步无刷新请求功能，不过只能在浏览器异步请求服务器，也就是拉取；而推送一般通过类似 Comet 的技术，略显复杂。

而 WebSocket 是一种全新的协议，将 TCP 的 Socket 应用在了 web page 上，从而使通信双方建立起一个保持在活动状态连接通道，并且属于全双工工作模式。

<!-- more -->

## 示例

WebSocket 协议是借用 HTTP 协议的 `101 switch protocol` 来达到协议转换的，从 HTTP 协议切换成 WebSocket 通信协议。

WebSocket 是基于 TCP 协议，握手阶段采用 HTTP 协议，而且没有同源限制，客户端可以与任意服务器通信，协议标识符是 ws (如果加密，则为wss)。

其中示例程序如下：

{% highlight javascript %}
// 与服务端建立链接
var ws = new WebSocket("wss://echo.websocket.org");

// 建立链接后的回调函数
ws.onopen = function(event) {
	console.log("Connection open ...");
	ws.send("Hello WebSockets!");
};

// 收到服务器数据后的回调函数
ws.onmessage = function(event) {
	// 数据格式可以是文本
	if(typeof event.data === String) {
		console.log("Received Message: " + event.data);
	}

	// 也可能是二进制数据
	if(event.data instanceof ArrayBuffer){
		var buffer = event.data;
		console.log("Received arraybuffer");
	}

	ws.close();
};

// 链接关闭后的回调函数
ws.onclose = function(event) {
	console.log("Connection closed.");
};

// 错误异常处理
socket.onerror = function(event) {
	// handle error event
};
{% endhighlight %}

其中每个实例中有 readyState 属性，用于返回实例对象的当前状态，共有四种。

* CONNECTING：值为0，表示正在连接。
* OPEN：值为1，表示连接成功，可以通信了。
* CLOSING：值为2，表示连接正在关闭。
* CLOSED：值为3，表示连接已经关闭，或者打开连接失败。

对于回调函数，在如上的示例中只指定了一个，也可以通过如下代码添加多个回调函数：

{% highlight javascript %}
ws.addEventListener('open', function(event) {
	ws.send('Hello Server!');
});
{% endhighlight %}

如上收到数据后判断类型时，除了上述动态判断收到的数据类型，也可以使用 binaryType 属性，显式指定收到的二进制数据类型。

{% highlight javascript %}
// 收到的是 blob 数据
ws.binaryType = "blob";
ws.onmessage = function(e) {
	console.log(e.data.size);
};

// 收到的是 ArrayBuffer 数据
ws.binaryType = "arraybuffer";
ws.onmessage = function(e) {
	console.log(e.data.byteLength);
};
{% endhighlight %}

## 协议简介

WebSocket 协议是基于 Frame 而非 Stream ，也就是说，数据的传输不是像传统的流式读写一样按字节发送，而是采用一帧一帧的 Frame，并且每个 Frame 都定义了严格的数据结构，因此所有的信息就在这个 Frame 载体中。

默认情况下，WebSocket 协议使用 80 端口建立普通连接，加密的 TLS 连接默认使用 443 端口，与 HTTP 相同。

### 服务器握手响应

首先需要浏览器主动发起一个 HTTP 请求，每一个响应头之间用 `\r\n` 间隔，最后再放一个 `\r\n` 空行，请求内容大致如下：

{% highlight text %}
GET HTTP/1.1
Upgrade: websocket
Connection: Upgrade
Sec-WebSocket-Accept: s3pPLMBiTxaQ9kYGzzhZRbK+xOo=
{% endhighlight %}

其中包含了几个关键的字段：

* Upgrade 内容为 websocket，该字段标示改变 HTTP 协议版本或换用其他协议，这里是换用了 websocket 协议。
* Sec-WebSocket-Key 一个随机的经过 Base64 编码的字符串，像密钥一样用于服务器和客户端的握手过程。

当服务器接收到来自客户端的 upgrade 请求，就将请求头中的 `Sec-WebSocket-Key` 字段提取出来，追加一个固定的 `258EAFA5-E914-47DA-95CA-C5AB0DC85B11` 字符串，并进行 `SHA-1` 加密，然后再次经过 Base64 编码生成一个新的 key，作为响应头中的 `Sec-WebSocket-Accept` 字段的内容返回给浏览器。

浏览器接收到来自服务器的响应，便会解析响应中的 `Sec-WebSocket-Accept` 字段，与自己加密编码后的串进行匹配，一旦匹配成功，便可以准备建立 WebSocket 链接了。

然后服务器响应类似如下的 HTTP 报文。

{% highlight text %}
HTTP/1.1 101 Switching Protocols
Upgrade: websocket
Connection: Upgrade
Sec-WebSocket-Accept: 4O33ZinyFxWKCaxf7T4yCA==
{% endhighlight %}

另外，客户端向服务器发起握手请求的 header 中，可以通过 `Sec-WebSocket-Protocol` 字段指定一个子协议，服务器在建立连接的响应头中包含同样的字段，内容就是选择的子协议之一。

### 协议

接下来的数据传输就是通过 WebSocket 协议了，传输的数据都是以 Frame 形式实现，类似于 TCP/UDP 协议中的报文段 Segment。

{% highlight text %}
  0                   1                   2                   3
  0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
 +-+-+-+-+-------+-+-------------+-------------------------------+
 |F|R|R|R| opcode|M| Payload len |    Extended payload length    |
 |I|S|S|S|  (4)  |A|     (7)     |             (16/64)           |
 |N|V|V|V|       |S|             |   (if payload len==126/127)   |
 | |1|2|3|       |K|             |                               |
 +-+-+-+-+-------+-+-------------+ - - - - - - - - - - - - - - - +
 |     Extended payload length continued, if payload len == 127  |
 + - - - - - - - - - - - - - - - +-------------------------------+
 |                               |Masking-key, if MASK set to 1  |
 +-------------------------------+-------------------------------+
 | Masking-key (continued)       |          Payload Data         |
 +-------------------------------- - - - - - - - - - - - - - - - +
 :                     Payload Data continued ...                :
 + - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - +
 |                     Payload Data continued ...                |
 +---------------------------------------------------------------+
{% endhighlight %}


#### Masking-key

当 mask 字段的值为 1 时，payload-data 字段的数据需要经这个掩码进行解密；如果是 0 ，那么数据的解析就不涉及到掩码，直接使用就行。

不过，如果消息是从客户端发送到服务器，那么 mask 一定是 1，而 Masking-key 一定是一个 32bit 的值。

### 分片 (Fragment)

理论上来说，每个帧 (Frame) 的大小是没有限制的，但是发送的数据有不能太大，否则无法高效的利用网络带宽 (由于MSS)，如果要传输大片数据就需要使用分片。

将原本一个大的帧拆分成数个小的帧，下面是把一个大的 Frame 分片的图示：

Number:

   FIN:
Opcode: !0 0 0

  编号：      0  1  ....  n-2 n-1
    分片：     |——|——|......|——|——|
      FIN：      0  0  ....   0  1
        Opcode：   !0 0  ....   0  0

由上述可知，第一个分片的 FIN 为0，Opcode为非0值（0x1或0x2），最后一个分片的FIN为1，Opcode为0。中间分片的FIN和Opcode二者均为0。

	Note1：消息的分片必须由发送者按给定的顺序发送给接收者。

	Note2：控制帧禁止分片

	Note3：接受者不必按顺序缓存整个frame来处理




## 参考

类似 CGI ，通过标准输入输出作为 WebSocket 的输入输出，很特别但是实用的服务器 [http://websocketd.com/](http://websocketd.com/) ，一个通过前端显示 vmstats 的工程实现，详见 [Github web-vmstats](https://github.com/joewalnes/web-vmstats) 。

基于 [libwebsockets](https://github.com/warmcat/libwebsockets) 实现的工程，包括：数据展示 [JitterTrap](https://github.com/acooks/jittertrap)、共享终端 [ttyd](https://github.com/tsl0922/ttyd) 。

<!--
[WebSocket API](https://developer.mozilla.org/en-US/docs/Web/API/WebSocket) 。


https://github.com/abbshr/abbshr.github.io/issues/22
https://github.com/abbshr/abbshr.github.io/issues/47
http://www.antyme.com/2016/12/14/WebSocket%E5%8D%8F%E8%AE%AE-SocketRocket%E6%BA%90%E7%A0%81%E5%AD%A6%E4%B9%A0/
https://tools.ietf.org/html/rfc6455

https://github.com/uNetworking/uWebSockets
https://github.com/tatsuhiro-t/wslay
https://github.com/mortzdk/Websocket
https://github.com/payden/libwebsock
https://github.com/payden/libwsclient

http://senlinzhan.github.io/2017/02/10/Linux%E7%9A%84TCP-CORK/
https://github.com/nodejs/http-parser
http://blog.mingsixue.com/debug/Fiddler-online-map-localfile.html
-->



{% highlight text %}
{% endhighlight %}
