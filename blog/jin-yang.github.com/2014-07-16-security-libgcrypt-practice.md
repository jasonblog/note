---
title: libgcrypt 使用
layout: post
comments: true
language: chinese
category: [misc]
keywords: 加密算法
description:
---

libgcrypt 是一個非常成熟的加密算法庫，也是著名的開源加密軟件 GnuPG 的底層庫，支持多種對稱、非對稱加密算法，以及多種 Hash 算法。

接下來，看看該庫的使用方式。

<!-- more -->

## 簡介

現代加密算法中，對稱密鑰算法通常被分為 ```Stream Ciphers``` 和 ```Block Ciphers``` 。

分組密碼也被稱分塊密碼，是一種對稱密鑰密碼，會將明文分成多個等長的分組 (其中常見的有64 bits、128 bits、256 bits)，並用相同的密碼算法和密鑰對每組分別進行加密和解密，常見的有 DES、3DES、AES、IDEA、Blowfish、Twofish 。

分塊密碼加密以 bit 為單位，用來處理固定長度的字符串，需要加密的比特長度必須和分塊大小一樣長，或者是分塊的整數倍，而且輸入 (明文) 和輸出 (密文) 都是同樣長度的。

之所以輸出不能比輸入短，是遵循 ```Pigeonhole Principle``` (鴿巢原理) 和密碼必須是可逆的事實，然而，輸出比輸入更長又是不可取的。

流加密算法每次對 1bit 或者 1byte 進行加密，通過無線循環的偽隨機數生成器 (PRG，pseudo-random generator) 作為 key ，所以需要保證 PRG 的不可預測性，不過其生成的密鑰本身可能會比要加密的數據大，常見的有 RC4 ，不過一些塊加密模式 CTR、OFB 有類似流加密的功能。

## 對稱加密

簡單介紹下常見的對稱加密算法。

### XOR

簡單的異或就是對稱加密最基本的操作。

{% highlight text %}
01100001 --- a
01100010 --- b
------------ XOR
00000011
{% endhighlight %}

如果把 ```a``` 稱作明文，```b``` 稱作密鑰，```XOR``` 則為加密，得到的結果是密文。

將明文與一串等長的隨機比特序列進行XOR運算，則密碼被稱為一次性密碼。

一次性密碼是由 ```G.S.Vernam``` 與 1917 年提出的，因此又稱維納密碼 (Vernam Cipher)；1949 年香農 (C.E.Shannon) 通過數學方法證明一次性密碼無法破解。

為什麼說一次性密碼無法破解？假設我們對其進行暴力破解，遍歷所有可能的密鑰只是時間問題，然而即使可以遍歷所有密鑰，你也無法判斷解密出的數據哪個是正確的明文。

拿上文中的異或例子來說，遍歷所有可能的 `b` 一共有 256 種可能，例如，通過暴力破解結果可能是 `00000011`、`00000010`、`00000110` 等等，那麼哪個是正確的呢？

之所以一次性密碼並沒有被使用，主要有兩個原因：A) 只用一次的密鑰如何配送；B) 密鑰必須與明文等長。

### RC4

Rivest Cipher 4, RC4 由美國密碼學家羅納德·李維斯特 (Ron Rivest) 在 1987 年設計，是一種流加密算法，密鑰長度可變，它加解密使用相同的密鑰，因此也屬於對稱加密算法。

RC4 是有線等效加密 (WEP) 中採用的加密算法，也曾經是 TLS 可採用的算法之一，由於 RC4 算法存在弱點，2015.02 所發佈的 RFC 7465 規定禁止在 TLS 中使用 RC4 加密算法。

<!--
算法

總結起來就3步：

    通過算法生成一個256字節的S-box。
    再通過算法每次取出S-box中的某一字節K.
    將K與明文做異或得到密文。

Step1. 密鑰變換算法

for i from 0 to 255
    S[i] := i
endfor
j := 0
for i from 0 to 255
    j := (j + S[i] + key[i mod keylength]) mod 256
    swap values of S[i] and S[j]
endfor

先初始化S-box，使得S[0] = 0, S[1] = 1 ... S[255] = 255。
而後再打亂S-box，這一步會引入密鑰。打亂後得到一個亂序的S-box。

Step2. 偽隨機算法

i := 0
j := 0
while GeneratingOutput:
    i := (i + 1) mod 256
    j := (j + S[i]) mod 256
    swap values of S[i] and S[j]
    K := S[(S[i] + S[j]) mod 256]
    output K
endwhile

通過算法取出S-box中的一位K。

Step3. 加密

將上一步取出的K與明文當前字節做異或得到密文。明文的每一字節都會重複2，3步驟。
-->

### DES

Data Encryption Standard, DES 是一種對稱密鑰分組加密算法，1976 年被美國聯邦政府的國家標準局確定為聯邦資料處理標準 (FIPS)，它基於 56 位密鑰的對稱算法。

DES 現在已經不再是一種安全的加密方法，主要因為它使用的 56 位密鑰過短，而且在 1999.01 時，distributed.net 與電子前哨基金會合作，在 22 小時 15 分鐘內即公開破解了一個 DES 密鑰。

DES 是一種分組加密算法，一組長度為 64 位，不過只有 56 位被實際用於算法，其餘 8 位可以被用於校驗，並在算法中被丟棄，也就是說 DES 的有效密鑰長度僅為 56 位。

<!--
Feistel結構

    明文數據（64bit）被分成左右兩部分（各32bit）。
    將輸入的右側使用輪函數處理。
    處理後得出的數據與左側做異或得到加密後的左側

這個過程被稱為一輪。一輪結束後交換左右數據，進行下一輪計算。DES會進行16輪計算（最後一輪不交換左右數據）。這種左右交叉處理的結構成為feistel結構。feistel結構也被應用到其它許多加密算法中。

實際上首尾各有一次置換，稱為IP與FP（或稱`IP^-1`，FP為IP的反函數（即IP“撤銷”FP的操作，反之亦然）。IP和FP幾乎沒有密碼學上的重要性，為了在1970年代中期的硬件上簡化輸入輸出數據庫的過程而被顯式的包括在標準中。更多置換的細節見DES補充材料。
F函數

上圖顯示了F函數的步驟，主要包含4個步驟：

    擴張：將32位的半塊擴展到48位（圖中的E）。
    與密鑰混合：用異或操作將擴張的結果和一個子密鑰進行混合。
    S-box：在與子密鑰混合之後，塊被分成8個6位的塊。8個S-box（圖中S1-S8）的每一個都使用以查找表方式提供的非線性的變換將它的6個輸入位變成4個輸出位。
    置換：最後，S-box的32個輸出位利用固定的置換，“P置換”進行重組。這個設計是為了將每個S-box的4位輸出在下一輪的擴張後，使用4個不同的S-box進行處理。

下面具體分析每一步：

擴張

按照固定的方式，將32bit重新排列成48bit。某些位在輸出中被用到了不止一次，例如輸入的第5位出現在輸出的第6和8位。

子密鑰

子密鑰生成方法被稱為密鑰調度。首先通過選擇置換1（PC-1）從64位輸入密鑰中選出56位的密鑰，剩下的8位要麼直接丟棄，要麼作為奇偶校驗位。然後，56位分成兩個28位的半密鑰；每個半密鑰接下來都被分別處理。在接下來的回次中，兩個半密鑰都被左移1或2位（由回次數決定），然後通過選擇置換2（PC-2）產生48位的子密鑰（每個半密鑰24位）。

    選擇置換1（PC-1）

    從原密鑰中採用固定取法取出56位，如下圖左半密鑰第一位57代表原密鑰第57位。用此方法重新組合了密鑰。

    選擇置換2（PC-2）

    左移

S-box

在密碼學中，一個S盒（Substitution-box，置換盒）是對稱密鑰加密算法執行置換計算的基本結構。它們通常用於模糊密鑰與密文之間的關係。S盒通常是固定的（例如DES和AES加密算法）, 也有一些加密算法的S盒是基於密鑰動態生成的。

回到DES算法，DES的其中一個S-box：

給定6比特輸入，將外面兩個比特（第一個和最後一個比特）作為行條件，中間四個比特（inner four bits）作為列條件進行查表，最終獲得4比特輸出。例如，輸入“011011”，通過外面兩個比特“01”和中間的比特“1101”進行查表，最終的輸出應該是“1001”。

P置換

P置換將32位的半塊數據重新排列。
-->


### Blowfish

Blowfish 是 1993 年布魯斯·施奈爾 (Bruce Schneier) 開發的對稱密鑰區塊加密算法，區塊長為 64 位，密鑰為 1 至 448 位的可變長度，是 [Feistel Cipher](https://en.wikipedia.org/wiki/Feistel_cipher) 的一種，與 DES 等算法相比，其處理速度較快，而且可以免費使用。

![cipher ecb mode encrypt]({{ site.url }}/images/linux/cipher-feistel-diagram.png "cipher ecb mode encrypt"){: .pull-center }

<!--
## 非對稱加密
https://l2x.gitbooks.io/understanding-cryptography/docs/chapter-3/rsa.html
-->


## 工作模式

前面說的 DES、AES 等分組密碼，都只能加密固定長度的明文，例如 AES 輸入是 128bit，而實際使用時明文長度不確定，那麼此時如何加密呢？

工作模式本質上是分組密碼迭代方式，如果模式選擇不恰當，那麼可能會帶來安全隱患。我們需要尋找一種模式，至少得滿足：A) 相同的明文分組加密後密文不同；B) 明文微小變化都能造成密文有很大變化。

詳細可以查看 [Block cipher mode of operation](https://en.wikipedia.org/wiki/Block_cipher_mode_of_operation) 中的介紹。

### ECB 模式

也就是電子密碼本模式 (Electronic CodeBook, ECB)，這是最簡單的塊密碼加密模式，加密前根據加密塊大小 (如 AES 為 128 bit) 分成若干塊，之後將每塊使用相同的密鑰單獨加密，解密同理。

![cipher ecb mode encrypt]({{ site.url }}/images/linux/cipher-ecb-mode-encrypt.png "cipher ecb mode encrypt"){: .pull-center }

![cipher ecb mode decrypt]({{ site.url }}/images/linux/cipher-ecb-mode-decrypt.png "cipher ecb mode decrypt"){: .pull-center }

ECB 模式是最簡單的一種，好處是每塊數據的加解密可以並行計算；但是也有很嚴重的問題，就是相同的明文會得到同樣的密文，在某些環境下不能提供嚴格的數據保密性。

### CBC 模式

密碼分組鏈接 (Cipher-Block Chaining, CBC)，由 IBM 在 1976 年發明，每個明文塊先與前一個密文塊進行異或後，再進行加密，其中第一個明文塊需要與一個叫初始化向量的數據塊異或，一般來說初始化向量採用隨機值。

![cipher cbc mode encrypt]({{ site.url }}/images/linux/cipher-cbc-mode-encrypt.png "cipher cbc mode encrypt"){: .pull-center }

![cipher cbc mode decrypt]({{ site.url }}/images/linux/cipher-cbc-mode-decrypt.png "cipher cbc mode decrypt"){: .pull-center }

CBC 相比 ECB 有更高的保密性，但由於對每個數據塊的加密依賴與前一個數據塊的加密，所以加密無法並行。而且與 ECB 一樣在加密前可能需要對數據進行填充，不適合對流數據進行加密。

### CFB 模式

密文反饋模式 (Cipher FeedBack, CFB)，前一個密文分組會被送入密碼算法的輸入端，再將輸出的結果與明文做異或。

與 ECB 和 CBC 模式只能夠加密塊數據不同，CFB 能夠將塊密文 (Block Cipher) 轉換為流密文 (Stream Cipher)。CFB 的加密工作分為兩部分：A) 將一前段加密得到的密文再加密；B) 將第 A 步加密得到的數據與當前段的明文異或。

![cipher cfb mode encrypt]({{ site.url }}/images/linux/cipher-cfb-mode-encrypt.png "cipher cfb mode encrypt"){: .pull-center }

![cipher cfb mode decrypt]({{ site.url }}/images/linux/cipher-cfb-mode-decrypt.png "cipher cfb mode decrypt"){: .pull-center }

注意：CFB、OFB 和 CTR 模式中解密也都是用的加密器而非解密器。

### OFB 模式

輸出反饋模式 (Output Feedback, OFB)，前一組密碼算法輸出會輸入到下一組密碼算法輸入。

![cipher ofb mode encrypt]({{ site.url }}/images/linux/cipher-ofb-mode-encrypt.png "cipher ofb mode encrypt"){: .pull-center }

![cipher ofb mode decrypt]({{ site.url }}/images/linux/cipher-ofb-mode-decrypt.png "cipher ofb mode decrypt"){: .pull-center }

### CTR 模式

計數器模式 (Counter, CTR)，每個分組對應一個累加的計數器，並通過計數器來生成加密密鑰流。

![cipher ctr mode encrypt]({{ site.url }}/images/linux/cipher-ctr-mode-encrypt.png "cipher ctr mode encrypt"){: .pull-center }

![cipher ctr mode decrypt]({{ site.url }}/images/linux/cipher-ctr-mode-decrypt.png "cipher ctr mode decrypt"){: .pull-center }

圖中 ```Nonce+Counter``` 是一個計數器，Nonce 和前面幾種模式的 IV 類似，每次加密都需要隨機生成，而計數器 Counter 是累加的。CTR 模式特點是每組加密都是獨立的，不依賴前一組，這就意味著在生成計數器後，每個分組可以並行計算。

### 模式選擇

簡單介紹下各個模式的有缺點。

{% highlight text %}
<<<<< ECB
優點
    簡單
    快速
    支持並行計算（加密、解密）
缺點
    明文中的重複排列會反映在密文中
    通過刪除、替換密文分組可以對明文進行操作
備註：不應使用

<<<<< CBC
優點
    明文的重複排列不會反映在密文中
    支持並行計算（解密）
缺點
    加密不支持並行計算
備註：推薦使用

<<<<< CFB
優點
    不需要填充
    支持並行計算（解密）
缺點
    加密不支持並行
備註：推薦使用CTR模式代替

<<<<< OFB
優點
    不需要填充
    可事先進行加密、解密準備
    加解密使用相同結構
缺點
    不支持並行
備註：推薦使用CTR模式代替

<<<<< CTR
優點
    不需要填充
    可事先進行加密、解密準備
    加解密使用相同結構
    支持並行計算（加密、解密）
備註：推薦使用
{% endhighlight %}





<!--
Password-Based Key Derivation Function, PBKDF2 用來導出密鑰的函數用於生成加密的密碼。

它的基本原理是通過一個偽隨機函數（例如HMAC函數），把明文和一個鹽值作為輸入參數，然後重複進行運算，並最終產生密鑰。

如果重複的次數足夠大，破解的成本就會變得很高。而鹽值的添加也會增加“彩虹表”攻擊的難度。
-->

## libgcrypt 編程

簡單介紹常見的編程方式，詳細內容可以參考官方文檔 [The Libgcrypt Reference Manual](https://www.gnupg.org/documentation/manuals/gcrypt/index.html)。

### 常用概念

#### 初始化向量

```Initialization Vector, IV``` 也就是用於開始隨機化加密的一塊數據，因此可以由相同的明文，相同的密鑰產生不同的密文，而無需重新產生密鑰。

一般來說 IV 無需保密，而且在同一密鑰的情況下不要使用相同的 IV ，否則可能會導致不安全。

<!--
例如，對於CBC和CFB，重用IV會導致洩露平文首個塊的某些信息，亦包括兩個不同消息中相同的前綴。對於OFB和CTR而言，重用IV會導致完全失去安全性。另外，在CBC模式中，IV在加密時必須是無法預測的；特別的，在許多實現中使用的產生IV的方法，例如SSL2.0使用的，即採用上一個消息的最後一塊密文作為下一個消息的IV，是不安全的[12]。
-->

#### 填充

塊密碼只能對指定長度的數據塊進行處理，而消息的長度通常是可變的，因此部分模式 (ECB、CBC) 需要將最後一塊在加密前進行填充，以滿足特定長度的需求。

<!--
有數種填充方法，其中最簡單的一種是在平文的最後填充空字符以使其長度為塊長度的整數倍，但必須保證可以恢復平文的原始長度；例如，若平文是C語言風格的字符串，則只有串尾會有空字符。稍微複雜一點的方法則是原始的DES使用的方法，即在數據後添加一個1位，再添加足夠的0位直到滿足塊長度的要求；若消息長度剛好符合塊長度，則添加一個填充塊。最複雜的則是針對CBC的方法，例如密文竊取，殘塊終結等，不會產生額外的密文，但會增加一些複雜度。布魯斯·施奈爾和尼爾斯·弗格森提出了兩種簡單的可能性：添加一個值為128的字節（十六進制的80），再以0字節填滿最後一個塊；或向最後一個塊填充n個值均為n的字節[13]。

CFB、OFB 和 CTR 模式不需要對長度進行處理，因為這些模式是會對加密塊與明文進行異或操作，那麼最後一個明文塊 (可能不完整) 與加密流塊異或後，產生了所需大小的密文塊。

流密碼的這個特性使得它們可以應用在需要密文和平文數據長度嚴格相等的場合，也可以應用在以流形式傳輸數據而不便於進行填充的場合。
-->

### 對稱加密

也就是直接使用 libgcrypt 對稱加密。

#### 1. 傳入密鑰

一般不會直接使用用戶的輸入直接作為密鑰，而是通過一個密鑰導出函數 (如PBKDF2) 生成，入參中有兩個比較重要的函數：A) 輸入密碼，用戶輸入；B) 初始化向量，程序提供。

{% highlight text %}
gpg_error_t gcry_kdf_derive ( const void *passphrase, size_t passphraselen,
        int algo, int subalgo, const void *salt, size_t saltlen,
        unsigned long iterations, size_t keysize, void *keybuffer );
參數主要包括四部分：
  1. passphrase, passphraselen  傳入的密鑰明文和長度
  2. algo, subalgo, iterations 使用的 Key Derivation Function (KDF) 算法，及其迭代次數
  3. salt, saltlen 加鹽的鹽串和長度
  4. keysize, keybuffer 用於保存生成密鑰的緩存長度，以及返回密鑰內容
{% endhighlight %}

#### 2. 初始化加密句柄

在獲得密鑰之後，就需要對加密的句柄進行設置，在後續的所有操作中都會使用該句柄操作。

接著，需要選定加密算法，以及加密模式，在 libgcrypt 中，加密算法用宏來標識，需要傳遞指定的宏，來告知它想用哪種加密算法。

{% highlight text %}
size_t gcry_cipher_get_algo_keylen (int algo);
size_t gcry_cipher_get_algo_blklen (int algo);
gcry_error_t gcry_cipher_open (gcry_cipher_hd_t *hd, int algo, int mode, unsigned int flags);
gcry_error_t gcry_cipher_setkey (gcry_cipher_hd_t h, const void *k, size_t l);
gcry_error_t gcry_cipher_setiv (gcry_cipher_hd_t h, const void *k, size_t l);
{% endhighlight %}

這裡需要注意的是，初始化向量一般是一個 Block Size，而後續準備加密的數據需要保證是 Block Size 的整數倍，否則會報錯。

#### 3. 加解密

接著就是通過如下函數加解密了。

{% highlight text %}
gcry_error_t gcry_cipher_encrypt (gcry_cipher_hd_t h, unsigned char *out, size_t outsize,
             const unsigned char *in, size_t inlen)
gcry_error_t gcry_cipher_decrypt (gcry_cipher_hd_t h, unsigned char *out, size_t outsize,
             const unsigned char *in, size_t inlen)
{% endhighlight %}


<!--
PBKDF2算法
https://segmentfault.com/a/1190000004261009
libgcrypt示例程序
https://github.com/linmx0130/gcrypt_demo
http://blog.csdn.net/weiyuefei/article/details/71480140

In general a secure memory is locked with mlock so it can't be paged out.
-->

### 示例程序

列舉常見的示例程序，對稱加解密 [symmetric.c]( {{ site.example_repository }}/cipher/libgcrypt/symmetric.c )、哈希函數 [hash.c]( {{ site.example_repository }}/cipher/libgcrypt/hash.c )、以及密鑰導出函數 [pbkdf.c]( {{ site.example_repository }}/cipher/libgcrypt/pbkdf.c ) 。


## 其它

### HMAC

HMAC 是密鑰相關的哈希運算消息認證碼，HMAC 運算時利用哈希算法，以一個密鑰和一個消息為輸入，生成一個消息摘要作為輸出。

HMAC 的一個典型應用是用在 ```Challenge/Response``` 身份認證中，一般的處理流程如下：

1. 客戶端向服務器發出一個驗證請求。
2. 服務器接到此請求後生成一個隨機數並通過網絡傳輸給客戶端 (Challenge)。
3. 客戶端將收到的隨機數與客戶保存的密碼做 HMAC-MD5 計算，並將結果作為認證證據傳給服務器 (Response)。
4. 服務器同樣執行 HMSC-MD5 運算，與客戶端傳回的響應結果比較，如果相同則認為客戶端是一個合法用戶。

## 參考

對各種密碼保存方案的評估，例如安全性，可以直接參考 [On The Security of Password Manager Database Formats](https://www.cs.ox.ac.uk/files/6487/pwvault.pdf) 。

<!--
Password Safe庫信息
https://pwsafe.org/readmore.shtml
從FireFox獲取密碼
http://www.nirsoft.net/utils/passwordfox.html
密碼保存策略
https://nakedsecurity.sophos.com/2013/11/20/serious-security-how-to-store-your-users-passwords-safely/
Twofish
https://www.schneier.com/academic/twofish/

AES-128-CBC
OpenSSL (http://www.openssl.org/)  Apache 109.93MB/s
Crypto++ (http://www.cryptopp.com/) Boost Software License 1.0 188.92~222MB/s
Cryptlib (http://www.cs.auckland.ac.nz/~pgut001/cryptlib/) GPL-compatible 192.57MB/s
Botan (http://botan.randombit.net/) BSD-2 License 164.299MB/s
libgcrypt (http://www.gnu.org/software/libgcrypt/) GPLv2 100MB/s
CyaSSL (https://www.wolfssl.com/wolfSSL/Home.html)  GPLv2 178.97MB/s
MatrixSSL (http://www.matrixssl.org/)  GPLv2
axTLS (http://sourceforge.net/projects/axtls/) BSD License

http://china.safenet-inc.com/webback/UploadFile/DownloadDoc/416b8c01-c42f-4251-a68d-16eb3e192ec1.pdf



調用鏈判斷
touch /tmp/{top.sh,mid.sh,bot.sh}
echo '/tmp/top.sh |/tmp/mid.sh|/tmp/bot.sh' > script_chain
fpge script_chain  生成指紋，腳本會使用絕對路徑，指紋庫保存在 /usr/local/var/.bfbase 文件中
   md5sum /tmp/top.sh | awk '{print $1}'    -> 'xxxxx'
   echo -n "xxxxx" | md5sum | awk '{print $1}' -> 'yyyyy'
   md5sum /tmp/top.sh | awk '{print $1}'    -> 'zzzzz'
   echo -n "yyyyy" "zzzzz" | md5sum | awk '{print $1}'
   最終調用 password -i -fp "111111111111111"
   password -i -fp footprint 經過一系列HASH算法計算，保存到指紋文件中
password -e "test" 加密密碼，其中 -k 指定的key為數組動態選擇
   echo 'test'|openssl aes-256-cbc -e -k U2FsdGVkX1+AVVVxxWOuFFCBEU7jwC4dbksnF0/Wz44 -base64
   生成密文： 222222222222
password -d "222222222222" -fp "111111111111111"
   首先會驗證傳入的指紋與文件中保存的是否相同
   echo '222222222222'|openssl aes-256-cbc -d -k U2FsdGVkX1+AVVVxxWOuFFCBEU7jwC4dbksnF0/Wz44 -base64
password -z 刪除指紋庫，直接rm刪除

腳本中使用方式如下：top.sh添加
export __FPRINT__=""
__dig__=`md5sum $0|awk '{print $1}'`
__FPRINT__=`echo -n "$__FPRINT__""$__dig__"|md5sum|awk '{print $1}'`
mid.sh和bot.sh中添加
__dig__=`md5sum $0|awk '{print $1}'`
__FPRINT__=`echo -n "$__FPRINT__""$__dig__"|md5sum|awk '{print $1}'`
解密使用
password -d "口令密文" -fp "$__FPRINT__"


從棧中獲取密碼
$ read -s -p "password: " PASSWD; echo
password:
$ echo $PASSWD
foobartest
$ ls -al /proc/self/environ
-r-------- 1 michael michael 0 May  6 14:46 /proc/self/environ
$ grep PASSWD /proc/self/environ
$ export PASSWD
$ strings /proc/self/environ | grep PASSWD
PASSWD=foobartest

$ echo $$
19613
$ gdb -p 19613
(gdb) info proc mappings
     0x91f2000  0x9540000   0x34e000          0           [heap]
(gdb) dump memory /tmp/bash.mem 0x91f2000 0x9540000
$ strings /tmp/bash.mem |grep ^PASSWD
PASSWD=soopersekrit
-->

{% highlight text %}
{% endhighlight %}
