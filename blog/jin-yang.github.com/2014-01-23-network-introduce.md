---
title: Linux 網絡協議棧簡介
layout: post
comments: true
language: chinese
category: [network]
keywords: linux,network,網絡,協議棧
description: Linux 的 TCP/IP 協議棧估計是目前最靈活、應用最廣的網絡協議棧了，具有清晰的層次結構以及清晰定義的原語和接口，不僅使得上層應用開發者可以無需關心下層架構或者內部機制，從而提供相對透明的操作網絡。在本文中簡單介紹一下與協議棧相關的基本內容，如標準模型、數據傳輸等。
---

Linux 的 TCP/IP 協議棧估計是目前最靈活、應用最廣的網絡協議棧了，具有清晰的層次結構以及清晰定義的原語和接口，不僅使得上層應用開發者可以無需關心下層架構或者內部機制，從而提供相對透明的操作網絡。

在本文中簡單介紹一下與協議棧相關的內容。

<!-- more -->

## 標準模型

實際上有兩套模型：七層 OSI 和 四層 TCP/IP 模型，由於種種原因，前者只存在於理論中，而實際應用的是後者，也就是 Linux 協議棧的實現，兩者的對應關係如下：

![OSI and TCP/IP Models]({{ site.url }}/images/network/network-introduce-OSI-TCP-IP.png "OSI and TCP/IP Models"){: .pull-center}

在 TCP/IP 模型中，不同的協議層對應數據的名稱也有所區別，而且每層都有不同的協議棧，詳細的可以查看圖片 [OSI 七層模型.png]({{ site.url }}/images/network/network-introduce-OSI-models.gif) 。


## 數據傳輸

接下來看看數據是如何通過協議棧在網絡中進行傳輸的。假設有如下的網絡，其中 Host X 嘗試從 Server X 上下載 Html 文件，也就是現在我們瀏覽器經常辦的事。

![How the Network Stack Operates]({{ site.url }}/images/network/network-introduce-how-works.png "How the Network Stack Operates"){: .pull-center}

那麼有如下的疑問：

* Host X 和 Server Y 在不同的局域網中，它們是如何進行通訊的呢？

* Host X 不知道 Server Y 的物理地址，那麼 X 如何發送請求呢？

首先在應用層 (Application Layer)，也就是用戶在瀏覽器中輸入 www.baidu.com 然後按下回車後，此時首先會通過 DNS 解析出該 URL 中主機的 IP 地址，假設是上圖中的 208.201.237.37，那麼接下來兩臺機器間就是通過 IP 來進行通訊。

在 TCP 傳輸層 (Transport Layer) 中，如果消息過長會將要發送的數據拆分成多個請求，並且會將源 IP、目的 IP、源 Port、目的 Port 包在消息頭中，其中目的 Port 對於 HTTP 請求來說默認是 80，而源 Port 則是 OS 動態分配的。此時，TCP 並不知道如何將報文發送到目的地，實際上這是 IP 層所作的事情。

IP 網絡層 (Network Layer) 嘗試將報文直接發送出去，但是路由表中沒有找到目的 IP，那麼就會發送給系統指定的默認網關，也就是 Router RT1。

然後就是鏈路層 (Link Layer)，不同的鏈路層實現的機制有所區別，對於以太網來說，是通過 MAC 地址標示地址的，而 IP 地址和 MAC 的映射是通過 ARP 查詢，有了路由器 RT1 的 MAC 地址之後，那麼在局域網中就可以直接傳輸報文了。

接下來，就是在 RT1、RT2 ... ... 中的傳輸了，實際上除了最後一個路由器之外，其操作相同：

1. 移除鏈路層頭部。

2. 找到對應的 IP 地址。

3. 發現該 IP 仍然不在該局域網中，找到下跳 MAC，新建一個鏈路層頭。

4. 然後發送到下一個路由中。

實際上打包的過程，以及報文的內容如下：

![messages travel the stack]({{ site.url }}/images/network/netwok-introduce-travels.png "messages travel the stack"){: .pull-center}

其中 a~d 為在主機 Host X 上各層的打包過程，而 e 是在 RT1-RT2 的報文。



## Linux 協議棧

Linux 協議棧的內容大致如下圖所示。

![the linux TCP/IP stack]({{ site.url }}/images/network/introduce-linux-TCP-IP-stack.jpg "the linux TCP/IP stack"){: .pull-center}

其中應用層通過 glibc 封裝的系統調用與內核交換程序，例如 socket()、bind()、recv()、send() 等函數，在內核中實際實現的一個與虛擬文件系統類似的接口，也就是 socketfs 。

接下來就是對不同協議棧的實現，最常用的就是 TCP/IP 協議棧了，包括了 TCP、UDP、IP、ICMP、ARP 等等，下面我們會逐步講解內核中各個模塊的實現。

## 雜項

### Request For Comments, RFC

RFC 是最重要，也是最常用的資料之一。通常，當某家機構或團體開發出了一套標準或提出對某種標準的設想，想要徵詢外界的意見時，就會在 Internet 上發放一份 RFC。

一個 RFC 文件在成為官方標準前一般至少要經歷 4 個階段 【RFC2026】：因特網草案、建議標準、草案標準、因特網標準。

如下是一些常見的協議，可以選擇 txt、pdf 等版本：

* [RFC-6176 Secure Sockets Layer (SSL) V2.0](https://tools.ietf.org/html/rfc6176)
* [RFC-6101 Secure Sockets Layer (SSL) V3.0](https://tools.ietf.org/html/rfc6101)



## 參考

其中與 Linux 協議棧相關的兩本很不錯的書籍，包括 《Understanding Linux Network Internals》 以及 《The Linux Networking Architecture: Design and Implementation of Network Protocols in the Linux Kernel》。

{% highlight c %}
{% endhighlight %}
