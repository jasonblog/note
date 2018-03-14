---
title: HTTP 協議簡介
layout: post
comments: true
category: [linux, network]
language: chinese
keywords: http,協議,簡介
description: HTTP 協議在 TCP/IP 協議之上，目前已經成為了互聯網的基礎協議，也是網頁開發的必備知識，這裡簡單介紹下 HTTP 協議的歷史演變、設計思路，以及相關的解析實現。
---

HTTP 協議在 TCP/IP 協議之上，目前已經成為了互聯網的基礎協議，也是網頁開發的必備知識，這裡簡單介紹下 HTTP 協議的歷史演變、設計思路，以及相關的解析實現。

<!-- more -->

![http introduce]({{ site.url }}/images/network/http-introduce.jpg "http introduce"){: .pull-center }

## 簡介

HTTP 協議到現在為止總共經歷了 3 個版本的演化，第一個 HTTP 協議誕生於 1989.3 。

### 換行符

在 Web 的發展過程中，UNIX 系統從一開始到現在一直是主要的開發和運行平臺，但是在 HTTP 協議中，各個 header 之間用的卻是 `CRLF (\r\n)` 這樣的 Windows/DOS 換行方式，而不是 UNIX 普遍的的 `LF (\n)` 換行方式。

實際上 ASCII 沿襲了傳統打字機的設計，也就是說在某種意義上來說 `CR-LF` 才是正統。

![http introduce]({{ site.url }}/images/network/http-introduce-printer.jpg "http introduce"){: .pull-center width="70%" }

左側墨帶盒上邊的金屬手柄了就是換行手柄。

打完一行字之後，滾筒已經基本移出了打字機主體，需要推左手邊的換行手柄，然後滾筒會被推到最右邊，打字頭對準了行首，這就是 CR 。

推到頭之後，繼續用力，換行手柄大約會被扳動 30 度左右，這時候你會發現紙往上捲了一行，這就是 LF 。

所以遵循傳統，ASCII 設計了 CR-LF 順序。

## HTTP/0.9

規定了客戶端和服務器之間的通信格式，默認使用 80 端口，最早版本是 1991 年發佈的 0.9 版。

該版本極簡單，只允許客戶端發送 `GET` 這一種請求，不支持請求頭。由於沒有協議頭，造成了 HTTP 0.9 協議只支持一種內容，即純文本，網頁仍支持 HTML 格式化，不過無法插入圖片。

HTTP 0.9 具有典型的無狀態性，每個事務獨立進行處理，事務結束時就釋放這個連接，也就是說，HTTP 協議的無狀態特點在其第一個版本 0.9 中已經成型。

該版本只有一個命令 GET 。

{% highlight text %}
GET /index.html
{% endhighlight %}

上面命令表示，TCP 連接建立後，客戶端向服務器請求網頁`index.html` ；協議規定，服務器只能迴應 HTML 格式的字符串，不能迴應別的格式。

{% highlight c %}
<html>
    <body>Hello World</body>
</html>
{% endhighlight %}

服務器發送完畢，就關閉 TCP 連接。

## HTTP/1.0

1996.05 該版本發佈，內容大大增加，主要包括瞭如下特性：

1. 除了數據部分，請求與響應支持頭信息 (HTTP Header)，用來描述一些元數據。
1. 響應對象以一個響應狀態行開始，而且支持圖像、視頻、二進制文件等多種格式。
2. 還引入了 POST、HEAD 方法，豐富了瀏覽器與服務器的互動手段。
2. 默認仍然使用短連接，但是支持長連接。

其它新增功能還包括狀態碼 (Status Code)、多字符集支持、多部分發送 (Multi-part Type)、權限 (Authorization)、緩存 (Cache)、內容編碼 (Content Encoding) 等。

### 請求格式

下面是一個 1.0 版的 HTTP 請求的例子。

{% highlight text %}
GET / HTTP/1.0
User-Agent: Mozilla/5.0 (Macintosh; Intel Mac OS X 10_10_5)
Accept: */*
{% endhighlight %}

第一行是請求命令，尾部添加協議版本 `HTTP/1.0`，後面就是多行頭信息，描述客戶端的情況。

### 響應格式

服務器的迴應如下。

{% highlight text %}
HTTP/1.0 200 OK
Content-Type: text/plain
Content-Length: 137582
Expires: Thu, 05 Dec 1997 16:00:00 GMT
Last-Modified: Wed, 5 August 1996 15:55:28 GMT
Server: Apache 0.84

<html>
	<body>Hello World</body>
</html>
{% endhighlight %}

格式是 `頭信息 + 一個空行 (\r\n) + 數據`，第一行是 `協議版本 + 狀態碼 (status code) + 狀態描述` 。

### Content-Type 字段

關於字符的編碼，`1.0` 版規定，頭信息必須是 ASCII 碼，後面的數據可以是任何格式，因此，服務器響應時候，必須告訴客戶端，數據是什麼格式，這就是 `Content-Type` 字段的作用。

下面是一些常見的 `Content-Type` 字段的值。

{% highlight text %}
text/plain
text/html
text/css
image/jpeg
image/svg+xml
audio/mp4
video/mp4
application/pdf
application/atom+xml
{% endhighlight %}

這些數據類型總稱為 `MIME Type`，每個值包括一級類型和二級類型，之間用斜槓分隔。

<!--
除了預定義的類型，廠商也可以自定義類型。

    application/vnd.debian.binary-package

上面的類型表明，發送的是Debian系統的二進制數據包。

MIME type還可以在尾部使用分號，添加參數。

    Content-Type: text/html; charset=utf-8

上面的類型表明，發送的是網頁，而且編碼是UTF-8。

客戶端請求的時候，可以使用Accept字段聲明自己可以接受哪些數據格式。

    Accept: */*

上面代碼中，客戶端聲明自己可以接受任何格式的數據。
-->

`MIME Type` 不僅用在 HTTP 協議，還可以用在其它地方，比如 `HTML` 網頁。

{% highlight text %}
<meta http-equiv="Content-Type" content="text/html; charset=UTF-8" />
<!-- 等同於 -->
<meta charset="utf-8" />
{% endhighlight %}

### Content-Encoding 字段

由於發送的數據可以是任何格式，因此可以把數據壓縮後再發送，`Content-Encoding` 字段說明數據的壓縮方法。

{% highlight text %}
Content-Encoding: gzip
Content-Encoding: compress
Content-Encoding: deflate
{% endhighlight %}

客戶端在請求時，用 `Accept-Encoding` 字段說明自己可以接受哪些壓縮方法，例如：

{% highlight text %}
Accept-Encoding: gzip, deflate
{% endhighlight %}

### Content-Range 字段

一般斷點下載時才會用到 `Range` 和 `Content-Range` 實體頭；其中 `Range` 用於請求頭，指定第一個字節的位置和最後一個字節的位置，如 `Range: 200-300`；而 `Content-Range` 用於響應頭。

其示例請求頭如下：

{% highlight text %}
GET /test.rar HTTP/1.1
Connection: close
Host: 116.1.219.219
Range: bytes=0-100
{% endhighlight %}

其中，Range 頭可以請求實體的一個或者多個子範圍，第一個字節是 0 ，常見的表示如下：

{% highlight text %}
bytes=0-499      頭500個字節
bytes=500-999    第二個500字節
bytes=-500       最後500個字節
bytes=500-       500字節以後的範圍
bytes=0-0,-1     第一個和最後一個字節
bytes=500-600,601-999  同時指定幾個範圍
{% endhighlight %}

一般正常響應如下：

{% highlight text %}
HTTP/1.1 206 OK
Content-Length:  801
Content-Type:  application/octet-stream
Content-Location: http://www.onlinedown.net/hj_index.htm
Content-Range: bytes 0-100/2350
Last-Modified: Mon, 16 Feb 2009 16:10:12 GMT
Accept-Ranges: bytes
{% endhighlight %}

上述的響應頭中，其中的 `2350` 表示文件總大小，如果用戶的請求中含有 `Range` 且服務器支持斷點續傳，那麼服務器的相應代碼為 `206` 。

### 缺點

`HTTP/1.0` 版的主要缺點是，每個 `TCP` 連接只能發送一個請求，發送數據完畢，連接就關閉，如果還要請求其它資源，就必須再新建一個連接。

TCP 連接的新建成本很高，因為需要客戶端和服務器三次握手，並且開始時發送速率較慢 (slow start)。所以，`HTTP 1.0` 版本的性能比較差，尤其隨著網頁加載的外部資源越來越多，這個問題就愈發突出了。

為了解決這個問題，有些瀏覽器在請求時，用了一個非標準的 `Connection` 字段。

{% highlight text %}
Connection: keep-alive
{% endhighlight %}

這個字段要求服務器不要關閉 `TCP` 連接，以便其他請求複用，服務器同樣迴應這個字段。

{% highlight text %}
Connection: keep-alive
{% endhighlight %}

一個可以複用的TCP連接就建立了，直到客戶端或服務器主動關閉連接。但是，這不是標準字段，不同實現的行為可能不一致，因此不是根本的解決辦法。

## HTTP/1.1

1997.01 `HTTP/1.1` 版本發佈，只比 `1.0` 版本晚了半年。它進一步完善了 HTTP 協議，一直用到了 20 年後的今天，直到現在還是最流行的版本。

### 持久連接

`1.1` 版的最大變化，就是引入了持久連接 (Persistent Connection)，即 TCP 連接默認不關閉，可以被多個請求複用，不用聲明 `Connection: keep-alive` 。

客戶端和服務器發現對方一段時間沒有活動，就可以主動關閉連接。不過，規範的做法是，客戶端在最後一個請求時，發送 `Connection: close` 明確要求服務器關閉 TCP 連接。

目前，對於同一個域名，大多數瀏覽器允許同時建立 6 個持久連接。

### 管道機制

`1.1` 版還引入了管道機制 (Pipelining)，即在同一個 TCP 連接裡面，客戶端可以同時發送多個請求，這樣就進一步改進了 HTTP 協議的效率。

舉例來說，客戶端需要請求兩個資源。

以前的做法是，在同一個 TCP 連接裡面，先發送 A 請求，然後等待服務器做出迴應，收到後再發出 B 請求。管道機制則是允許瀏覽器同時發出 A 請求和 B 請求，但是服務器還是按照順序，先回應 A 請求，完成後再回應 B 請求。

### Content-Length 字段

一個 TCP 連接現在可以傳送多個迴應，勢必就要有一種機制，區分數據包是屬於哪一個迴應的。這就是 `Content-length` 字段的作用，聲明本次迴應的數據長度。

{% highlight text %}
Content-Length: 3495
{% endhighlight %}

上面代碼告訴瀏覽器，本次迴應的長度是 3495 個字節，後面的字節就屬於下一個迴應了。

在 1.0 版中，`Content-Length` 字段不是必需的，因為瀏覽器發現服務器關閉了 TCP 連接，就表明收到的數據包已經全了。

### 分塊傳輸編碼

使用 `Content-Length` 字段的前提條件是，服務器發送迴應之前，必須知道迴應的數據長度。

對於一些很耗時的動態操作來說，這意味著，服務器要等到所有操作完成，才能發送數據，顯然這樣的效率不高。更好的處理方法是，產生一塊數據，就發送一塊，採用"流模式" (stream) 取代"緩存模式" (buffer)。

<!--
因此，1.1版規定可以不使用Content-Length字段，而使用"分塊傳輸編碼"（chunked transfer encoding）。只要請求或迴應的頭信息有Transfer-Encoding字段，就表明迴應將由數量未定的數據塊組成。

    Transfer-Encoding: chunked

每個非空的數據塊之前，會有一個16進制的數值，表示這個塊的長度。最後是一個大小為0的塊，就表示本次迴應的數據發送完了。下面是一個例子。


    HTTP/1.1 200 OK
    Content-Type: text/plain
    Transfer-Encoding: chunked

    25
    This is the data in the first chunk

    1C
    and this is the second one

    3
    con

    8
    sequence

    0

3.5 其他功能

1.1版還新增了許多動詞方法：PUT、PATCH、HEAD、 OPTIONS、DELETE。

另外，客戶端請求的頭信息新增了Host字段，用來指定服務器的域名。


    Host: www.example.com

有了Host字段，就可以將請求發往同一臺服務器上的不同網站，為虛擬主機的興起打下了基礎。
3.6 缺點

雖然1.1版允許複用TCP連接，但是同一個TCP連接裡面，所有的數據通信是按次序進行的。服務器只有處理完一個迴應，才會進行下一個迴應。要是前面的迴應特別慢，後面就會有許多請求排隊等著。這稱為"隊頭堵塞"（Head-of-line blocking）。

為了避免這個問題，只有兩種方法：一是減少請求數，二是同時多開持久連接。這導致了很多的網頁優化技巧，比如合併腳本和樣式表、將圖片嵌入CSS代碼、域名分片（domain sharding）等等。如果HTTP協議設計得更好一些，這些額外的工作是可以避免的。
-->


## 參考

[The Original HTTP as defined in 1991](https://www.w3.org/Protocols/HTTP/AsImplemented.html)

<!--
http://www.ruanyifeng.com/blog/2016/08/http.html
-->

{% highlight text %}
{% endhighlight %}
