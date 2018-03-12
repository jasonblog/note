---
title: 加密算法简介
layout: post
comments: true
language: chinese
category: [misc]
keywords: 加密算法
description:
---

开放网络中数据传输的安全性一直是一个热门的话题，特别是电子商务兴起的今天，各种攻击欺骗手段更是层出不穷，安全的网络传输需求愈显重要。

在此简单介绍一些常见的加密算法等。

<!-- more -->

## 对称加密

也就是 symmetric-key encryption，其中常见的算法包括了 AES、DES、3DES 等。

对称加密指的是可以使用同一个密钥对内容进行加密和解密，相比非对称加密，它的特点是加/解密速度快，并且加密的内容长度几乎没有限制。

![encrypt symmetric key]({{ site.url }}/images/linux/encrypt-symmetric-key.gif "encrypt symmetric key"){: .pull-center }

DES 是一种分组数据加密技术，也即先将数据分成固定长度的小数据块，然后进行加密，速度较快，适用于大量数据加密。3DES 是一种基于 DES 的加密算法，使用 3 个不同密匙对同一个分组数据块进行 3 次加密，从而使得密文强度更高。

相较于 DES/3DES 而言，AES 算法有着更高的速度和资源使用效率，安全级别也较之更高了，被称为下一代加密标准。


## 非对称加密/公钥加密

也就是 asymmetric/public-key encryption，常见的加密算法有 RSA、DSA、ECC 等。

非对称加密有两个密钥，分别为公钥和私钥，其中公钥公开给所有人，私钥永远只能自己知道。

![encrypt asymmetric key]({{ site.url }}/images/linux/encrypt-asymmetric-encryption.gif "encrypt asymmetric key"){: .pull-center }

使用公钥加密的信息只能使用私钥解密，使用私钥加密只能使用公钥解密。前者用来传输需要保密的信息，因为全世界只有知道对应私钥的人才可以解密；后者用来作数字签名，因为公钥对所有人公开的，可以用来确认这个信息是否是从私钥的拥有者发出的。

RSA 和 DSA 的安全性及其它各方面性能相似，而 ECC 较之则有很多性能优越，包括处理速度、带宽要求、存储空间等。

PS. 理论上来说，无法通过公钥算出私钥，或者说以现在的计算能力需要几亿万年才能算出来。

## 安全散列算法

也就是 Secure Hash Algorithm，常见的算法包括了 MD5、SHA1、HMAC 等。

将任意长度的二进制值映射为较短的固定长度的二进制值，这个短的二进制值称为哈希值，这个算法具有不可逆、碰撞低等特性。同时该类算法可以用作数字签名，用来证实某个信息确实是由某个人发出的，同时可以保证信息没有被修改。

实际上，简单来说，这种算法有两个特性：A) 不同的输入一定得出不同的 hash 值；B) 无法从 hash 值倒推出原来的输入。

![encrypt digital signature]({{ site.url }}/images/linux/encrypt-digital-signature.jpg "encrypt digital signature"){: .pull-center width="70%" }

数字签名通常先使用一个 Hash 函数或者消息摘要算法，获得可以唯一对应原信息的摘要信息，然后通过私钥加密。这样其它人就可以通过公钥解密摘要信息，然后使用同样的算法对接收到的信息计算摘要，如果两者获得的摘要信息相同即可。

在数据传输前，使用 MD5 和 SHA1 算法需要发送和接收方在数据传送之前就知道密匙生成算法，而 HMAC 需要生成一个密匙，发送方用此密匙对数据进行摘要处理，接收方再利用此密匙对接收到的数据进行摘要处理，再判断生成的密文是否相同。

### 简介

MD5 的全称是 ```Message-Digest Algorithm 5``` (信息-摘要算法)，128 位长度，目前 MD5 是一种不可逆算法，具有很高的安全性，它对应任何字符串都可以加密成一段唯一的固定长度的代码。

SHA1 的全称是 ```Secure Hash Algorithm``` (安全哈希算法) ，SHA1 基于 MD5，加密后的数据长度更长，它对长度小于 264 的输入，产生长度为 160bit 的散列值。

另外，Python 内置的 hashlib 模块就包括了 md5 和 sha1 算法，而且使用起来也极为方便。

## 其它

简单介绍一下一些常见的使用内容。

### 选择密钥长度

如果在选定了加密算法后，那采用多少位的密钥呢？

一般来说，密钥越长，运行的速度就越慢，应该根据的我们实际需要的安全级别来选择，一般来说，RSA 建议采用 1024 位的数字，ECC 建议采用 160 位，AES 采用 128 位即可。

常见的加密算法速度可以参考如下。

![encrypt truecrypt speedtest]({{ site.url }}/images/linux/encrypt-truecrypt-speedtest.png "encrypt truecrypt speedtest"){: .pull-center }

如果只是加密一段信息，也不是经常需要，那可以用最强的加密算法，甚至也可以用不同的算法加密两次。如果想要加密速度快点，那就用 AES 。

### RADIX64 BASE64

PGP 和 S/MIME 均使用了 Radix-64 编码，而该编码实际是基于 Base64。由两部分构成，分别是 Base64 编码后的数据和一个 CRC(24-bits) 校验和。

Base64 是使用 64 个通用的可打印字符来存储和表示二进制字数据，同时也可以进行简单的加密。Base64 字符集包括 ```"A-Z"```、```"a-z"```、```"0-9"```、```"+"```、```"/"``` 以及后缀填充 ```"="``` 共计 65 个字符。

Base64 实质上是以 6bits 为单位，并将其影射为规定的字符集。通常，数据以字节为单位的存储，也即 8bits，而 Base64 使用 6bits 表示一个字符，这就导致了计算机中的三个字节 (8bits\*3) 在经过 Base64 编码后将变成四个字符 (6bits\*4)，转码后比原数据长大约三分之一。

转码时只需使用一个 24bits 的缓冲区，依次从原数据中读入三个字节的数据，再每次从缓冲中取 6bits 转化成 Base64 字符输出。因此，如果是 3bytes 的整数倍，则不需要 "=" 填充，生于两字节添加一个 "="，剩余一个字节添加 "==" 。

其中，rfc2045 定义了 MIME 中的 Base64 传输编码；而 rfc4880 协议则定义了 OpenPGP 中使用的 Radix-64 编码。

## 文件完整性认证

通常为了防止文件的内容被修改，如一个文件号称是包含了补丁的文件，却被人加入了木马；同时也可以用来保证文件的完整性，防止文件被损坏。

当然，一些 hash 算法可能会被破解，所以一些网站则提供了多种完整性校验方法。

### MD5

计算出下载文件的 MD5 哈希值，然后与发布者提供的 MD5 哈希值比较，通过判断是否一致来断定文件在发布者发布之后有没有被篡改过。

使用的假设是，同一个文件产生的 MD5 哈希值是唯一的，但这点已经有办法通过对文件进行少量的修改，让文件的 MD5 后的哈希值保持一致，虽然操作起来有点难，但是确实有风险。

{% highlight text %}
$ md5sum file-name                              # 计算MD5哈希值
da9741366673b5066e74f5307c7d08d7  file-name
$ md5sum file-name > file-name.md5              # 将计算的哈希值保存到文件，可含多个
$ md5sum -c file-name.md5                       # 校验文件是否被修改
file-name: OK                                   # 失败为 file-name: FAILED ...
{% endhighlight %}


### SHA

原理同 MD5 一样，相比 MD5 来说更安全一些，而且在在 HASH 求值方面，MD5 退出的舞台将由 SHAn 占据。SHA 家族有五个算法：SHA-1、SHA-224、SHA-256、SHA-384 和 SHA-512，后四种有时候称为 SHA2 ，分别对应 linux 中的 sha...sum 命令。


{% highlight text %}
$ sha1sum file-name                                 # 计算SHA1哈希值
b14cb291f379140f1466381be80466c6722e4b1a  file-name
$ sha1sum file-name > file-name.sha1                # 将计算的哈希值保存到文件，可含多个
$ sha1sum -c file-name.sha1                         # 校验文件是否被修改
file-name: OK                                       # 失败为 file-name: FAILED ...
{% endhighlight %}

### PGP

实际上原理很简单，也就是使用非对称加密。首先生成唯一的密钥对，包括了公钥和私钥，然后执行如下步骤。

{% highlight text %}
发布者：
    1. 将密钥对中的公钥发布到公钥服务器；
    2. 通过私钥对需要发布的文件进行签名，得到签名文件；
    3. 将文件和用私钥生成的签名一起发布；

下载者：
    1. 下载发布者发布的文件和签名；
    2. 使用 PGP 获取发布者发布到服务器的公钥；
    3. 使用公钥校验文件签名。
{% endhighlight %}

关于 GnuGP 的使用，详细可参考 MySQL 的内容 [2.1.3.2 Signature Checking Using GnuPG](http://dev.mysql.com/doc/refman/5.7/en/checking-gpg-signature.html) 。

{% highlight text %}
$ gpg --import pubkey.asc                                      # 从官网上下载公钥，然后导入
$ gpg --keyserver keyserver.ubuntu.com --recv-keys 8D253E8A    # 或者可以直接从服务器导入

$ gpg --verify downloaded-file-sign.asc                        # 如果没有导入公钥，则会提示No public key
{% endhighlight %}

到现在为止，我们可以确认该文件在上传之后没有被修改过。

但是，需要注意的是这个是未受信任的签名认证，因为这个公钥谁都可以发布上去的。如果确实需要进一步认证，可能还要联系下真正的发布者，确认这个密钥的指纹 (fingerprint)，这也就是为什么在上述验证时可能会出现 WARNING 。

但是通常来说，通过用户名和邮箱就可以基本确定的。当确认信任该公钥后，可以通过如下的子命令设置信任该公钥，这样就不会再出现上面的 WARNING 。

{% highlight text %}
$ gpg --edit-key 'user-id' + sign
{% endhighlight %}



## 参考

[Reflections on Trusting Trust](http://www.win.tue.nl/~aeb/linux/hh/thompson/trust.html) 通常简称 RoTT 《对信任不疑的信任》，一个比较经典的文章，是肯汤普逊 (Ken Thompson) 1983 年获得图灵奖时的演说：你最终还是要绝对信任某一人，在这个问题上没有第二条路可走。

[安全加解密技术 盘点密码学进阶史](http://server.51cto.com/News-369437_all.htm) 一篇不错的介绍文章，可以参考 [本地文档](/reference/linux/history_of_encrypt.mht) 。


<!--
关于RSA加密算法的介绍  AES DES
http://www.ruanyifeng.com/blog/2013/06/rsa_algorithm_part_one.html
http://www.cnblogs.com/vamei/p/3480994.html
-->


{% highlight text %}
{% endhighlight %}
