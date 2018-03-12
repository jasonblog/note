---
title: PGP 简介
layout: post
comments: true
language: chinese
category: [linux,misc]
keywords: linux,pgp
description: PGP 是一个基于公钥加密体系的加密软件，功能强大，有很快的速度，而且源码是免费的。这里简单介绍下 PGP 相关内容。
---

GnuPGP 是一个基于公钥加密体系的加密软件，功能强大，有很快的速度，而且源码是免费的。

这里简单介绍下 PGP 相关内容。

<!-- more -->

## 简介

Pretty Good Privacy, PGP 加密由一系列散列、数据压缩、对称密钥加密，以及公钥加密的算法组合而成，遵守 OpenPGP 数据加密标准 [RFC 4880](http://tools.ietf.org/rfc/rfc4880.txt) ，每个步骤支持几种算法，可以选择一个使用。

<!-- https://tools.ietf.org/html/rfc3156 -->

注意 PGP 是一个商用软件，对应的开源软件为 GnuPGP ，两者都遵守 OpenPGP 标准。

![pgp diagram]({{ site.url }}/images/security/pgp-diagram.png "pgp diagram"){: .pull-center }


通常，使用如上的加密和解密过程大致为。

1. 用户产生需要加密的信息。
2. OpenPGP 发送方产生一串随机数，作为对称加密密钥，这一随机数只对该信息或该会话有效。
3. 使用接受者的公钥加密上述的随机数 (密钥)，放置到需要发送消息的开头。
4. 通过上述产生的随机数加密需要加密的信息，通常会先对信息进行压缩。
5. 接收方则会先通过私钥解密随机数，然后解密信息。

当然，也可以先产生一个数字签名，并将数字签名添加到信息中，然后同样会通过随机串加密。在 OpenPGP 中也可以对信息进行压缩，通常压缩是在获得数字签名后且在压缩之前。

> The Public-Key Cryptography Standards(PKCS) 是由美国 RSA 数据安全公司及其合作伙伴制定的一组公钥密码学标准，包括证书申请、证书更新、证书作废表发布、扩展证书内容以及数字签名、数字信封的格式等方面的一系列相关协议。

<!--
    PGP的实际操作由五种服务组成：鉴别、机密性、电子邮件的兼容性、压缩、分段和重装。

    1.1 鉴别

    步骤如下：
    （1）发送者创建报文；
    （2）发送者使用SHA-1生成报文的160bit散列代码（邮件文摘）；
    （3）发送者使用自己的私有密钥，采用RSA算法对散列代码进行加密，串接在报文的前面；
    （4）接收者使用发送者的公开密钥，采用RSA解密和恢复散列代码；
    （5）接收者为报文生成新的散列代码，并与被解密的散列代码相比较。如果两者匹配，则报文作为已鉴别的报文而接收。
    另外，签名是可以分离的。例如法律合同，需要多方签名，每个人的签名是独立的，因而可以仅应用到文档上。否则，签名将只能递归使用，第二个签名对文档的第一个签名进行签名，依此类推。

    1.2 机密性
    在PGP中，每个常规密钥只使用一次，即对每个报文生成新的128bit的随机数。为了保护密钥，使用接收者的公开密钥对它进行加密。描述如下：
    （1）发送者生成报文和用作该报文会话密钥的128bit随机数；
    （2）发送者采用CAST-128加密算法，使用会话密钥对报文进行加密。也可使用IDEA或3DES；
    （3）发送者采用RSA算法，使用接收者的公开密钥对会话密钥进行加密，并附加到报文前面；
    （4）接收者采用RSA算法，使用自己的私有密钥解密和恢复会话密钥；
    （5）接收者使用会话密钥解密报文。
    除了使用RSA算法加密外，PGP还提供了DiffieHellman的变体EIGamal算法。

    3 公开密钥管理

    3.1 公开密钥管理机制
    一个成熟的加密体系必然要有一个成熟的密钥管理机制配磁。公钥体制的提出就是为了解决传统加密体系的密钥分配过程不安全、不方便的缺点。例如网络黑客们常用的手段之一就是“监听”，通过网络传送的密钥很容易被截获。对PGP来说，公钥本来就是要公开，就没有防监听的问题。但公钥的发布仍然可能存在安全性问题，例如公钥被篡改（public key tampering），使得使用公钥与公钥持有人的公钥不一致。这在公钥密码体系中是很严重的安全问题。因此必须帮助用户确信使用的公钥是与他通信的对方的公钥。

    以用户A和用户B通信为例，现假设用户A想给用户B发信。首先用户A就必须获取用户B的公钥，用户A从BBS上下载或通过其它途径得到B的公钥，并用它加密信件发给B。不幸的是，用户A和B都不知道，攻击者C潜入BBS或网络中，侦听或截取到用户B的公钥，然后在自己的PGP系统中以用户B的名字生成密钥对中的公钥，替换了用户B的公钥，并放在BBS上或直接以用户B的身份把更换后的用户B的“公钥”发给用户A。那A用来发信的公钥是已经更改过的，实际上是C伪装B生成的另一个公钥（A得到的B的公钥实际上是C的公钥/密钥对，用户名为B）。这样一来B收到A的来信后就不能用自己的私钥解密了。更可恶的是，用户C还可伪造用户B的签名给A或其他人发信，因为A手中的B的公钥是仿造的，用户A会以为真是用户B的来信。于是C就可以用他手中的私钥来解密A给B的信，还可以用B真正的公钥来转发A给B的信，甚至还可以改动A给B的信。

    3.2 防止篡改公钥的方法

    （1）直接从B手中得到其公钥，这种方法有局限性。

    （2）通过电话认证密钥：在电话上以radix-64的形式口述密钥或密钥指纹。密钥指纹（keys fingerprint）就是PGP生成密钥的160bit的SHA-1摘要（16个8位十六进制）。

    （3）从双方信任的D那里获得B的公钥。如果A和B有一个共同的朋友D，而D知道他手中的B的公钥是正确的。D签名的B的公钥上载到BBS上让用户去拿，A想要获得B的公钥就必须先获取D的公钥来解密BBS或网上经过D签名的B的公钥，这样就等于加了双重保险，一般没有可能去篡改而不被用户发现，即使是BBS管理员。这就是从公共渠道传递公钥的安全手段。有可能A拿到的D或其他签名的朋友的公钥也是假的，但这就要求攻击者C必须对三人甚至很多人都很熟悉，这样的可能性不大，而且必需经过长时间的策划。

    只通过一个签名证力度可能是小了一点，于是PGP把用不同私钥签名的公钥收集在一起，发送到公共场合，希望大部分从至少认识其中一个，从而间接认证了用户（A）的公钥。同样用户（D）签了朋友（A）的公钥后应该寄回给他（A）（朋友），这样可以让他（A）通过该用户（D）被该用户（D）的其他朋友所认证。与现实中人的交往一样。PGP会自动根据用户拿到的公钥分析出哪些是朋友介绍来的签名的公钥，把它们赋以不同的信任级别，供用户参考决定对它们的信任程度。也可指定某人有几层转介公钥的能力，这种能力随着认证的传递而递减的。

    （4）由一个普通信任的机构担当第三方，即“认证机构”。这样的“认证机构”适合由非个人控制的组织或政府机构充当，来注册和管理用户的密钥对。现在已经有等级认证制定的机构存在，如广东省电子商务电子认证中心（WWW.cnca.net）就是一个这样的认证机构。对于那些非常分散的用户，PGP更赞成使用私人方式的密钥转介。

    3.3 信任的使用

    PGP确实为公开密钥附加侂任和开发信任信息提供了一种方便的方法使用信任。

    公开密钥环的每个实体都是一个公开的密钥证书。与每个这亲的实体相联系的是密钥合法性字段，用来指示PGP信任“这是这个用户合法的公开密钥”的程度；信任程度越高，这个用户ID与这个密钥的绑定越紧密。这个字段由PGP计算。与每个实体相联系的还有用户收集的多个签名。反过来，每个签名都带有签名信任字段，用来指示该PGP用户信任签名者对这个公开密钥证明的程度。密钥合法性字段是从这个实体的一组签名信任字节中推导出来的。最后，每个实体定义了与特定的拥有者相联系的公开密钥，包括拥有者信任字段，用来指示这个公开密钥对其他公开密钥证书进行签名的信任程度（这个信任程度是由该用户指定的）。可以把签名信任字段看成是来自于其他实体的拥有者信任字段的副本。

    例如正在处理用户A的公开密钥环，操作描述如下：

    （1）当A在公开密钥环中插入了新的公开密钥时，PGP为与这个公开密钥拥有者相关联的信任标志赋值，插入KUa，则赋值=1终极信任；否则，需说明这个拥有者是未知的、不可任信的、少量信任的和完全可信的等，赋以相应的权重值1/x、1/y等。

    （2）当新的公开密钥输入后，可以在它上面附加一个或多个签名，以后还可以增加更多的签名。在实体中插入签名时，PGP在公开密钥环中搜索，查看这个签名的作者是否属于已知的公开密钥拥有者。如果是，为这个签名的SIGTRUST字段赋以该拥的者的OWNERTRUST值。否则，赋以不认识的用户值。

    （3）密钥合法性字段的值是在这个实体的签名信任字段的基础上计算的。如果至少一个签名具有终极信任的值，那么密钥合法性字段的设置为完全；否则，PGP计算信任值的权重和。对于总是可信任的签名赋以1/x的权重，对于通常可信任的签名赋以权重1/y，其中x和y都是用户可配置的参数。当介绍者的密钥/用户ID绑定的权重总达到1时，绑定被认为是值得信任的，密钥合法性被设置为完全。因此，在没有终极信任的情况下，需要至少x个签名总是可信的，或者至少y个签名是可信的，或者上述两种情况的某种组合。如图4所示。

    总之，PGP采用了RSA和传统加密的杂合算法，用于数字签名的邮件文摘算法、加密前压缩等，可以用来加密文件，还可以代替Uuencode生成RADIX 64格式（就是MIME的BASE 64格式）的编码文件。PGP创造性地把RSA公钥体系的方便和传统加密体系的高速度结合起来，并且在数字签名和密钥认证管理机制上有巧妙的设计。这是目前最难破译的密码体系之一。

    用户通过PGP的软件加密程序，可以在不安全的通信链路上创建安全的消息和通信。PGP协议已经成为公钥加密技术和全球范围内消息安全性的事实标准。因为所有人都能看到它的源代码，从而查找出故障和安全性漏局
-->

### 信任模型

PGP 的好处在于它的信任模型 \-\- Web Trust Model，一种非常贴近生活中人与人的信任模型。在信息世界里面，人与人之间的信任纽带有两种方式被建立。

#### 集中式

如 CA 结构，大家都信任 CA ，于是 Peter 是不是 Peter ，是根据 CA 是否认识这个 Peter ，我们现在通常所说的的数字证书认证，就是这种集中认证的模式。

#### 分布式

假设没有CA，只信任自己和朋友，每个人都是在一个朋友圈子中，朋友圈子以外的人，则需要一个中间人作为介绍人，这就是现实的生活。

我有一个朋友叫 Andy，且我有另一个朋友叫 Bob，如果 Bob 想认识 Andy，于是他想通过我来认识 Andy，方法很简单，鉴于我认识他们两者，通过我为 Bob 的公钥签名，于是 Andy 可以直接向 Bob 证明他是我的朋友。

PGP 采用的后者是这种方式，每个人都以自己为中心，然后是朋友圈，然后是朋友的朋友。


## GnuPGP

公钥算法的关键点在于如何传播公钥，如果有恶意用户传递了一把虚假公钥，此时，如果别人不知就里，用这把公钥加密信息，持有该虚假钥匙的侵入者就可以解密而读到信息。而如果侵入者再将解密的讯息加以修改，并以真正的公开钥匙加密，然后传送出去，这种进攻无法被发现。

PGP 的解决方法是对公钥进行签名，也即生成指纹 (fingerprint)，只有公钥会有指纹。每个公钥均绑定唯一的用户身份 (包括用户名、注释、邮箱)，当然一个人的公钥可以由别人来签名，**签名 (指纹) 是用来测试该公钥是否是由其声明的用户发出的** 。

至于有多信任这些签名，完全取决于 GPG 用户，当你信任给这把钥匙签名的人时，你认为这把钥匙是可信的，并确信这把钥匙确实属于拥有相应用户身份的人。只有当你信任签名者的公开钥匙时，你才能信任这个签名。要想绝对确信一把钥匙是正确和真实的，你就得在给予绝对信任之前，通过可靠渠道比较钥匙的 "指纹"。

### 使用

在 ```~/.gnupg``` 目录下，保存了一些必须的文件，其中密钥以加密形式存储在磁盘上，包括了两个文件，一个存储公钥 (```pubring.gpg```)、一个存储私钥 (```secring.gpg```)。

GPG 会通过 username、comment、email 生成一个 UID(hash value)，在使用时可以用其中的一种方式。

#### 1. 生成密钥对 (公钥+私钥)

生成时需要选择 A) 加密算法、密钥长度 (长度越长解密时间越长，不过也会更安全)、密钥的有效时间；B) 区分该密钥对的用户名、e-mail、注释 (这些信息是可以修改的)；C) 一个保护该私钥的验证密码。

{% highlight text %}
$ gpg --gen-key
Please select what kind of key you want:                  # 选择加密以及指纹算法
   (1) RSA and RSA (default)
   (2) DSA and Elgamal
   (3) DSA (sign only)
   (4) RSA (sign only)
Your selection? 1
RSA keys may be between 1024 and 4096 bits long.
What keysize do you want? (2048)
Requested keysize is 2048 bits
Please specify how long the key should be valid.
         0 = key does not expire
      <n>  = key expires in n days
      <n>w = key expires in n weeks
      <n>m = key expires in n months
      <n>y = key expires in n years
Key is valid for? (0)
Key does not expire at all
Is this correct? (y/N) y

GnuPG needs to construct a user ID to identify your key.

Real name: foobar
Email address: foobar@ooops.com
Comment: just for test
You selected this USER-ID:
    "foobar (just for test) <foobar@ooops.com>"

Change (N)ame, (C)omment, (E)mail or (O)kay/(Q)uit? O
You need a Passphrase to protect your secret key.         # 输入密码

gpg: key FCC7A473 marked as ultimately trusted            # 用户的hash值，可以用来替代userid
public and secret key created and signed.

gpg: checking the trustdb
gpg: 3 marginal(s) needed, 1 complete(s) needed, PGP trust model
gpg: depth: 0  valid:   1  signed:   0  trust: 0-, 0q, 0n, 0m, 0f, 1u
pub   2048R/FCC7A473 2016-07-06
      Key fingerprint = ACB2 B707 B2D8 A548 55A8  5E24 A5CC BA5C FCC7 A473   # 可以用来电话验证^_^
uid                  foobar (just for test) <foobar@ooops.com>               # 三元素组成userid
sub   2048R/3B44DA00 2016-07-06
{% endhighlight %}

生成的公钥 (```pubring.gpg```)、私钥 (```secring.gpg```) 保存在 ```~/.gnupg/``` 目录下。

<!-- 生成密码时需要花费一些时间获取随机数据，通常选择 2048~4096bit 的 RSA 加密方式。最后需要输入一个密码，不知道是作什么用的 -->

#### 2. 撤销公钥

当密钥对生成之后，应该立即做一个公钥回收证书，当忘记了私钥的口令或者私钥丢失或被盗窃，可以发布这个证书来声明以前的公钥不再有效。

{% highlight text %}
$ gpg --output revoke.asc --gen-revoke "user-name/e-mail/user-id"           # 生成回收证书，以备不时之需
$ gpg --import revoke.asc                              # 导入回收证书
$ gpg --keyserver keys.gnugp.net --send "8D253E8A"     # 发送回收证书到服务器，声明原GPG Key作废
{% endhighlight %}

当然如果要这么做，一定要先有私钥，否则任何人都能取消你的公钥。

#### 3. 传播公钥

导出公钥之后，就可以通过一些可靠的方式传播公钥。可以将公钥通过 e-mail 发送给好友，或者上传到指定的服务器(如果不指定使用默认的 [keys.gnupg.net](http://keys.gnupg.net) ，其它的还有 [keyserver.ubuntu.com](http://keyserver.ubuntu.com/) 。

{% highlight text %}
$ gpg --keyserver keys.gnugp.net --send-keys "8D253E8A"                     # 上传到服务器
$ gpg --fingerprint                                                         # 打印公钥指纹
$ gpg --keyserver keys.gnugp.net --search-keys "user-name/e-mail/user-all"  # 在服务器上查找
{% endhighlight %}

由于公钥服务器没有检查机制，任何人都可以用你的名义上传公钥，所以没有办法保证服务器上的公钥的可靠性。通常，可以在网站上公布一个公钥指纹，让其他人核对下载到的公钥是否为真。

#### 4. 查看密钥

{% highlight text %}
$ gpg --list-keys                       # 输出现有公钥，包括UID、公私钥的HASH值，等同于gpg -k
$ gpg --list-sigs                       # 同时显示签名，实际也就是上述的HASH值
$ gpg --fingerprint                     # 输出公钥的指纹
$ gpg --list-secret-keys                # 列出私钥，PS. 列出私钥的指纹和签名根本就没用，等同于gpg -K
{% endhighlight %}

通过 ```gpg -k``` 列出所有公钥，其中第一行表示保存的文件名 (```pubring.gpg```)；第二行为公钥的特征 (加密方式:如 ```2048R``` 表示 ```2048-bit RSA```，```user-id``` 的 ```hash``` 值，生成时间)；第三行表示用户 ID ；第四行表示私钥特征。

#### 5. 导出公钥

将指定用户的公钥以 ASCII 的格式导出。

{% highlight text %}
$ gpg --armor --export "user-name/e-mail/user-id"                           # 导出ASCII公钥到终端
$ gpg --armor --output public-key.gpg --export "user-name/e-mail/user-id"   # 导出ASCII公钥到文件，或者-o
$ gpg --armor --export-secret-keys                                          # 导出私钥
$ gpg --refresh-keys                                                        # 从服务器定期更新公钥
{% endhighlight %}

其中 ```--armor``` 表示将内容转换成可见的 ASCII 码输出，否则是二进制不方便阅读。

#### 6. 导入公钥

除了生成自己的密钥，还需要别人的公钥，用于生成加密信息或者验证数字签名等。为此，就需要将他人的公钥或者你的其他密钥输入系统，这时可以使用 import 参数。

{% highlight text %}
$ gpg --import public-key.gpg                                                       # 导入ASCII公钥到文件
$ gpg --keyserver hkp://subkeys.pgp.net --search-keys "user-name/e-mail/user-all"   # 从服务器查看然后选择
$ gpg --keyserver subkeys.gpg.net --recv-keys 8D253E8A                              # 直接导入
{% endhighlight %}

由于任何人都可以导入公钥，我们无法保证服务器上的公钥是否可靠，下载后还需要用其他机制验证，如数字签字。

#### 7. 删除密钥

我们可以通过如下命令删除密钥。

{% highlight text %}
$ gpg --delete-keys "user-name/e-mail/user-id"                    # 从公钥钥匙环里删除密钥
$ gpg --delete-secret-keys "user-name/e-mail/user-id"             # 从私钥钥匙环里删除密钥
$ gpg --delete-secret-and-public-key "user-name/e-mail/user-id"   # 同时删除公钥私钥
{% endhighlight %}

#### 8. 修改密钥

用此命令你可以修改钥匙的失效日期，加进一个指纹，对钥匙签名等等。 尽管显得太清楚而不用提，这里还是要说，要做以上事情你得用你的通行句。 敲入通行句后，你会见到命令行。

{% highlight text %}
$ gpg --edit-key "user-name/e-mail/user-id"
{% endhighlight %}

#### 9. 加密/解密

在安装和按照需要设置好所有事以后，我们就可以开始加密和解密了。

{% highlight text %}
$ gpg --recipient "8D253E8A" --output demo.en.txt --encrypt demo.txt   # 用指定用户公钥加密

$ gpg --output demo.de.txt --decrypt demo.en.txt                       # 解密，可以不指定用户名
$ gpg demo.en.txt                                                      # 解密，最简单方式
{% endhighlight %}

```--recipient``` 参数指定接收者的公钥，```--output``` 指定加密后的文件名，```--encrypt``` 指定源文件。

#### 10. 签名

有时只需要对文件签名，表示这个文件确实是我本人发出的。

{% highlight text %}
$ gpg --sign demo.txt                   # 生成二进制签名demo.txt.gpg，同时包含文本内容
$ gpg --clearsign demo.txt              # 生成ASCII的签名文件demo.txt.asc，同时包含文本内容
$ gpg --detach-sign demo.txt            # 只生成二进制签名demo.txt.sig
$ gpg --armor --detach-sign demo.txt    # 只生成ASCII签名demo.txt.asc
$ gpg --verify demo.txt.asc demo.txt    # 验证解密的文件与签名是否相符

$ gpg --armor --detach-sign -u UID demo.txt    # 可以指定用户
{% endhighlight %}


#### 11. 签名+加密

此时签名和加密同时保存在同一个文件中。

{% highlight text %}
$ gpg --local-user "user-name/e-mail/user-id" --recipient "8D253E8A" --armor --sign --encrypt demo.txt
$ gpg demo.txt.asc                      # 解密
{% endhighlight %}


### 公钥的管理

正如前言所提到的，这个系统有一个最大的薄弱点，那就是公钥的真实性问题。可以通过直接向公钥的提供者当面或者电话验证指纹，从而确定公钥的真实性问题。

当确定了公钥的真实性之后可以对公钥签名，表示你绝对确信这把钥匙是真实的，有了这个保证，用户就可以开始放心用这把钥匙加密了。

要对一把钥匙签名，用 ```gpg --edit-key UID``` ，然后用 sign 命令。

GPG 会根据现有的签名和对 "主人的信任度" 来决定钥匙的真实性，包括了四个值。

{% highlight text %}
1 = 我不知道
2 = 我不信任（这把钥匙）
3 = 我勉强信任
4 = 我完全信任
{% endhighlight %}

对于用户不信任的签名，就会将废弃这个签名不用。注意这些信任信息不是存在储存钥匙的文件里，而是存在另一个文件里。

<!--

<br><h2>编辑密钥</h2><p>
另外，一些缩写的对应方式大致为
sec ('SECret key')
ssb => 'Secret SuBkey'
pub => 'PUBlic key'
sub => 'public SUBkey'

Constant           Character      Explanation
─────────────────────────────────────────────────────
PUBKEY_USAGE_SIG      S       key is good for signing
PUBKEY_USAGE_CERT     C       key is good for certifying other signatures
PUBKEY_USAGE_ENC      E       key is good for encryption
PUBKEY_USAGE_AUTH     A       key is good for authentication
</p>



<br><h2>使用 Subkey</h2><p>
通过 subkey 可以更方便管理密钥，以及在某些情况下可以保护好密钥。例如电脑丢了，



A few weeks ago I created my new GPG/PGP key with subkeys and a few people asked me why and how. The rationale for creating separate subkeys for signing and encryption is written very nicely in the subkeys page of the debian wiki. The short answer is that having separate subkeys makes key management a lot easier and protects you in certain occasions, for example you can create a new subkey when you need to travel or when your laptop gets stolen, without losing previous signatures. Obviously you need to keep your master key somewhere very very safe and certainly not online or attached to a computer.

You can find many other blog posts on the net on the subject, but most of them are missing a few parts. I’ll try to keep this post as complete as possible. If you are to use gpg subkeys you definitely need an encrypted usb to store the master key at the end. So if you don’t already have an encrypted USB go and make one first.

When this process is over you will have a gpg keypair on your laptop without the master key, you will be able to use that for everyday encryption and signing of documents but there’s a catch. You won’t be able to sign other people’s keys. To do that you will need the master key. But that is something that does not happen very often so it should not be a problem in your everyday gpg workflow. You can read about signing other people’s keys at the end of this post. AFAIK you can’t remove your master key using some of the gpg GUIs, so your only hope is the command line. Live with it…

First some basic information that will be needed later.
When listing secret keys with gpg -K keys are marked with either ‘sec’ or ‘ssb’. When listing (public) keys with gpg -k keys are marked with ‘pub’ or ‘sub’.

sec => 'SECret key'
ssb => 'Secret SuBkey'
pub => 'PUBlic key'
sub => 'public SUBkey'

When editing a key you will see a usage flag on the right. Each key has a role and that is represented by a character. These are the roles and their corresponding characters:

Constant           Character      Explanation
─────────────────────────────────────────────────────
PUBKEY_USAGE_SIG      S       key is good for signing
PUBKEY_USAGE_CERT     C       key is good for certifying other signatures
PUBKEY_USAGE_ENC      E       key is good for encryption
PUBKEY_USAGE_AUTH     A       key is good for authentication

Before doing anything make sure you have a backup of your .gnupg dir.
$ umask 077; tar -cf $HOME/gnupg-backup.tar -C $HOME .gnupg
<br><br>


<pre style="font-size:0.8em; face:arial;">
$ gpg --gen-key                          // 此时会要求输入主密码，以后会用的到
pub   2048R/43E315AF 2015-05-30
      Key fingerprint = 70B5 4B54 B31A 32E8 2E0B  F4A0 67DC A662 43E3 15AF
uid                  foobar (test) &lt;foobar@oops.com&gt;
sub   2048R/2065CC7D 2015-05-30

$ gpg --edit-key 43E315AF                // 使用公钥ID，添加新的UID
gpg> adduid                              // 输入新的UID，最后确认时需要上述的密码
... ...                                  // 此时包含了两个UID
gpg> uid 2
gpg> primary                             // 将新的设置为主
gpg> save

$ gpg --edit-key 43E315AF                // 添加新的subkey
gpg> addkey                              // 新的subkey
... ...
pub  2048R/43E315AF  created: 2015-05-30  expires: never       usage: SC
                     trust: ultimate      validity: ultimate
sub  2048R/2065CC7D  created: 2015-05-30  expires: never       usage: E
sub  2048R/AD5CC9E2  created: 2015-05-30  expires: never       usage: S
gpg> save
</pre>
上述产生的密码中，43E315AF 含有SC标志，是主密码；2065CC7D 含有E标志，是用于加密的从密码； AD5CC9E2 含有S标志，用于Sign的从密码。<br><br>

然后可以生成的撤销证书，可以加密后保存在单独的地方，最好网络不可达的，如一个加密的U盘，当然也可以打印出来，因为需要输入的内容很少。<br><br>

接下来，就是比较关键的部分了。将主健(master key)从电脑上删除，仅留下子键(subkeys)，然后将主健保存在加密U盘即可。
<pre style="font-size:0.8em; face:arial;">
$ rsync -avp $HOME/.gnupg /media/encrypted-usb               // 将数据同步到加密的磁盘，或者同下
$ umask 077; tar -cf gnupg-backup-new.tar -C $HOME .gnupg

$ gpg --export-secret-subkeys 43E315AF &gt; subkeys          // 将subkeys导出
$ gpg --delete-secret-key 43E315AF                           // 删除主健，此时私钥已经删除，公钥还在
$ gpg --import subkeys
</pre>
注意，此时主健保存在了加密后的U盘上，所以不要丢掉U盘。可以通过如下查看删除主钥后的差别。
<pre style="font-size:0.8em; face:arial;">
$ gpg -K 43E315AF
sec#  2048R/43E315AF 2015-05-30
... ...
$ gpg --home=gnupg -K 43E315AF
sec  2048R/43E315AF 2015-05-30
</pre>
删除主健后显示 # 字符，此时可以将新的健上传到服务器。<br><br>

此时，仍然可以作签名。
<pre style="font-size:0.8em; face:arial;">
$ gpg --armor -b -u 0x6F87F32E2234961E -o sig2.txt gpg-transition.txt
$ gpg --clearsign demo.txt
$ gpg --verify file.asc
</pre>
</p>





http://www.alexgao.com/2009/01/24/gpg/
http://moser-isi.ethz.ch/gpg.html
https://wiki.debian.org/Subkeys?action=show&redirect=subkeys
http://ekaia.org/blog/2009/05/10/creating-new-gpgkey/
http://www.ruanyifeng.com/blog/2011/08/what_is_a_digital_signature.html
http://archboy.org/2013/04/18/gnupg-pgp-encrypt-decrypt-message-and-email-and-digital-signing-easy-tutorial/
http://www.codemud.net/~thinker/GinGin_CGI.py/show_id_doc/478
https://www.gnupg.org/howtos/card-howto/en/ch05s02.html

http://www.cnblogs.com/littlehann/p/3662161.html

http://blog.jobbole.com/22367/
http://www.atatech.org/articles/33108?rnd=1344733092

http://www.networkworld.com/article/2268575/lan-wan/chapter-2--ssl-vpn-technology.html   Chapter 2: SSL VPN Technology
-->


## 参考

关于如何使用 subkey [Creating a new GPG key with subkeys](https://www.void.gr/kargig/blog/2013/12/02/creating-a-new-gpg-key-with-subkeys/) 一个不错的介绍，可以参考 [本地文档](/reference/security/Creating_a_new_GPG_key_with_subkeys.maff)；另一篇参考的是最佳实践，可以参考 [OpenPGP Best Practices](https://help.riseup.net/en/security/message-security/openpgp/best-practices) ，或者 [本地文档](/reference/security/OpenPGP_Best_Practices.maff) 。

一个官方的中文文档 [GnuPG 袖珍 HOWTO](https://www.gnupg.org/howtos/zh/index.html) ，或者英文版 [The GNU Privacy Handbook](https://www.gnupg.org/gph/en/manual.html) 。

<!--
GnuPG 的官方为
https://www.gnupg.org/index.html

Speed Hashing，包括了加密揭秘速度
http://blog.codinghorror.com/speed-hashing/

关于SHA1～SHA3的介绍
http://www.answers.com/article/1163617/the-evolution-of-the-nist-secure-hash-algorithm-from-sha-1-to-sha-3

http://www.ruanyifeng.com/blog/2013/07/gpg.html
-->


{% highlight text %}
{% endhighlight %}
