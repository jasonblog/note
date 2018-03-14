---
title: WebSocket 協議簡介
layout: post
comments: true
language: chinese
category: [linux,network]
keywords: websocket,protocol,http,tcp
description: 對於 HTTP 協議，通信只能由客戶端發起，服務端無法主動向客戶端推送信息，通過輪詢方式就很消耗資源。為了達到實時響應以及良好的用戶體驗，通常通過 AJAX 實現異步無刷新請求功能，不過只能在瀏覽器異步請求服務器，也就是拉取；而推送一般通過類似 Comet 的技術，略顯複雜。而 WebSocket 是一種全新的協議，將 TCP 的 Socket 應用在了 web page 上，從而使通信雙方建立起一個保持在活動狀態連接通道，並且屬於全雙工工作模式。
---

對於 HTTP 協議，通信只能由客戶端發起，服務端無法主動向客戶端推送信息，通過輪詢方式就很消耗資源。

為了達到實時響應以及良好的用戶體驗，通常通過 AJAX 實現異步無刷新請求功能，不過只能在瀏覽器異步請求服務器，也就是拉取；而推送一般通過類似 Comet 的技術，略顯複雜。

而 WebSocket 是一種全新的協議，將 TCP 的 Socket 應用在了 web page 上，從而使通信雙方建立起一個保持在活動狀態連接通道，並且屬於全雙工工作模式。

<!-- more -->

## 示例

WebSocket 協議是借用 HTTP 協議的 `101 switch protocol` 來達到協議轉換的，從 HTTP 協議切換成 WebSocket 通信協議。

WebSocket 是基於 TCP 協議，握手階段採用 HTTP 協議，而且沒有同源限制，客戶端可以與任意服務器通信，協議標識符是 ws (如果加密，則為wss)。

其中示例程序如下：

{% highlight javascript %}
// 與服務端建立鏈接
var ws = new WebSocket("wss://echo.websocket.org");

// 建立鏈接後的回調函數
ws.onopen = function(event) {
	console.log("Connection open ...");
	ws.send("Hello WebSockets!");
};

// 收到服務器數據後的回調函數
ws.onmessage = function(event) {
	// 數據格式可以是文本
	if(typeof event.data === String) {
		console.log("Received Message: " + event.data);
	}

	// 也可能是二進制數據
	if(event.data instanceof ArrayBuffer){
		var buffer = event.data;
		console.log("Received arraybuffer");
	}

	ws.close();
};

// 鏈接關閉後的回調函數
ws.onclose = function(event) {
	console.log("Connection closed.");
};

// 錯誤異常處理
socket.onerror = function(event) {
	// handle error event
};
{% endhighlight %}

其中每個實例中有 readyState 屬性，用於返回實例對象的當前狀態，共有四種。

* CONNECTING：值為0，表示正在連接。
* OPEN：值為1，表示連接成功，可以通信了。
* CLOSING：值為2，表示連接正在關閉。
* CLOSED：值為3，表示連接已經關閉，或者打開連接失敗。

對於回調函數，在如上的示例中只指定了一個，也可以通過如下代碼添加多個回調函數：

{% highlight javascript %}
ws.addEventListener('open', function(event) {
	ws.send('Hello Server!');
});
{% endhighlight %}

如上收到數據後判斷類型時，除了上述動態判斷收到的數據類型，也可以使用 binaryType 屬性，顯式指定收到的二進制數據類型。

{% highlight javascript %}
// 收到的是 blob 數據
ws.binaryType = "blob";
ws.onmessage = function(e) {
	console.log(e.data.size);
};

// 收到的是 ArrayBuffer 數據
ws.binaryType = "arraybuffer";
ws.onmessage = function(e) {
	console.log(e.data.byteLength);
};
{% endhighlight %}

## 協議簡介

WebSocket 協議是基於 Frame 而非 Stream ，也就是說，數據的傳輸不是像傳統的流式讀寫一樣按字節發送，而是採用一幀一幀的 Frame，並且每個 Frame 都定義了嚴格的數據結構，因此所有的信息就在這個 Frame 載體中。

默認情況下，WebSocket 協議使用 80 端口建立普通連接，加密的 TLS 連接默認使用 443 端口，與 HTTP 相同。

### 服務器握手響應

首先需要瀏覽器主動發起一個 HTTP 請求，每一個響應頭之間用 `\r\n` 間隔，最後再放一個 `\r\n` 空行，請求內容大致如下：

{% highlight text %}
GET HTTP/1.1
Upgrade: websocket
Connection: Upgrade
Sec-WebSocket-Accept: s3pPLMBiTxaQ9kYGzzhZRbK+xOo=
{% endhighlight %}

其中包含了幾個關鍵的字段：

* Upgrade 內容為 websocket，該字段標示改變 HTTP 協議版本或換用其他協議，這裡是換用了 websocket 協議。
* Sec-WebSocket-Key 一個隨機的經過 Base64 編碼的字符串，像密鑰一樣用於服務器和客戶端的握手過程。

當服務器接收到來自客戶端的 upgrade 請求，就將請求頭中的 `Sec-WebSocket-Key` 字段提取出來，追加一個固定的 `258EAFA5-E914-47DA-95CA-C5AB0DC85B11` 字符串，並進行 `SHA-1` 加密，然後再次經過 Base64 編碼生成一個新的 key，作為響應頭中的 `Sec-WebSocket-Accept` 字段的內容返回給瀏覽器。

瀏覽器接收到來自服務器的響應，便會解析響應中的 `Sec-WebSocket-Accept` 字段，與自己加密編碼後的串進行匹配，一旦匹配成功，便可以準備建立 WebSocket 鏈接了。

然後服務器響應類似如下的 HTTP 報文。

{% highlight text %}
HTTP/1.1 101 Switching Protocols
Upgrade: websocket
Connection: Upgrade
Sec-WebSocket-Accept: 4O33ZinyFxWKCaxf7T4yCA==
{% endhighlight %}

另外，客戶端向服務器發起握手請求的 header 中，可以通過 `Sec-WebSocket-Protocol` 字段指定一個子協議，服務器在建立連接的響應頭中包含同樣的字段，內容就是選擇的子協議之一。

### 協議

接下來的數據傳輸就是通過 WebSocket 協議了，傳輸的數據都是以 Frame 形式實現，類似於 TCP/UDP 協議中的報文段 Segment。

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

當 mask 字段的值為 1 時，payload-data 字段的數據需要經這個掩碼進行解密；如果是 0 ，那麼數據的解析就不涉及到掩碼，直接使用就行。

不過，如果消息是從客戶端發送到服務器，那麼 mask 一定是 1，而 Masking-key 一定是一個 32bit 的值。

### 分片 (Fragment)

理論上來說，每個幀 (Frame) 的大小是沒有限制的，但是發送的數據有不能太大，否則無法高效的利用網絡帶寬 (由於MSS)，如果要傳輸大片數據就需要使用分片。

將原本一個大的幀拆分成數個小的幀，下面是把一個大的 Frame 分片的圖示：

Number:

   FIN:
Opcode: !0 0 0

  編號：      0  1  ....  n-2 n-1
    分片：     |——|——|......|——|——|
      FIN：      0  0  ....   0  1
        Opcode：   !0 0  ....   0  0

由上述可知，第一個分片的 FIN 為0，Opcode為非0值（0x1或0x2），最後一個分片的FIN為1，Opcode為0。中間分片的FIN和Opcode二者均為0。

	Note1：消息的分片必須由發送者按給定的順序發送給接收者。

	Note2：控制幀禁止分片

	Note3：接受者不必按順序緩存整個frame來處理




## 參考

類似 CGI ，通過標準輸入輸出作為 WebSocket 的輸入輸出，很特別但是實用的服務器 [http://websocketd.com/](http://websocketd.com/) ，一個通過前端顯示 vmstats 的工程實現，詳見 [Github web-vmstats](https://github.com/joewalnes/web-vmstats) 。

基於 [libwebsockets](https://github.com/warmcat/libwebsockets) 實現的工程，包括：數據展示 [JitterTrap](https://github.com/acooks/jittertrap)、共享終端 [ttyd](https://github.com/tsl0922/ttyd) 。

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
