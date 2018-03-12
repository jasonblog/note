---
title: libgcrypt 使用
layout: post
comments: true
language: chinese
category: [misc]
keywords: 加密算法
description:
---

libgcrypt 是一个非常成熟的加密算法库，也是著名的开源加密软件 GnuPG 的底层库，支持多种对称、非对称加密算法，以及多种 Hash 算法。

接下来，看看该库的使用方式。

<!-- more -->

## 简介

现代加密算法中，对称密钥算法通常被分为 ```Stream Ciphers``` 和 ```Block Ciphers``` 。

分组密码也被称分块密码，是一种对称密钥密码，会将明文分成多个等长的分组 (其中常见的有64 bits、128 bits、256 bits)，并用相同的密码算法和密钥对每组分别进行加密和解密，常见的有 DES、3DES、AES、IDEA、Blowfish、Twofish 。

分块密码加密以 bit 为单位，用来处理固定长度的字符串，需要加密的比特长度必须和分块大小一样长，或者是分块的整数倍，而且输入 (明文) 和输出 (密文) 都是同样长度的。

之所以输出不能比输入短，是遵循 ```Pigeonhole Principle``` (鸽巢原理) 和密码必须是可逆的事实，然而，输出比输入更长又是不可取的。

流加密算法每次对 1bit 或者 1byte 进行加密，通过无线循环的伪随机数生成器 (PRG，pseudo-random generator) 作为 key ，所以需要保证 PRG 的不可预测性，不过其生成的密钥本身可能会比要加密的数据大，常见的有 RC4 ，不过一些块加密模式 CTR、OFB 有类似流加密的功能。

## 对称加密

简单介绍下常见的对称加密算法。

### XOR

简单的异或就是对称加密最基本的操作。

{% highlight text %}
01100001 --- a
01100010 --- b
------------ XOR
00000011
{% endhighlight %}

如果把 ```a``` 称作明文，```b``` 称作密钥，```XOR``` 则为加密，得到的结果是密文。

将明文与一串等长的随机比特序列进行XOR运算，则密码被称为一次性密码。

一次性密码是由 ```G.S.Vernam``` 与 1917 年提出的，因此又称维纳密码 (Vernam Cipher)；1949 年香农 (C.E.Shannon) 通过数学方法证明一次性密码无法破解。

为什么说一次性密码无法破解？假设我们对其进行暴力破解，遍历所有可能的密钥只是时间问题，然而即使可以遍历所有密钥，你也无法判断解密出的数据哪个是正确的明文。

拿上文中的异或例子来说，遍历所有可能的 `b` 一共有 256 种可能，例如，通过暴力破解结果可能是 `00000011`、`00000010`、`00000110` 等等，那么哪个是正确的呢？

之所以一次性密码并没有被使用，主要有两个原因：A) 只用一次的密钥如何配送；B) 密钥必须与明文等长。

### RC4

Rivest Cipher 4, RC4 由美国密码学家罗纳德·李维斯特 (Ron Rivest) 在 1987 年设计，是一种流加密算法，密钥长度可变，它加解密使用相同的密钥，因此也属于对称加密算法。

RC4 是有线等效加密 (WEP) 中采用的加密算法，也曾经是 TLS 可采用的算法之一，由于 RC4 算法存在弱点，2015.02 所发布的 RFC 7465 规定禁止在 TLS 中使用 RC4 加密算法。

<!--
算法

总结起来就3步：

    通过算法生成一个256字节的S-box。
    再通过算法每次取出S-box中的某一字节K.
    将K与明文做异或得到密文。

Step1. 密钥变换算法

for i from 0 to 255
    S[i] := i
endfor
j := 0
for i from 0 to 255
    j := (j + S[i] + key[i mod keylength]) mod 256
    swap values of S[i] and S[j]
endfor

先初始化S-box，使得S[0] = 0, S[1] = 1 ... S[255] = 255。
而后再打乱S-box，这一步会引入密钥。打乱后得到一个乱序的S-box。

Step2. 伪随机算法

i := 0
j := 0
while GeneratingOutput:
    i := (i + 1) mod 256
    j := (j + S[i]) mod 256
    swap values of S[i] and S[j]
    K := S[(S[i] + S[j]) mod 256]
    output K
endwhile

通过算法取出S-box中的一位K。

Step3. 加密

将上一步取出的K与明文当前字节做异或得到密文。明文的每一字节都会重复2，3步骤。
-->

### DES

Data Encryption Standard, DES 是一种对称密钥分组加密算法，1976 年被美国联邦政府的国家标准局确定为联邦资料处理标准 (FIPS)，它基于 56 位密钥的对称算法。

DES 现在已经不再是一种安全的加密方法，主要因为它使用的 56 位密钥过短，而且在 1999.01 时，distributed.net 与电子前哨基金会合作，在 22 小时 15 分钟内即公开破解了一个 DES 密钥。

DES 是一种分组加密算法，一组长度为 64 位，不过只有 56 位被实际用于算法，其余 8 位可以被用于校验，并在算法中被丢弃，也就是说 DES 的有效密钥长度仅为 56 位。

<!--
Feistel结构

    明文数据（64bit）被分成左右两部分（各32bit）。
    将输入的右侧使用轮函数处理。
    处理后得出的数据与左侧做异或得到加密后的左侧

这个过程被称为一轮。一轮结束后交换左右数据，进行下一轮计算。DES会进行16轮计算（最后一轮不交换左右数据）。这种左右交叉处理的结构成为feistel结构。feistel结构也被应用到其它许多加密算法中。

实际上首尾各有一次置换，称为IP与FP（或称`IP^-1`，FP为IP的反函数（即IP“撤销”FP的操作，反之亦然）。IP和FP几乎没有密码学上的重要性，为了在1970年代中期的硬件上简化输入输出数据库的过程而被显式的包括在标准中。更多置换的细节见DES补充材料。
F函数

上图显示了F函数的步骤，主要包含4个步骤：

    扩张：将32位的半块扩展到48位（图中的E）。
    与密钥混合：用异或操作将扩张的结果和一个子密钥进行混合。
    S-box：在与子密钥混合之后，块被分成8个6位的块。8个S-box（图中S1-S8）的每一个都使用以查找表方式提供的非线性的变换将它的6个输入位变成4个输出位。
    置换：最后，S-box的32个输出位利用固定的置换，“P置换”进行重组。这个设计是为了将每个S-box的4位输出在下一轮的扩张后，使用4个不同的S-box进行处理。

下面具体分析每一步：

扩张

按照固定的方式，将32bit重新排列成48bit。某些位在输出中被用到了不止一次，例如输入的第5位出现在输出的第6和8位。

子密钥

子密钥生成方法被称为密钥调度。首先通过选择置换1（PC-1）从64位输入密钥中选出56位的密钥，剩下的8位要么直接丢弃，要么作为奇偶校验位。然后，56位分成两个28位的半密钥；每个半密钥接下来都被分别处理。在接下来的回次中，两个半密钥都被左移1或2位（由回次数决定），然后通过选择置换2（PC-2）产生48位的子密钥（每个半密钥24位）。

    选择置换1（PC-1）

    从原密钥中采用固定取法取出56位，如下图左半密钥第一位57代表原密钥第57位。用此方法重新组合了密钥。

    选择置换2（PC-2）

    左移

S-box

在密码学中，一个S盒（Substitution-box，置换盒）是对称密钥加密算法执行置换计算的基本结构。它们通常用于模糊密钥与密文之间的关系。S盒通常是固定的（例如DES和AES加密算法）, 也有一些加密算法的S盒是基于密钥动态生成的。

回到DES算法，DES的其中一个S-box：

给定6比特输入，将外面两个比特（第一个和最后一个比特）作为行条件，中间四个比特（inner four bits）作为列条件进行查表，最终获得4比特输出。例如，输入“011011”，通过外面两个比特“01”和中间的比特“1101”进行查表，最终的输出应该是“1001”。

P置换

P置换将32位的半块数据重新排列。
-->


### Blowfish

Blowfish 是 1993 年布鲁斯·施奈尔 (Bruce Schneier) 开发的对称密钥区块加密算法，区块长为 64 位，密钥为 1 至 448 位的可变长度，是 [Feistel Cipher](https://en.wikipedia.org/wiki/Feistel_cipher) 的一种，与 DES 等算法相比，其处理速度较快，而且可以免费使用。

![cipher ecb mode encrypt]({{ site.url }}/images/linux/cipher-feistel-diagram.png "cipher ecb mode encrypt"){: .pull-center }

<!--
## 非对称加密
https://l2x.gitbooks.io/understanding-cryptography/docs/chapter-3/rsa.html
-->


## 工作模式

前面说的 DES、AES 等分组密码，都只能加密固定长度的明文，例如 AES 输入是 128bit，而实际使用时明文长度不确定，那么此时如何加密呢？

工作模式本质上是分组密码迭代方式，如果模式选择不恰当，那么可能会带来安全隐患。我们需要寻找一种模式，至少得满足：A) 相同的明文分组加密后密文不同；B) 明文微小变化都能造成密文有很大变化。

详细可以查看 [Block cipher mode of operation](https://en.wikipedia.org/wiki/Block_cipher_mode_of_operation) 中的介绍。

### ECB 模式

也就是电子密码本模式 (Electronic CodeBook, ECB)，这是最简单的块密码加密模式，加密前根据加密块大小 (如 AES 为 128 bit) 分成若干块，之后将每块使用相同的密钥单独加密，解密同理。

![cipher ecb mode encrypt]({{ site.url }}/images/linux/cipher-ecb-mode-encrypt.png "cipher ecb mode encrypt"){: .pull-center }

![cipher ecb mode decrypt]({{ site.url }}/images/linux/cipher-ecb-mode-decrypt.png "cipher ecb mode decrypt"){: .pull-center }

ECB 模式是最简单的一种，好处是每块数据的加解密可以并行计算；但是也有很严重的问题，就是相同的明文会得到同样的密文，在某些环境下不能提供严格的数据保密性。

### CBC 模式

密码分组链接 (Cipher-Block Chaining, CBC)，由 IBM 在 1976 年发明，每个明文块先与前一个密文块进行异或后，再进行加密，其中第一个明文块需要与一个叫初始化向量的数据块异或，一般来说初始化向量采用随机值。

![cipher cbc mode encrypt]({{ site.url }}/images/linux/cipher-cbc-mode-encrypt.png "cipher cbc mode encrypt"){: .pull-center }

![cipher cbc mode decrypt]({{ site.url }}/images/linux/cipher-cbc-mode-decrypt.png "cipher cbc mode decrypt"){: .pull-center }

CBC 相比 ECB 有更高的保密性，但由于对每个数据块的加密依赖与前一个数据块的加密，所以加密无法并行。而且与 ECB 一样在加密前可能需要对数据进行填充，不适合对流数据进行加密。

### CFB 模式

密文反馈模式 (Cipher FeedBack, CFB)，前一个密文分组会被送入密码算法的输入端，再将输出的结果与明文做异或。

与 ECB 和 CBC 模式只能够加密块数据不同，CFB 能够将块密文 (Block Cipher) 转换为流密文 (Stream Cipher)。CFB 的加密工作分为两部分：A) 将一前段加密得到的密文再加密；B) 将第 A 步加密得到的数据与当前段的明文异或。

![cipher cfb mode encrypt]({{ site.url }}/images/linux/cipher-cfb-mode-encrypt.png "cipher cfb mode encrypt"){: .pull-center }

![cipher cfb mode decrypt]({{ site.url }}/images/linux/cipher-cfb-mode-decrypt.png "cipher cfb mode decrypt"){: .pull-center }

注意：CFB、OFB 和 CTR 模式中解密也都是用的加密器而非解密器。

### OFB 模式

输出反馈模式 (Output Feedback, OFB)，前一组密码算法输出会输入到下一组密码算法输入。

![cipher ofb mode encrypt]({{ site.url }}/images/linux/cipher-ofb-mode-encrypt.png "cipher ofb mode encrypt"){: .pull-center }

![cipher ofb mode decrypt]({{ site.url }}/images/linux/cipher-ofb-mode-decrypt.png "cipher ofb mode decrypt"){: .pull-center }

### CTR 模式

计数器模式 (Counter, CTR)，每个分组对应一个累加的计数器，并通过计数器来生成加密密钥流。

![cipher ctr mode encrypt]({{ site.url }}/images/linux/cipher-ctr-mode-encrypt.png "cipher ctr mode encrypt"){: .pull-center }

![cipher ctr mode decrypt]({{ site.url }}/images/linux/cipher-ctr-mode-decrypt.png "cipher ctr mode decrypt"){: .pull-center }

图中 ```Nonce+Counter``` 是一个计数器，Nonce 和前面几种模式的 IV 类似，每次加密都需要随机生成，而计数器 Counter 是累加的。CTR 模式特点是每组加密都是独立的，不依赖前一组，这就意味着在生成计数器后，每个分组可以并行计算。

### 模式选择

简单介绍下各个模式的有缺点。

{% highlight text %}
<<<<< ECB
优点
    简单
    快速
    支持并行计算（加密、解密）
缺点
    明文中的重复排列会反映在密文中
    通过删除、替换密文分组可以对明文进行操作
备注：不应使用

<<<<< CBC
优点
    明文的重复排列不会反映在密文中
    支持并行计算（解密）
缺点
    加密不支持并行计算
备注：推荐使用

<<<<< CFB
优点
    不需要填充
    支持并行计算（解密）
缺点
    加密不支持并行
备注：推荐使用CTR模式代替

<<<<< OFB
优点
    不需要填充
    可事先进行加密、解密准备
    加解密使用相同结构
缺点
    不支持并行
备注：推荐使用CTR模式代替

<<<<< CTR
优点
    不需要填充
    可事先进行加密、解密准备
    加解密使用相同结构
    支持并行计算（加密、解密）
备注：推荐使用
{% endhighlight %}





<!--
Password-Based Key Derivation Function, PBKDF2 用来导出密钥的函数用于生成加密的密码。

它的基本原理是通过一个伪随机函数（例如HMAC函数），把明文和一个盐值作为输入参数，然后重复进行运算，并最终产生密钥。

如果重复的次数足够大，破解的成本就会变得很高。而盐值的添加也会增加“彩虹表”攻击的难度。
-->

## libgcrypt 编程

简单介绍常见的编程方式，详细内容可以参考官方文档 [The Libgcrypt Reference Manual](https://www.gnupg.org/documentation/manuals/gcrypt/index.html)。

### 常用概念

#### 初始化向量

```Initialization Vector, IV``` 也就是用于开始随机化加密的一块数据，因此可以由相同的明文，相同的密钥产生不同的密文，而无需重新产生密钥。

一般来说 IV 无需保密，而且在同一密钥的情况下不要使用相同的 IV ，否则可能会导致不安全。

<!--
例如，对于CBC和CFB，重用IV会导致泄露平文首个块的某些信息，亦包括两个不同消息中相同的前缀。对于OFB和CTR而言，重用IV会导致完全失去安全性。另外，在CBC模式中，IV在加密时必须是无法预测的；特别的，在许多实现中使用的产生IV的方法，例如SSL2.0使用的，即采用上一个消息的最后一块密文作为下一个消息的IV，是不安全的[12]。
-->

#### 填充

块密码只能对指定长度的数据块进行处理，而消息的长度通常是可变的，因此部分模式 (ECB、CBC) 需要将最后一块在加密前进行填充，以满足特定长度的需求。

<!--
有数种填充方法，其中最简单的一种是在平文的最后填充空字符以使其长度为块长度的整数倍，但必须保证可以恢复平文的原始长度；例如，若平文是C语言风格的字符串，则只有串尾会有空字符。稍微复杂一点的方法则是原始的DES使用的方法，即在数据后添加一个1位，再添加足够的0位直到满足块长度的要求；若消息长度刚好符合块长度，则添加一个填充块。最复杂的则是针对CBC的方法，例如密文窃取，残块终结等，不会产生额外的密文，但会增加一些复杂度。布鲁斯·施奈尔和尼尔斯·弗格森提出了两种简单的可能性：添加一个值为128的字节（十六进制的80），再以0字节填满最后一个块；或向最后一个块填充n个值均为n的字节[13]。

CFB、OFB 和 CTR 模式不需要对长度进行处理，因为这些模式是会对加密块与明文进行异或操作，那么最后一个明文块 (可能不完整) 与加密流块异或后，产生了所需大小的密文块。

流密码的这个特性使得它们可以应用在需要密文和平文数据长度严格相等的场合，也可以应用在以流形式传输数据而不便于进行填充的场合。
-->

### 对称加密

也就是直接使用 libgcrypt 对称加密。

#### 1. 传入密钥

一般不会直接使用用户的输入直接作为密钥，而是通过一个密钥导出函数 (如PBKDF2) 生成，入参中有两个比较重要的函数：A) 输入密码，用户输入；B) 初始化向量，程序提供。

{% highlight text %}
gpg_error_t gcry_kdf_derive ( const void *passphrase, size_t passphraselen,
        int algo, int subalgo, const void *salt, size_t saltlen,
        unsigned long iterations, size_t keysize, void *keybuffer );
参数主要包括四部分：
  1. passphrase, passphraselen  传入的密钥明文和长度
  2. algo, subalgo, iterations 使用的 Key Derivation Function (KDF) 算法，及其迭代次数
  3. salt, saltlen 加盐的盐串和长度
  4. keysize, keybuffer 用于保存生成密钥的缓存长度，以及返回密钥内容
{% endhighlight %}

#### 2. 初始化加密句柄

在获得密钥之后，就需要对加密的句柄进行设置，在后续的所有操作中都会使用该句柄操作。

接着，需要选定加密算法，以及加密模式，在 libgcrypt 中，加密算法用宏来标识，需要传递指定的宏，来告知它想用哪种加密算法。

{% highlight text %}
size_t gcry_cipher_get_algo_keylen (int algo);
size_t gcry_cipher_get_algo_blklen (int algo);
gcry_error_t gcry_cipher_open (gcry_cipher_hd_t *hd, int algo, int mode, unsigned int flags);
gcry_error_t gcry_cipher_setkey (gcry_cipher_hd_t h, const void *k, size_t l);
gcry_error_t gcry_cipher_setiv (gcry_cipher_hd_t h, const void *k, size_t l);
{% endhighlight %}

这里需要注意的是，初始化向量一般是一个 Block Size，而后续准备加密的数据需要保证是 Block Size 的整数倍，否则会报错。

#### 3. 加解密

接着就是通过如下函数加解密了。

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

列举常见的示例程序，对称加解密 [symmetric.c]( {{ site.example_repository }}/cipher/libgcrypt/symmetric.c )、哈希函数 [hash.c]( {{ site.example_repository }}/cipher/libgcrypt/hash.c )、以及密钥导出函数 [pbkdf.c]( {{ site.example_repository }}/cipher/libgcrypt/pbkdf.c ) 。


## 其它

### HMAC

HMAC 是密钥相关的哈希运算消息认证码，HMAC 运算时利用哈希算法，以一个密钥和一个消息为输入，生成一个消息摘要作为输出。

HMAC 的一个典型应用是用在 ```Challenge/Response``` 身份认证中，一般的处理流程如下：

1. 客户端向服务器发出一个验证请求。
2. 服务器接到此请求后生成一个随机数并通过网络传输给客户端 (Challenge)。
3. 客户端将收到的随机数与客户保存的密码做 HMAC-MD5 计算，并将结果作为认证证据传给服务器 (Response)。
4. 服务器同样执行 HMSC-MD5 运算，与客户端传回的响应结果比较，如果相同则认为客户端是一个合法用户。

## 参考

对各种密码保存方案的评估，例如安全性，可以直接参考 [On The Security of Password Manager Database Formats](https://www.cs.ox.ac.uk/files/6487/pwvault.pdf) 。

<!--
Password Safe库信息
https://pwsafe.org/readmore.shtml
从FireFox获取密码
http://www.nirsoft.net/utils/passwordfox.html
密码保存策略
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



调用链判断
touch /tmp/{top.sh,mid.sh,bot.sh}
echo '/tmp/top.sh |/tmp/mid.sh|/tmp/bot.sh' > script_chain
fpge script_chain  生成指纹，脚本会使用绝对路径，指纹库保存在 /usr/local/var/.bfbase 文件中
   md5sum /tmp/top.sh | awk '{print $1}'    -> 'xxxxx'
   echo -n "xxxxx" | md5sum | awk '{print $1}' -> 'yyyyy'
   md5sum /tmp/top.sh | awk '{print $1}'    -> 'zzzzz'
   echo -n "yyyyy" "zzzzz" | md5sum | awk '{print $1}'
   最终调用 password -i -fp "111111111111111"
   password -i -fp footprint 经过一系列HASH算法计算，保存到指纹文件中
password -e "test" 加密密码，其中 -k 指定的key为数组动态选择
   echo 'test'|openssl aes-256-cbc -e -k U2FsdGVkX1+AVVVxxWOuFFCBEU7jwC4dbksnF0/Wz44 -base64
   生成密文： 222222222222
password -d "222222222222" -fp "111111111111111"
   首先会验证传入的指纹与文件中保存的是否相同
   echo '222222222222'|openssl aes-256-cbc -d -k U2FsdGVkX1+AVVVxxWOuFFCBEU7jwC4dbksnF0/Wz44 -base64
password -z 删除指纹库，直接rm删除

脚本中使用方式如下：top.sh添加
export __FPRINT__=""
__dig__=`md5sum $0|awk '{print $1}'`
__FPRINT__=`echo -n "$__FPRINT__""$__dig__"|md5sum|awk '{print $1}'`
mid.sh和bot.sh中添加
__dig__=`md5sum $0|awk '{print $1}'`
__FPRINT__=`echo -n "$__FPRINT__""$__dig__"|md5sum|awk '{print $1}'`
解密使用
password -d "口令密文" -fp "$__FPRINT__"


从栈中获取密码
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
