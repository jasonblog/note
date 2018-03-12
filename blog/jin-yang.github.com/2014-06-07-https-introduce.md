---
title: HTTPS 协议详解
layout: post
comments: true
language: chinese
category: [linux,webserver,misc]
keywords: https,ssl,tls
description: 我们知道 HTTP 采用的是明文传输，而在互联网中，比如要在淘宝买个东西，使用支付宝转个帐，必须要保证这些信息只有客户端和服务器才知道的，也就是通过 HTTPS 协议。接下来，我们就看看 HTTPS 协议是如何实现的。
---

我们知道 HTTP 采用的是明文传输，而在互联网中，比如要在淘宝买个东西，使用支付宝转个帐，必须要保证这些信息只有客户端和服务器才知道的，也就是通过 HTTPS 协议。

接下来，我们就看看 HTTPS 协议是如何实现的。

<!-- more -->

![https logo]({{ site.url }}/images/linux/https-logo.jpg "https logo"){: .pull-center }

## 简介

Secure Hypertext Transfer Protocol, HTTPS 也就是安全超文本传输协议，是一个安全通信通道，它基于 HTTP 开发用于在客户计算机和服务器之间交换信息。

实际上，简单来说，它是 HTTP 的安全版，是使用 SSL(Secure Socket Layer)/TLS(Transport Layer Security) 加密的 HTTP 协议。通过 TLS/SSL 协议的的身份验证、信息加密和完整性校验的功能，从而避免信息窃听、信息篡改和信息劫持的风险。

简单来说，HTTPS 提供了加密 (Encryption)、认证 (Verification)、鉴定 (Identification) 三种功能。如下的解释中，假设是张三和李四在通讯。

{% highlight text %}
私密性(Confidentiality/Privacy):
    也就是提供信息加密；保证信息只有张三和李四知道，而不会被窃听。
可信性(Authentication):
    身份验证，主要是服务器端的，有些银行也会对客户端进行认证；用来证明李四就是李四。
完整性(Message Integrity):
    保证信息传输过程中的完整性，防止被修改；李四接收到的消息就是张三发送的。
{% endhighlight %}

如下是 SSL/TLS 协议的介绍。

### SSL/TLS 协议

SSL 最初是在 1996 年由 Netscape 发布，由于一些安全的原因 SSL v1.0 和 SSL v2.0 都没有公开，直到 1996 年的 SSL v3.0。TLS 是 v3.0 的升级版，目前市面上所有的 HTTPS 都是用的是 TLS 而非 SSL，而且很多地方都混用两者。

![https]({{ site.url }}/images/linux/https-ssl-tls-layer.png "https"){: .pull-center width="75%" }

其中上图中顶层的三块又组成了 SSL Handshaking Protocols，在处理时有三种状态。

{% highlight text %}
empty state -------------------> pending state ------------------> current state

             Handshake Protocol                Change Cipher Spec
{% endhighlight %}

当完成握手后，客户端和服务端确定了加密、压缩和 MAC 算法及其参数，数据通过指定算法处理；而之前的所有操作都是明文传输的。


### 关键技术

首先，在实现时加密算法是公开的，而私密性是通过密钥保证的。另外，为了实现上述的机制，需要一些比较常见的技术。

其中与密码学相关的内容，可以参考 [加密算法简介](/blog/encryption-introduce.html) 。

#### 私钥交换 Key Exchange Algorithms

发送信息的时候，需要通过对称加密来加密数据，此时就涉及到了私钥的交换，通常使用的算法包括了 Diffie-Hellman、RSA 。

{% highlight text %}
Bob's Public Key + Alice's Secret Key     Bob's Secret Key (Decrypt)
          |----->-------->------->-------->------>-----|
                                              Get Alice's Secret Key
{% endhighlight %}

对于 RSA 来说，所有人都知道 Bob 的公钥，只需要将 Alice 的私钥通过公钥进行加密即可，该信息只有通过 Bob 的私钥才可以解密。

当然，在实际使用时还会增加其它的随机值。

#### 数字证书 Digital Certificates

到此，需要通过一种方法获取到 Bob 的公钥，这就是通过数字证书获取的。数字证书中除了包括了公钥信息，还有与 Bob 相关的信息。

#### 数字证书认证机构 Certification Authority, CA

接下来解决的是，如何确认数字证书的拥有着就像它声明的那样，为此引入了 Public Key Infrastructure, PKI ，包括一些与数字证书相关的软件、硬件等信息。

CA 就是一个三方机构，用来证明 Bob 确实就是 Bob 。

CA 认证分为三类：DV (domain validation)，OV (organization validation)，EV (extended validation)，证书申请难度从前往后递增，貌似 EV 这种不仅仅是有钱就可以申请的。

#### 数字签名 消息完整性

实际上有篇很不错的文章 [What is a Digital Signature?](http://www.youdzone.com/signature.html) ，也可以参考 [本地文章](/reference/linux/What is a Digital Signature.mht)，在此就不做过多介绍了。



## 执行流程

我们直接从 WireShark 的官网网站上下载了一个 HTTPS 的示例，详细的下载地址可以参考文章末尾的参考内容。

<!--
{% highlight text %}
     Client                 |   Server
                            |
                            |
                            |
                            |
+----------------------     |      -------------------
|=== Client Hello ===       |
                            |
                            |
                            |
                            |
                            |
                            |
                            |
                            |
                            |
                            |
                            |
{% endhighlight %}
-->

首先，是正常的 TCP 开始的三次握手连接，在此就不做过多的介绍了；完成之后才会开始 SSL 之间的沟通，这也是接下来重点介绍的内容。

在进行正常的数据传输之前，会有一个协商的过程，包括了协议的版本、会话ID、加密算法 (含有一套，后面介绍)、压缩方法；另外，在该阶段，还会相互交换服务端和客户端的随机值。

### 1. Client Hello

示例中有两个会话的数据传输，分别是 SSLv2.0 以及 SSLv3.0，现在一般使用的是 v3.0，其中 v2.0 在该阶段会使用 challenge 机制，不过不太清楚详细内容，在 [RFC-6101](https://tools.ietf.org/rfc/rfc6101.txt) 中略有介绍。

这里，仅介绍 v3.0 的内容。

![https]({{ site.url }}/images/linux/https-protocol-00.png "https"){: .pull-center width="80%" }

在上述的图片中，包括了协议的版本信息，还有些比较重要的信息。

##### 随机数

开始的四个字节以 Unix 时间格式记录了客户端 UTC 时间 (从1970年1月1日开始到当前时刻所经历的秒数)；接着后面有 28 字节的随机数，在后面的过程中我们会用到这个 **客户端的随机数** 。

##### Session ID, SID

一般来说，第一次建立连接的时候 ID 是一个空值 (NULL)，当然也有可能会重用之前的会话，从而避免一个完整的握手过程。

##### 密文族 (Cipher Suites)

密文族是浏览器所支持的加密算法的清单，整个密文族有 20 种。

另外，有是还会有 Server_name 的扩展，能告诉服务器浏览器当前尝试访问的域名。

### 2. Server Hello

在服务端回复的报文中，包含了三部分子信息：Server Hello、Certificate、Server Hello Done，实际上这是第一个会话的内容，如果复用会话，这里的报文内容会有所区别。

![https]({{ site.url }}/images/linux/https-protocol-01.png "https"){: .pull-center width="80%" }

#### Server Hello

在服务器发送的 Hello 握手报文中，同样包括了 4 字节的 UTC 时间戳以及 28 字节随机数；还有会话 ID 以及服务器最终选择的加密族。

可以看到，在一堆加密族中，最终选择的是 TLS_RSA_WITH_3DES_EDE_CBC_SHA，这也就意味着将会使用 RSA 公钥加密算法来区分证书签名和交换密钥；通过 3DES_EDE_CBC 加密算法来加密数据；利用 SHA 来做 hash 校验信息。

#### Certificate

也就是证书信息，该证书允许客户端在服务器上进行认证，证书的内容是明文保存的，可以通过浏览器查看，当然也包括了 WireShark 。

![https]({{ site.url }}/images/linux/https-protocol-02.png "https"){: .pull-center width="67%" }

#### Server Hello Done

这是一个零字节信息，用于告诉客户端整个协商过程已经结束，并且表明服务器不会再向客户端询问证书。

### 3. 校验证书

此时，浏览器已经获取了服务器的证书，浏览器会通过证书确认网站是否受信，它会检查支持的域名、是否在证书有效时间范围内、确认证书所携带的公共密钥已被授权用于交换密钥。

为什么我们要信任这个证书？证书中所包含的签名是一串非常长的大端格式的数字：

![https]({{ site.url }}/images/linux/https-protocol-03.png "https"){: .pull-center width="67%" }

在验证证书的有效性的时候，会逐级去寻找签发者的证书，直至根证书为结束，然后通过公钥一级一级验证数字签名的正确性。

### 4. 生成 Premaster

当客户端通过了证书校验之后，会生成 48 字节的随机数，称为 Pre-master Secret 。如前所述，最终选择 RSA 来交换密钥，此时 pre-master 会用来计算 master secret 以及 key block 。

![https]({{ site.url }}/images/linux/https-protocol-04.png "https"){: .pull-center width="67%" }

其中，pre-master secret 在发送时，会通过服务器的公钥进行加密。

另外，该报文中还包括了 "Change Cipher Spec" ，用来告知服务器，后面客户端发送的信息，都是按照之前协商好的算法进行。

而最后的 "Encrypted Handshake Message" 则包含了之前通讯中的 hash 和 MAC 信息。

### 5. 计算 Master Secret 以及 Key Block

到此为止，服务端和客户端都知道了 Pre-master secret (48Bytes) + Client.random (28Bytes) + Server.random (28Bytes)，接下来就会按照协议，通过如下的算法生成 Master Secret 。

{% highlight text %}
master_secret = PRF(pre_master_secret, "master secret", ClientHello.random + ServerHello.random)
{% endhighlight %}

其中 PRF 是一个随机函数，定义如下。

{% highlight text %}
PRF(secret, label, seed) = P_MD5(S1, label + seed) XOR P_SHA-1(S2, label + seed)
{% endhighlight %}

在计算 Master Secret 的同时，服务器和客户端还会同时计算 Key Block ，算法如下。

{% highlight text %}
key_block = PRF(SecurityParameters.master_secret, "key expansion",
    SecurityParameters.server_random + SecurityParameters.client_random);
{% endhighlight %}

Key Block 会被分为不同的 Blocks。

![https]({{ site.url }}/images/linux/https-key-blocks.png "https"){: .pull-center }

其中，MAC Key 用来对数据进行验证的，Encryption Key 用来对数据进行加解密的会话密钥。

### 6. 服务端确认加密

服务端在收到客户端发送的 "Encrypted Message" 消息之后，会对其中的值进行校验，如果校验失败则会关闭连接。

![https]({{ site.url }}/images/linux/https-protocol-05.png "https"){: .pull-center width="67%" }

校验成功则会与客户端一样发送 "Change Cipher Spec" 。

### 7. 发送加密信息

到此为止，双方就可以愉快的发送加密信息了。

### 8. 杂项

记录下，杂七杂八的东西。

#### Encrypted Handshake Message

实际上，在应用数据在传输之前，也就是在握手中的加密数据，要附加上 MAC Secret，然后再对这个数据包使用 write encryption key 进行加密。

在服务端收到密文之后，使用 Client write encryption key 进行解密，客户端收到服务端的数据之后使用 Server write encryption key 进行解密，然后使用各自的 write MAC key 对数据的完整性包括是否被串改进行验证。

#### Premaster Secret

其长度为 48 个字节，前 2 个字节是协议版本号，剩下的 46 个字节填充一个随机数。

<!--
PreMaster secret前两个字节是TLS的版本号，这是一个比较重要的用来核对握手数据的版本号，因为在Client Hello阶段，客户端会发送一份加密套件列表和当前支持的SSL/TLS的版本号给服务端，而且是使用明文传送的，如果握手的数据包被破解之后，攻击者很有可能串改数据包，选择一个安全性较低的加密套件和版本给服务端，从而对数据进行破解。所以，服务端需要对密文中解密出来对的PreMaster版本号跟之前Client Hello阶段的版本号进行对比，如果版本号变低，则说明被串改，则立即停止发送任何消息。
-->

需要注意，前两个随机数都是明文传输的，窃听者是可以轻易获取到的，只有最后一个 PreMaster Secret 是加密传输的，只有拥有服务器私钥才能解密，一旦 PreMaster Secret 泄露，那么本次通信就就完全可被破解了。

#### 关于随机数

如上，是通过三个随机数来生成最终的 Master Secret 的，SSL 协议不信任每个主机都能生成完全随机的随机数，所以这里需要服务器和客户端共生成 3 个随机数，每增加一个自由度，随机性就会相应增加。






## 其它

### Cipher Suite

在协商阶段，服务器和客户端会协商使用的加密协议，如下是客户端提供的协议，服务端会选择相应的协议。

![https cipher specs]({{ site.url }}/images/linux/https-cipher-specs.png "https cipher specs"){: .pull-center }

在示例中，会使用 TLS_RSA_WITH_AES_256_CBC_SHA (0x0035)，其中每组包含了四部分信息，分别是：

* 密钥交换算法。客户端与服务器之间在握手的过程中如何认证，用到的算法包括 RSA、Diffie-Hellman、ECDH、PSK 等；
* 加密算法。加密消息流，该名称后通常会带有两个数字，分别为密钥的长度和初始向量的长度；如果是一个数则表示相同，如 DES、RC2、RC4、AES 等；
* 报文认证信息码 (MAC) 算法。用于创建报文摘要，防止被篡改，从而确保消息的完整性，如 MD5、SHA 等。

对于上述的协议，实际使用的算法如下。

{% highlight text %}
TLS_RSA_WITH_AES_256_CBC_SHA (0x0035)
   基于 TLS 协议；
   使用 RSA 作为密钥交换算法；
   加密算法是 AES，其中密钥和初始向量的长度都是 256；
   MAC 算法，在这里就是哈希算法是 SHA。
{% endhighlight %}






## CA 证书详情

假设你要通过支付宝转笔钱，首先要根服务器建立连接，浏览器收到了服务器发送的证书，其中包括了服务器的公钥，那么怎么证明收到的公钥就是该服务器的呢？

![https]({{ site.url }}/images/linux/https-ca-intro.png "https"){: .pull-center width="50%" }

如果无法解决如上的问题，那么就可能会导致中间人攻击，以及信息抵赖，这时，CA 就出场了。

解决上述身份验证问题的关键是确保获取的公钥途径是合法的，能够验证服务器的身份信息，为此需要引入权威的第三方机构 (Certificated Authority, CA)，一个第三方可信证书颁发机构，具有权威性、公正性的机构。

CA 负责核实公钥的拥有者的信息，并颁发认证 "证书"，同时能够为使用者提供证书验证服务，即 PKI 体系。


### 证书内容

证书包含的内容可以概述为三部分，用户的信息、用户的公钥、还有 CA 中心对该证书里面的信息的签名。


![https]({{ site.url }}/images/linux/https-ca-github.png "https"){: .pull-center width="60%"}

如上是 Firfox 显示的 Github 中的证书，该页面显示了证书中比较重要的内容，而详细的信息可以从 Details 标签页中查看。

下面是详细内容：

* 版本号(Version)，用来指明 X.509 证书的格式版本，包括了 V1、V2、V3；

* 序列号(Serial Number)，CA 分配给证书的唯一标识符，用于证书被取消时；

* 签名算法(Signature Algorithm)，CA 签发证书使用的签名算法，如 sha256WithRSAEncryption；

* 签发机构名(Issuer)，标识签发证书的 CA 的 X.500 DN(DN-Distinguished Name) 名字，包括了国家(C)、省市(ST)、地区(L)、组织机构(O)、单位部门(OU)、通用名(CN)、邮箱地址；

* 有效期(Validity)，指定证书的有效期，包括了开始和失效日期；

* 证书用户名(Subject)，使用该证书的机构，包含的内容与签发机构名相同；

* 证书持有者公钥信息(Subject Public Key Info)，包括了公钥值以及公钥使用的算法；

* 扩展项(extension)，V3 证书对 V2 的扩展项，以使证书能够附带额外信息；

### 证书链




.减少根证书结构的管理工作量，可以更高效的进行证书的审核与签发;

b.根证书一般内置在客户端中，私钥一般离线存储，一旦私钥泄露，则吊销过程非常困难，无法及时补救;

c.中间证书结构的私钥泄露，则可以快速在线吊销，并重新为用户签发新的证书;

d.证书链四级以内一般不会对 HTTPS 的性能造成明显影响。

CA 机构能够签发证书，同样也存在机制宣布以往签发的证书无效，也就是吊销证书。证书使用者不合法，CA 需要废弃该证书；或者私钥丢失，使用者申请让证书无效。





### 提交验证流程

首先，介绍下服务器的开发者是如何向 CA 申请证书的。

服务器的开发者向 CA 提出申请，CA 在核实申请者的身份之后，会用自己的私钥对信息进行加密，也就是数字签名；服务器则将这份由 CA 颁发的公钥证书在握手阶段发送给客户端，证书就相当于是服务器的身份证。

![encrypt digital signature]({{ site.url }}/images/linux/encrypt-digital-signature.jpg "encrypt digital signature"){: .pull-center width="60%" }

在验证证书的有效性的时候，会逐级去寻找签发者的证书，直至根证书为结束，然后通过公钥一级一级验证数字签名的正确性。

![client verify]({{ site.url }}/images/linux/https-client-verify.gif "client verify"){: .pull-center width="60%" }

也就是说，无论如何，都需要有根证书才可以的，其中有些浏览器是使用的系统的，而有些是自己管理。


### 自签名证书

如上所述，Firefox 使用自己的证书管理器 (Certificates Manager)，并没有像 IE 和 Chrome 那样直接使用系统中的证书存储，所以，对于 Firefox 来说，需要将自签发的 SSL 证书导入受信任根证书。

对于 Firefox 来说，证书的管理在 Edit => Preferences => Advanced => Certificates 中，可以选择导入、导出、删除等操作。










## 攻击

Web 安全是一项系统工程，任何细微疏忽都可能导致整个安全壁垒土崩瓦解。

如上所述，虽然 HTTPS 提供了 内容加密、数据完整性、身份认证 三大安全保证；但是，也会受到非法根证书、服务端配置错误、SSL 库漏洞、私钥被盗等等风险的影响。

### Man-in-the-middle

也就是中间人攻击，能够与网络通讯两端分别创建连接，交换其收到的数据，使得通讯两端都认为自己直接与对方对话，事实上整个会话都被中间人所控制。

类似于一些抓包调试工具，基本都是通过创建本地 Proxy 服务，再修改浏览器 Proxy 设置来达到拦截流量的目的，它们的工作原理与中间人攻击一致，常见的有 Fiddler、Charles 和 Whistle。

在此主要讨论 HTTPS 中间人，简单示意如下：

{% highlight text %}
Server <---> Local Proxy <---> Browser
         ^                 ^
       HTTPS(1)          HTTPS(2)
{% endhighlight %}

上述 HTTPS(1) 连接，是中间人冒充客户端与服务端建立的连接，由于 HTTPS 服务端一般不认证客户端身份，这一步通常没有问题。

对于 HTTPS(2) 连接来说，中间人想要冒充服务端，必须拥有对应域名的证书私钥，而攻击者要拿到私钥，只能通过这些手段：1）去网站服务器上拿；2）从 CA 处签发证书；3）自己签发证书。

要防范前两点，需要保障主机安全避免私钥被盗，避免攻击者重签证书；对于自签证书，需要防止在客户端添加内置根证书，如下是 Fiddler 工具添加的受信根证书。

![https root]({{ site.url }}/images/linux/https-man-in-the-middle.png "https root"){: .pull-center width="60%" }

### RSA Private Key

在参考中有介绍如何通过 FireFox + Wireshark 抓包直接读取并分析网卡数据，这里的示例实际提供了私钥，所以看看如何通过私钥来解密这个网站的加密流量。

打开 Wireshark 的 SSL 协议设置，也就是 Edit => Preferences => Protocols => SSL，把 IP、端口、协议和证书私钥都配上，其中私钥必须存为 PEM 格式。

![https]({{ site.url }}/images/linux/https-protocol-10.png "https"){: .pull-center width="80%" }

然后就可以看到，密文已经被解密了，只能解密 HTTP-1，而不能解密 HTTP-2。










## 参考

一篇介绍 https 不错的文章，可以参考中文版 [HTTPS连接的前几毫秒发生了什么](http://blog.jobbole.com/48369/)，或者英文版 [The First Few Milliseconds of an HTTPS Connection](http://www.moserware.com/2009/06/first-few-milliseconds-of-https.html)，或者 [本地保存的版本](/reference/linux/The First Few Milliseconds of an HTTPS Connection.mht)；细节内容也可以参考协议的实现 [SSL V3.0](https://tools.ietf.org/rfc/rfc6101.txt) 。

其中的示例是从 [WireShark SampleCaptures](https://wiki.wireshark.org/SampleCaptures) 上下载的，也就是 [SSL with decryption keys](https://wiki.wireshark.org/SampleCaptures?action=AttachFile&do=get&target=snakeoil2_070531.tgz)，也可以从 [本地](/reference/linux/ssl-example.tar.bz2) 下载。


#### 其它

一些常见的技巧可以参考：

* [使用 Wireshark 调试 HTTP/2 流量](http://blog.jobbole.com/95106/)，通过 FireFox 导出密钥，可以参考 [本地文档](/reference/linux/Wireshark_HTTP_2.mht) 。

* 网站检测的链接 [SSL LABS](https://www.ssllabs.com/)、[SSL Shopper](https://www.sslshopper.com/ssl-checker.html)、[China SSL](https://www.chinassl.net/ssltools/ssl-checker.html) 。



<!--

openssl s_client -connect 10.44.32.91~94\81~84:443

一个关于SSL不错的Blog
https://blog.ivanristic.com/SSL_Threat_Model.png
HTTPS的七个误解（译文）
http://www.ruanyifeng.com/blog/2011/02/seven_myths_about_https.html
浅谈三种解密 HTTPS 流量的方法
http://www.2cto.com/article/201603/496004.html
很详细介绍的SSL
https://blog.cloudflare.com/keyless-ssl-the-nitty-gritty-technical-details/
TLS/SSL 高级进阶
https://segmentfault.com/a/1190000007283514
HTTPS 理论详解与实践
https://segmentfault.com/a/1190000004985253

https://www.wosign.com/faq/faq2016-0309-04.htm
http://blog.jobbole.com/86660/
http://www.itrus.cn/service_13html
http://resources.infosecinstitute.com/cryptography-101-with-ssl/
-->


{% highlight text %}
{% endhighlight %}
