---
title: HTTPS 協議詳解
layout: post
comments: true
language: chinese
category: [linux,webserver,misc]
keywords: https,ssl,tls
description: 我們知道 HTTP 採用的是明文傳輸，而在互聯網中，比如要在淘寶買個東西，使用支付寶轉個帳，必須要保證這些信息只有客戶端和服務器才知道的，也就是通過 HTTPS 協議。接下來，我們就看看 HTTPS 協議是如何實現的。
---

我們知道 HTTP 採用的是明文傳輸，而在互聯網中，比如要在淘寶買個東西，使用支付寶轉個帳，必須要保證這些信息只有客戶端和服務器才知道的，也就是通過 HTTPS 協議。

接下來，我們就看看 HTTPS 協議是如何實現的。

<!-- more -->

![https logo]({{ site.url }}/images/linux/https-logo.jpg "https logo"){: .pull-center }

## 簡介

Secure Hypertext Transfer Protocol, HTTPS 也就是安全超文本傳輸協議，是一個安全通信通道，它基於 HTTP 開發用於在客戶計算機和服務器之間交換信息。

實際上，簡單來說，它是 HTTP 的安全版，是使用 SSL(Secure Socket Layer)/TLS(Transport Layer Security) 加密的 HTTP 協議。通過 TLS/SSL 協議的的身份驗證、信息加密和完整性校驗的功能，從而避免信息竊聽、信息篡改和信息劫持的風險。

簡單來說，HTTPS 提供了加密 (Encryption)、認證 (Verification)、鑑定 (Identification) 三種功能。如下的解釋中，假設是張三和李四在通訊。

{% highlight text %}
私密性(Confidentiality/Privacy):
    也就是提供信息加密；保證信息只有張三和李四知道，而不會被竊聽。
可信性(Authentication):
    身份驗證，主要是服務器端的，有些銀行也會對客戶端進行認證；用來證明李四就是李四。
完整性(Message Integrity):
    保證信息傳輸過程中的完整性，防止被修改；李四接收到的消息就是張三發送的。
{% endhighlight %}

如下是 SSL/TLS 協議的介紹。

### SSL/TLS 協議

SSL 最初是在 1996 年由 Netscape 發佈，由於一些安全的原因 SSL v1.0 和 SSL v2.0 都沒有公開，直到 1996 年的 SSL v3.0。TLS 是 v3.0 的升級版，目前市面上所有的 HTTPS 都是用的是 TLS 而非 SSL，而且很多地方都混用兩者。

![https]({{ site.url }}/images/linux/https-ssl-tls-layer.png "https"){: .pull-center width="75%" }

其中上圖中頂層的三塊又組成了 SSL Handshaking Protocols，在處理時有三種狀態。

{% highlight text %}
empty state -------------------> pending state ------------------> current state

             Handshake Protocol                Change Cipher Spec
{% endhighlight %}

當完成握手後，客戶端和服務端確定了加密、壓縮和 MAC 算法及其參數，數據通過指定算法處理；而之前的所有操作都是明文傳輸的。


### 關鍵技術

首先，在實現時加密算法是公開的，而私密性是通過密鑰保證的。另外，為了實現上述的機制，需要一些比較常見的技術。

其中與密碼學相關的內容，可以參考 [加密算法簡介](/blog/encryption-introduce.html) 。

#### 私鑰交換 Key Exchange Algorithms

發送信息的時候，需要通過對稱加密來加密數據，此時就涉及到了私鑰的交換，通常使用的算法包括了 Diffie-Hellman、RSA 。

{% highlight text %}
Bob's Public Key + Alice's Secret Key     Bob's Secret Key (Decrypt)
          |----->-------->------->-------->------>-----|
                                              Get Alice's Secret Key
{% endhighlight %}

對於 RSA 來說，所有人都知道 Bob 的公鑰，只需要將 Alice 的私鑰通過公鑰進行加密即可，該信息只有通過 Bob 的私鑰才可以解密。

當然，在實際使用時還會增加其它的隨機值。

#### 數字證書 Digital Certificates

到此，需要通過一種方法獲取到 Bob 的公鑰，這就是通過數字證書獲取的。數字證書中除了包括了公鑰信息，還有與 Bob 相關的信息。

#### 數字證書認證機構 Certification Authority, CA

接下來解決的是，如何確認數字證書的擁有著就像它聲明的那樣，為此引入了 Public Key Infrastructure, PKI ，包括一些與數字證書相關的軟件、硬件等信息。

CA 就是一個三方機構，用來證明 Bob 確實就是 Bob 。

CA 認證分為三類：DV (domain validation)，OV (organization validation)，EV (extended validation)，證書申請難度從前往後遞增，貌似 EV 這種不僅僅是有錢就可以申請的。

#### 數字簽名 消息完整性

實際上有篇很不錯的文章 [What is a Digital Signature?](http://www.youdzone.com/signature.html) ，也可以參考 [本地文章](/reference/linux/What is a Digital Signature.mht)，在此就不做過多介紹了。



## 執行流程

我們直接從 WireShark 的官網網站上下載了一個 HTTPS 的示例，詳細的下載地址可以參考文章末尾的參考內容。

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

首先，是正常的 TCP 開始的三次握手連接，在此就不做過多的介紹了；完成之後才會開始 SSL 之間的溝通，這也是接下來重點介紹的內容。

在進行正常的數據傳輸之前，會有一個協商的過程，包括了協議的版本、會話ID、加密算法 (含有一套，後面介紹)、壓縮方法；另外，在該階段，還會相互交換服務端和客戶端的隨機值。

### 1. Client Hello

示例中有兩個會話的數據傳輸，分別是 SSLv2.0 以及 SSLv3.0，現在一般使用的是 v3.0，其中 v2.0 在該階段會使用 challenge 機制，不過不太清楚詳細內容，在 [RFC-6101](https://tools.ietf.org/rfc/rfc6101.txt) 中略有介紹。

這裡，僅介紹 v3.0 的內容。

![https]({{ site.url }}/images/linux/https-protocol-00.png "https"){: .pull-center width="80%" }

在上述的圖片中，包括了協議的版本信息，還有些比較重要的信息。

##### 隨機數

開始的四個字節以 Unix 時間格式記錄了客戶端 UTC 時間 (從1970年1月1日開始到當前時刻所經歷的秒數)；接著後面有 28 字節的隨機數，在後面的過程中我們會用到這個 **客戶端的隨機數** 。

##### Session ID, SID

一般來說，第一次建立連接的時候 ID 是一個空值 (NULL)，當然也有可能會重用之前的會話，從而避免一個完整的握手過程。

##### 密文族 (Cipher Suites)

密文族是瀏覽器所支持的加密算法的清單，整個密文族有 20 種。

另外，有是還會有 Server_name 的擴展，能告訴服務器瀏覽器當前嘗試訪問的域名。

### 2. Server Hello

在服務端回覆的報文中，包含了三部分子信息：Server Hello、Certificate、Server Hello Done，實際上這是第一個會話的內容，如果複用會話，這裡的報文內容會有所區別。

![https]({{ site.url }}/images/linux/https-protocol-01.png "https"){: .pull-center width="80%" }

#### Server Hello

在服務器發送的 Hello 握手報文中，同樣包括了 4 字節的 UTC 時間戳以及 28 字節隨機數；還有會話 ID 以及服務器最終選擇的加密族。

可以看到，在一堆加密族中，最終選擇的是 TLS_RSA_WITH_3DES_EDE_CBC_SHA，這也就意味著將會使用 RSA 公鑰加密算法來區分證書籤名和交換密鑰；通過 3DES_EDE_CBC 加密算法來加密數據；利用 SHA 來做 hash 校驗信息。

#### Certificate

也就是證書信息，該證書允許客戶端在服務器上進行認證，證書的內容是明文保存的，可以通過瀏覽器查看，當然也包括了 WireShark 。

![https]({{ site.url }}/images/linux/https-protocol-02.png "https"){: .pull-center width="67%" }

#### Server Hello Done

這是一個零字節信息，用於告訴客戶端整個協商過程已經結束，並且表明服務器不會再向客戶端詢問證書。

### 3. 校驗證書

此時，瀏覽器已經獲取了服務器的證書，瀏覽器會通過證書確認網站是否受信，它會檢查支持的域名、是否在證書有效時間範圍內、確認證書所攜帶的公共密鑰已被授權用於交換密鑰。

為什麼我們要信任這個證書？證書中所包含的簽名是一串非常長的大端格式的數字：

![https]({{ site.url }}/images/linux/https-protocol-03.png "https"){: .pull-center width="67%" }

在驗證證書的有效性的時候，會逐級去尋找簽發者的證書，直至根證書為結束，然後通過公鑰一級一級驗證數字簽名的正確性。

### 4. 生成 Premaster

當客戶端通過了證書校驗之後，會生成 48 字節的隨機數，稱為 Pre-master Secret 。如前所述，最終選擇 RSA 來交換密鑰，此時 pre-master 會用來計算 master secret 以及 key block 。

![https]({{ site.url }}/images/linux/https-protocol-04.png "https"){: .pull-center width="67%" }

其中，pre-master secret 在發送時，會通過服務器的公鑰進行加密。

另外，該報文中還包括了 "Change Cipher Spec" ，用來告知服務器，後面客戶端發送的信息，都是按照之前協商好的算法進行。

而最後的 "Encrypted Handshake Message" 則包含了之前通訊中的 hash 和 MAC 信息。

### 5. 計算 Master Secret 以及 Key Block

到此為止，服務端和客戶端都知道了 Pre-master secret (48Bytes) + Client.random (28Bytes) + Server.random (28Bytes)，接下來就會按照協議，通過如下的算法生成 Master Secret 。

{% highlight text %}
master_secret = PRF(pre_master_secret, "master secret", ClientHello.random + ServerHello.random)
{% endhighlight %}

其中 PRF 是一個隨機函數，定義如下。

{% highlight text %}
PRF(secret, label, seed) = P_MD5(S1, label + seed) XOR P_SHA-1(S2, label + seed)
{% endhighlight %}

在計算 Master Secret 的同時，服務器和客戶端還會同時計算 Key Block ，算法如下。

{% highlight text %}
key_block = PRF(SecurityParameters.master_secret, "key expansion",
    SecurityParameters.server_random + SecurityParameters.client_random);
{% endhighlight %}

Key Block 會被分為不同的 Blocks。

![https]({{ site.url }}/images/linux/https-key-blocks.png "https"){: .pull-center }

其中，MAC Key 用來對數據進行驗證的，Encryption Key 用來對數據進行加解密的會話密鑰。

### 6. 服務端確認加密

服務端在收到客戶端發送的 "Encrypted Message" 消息之後，會對其中的值進行校驗，如果校驗失敗則會關閉連接。

![https]({{ site.url }}/images/linux/https-protocol-05.png "https"){: .pull-center width="67%" }

校驗成功則會與客戶端一樣發送 "Change Cipher Spec" 。

### 7. 發送加密信息

到此為止，雙方就可以愉快的發送加密信息了。

### 8. 雜項

記錄下，雜七雜八的東西。

#### Encrypted Handshake Message

實際上，在應用數據在傳輸之前，也就是在握手中的加密數據，要附加上 MAC Secret，然後再對這個數據包使用 write encryption key 進行加密。

在服務端收到密文之後，使用 Client write encryption key 進行解密，客戶端收到服務端的數據之後使用 Server write encryption key 進行解密，然後使用各自的 write MAC key 對數據的完整性包括是否被串改進行驗證。

#### Premaster Secret

其長度為 48 個字節，前 2 個字節是協議版本號，剩下的 46 個字節填充一個隨機數。

<!--
PreMaster secret前兩個字節是TLS的版本號，這是一個比較重要的用來核對握手數據的版本號，因為在Client Hello階段，客戶端會發送一份加密套件列表和當前支持的SSL/TLS的版本號給服務端，而且是使用明文傳送的，如果握手的數據包被破解之後，攻擊者很有可能串改數據包，選擇一個安全性較低的加密套件和版本給服務端，從而對數據進行破解。所以，服務端需要對密文中解密出來對的PreMaster版本號跟之前Client Hello階段的版本號進行對比，如果版本號變低，則說明被串改，則立即停止發送任何消息。
-->

需要注意，前兩個隨機數都是明文傳輸的，竊聽者是可以輕易獲取到的，只有最後一個 PreMaster Secret 是加密傳輸的，只有擁有服務器私鑰才能解密，一旦 PreMaster Secret 洩露，那麼本次通信就就完全可被破解了。

#### 關於隨機數

如上，是通過三個隨機數來生成最終的 Master Secret 的，SSL 協議不信任每個主機都能生成完全隨機的隨機數，所以這裡需要服務器和客戶端共生成 3 個隨機數，每增加一個自由度，隨機性就會相應增加。






## 其它

### Cipher Suite

在協商階段，服務器和客戶端會協商使用的加密協議，如下是客戶端提供的協議，服務端會選擇相應的協議。

![https cipher specs]({{ site.url }}/images/linux/https-cipher-specs.png "https cipher specs"){: .pull-center }

在示例中，會使用 TLS_RSA_WITH_AES_256_CBC_SHA (0x0035)，其中每組包含了四部分信息，分別是：

* 密鑰交換算法。客戶端與服務器之間在握手的過程中如何認證，用到的算法包括 RSA、Diffie-Hellman、ECDH、PSK 等；
* 加密算法。加密消息流，該名稱後通常會帶有兩個數字，分別為密鑰的長度和初始向量的長度；如果是一個數則表示相同，如 DES、RC2、RC4、AES 等；
* 報文認證信息碼 (MAC) 算法。用於創建報文摘要，防止被篡改，從而確保消息的完整性，如 MD5、SHA 等。

對於上述的協議，實際使用的算法如下。

{% highlight text %}
TLS_RSA_WITH_AES_256_CBC_SHA (0x0035)
   基於 TLS 協議；
   使用 RSA 作為密鑰交換算法；
   加密算法是 AES，其中密鑰和初始向量的長度都是 256；
   MAC 算法，在這裡就是哈希算法是 SHA。
{% endhighlight %}






## CA 證書詳情

假設你要通過支付寶轉筆錢，首先要根服務器建立連接，瀏覽器收到了服務器發送的證書，其中包括了服務器的公鑰，那麼怎麼證明收到的公鑰就是該服務器的呢？

![https]({{ site.url }}/images/linux/https-ca-intro.png "https"){: .pull-center width="50%" }

如果無法解決如上的問題，那麼就可能會導致中間人攻擊，以及信息抵賴，這時，CA 就出場了。

解決上述身份驗證問題的關鍵是確保獲取的公鑰途徑是合法的，能夠驗證服務器的身份信息，為此需要引入權威的第三方機構 (Certificated Authority, CA)，一個第三方可信證書頒發機構，具有權威性、公正性的機構。

CA 負責核實公鑰的擁有者的信息，並頒發認證 "證書"，同時能夠為使用者提供證書驗證服務，即 PKI 體系。


### 證書內容

證書包含的內容可以概述為三部分，用戶的信息、用戶的公鑰、還有 CA 中心對該證書裡面的信息的簽名。


![https]({{ site.url }}/images/linux/https-ca-github.png "https"){: .pull-center width="60%"}

如上是 Firfox 顯示的 Github 中的證書，該頁面顯示了證書中比較重要的內容，而詳細的信息可以從 Details 標籤頁中查看。

下面是詳細內容：

* 版本號(Version)，用來指明 X.509 證書的格式版本，包括了 V1、V2、V3；

* 序列號(Serial Number)，CA 分配給證書的唯一標識符，用於證書被取消時；

* 簽名算法(Signature Algorithm)，CA 簽發證書使用的簽名算法，如 sha256WithRSAEncryption；

* 簽發機構名(Issuer)，標識簽發證書的 CA 的 X.500 DN(DN-Distinguished Name) 名字，包括了國家(C)、省市(ST)、地區(L)、組織機構(O)、單位部門(OU)、通用名(CN)、郵箱地址；

* 有效期(Validity)，指定證書的有效期，包括了開始和失效日期；

* 證書用戶名(Subject)，使用該證書的機構，包含的內容與簽發機構名相同；

* 證書持有者公鑰信息(Subject Public Key Info)，包括了公鑰值以及公鑰使用的算法；

* 擴展項(extension)，V3 證書對 V2 的擴展項，以使證書能夠附帶額外信息；

### 證書鏈




.減少根證書結構的管理工作量，可以更高效的進行證書的審核與簽發;

b.根證書一般內置在客戶端中，私鑰一般離線存儲，一旦私鑰洩露，則吊銷過程非常困難，無法及時補救;

c.中間證書結構的私鑰洩露，則可以快速在線吊銷，並重新為用戶簽發新的證書;

d.證書鏈四級以內一般不會對 HTTPS 的性能造成明顯影響。

CA 機構能夠簽發證書，同樣也存在機制宣佈以往簽發的證書無效，也就是吊銷證書。證書使用者不合法，CA 需要廢棄該證書；或者私鑰丟失，使用者申請讓證書無效。





### 提交驗證流程

首先，介紹下服務器的開發者是如何向 CA 申請證書的。

服務器的開發者向 CA 提出申請，CA 在核實申請者的身份之後，會用自己的私鑰對信息進行加密，也就是數字簽名；服務器則將這份由 CA 頒發的公鑰證書在握手階段發送給客戶端，證書就相當於是服務器的身份證。

![encrypt digital signature]({{ site.url }}/images/linux/encrypt-digital-signature.jpg "encrypt digital signature"){: .pull-center width="60%" }

在驗證證書的有效性的時候，會逐級去尋找簽發者的證書，直至根證書為結束，然後通過公鑰一級一級驗證數字簽名的正確性。

![client verify]({{ site.url }}/images/linux/https-client-verify.gif "client verify"){: .pull-center width="60%" }

也就是說，無論如何，都需要有根證書才可以的，其中有些瀏覽器是使用的系統的，而有些是自己管理。


### 自簽名證書

如上所述，Firefox 使用自己的證書管理器 (Certificates Manager)，並沒有像 IE 和 Chrome 那樣直接使用系統中的證書存儲，所以，對於 Firefox 來說，需要將自簽發的 SSL 證書導入受信任根證書。

對於 Firefox 來說，證書的管理在 Edit => Preferences => Advanced => Certificates 中，可以選擇導入、導出、刪除等操作。










## 攻擊

Web 安全是一項系統工程，任何細微疏忽都可能導致整個安全壁壘土崩瓦解。

如上所述，雖然 HTTPS 提供了 內容加密、數據完整性、身份認證 三大安全保證；但是，也會受到非法根證書、服務端配置錯誤、SSL 庫漏洞、私鑰被盜等等風險的影響。

### Man-in-the-middle

也就是中間人攻擊，能夠與網絡通訊兩端分別創建連接，交換其收到的數據，使得通訊兩端都認為自己直接與對方對話，事實上整個會話都被中間人所控制。

類似於一些抓包調試工具，基本都是通過創建本地 Proxy 服務，再修改瀏覽器 Proxy 設置來達到攔截流量的目的，它們的工作原理與中間人攻擊一致，常見的有 Fiddler、Charles 和 Whistle。

在此主要討論 HTTPS 中間人，簡單示意如下：

{% highlight text %}
Server <---> Local Proxy <---> Browser
         ^                 ^
       HTTPS(1)          HTTPS(2)
{% endhighlight %}

上述 HTTPS(1) 連接，是中間人冒充客戶端與服務端建立的連接，由於 HTTPS 服務端一般不認證客戶端身份，這一步通常沒有問題。

對於 HTTPS(2) 連接來說，中間人想要冒充服務端，必須擁有對應域名的證書私鑰，而攻擊者要拿到私鑰，只能通過這些手段：1）去網站服務器上拿；2）從 CA 處簽發證書；3）自己簽發證書。

要防範前兩點，需要保障主機安全避免私鑰被盜，避免攻擊者重簽證書；對於自簽證書，需要防止在客戶端添加內置根證書，如下是 Fiddler 工具添加的受信根證書。

![https root]({{ site.url }}/images/linux/https-man-in-the-middle.png "https root"){: .pull-center width="60%" }

### RSA Private Key

在參考中有介紹如何通過 FireFox + Wireshark 抓包直接讀取並分析網卡數據，這裡的示例實際提供了私鑰，所以看看如何通過私鑰來解密這個網站的加密流量。

打開 Wireshark 的 SSL 協議設置，也就是 Edit => Preferences => Protocols => SSL，把 IP、端口、協議和證書私鑰都配上，其中私鑰必須存為 PEM 格式。

![https]({{ site.url }}/images/linux/https-protocol-10.png "https"){: .pull-center width="80%" }

然後就可以看到，密文已經被解密了，只能解密 HTTP-1，而不能解密 HTTP-2。










## 參考

一篇介紹 https 不錯的文章，可以參考中文版 [HTTPS連接的前幾毫秒發生了什麼](http://blog.jobbole.com/48369/)，或者英文版 [The First Few Milliseconds of an HTTPS Connection](http://www.moserware.com/2009/06/first-few-milliseconds-of-https.html)，或者 [本地保存的版本](/reference/linux/The First Few Milliseconds of an HTTPS Connection.mht)；細節內容也可以參考協議的實現 [SSL V3.0](https://tools.ietf.org/rfc/rfc6101.txt) 。

其中的示例是從 [WireShark SampleCaptures](https://wiki.wireshark.org/SampleCaptures) 上下載的，也就是 [SSL with decryption keys](https://wiki.wireshark.org/SampleCaptures?action=AttachFile&do=get&target=snakeoil2_070531.tgz)，也可以從 [本地](/reference/linux/ssl-example.tar.bz2) 下載。


#### 其它

一些常見的技巧可以參考：

* [使用 Wireshark 調試 HTTP/2 流量](http://blog.jobbole.com/95106/)，通過 FireFox 導出密鑰，可以參考 [本地文檔](/reference/linux/Wireshark_HTTP_2.mht) 。

* 網站檢測的鏈接 [SSL LABS](https://www.ssllabs.com/)、[SSL Shopper](https://www.sslshopper.com/ssl-checker.html)、[China SSL](https://www.chinassl.net/ssltools/ssl-checker.html) 。



<!--

openssl s_client -connect 10.44.32.91~94\81~84:443

一個關於SSL不錯的Blog
https://blog.ivanristic.com/SSL_Threat_Model.png
HTTPS的七個誤解（譯文）
http://www.ruanyifeng.com/blog/2011/02/seven_myths_about_https.html
淺談三種解密 HTTPS 流量的方法
http://www.2cto.com/article/201603/496004.html
很詳細介紹的SSL
https://blog.cloudflare.com/keyless-ssl-the-nitty-gritty-technical-details/
TLS/SSL 高級進階
https://segmentfault.com/a/1190000007283514
HTTPS 理論詳解與實踐
https://segmentfault.com/a/1190000004985253

https://www.wosign.com/faq/faq2016-0309-04.htm
http://blog.jobbole.com/86660/
http://www.itrus.cn/service_13html
http://resources.infosecinstitute.com/cryptography-101-with-ssl/
-->


{% highlight text %}
{% endhighlight %}
