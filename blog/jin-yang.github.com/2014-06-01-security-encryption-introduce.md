---
title: 加密算法簡介
layout: post
comments: true
language: chinese
category: [misc]
keywords: 加密算法
description:
---

開放網絡中數據傳輸的安全性一直是一個熱門的話題，特別是電子商務興起的今天，各種攻擊欺騙手段更是層出不窮，安全的網絡傳輸需求愈顯重要。

在此簡單介紹一些常見的加密算法等。

<!-- more -->

## 對稱加密

也就是 symmetric-key encryption，其中常見的算法包括了 AES、DES、3DES 等。

對稱加密指的是可以使用同一個密鑰對內容進行加密和解密，相比非對稱加密，它的特點是加/解密速度快，並且加密的內容長度幾乎沒有限制。

![encrypt symmetric key]({{ site.url }}/images/linux/encrypt-symmetric-key.gif "encrypt symmetric key"){: .pull-center }

DES 是一種分組數據加密技術，也即先將數據分成固定長度的小數據塊，然後進行加密，速度較快，適用於大量數據加密。3DES 是一種基於 DES 的加密算法，使用 3 個不同密匙對同一個分組數據塊進行 3 次加密，從而使得密文強度更高。

相較於 DES/3DES 而言，AES 算法有著更高的速度和資源使用效率，安全級別也較之更高了，被稱為下一代加密標準。


## 非對稱加密/公鑰加密

也就是 asymmetric/public-key encryption，常見的加密算法有 RSA、DSA、ECC 等。

非對稱加密有兩個密鑰，分別為公鑰和私鑰，其中公鑰公開給所有人，私鑰永遠只能自己知道。

![encrypt asymmetric key]({{ site.url }}/images/linux/encrypt-asymmetric-encryption.gif "encrypt asymmetric key"){: .pull-center }

使用公鑰加密的信息只能使用私鑰解密，使用私鑰加密只能使用公鑰解密。前者用來傳輸需要保密的信息，因為全世界只有知道對應私鑰的人才可以解密；後者用來作數字簽名，因為公鑰對所有人公開的，可以用來確認這個信息是否是從私鑰的擁有者發出的。

RSA 和 DSA 的安全性及其它各方面性能相似，而 ECC 較之則有很多性能優越，包括處理速度、帶寬要求、存儲空間等。

PS. 理論上來說，無法通過公鑰算出私鑰，或者說以現在的計算能力需要幾億萬年才能算出來。

## 安全散列算法

也就是 Secure Hash Algorithm，常見的算法包括了 MD5、SHA1、HMAC 等。

將任意長度的二進制值映射為較短的固定長度的二進制值，這個短的二進制值稱為哈希值，這個算法具有不可逆、碰撞低等特性。同時該類算法可以用作數字簽名，用來證實某個信息確實是由某個人發出的，同時可以保證信息沒有被修改。

實際上，簡單來說，這種算法有兩個特性：A) 不同的輸入一定得出不同的 hash 值；B) 無法從 hash 值倒推出原來的輸入。

![encrypt digital signature]({{ site.url }}/images/linux/encrypt-digital-signature.jpg "encrypt digital signature"){: .pull-center width="70%" }

數字簽名通常先使用一個 Hash 函數或者消息摘要算法，獲得可以唯一對應原信息的摘要信息，然後通過私鑰加密。這樣其它人就可以通過公鑰解密摘要信息，然後使用同樣的算法對接收到的信息計算摘要，如果兩者獲得的摘要信息相同即可。

在數據傳輸前，使用 MD5 和 SHA1 算法需要發送和接收方在數據傳送之前就知道密匙生成算法，而 HMAC 需要生成一個密匙，發送方用此密匙對數據進行摘要處理，接收方再利用此密匙對接收到的數據進行摘要處理，再判斷生成的密文是否相同。

### 簡介

MD5 的全稱是 ```Message-Digest Algorithm 5``` (信息-摘要算法)，128 位長度，目前 MD5 是一種不可逆算法，具有很高的安全性，它對應任何字符串都可以加密成一段唯一的固定長度的代碼。

SHA1 的全稱是 ```Secure Hash Algorithm``` (安全哈希算法) ，SHA1 基於 MD5，加密後的數據長度更長，它對長度小於 264 的輸入，產生長度為 160bit 的散列值。

另外，Python 內置的 hashlib 模塊就包括了 md5 和 sha1 算法，而且使用起來也極為方便。

## 其它

簡單介紹一下一些常見的使用內容。

### 選擇密鑰長度

如果在選定了加密算法後，那採用多少位的密鑰呢？

一般來說，密鑰越長，運行的速度就越慢，應該根據的我們實際需要的安全級別來選擇，一般來說，RSA 建議採用 1024 位的數字，ECC 建議採用 160 位，AES 採用 128 位即可。

常見的加密算法速度可以參考如下。

![encrypt truecrypt speedtest]({{ site.url }}/images/linux/encrypt-truecrypt-speedtest.png "encrypt truecrypt speedtest"){: .pull-center }

如果只是加密一段信息，也不是經常需要，那可以用最強的加密算法，甚至也可以用不同的算法加密兩次。如果想要加密速度快點，那就用 AES 。

### RADIX64 BASE64

PGP 和 S/MIME 均使用了 Radix-64 編碼，而該編碼實際是基於 Base64。由兩部分構成，分別是 Base64 編碼後的數據和一個 CRC(24-bits) 校驗和。

Base64 是使用 64 個通用的可打印字符來存儲和表示二進制字數據，同時也可以進行簡單的加密。Base64 字符集包括 ```"A-Z"```、```"a-z"```、```"0-9"```、```"+"```、```"/"``` 以及後綴填充 ```"="``` 共計 65 個字符。

Base64 實質上是以 6bits 為單位，並將其影射為規定的字符集。通常，數據以字節為單位的存儲，也即 8bits，而 Base64 使用 6bits 表示一個字符，這就導致了計算機中的三個字節 (8bits\*3) 在經過 Base64 編碼後將變成四個字符 (6bits\*4)，轉碼後比原數據長大約三分之一。

轉碼時只需使用一個 24bits 的緩衝區，依次從原數據中讀入三個字節的數據，再每次從緩衝中取 6bits 轉化成 Base64 字符輸出。因此，如果是 3bytes 的整數倍，則不需要 "=" 填充，生於兩字節添加一個 "="，剩餘一個字節添加 "==" 。

其中，rfc2045 定義了 MIME 中的 Base64 傳輸編碼；而 rfc4880 協議則定義了 OpenPGP 中使用的 Radix-64 編碼。

## 文件完整性認證

通常為了防止文件的內容被修改，如一個文件號稱是包含了補丁的文件，卻被人加入了木馬；同時也可以用來保證文件的完整性，防止文件被損壞。

當然，一些 hash 算法可能會被破解，所以一些網站則提供了多種完整性校驗方法。

### MD5

計算出下載文件的 MD5 哈希值，然後與發佈者提供的 MD5 哈希值比較，通過判斷是否一致來斷定文件在發佈者發佈之後有沒有被篡改過。

使用的假設是，同一個文件產生的 MD5 哈希值是唯一的，但這點已經有辦法通過對文件進行少量的修改，讓文件的 MD5 後的哈希值保持一致，雖然操作起來有點難，但是確實有風險。

{% highlight text %}
$ md5sum file-name                              # 計算MD5哈希值
da9741366673b5066e74f5307c7d08d7  file-name
$ md5sum file-name > file-name.md5              # 將計算的哈希值保存到文件，可含多個
$ md5sum -c file-name.md5                       # 校驗文件是否被修改
file-name: OK                                   # 失敗為 file-name: FAILED ...
{% endhighlight %}


### SHA

原理同 MD5 一樣，相比 MD5 來說更安全一些，而且在在 HASH 求值方面，MD5 退出的舞臺將由 SHAn 佔據。SHA 家族有五個算法：SHA-1、SHA-224、SHA-256、SHA-384 和 SHA-512，後四種有時候稱為 SHA2 ，分別對應 linux 中的 sha...sum 命令。


{% highlight text %}
$ sha1sum file-name                                 # 計算SHA1哈希值
b14cb291f379140f1466381be80466c6722e4b1a  file-name
$ sha1sum file-name > file-name.sha1                # 將計算的哈希值保存到文件，可含多個
$ sha1sum -c file-name.sha1                         # 校驗文件是否被修改
file-name: OK                                       # 失敗為 file-name: FAILED ...
{% endhighlight %}

### PGP

實際上原理很簡單，也就是使用非對稱加密。首先生成唯一的密鑰對，包括了公鑰和私鑰，然後執行如下步驟。

{% highlight text %}
發佈者：
    1. 將密鑰對中的公鑰發佈到公鑰服務器；
    2. 通過私鑰對需要發佈的文件進行簽名，得到簽名文件；
    3. 將文件和用私鑰生成的簽名一起發佈；

下載者：
    1. 下載發佈者發佈的文件和簽名；
    2. 使用 PGP 獲取發佈者發佈到服務器的公鑰；
    3. 使用公鑰校驗文件簽名。
{% endhighlight %}

關於 GnuGP 的使用，詳細可參考 MySQL 的內容 [2.1.3.2 Signature Checking Using GnuPG](http://dev.mysql.com/doc/refman/5.7/en/checking-gpg-signature.html) 。

{% highlight text %}
$ gpg --import pubkey.asc                                      # 從官網上下載公鑰，然後導入
$ gpg --keyserver keyserver.ubuntu.com --recv-keys 8D253E8A    # 或者可以直接從服務器導入

$ gpg --verify downloaded-file-sign.asc                        # 如果沒有導入公鑰，則會提示No public key
{% endhighlight %}

到現在為止，我們可以確認該文件在上傳之後沒有被修改過。

但是，需要注意的是這個是未受信任的簽名認證，因為這個公鑰誰都可以發佈上去的。如果確實需要進一步認證，可能還要聯繫下真正的發佈者，確認這個密鑰的指紋 (fingerprint)，這也就是為什麼在上述驗證時可能會出現 WARNING 。

但是通常來說，通過用戶名和郵箱就可以基本確定的。當確認信任該公鑰後，可以通過如下的子命令設置信任該公鑰，這樣就不會再出現上面的 WARNING 。

{% highlight text %}
$ gpg --edit-key 'user-id' + sign
{% endhighlight %}



## 參考

[Reflections on Trusting Trust](http://www.win.tue.nl/~aeb/linux/hh/thompson/trust.html) 通常簡稱 RoTT 《對信任不疑的信任》，一個比較經典的文章，是肯湯普遜 (Ken Thompson) 1983 年獲得圖靈獎時的演說：你最終還是要絕對信任某一人，在這個問題上沒有第二條路可走。

[安全加解密技術 盤點密碼學進階史](http://server.51cto.com/News-369437_all.htm) 一篇不錯的介紹文章，可以參考 [本地文檔](/reference/linux/history_of_encrypt.mht) 。


<!--
關於RSA加密算法的介紹  AES DES
http://www.ruanyifeng.com/blog/2013/06/rsa_algorithm_part_one.html
http://www.cnblogs.com/vamei/p/3480994.html
-->


{% highlight text %}
{% endhighlight %}
