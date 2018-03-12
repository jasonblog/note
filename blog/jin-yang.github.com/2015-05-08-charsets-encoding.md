---
title: 字符集与编码杂谈
layout: post
comments: true
language: chinese
category: [misc]
keywords: 字符集,编码
description: 是不是还分不清什么是字符集，什么是编码，两者到底有什么区别？ 什么是 GB2312、GB18030、BIG5？ Unicode 和 UTF-8、UTF-16、UTF-32 到底什么关系？ BMP、UCS、emoji 又是什么？ 为什么会出现乱码？OK，接下来我们介绍一下他们之间剪不断理还乱的关系 ... ...
---

是不是还分不清什么是字符集，什么是编码，两者到底有什么区别？ 什么是 GB2312、GB18030、BIG5？ Unicode 和 UTF-8、UTF-16、UTF-32 到底什么关系？ BMP、UCS、emoji 又是什么？ 为什么会出现乱码？

OK，接下来我们介绍一下他们之间剪不断理还乱的关系 ... ...

<!-- more -->

## 为什么会有字符集和编码

![charsets-logo]{: .pull-right width="350"}

首先说一下什么是字符集，简单地说，那就是一堆的符号，一堆我们可以看得见（如 A、B、C）或者看不见（如一些控制字符，像回车、振铃等）的东西。当然，不同的字符集可能会包含不同的范围，可能只包含了英语用的字符，也可能包含了一些中文字符 ... ...

实际上，字符集通常与不同地域的语言相关，当然也可能包含了所有的字符，下面会做介绍。

OK，那什么是编码。我们知道，在计算机中储存的信息实际上都是用二进制数表示的 Bytes 流，而我们在屏幕上看到的英文、汉字、标点符号等字符都是二进制数转换之后的结果。而 A *<font color='red' size='6'>A</font>* **A** 或者 <font face='atrament-web-1' size='6'>A</font> 都表示是同一个字符，只是最后转换显示的格式不同。

通俗的说，按照何种规则将字符存储在计算机中，就是编码方式。如 'a' 用什么样的二进制表示，称为 "编码"；反之，将存储在计算机中的二进制数解析并显示出来，称为 "解码"。当然，如果在解码过程中，使用了错误的解码规则，可能会导致 'a' 会解析成 'b' 或者直接导致乱码（mojibake）。<br><br>

而字符集实际就是一堆字符的集合，对于一个字符集来说要正确编码解码一个字符需要三个关键元素：抽象字库表（abstract character repertoire）、编码字符集（coded character set）、字符编码（character encoding form），当然还有一个不太重要的字节序。

其中抽象字库表包括了所有字符的数据库，决定了整个字符集能够表示的所有字符的范围，字符就是各种文字和符号的总称，包括各国家文字、标点符号、图形符号、数字等。编码字符集，用来表示抽象字库表中对应字符的表示方法，通常使用用数子来表示，如 Unicode 中采用码值（code point）来标示一个字符在字库中的位置，GB2312 用区位码标示。字符编码，用来指定编码字符集和实际存储数值之间的转换关系。

为什么要整的这么复杂。其实，原因也是比较明了的，抽象字库表的目的是为了表示所能够涵盖字符的范围，可能值包含了部分字符（如ASCII），当然，也有可能打算包含这个世界上所有的字符（如Unicode）。但实际使用过程中会发现，真正用的上的字符相对整个字库表来说比例非常低，例如在美国 ASCII 基本就能满足需求，中文地区的程序几乎不会需要日语字符。而如果把每个字符都用字库表中的序号来存储的话，每个字符就需要 3 个字节（以 Unicode 字库为例），这样对于原本用仅占一个字符的 ASCII 编码的英语地区国家显然是一个额外成本，存储体积是原来的三倍。

为此，就把字符集和对应的编码方式分开，在保证字符集不变的情况下，可以灵活地改变编码方式。

有些字符集会直接将字符集中的编号与字符编码对应，如在 ASCII 中，'A' 在表中排第 65 位，而编码后 A 的数值也是 65；而有些则不同，如在 unicode 中，'中'的码值为 3197，而用 UTF-8 表示为 E4B8AD 。<br><br>


简单的说，实际用到的概念主要是 **字符集** 和 **编码方式**。其中 **字符集决定了我们所能使用的字符**，例如，当使用 ASCII 字符集的时候，不能表示中文。**编码方式则决定了这些字符在磁盘上的存储方式**，如对于 Unicode 字符集，我可以选择 UTF-8、UTF-16、UTF-32 中的一种方式进行存储。


常见字符集有，ASCII 字符集、GB2312 字符集、BIG5 字符集、GB18030 字符集、Unicode 字符集等。


## ASCII

ASCII（American Standard Code for Information Interchange, 美国信息交换标准代码）是最简单的字符集了。是一套基于拉丁字母的编码系统，主要用于显示现代英语，其扩展版本 EASCII 可以勉强显示其他西欧语言。

![ascii-table]{: .pull-center width='750' }

ASCII 码一共规定了 128 个字符的编码，只占用了一个字节的后面 7 位，最前面的 1 位统一规定为 0。<br><br>

另一个单字节编码是 ISO-8859-1，也就是 MySQL 上的 latin1 编码，其编码范围是 0x00~0xFF。并向下兼容 ASCII，其中 0x00~0x7F 与 ASCII 一致，0x80~0x9F 之间是控制字符，0xA0~0xFF 之间是文字符号，新加的字符如下。

![ISO-8859-1]{: .pull-center }

此字符集支持部分于欧洲使用的语言，包括阿尔巴尼亚语、丹麦语、荷兰语、法罗语、德语、格陵兰语、冰岛语、爱尔兰盖尔语、意大利语、拉丁语、挪威语、葡萄牙语、里托罗曼斯语、苏格兰盖尔语、西班牙语等；也可以支持欧洲以外的部分语言，如南非荷兰语、斯瓦希里语、印尼语及马来语等。


## 中文编码

最初的计算机，只采用 ASCII 就可以完成编码，但是当传播到天朝之后，如何显示中文就成了不可避免的一个问题，必须有种方法可以显示中文的字符。

### GB2312

GB2312 又称国标码，是指国家 1980 的一个标准，由国家标准总局发布，1981.5.1 开始实施，通行于大陆，新加坡等地也使用此编码。

GB2312 规定 "对任意一个图形字符都采用两个字节表示，每个字节均采用七位编码表示"，称第一个字节为高字节，也称为"区"；第二个字节为低字节，也称为"位"，所以也称为区位码。GB2312 是一个简化字的编码规范，也包括其他的符号、字母、日文假名等，一共收录了 7445 个图形字符，其中包括汉字 6763 个和 682 个其它符号。

在这些编码中，同时将 ASCII 本来就有的数字、标点、字母都重新编了两个字节长的编码，这就是常说的 "全角" 字符，而原来在 127 号以下的那些就叫 "半角" 字符了。

GB2312 将代码表分为 94 个区，对应第一字节（0xA1-0xFE）；每个区 94 个位（0xA1-0xFE），对应第二字节，两个字节的值分别为区号值和位号值。01-09 区为符号、数字区；16-87 区为汉字区；10-15区、88-94区是有待进一步标准化的空白区。

汉字区的编码范围高字节从 B0-F7，低字节从 A1-FE，占用的码位是 72*94=6768，其中有 5 个空位是 D7FA-D7FE，也就是如上所述的，包括了 6763 个汉字。

如，对于汉字 "啊" 的区位码是 1601，写成 16 进制是 0x10 0x01。而这和计算机广泛使用的 ASCII 编码冲突，为了兼容 00-7f 的 ASCII 编码，我们在区位码的高、低字节上分别加上 A0。这样 "啊" 的编码就成为 B0A1，我们将加过两个 A0 的编码也称为 GB2312 编码。

![gb2312-example]{: .pull-center}

GB2312 的详细编码可以参考 [GB2312 简体中文编码表][gb2312-charsets] 。


### BIG5

Big5 又称大五码，主要为香港、台湾、澳门等地域使用，是一个繁体字编码，共收录了 13,060 个汉字。每个汉字由两个字节构成，第一个字节的范围从 0x81-0xFE，共 126 种；第二个字节的范围不连续，分别为 0x40~0x7E 和 0xA1-0xFE，共 157 种。

![big5-example]{: .pull-center}

如图所示，GB2312 和 BIG5 的字符集对应的编码是不兼容的。


### GB18030

GB2312 所收录的汉字已经覆盖中国大陆 99.75% 的使用频率，但对于人名、古汉语等方面出现的罕用字，仍不能处理，这导致了后来 GBK 及 GB18030 汉字字符集的出现。

在 1995 年对 GB2312 进行了扩充，扩展为规范 GBK1.0。GBK 中每个汉字仍然包含两个字节，第一个字节的范围是 0x81-0xFE，第二个字节的范围是 0x40-0xFE，也就是 GBK 中有码位 23940 个，该字符集收录了 21886 个符号，分为汉字区和图形符号区，汉字区包括 21003 个字符，同时还包括了繁体（不过与 Big5 不兼容）。

2000 年的 GB18030 取代 GBK1.0 正式成为国家标准，编码空间约为 160 万码位。该标准收录了 27484 个汉字，同时还收录了藏文、蒙文、维吾尔文等主要的少数民族文字。从汉字字汇上说，GB18030 在 GB13000.1 的 20902 个汉字的基础上增加了 CJK 扩展 A 的 6582 个汉字（Unicode 码 0x3400-0x4db5），一共收录了 27484 个汉字。

GB18030 的编码采用单字节、双字节和 4 字节方案，其中单字节、双字节和 GBK 是完全兼容的。4 字节编码的码位就是收录了 CJK 扩展 A 的 6582 个汉字。

GB18030 标准采用单字节、双字节和四字节三种方式对字符编码。单字节部分使用 0×00 至 0×7F 码位，对应于 ASCII 码的相应码位。双字节部分，首字节码位从 0x81 至 0xFE，尾字节码位分别是 0x40 至 0x7E 和 0x80 至 0xFE。四字节部分采用 0x30 到 0x39 作为对双字节编码扩充的后缀，这样扩充的四字节编码，其中第一、三个字节编码码位均为 0x81 至 0xFE，第二、四个字节编码码位均为 0x30 至 0x39。码位总体结构见下图。

![gb18030-encoding]{: .pull-center}

从 ASCII、GB2312 到 GBK，这些编码方法是向下兼容的，即同一个字符在这些方案中总是有相同的编码，只是后面的标准可以支持更多的字符。在这些编码中，中英文可以统一处理，区分中文编码的方法是高字节的最高位不为 0。

关于 GB18030 可以参考 [GB18030][gb18030-charsets] 。

### 其它

微软的 CP936 通常被视为等同 GBK，事实上比较起来，GBK 定义之字符较 CP936 多出 95 字，15 个非汉字及 80 个汉字。



## Unicode

像天朝一样，在计算机传到世界各个国家后，为了适合当地语言和字符，都会设计和实现类似 GB2312/GBK/GB18030/BIG5 的编码方案。这样各搞一套，在本地使用没有问题，一旦出现在网络中，由于不兼容，互相访问时就会出现乱码现象。于是乎就出现了万能的 Unicode，Unicode 的发展由一个非营利机构 "统一码联盟" 负责。

当然，当时意识这一问题的还有一个国际标准化组织（ISO），采用通用字符集（Universal Character Set，UCS），也就是由 ISO 制定的 ISO-10646 标准所定义的标准字符集。在 1991 年前后，两个项目开始合并双方的工作成果，并为创立一个单一字符集而协同工作，从 Unicode 2.0 开始，Unicode 采用了与 ISO 10646-1 相同的字库和字码；同时 ISO 也承诺，ISO-10646 将不会替超出 U+10FFFF 的 UCS-4 编码赋值，以使得两者保持一致。当然，现在两者仍然在独立发布着自己的标准。

Unicode，又叫做万国码、国际码、统一码、单一码，是计算机科学领域里的一项业界标准，对世界上大部分的文字系统进行了整理、编码，使得电脑可以用更为简单的方式来呈现和处理不同地域的文字。而且，Unicode 至今仍在不断增修，每个新版本都加入更多新的字符。目前最新的稳定版本为 2014.6.16 公布的 7.0.0（8.0 会在 2015.6 公布），已收入超过十万个字符。

Unicode 编码系统就是为表达任意语言、任意字符而设计，使用 4 字节来表达字母、符号，或者表意文字（ideograph），每个数字代表唯一的至少在某种语言中使用的符号。被几种语言共用的字符通常使用相同的数字来编码，除非存在一个合理的语源学（etymological）理由不这样做。不考虑这种情况的话，每个字符对应一个数字，每个数字对应一个字符，即不存在二义性。如 U+0041 总是代表 'A'，即使这种语言没有 'A' 这个字符。

Unicode 不但包含了一些语言对应的字符，还包括了一堆其它的 ... ...

![unicode-tai-symbols]{: .pull-center width="380"}

额，先天八卦？易有太极，是生两仪，两仪生四象，四象生八卦 ... ...

![unicode-mahjong]{: .pull-center width="280"}

好吧，麻将 ！！！

![unicode-domino]{: .pull-center width="450"}

小时候玩过的骨牌 ^_^ 或者是多米诺 ... ...



### 字符集编码详解

Unicode 的编码空间从 U+0000 到 U+10FFFF，共有 1,112,064 个码位（code point）可用来映射字符，其编码空间划分为 17 个平面（plane），每个平面包含 2^16（65,536）个码位。17 个平面的码位可表示为从 U+xx0000 到 U+xxFFFF，其中 xx 表示十六进制值从 00 到 10，共计 17 个平面。这也意味着，现在实际上采用的是 3 字节来映射字符，而且可以有映射100多万的字符，对于目前的需求来说是足够了。

其中，第一个平面称为基本多语言平面（Basic Multilingual Plane, BMP），或称第零平面（Plane 0），其他平面统称为辅助平面（Supplementary Planes），如 0x10000-0x1FFFF 为 Supplementary Multilingual Plane 等。

注意，在基本多语言平面内，从 U+D800 到 U+DFFF 之间的码位区段是永久保留不映射到 Unicode 字符的，UTF-16 就利用这段区域来对辅助平面字符的码位进行编码的，下面会对其进行详细的介绍。

如果想看看，当所有 Unicode 字符放在一块是什么样情形，可以参考 [Unicode概览][unicode-picture] ，图片有些大，加载会有些慢。<br><br>


UTF（Unicode/UCS Transformation Format） 是对 unicode 字符集的编码方案，主要包括了三种： UTF-32、UTF-16、UTF-8，其中 UTF-8 使用最为广泛。

当然，还有 UCS（Universal Character Set）编码方案，针对的是 ISO 定义的标准，当然，是与 Unicode 兼容的。主要包含了有两种格式，UCS-2 和 UCS-4，从子面意思很容可以看出，UCS-2 用两个字节编码，UCS-4 用 4 个字节（实际上只用了 31 位，最高位必须为0）编码0。在 UCS-4 中，高两个字节为 0 的码位被称作 BMP。

关于 Unicode 可以参考 [5.2版本][unicode-v5.2] ，或者 [其它版本][unicode-other-version] ，再或者可以参考 [官方网站][unicode-official] ，关于中文的部分可以参考 Code Charts 的 CJK（Chinese Japanese Korean）部分。CJK 就是中日韩的意思，Unicode 为了节省码位，将中日韩三国语言中的文字统一编码。


### UTF-32

UTF-32 又称 UCS-4 是一种将 Unicode 字符编码的协议，对每个字符都使用 4 字节。这种方法有其优点，最重要的一点就是可以在常数时间内定位字符串里的第 N 个字符，因为第 N 个字符从第 4*Nth 个字节开始，但是对于只使用 ASCII 字符集而言，存储空间同样会增大 4 倍。


### UTF-16

尽管 Unicode 字符集中的字符非常多，但是实际上大多数人不会用到超过前 65535 个以外的字符，因此，就有了另外一种 Unicode 编码方式，叫做 UTF-16。

UTF-16 是一种变长的 2 或 4 字节编码模式，对于 BMP 内的字符使用 2 字节编码，其它的则使用 4 字节组成所谓的代理对来编码。

在 BMP 平面内，从 U+D800 到 U+DFFF 之间的码位区段是永久保留不映射到 Unicode 字符，UTF-16 就利用这段区域，对辅助平面的字符码进行编码。

其编码方法是，如果字符编码小于 0x10000，也即 U+0000~U+D7FF 以及 U+E000~U+FFFF 的码位，则直接使用两字节表示。

当字符编码大于 0x10000 时会有些复杂。由于 UNICODE 编码范围最大为 0x10FFFF，从 0x10000 到 0x10FFFF 之间，共有 0xFFFFF 个编码，也就是需要 20 个 bit 就可以标示这些编码。此时采用 4bytes 表示，前两个表示为前导代理(lead surrogates)，后两者为后尾代理(trail surrogates)。

首先将该码值减 0x10000 ，前导代理取高位的 10bits 值(0x000~0x3FF)加上 0xD800 ，该值的范围为 0xD800~0xDBFF； 后尾代理则取低位的 10bits 值，然后加上 0xDC00 得到。


### UTF-8

UTF-8 是一种针对 Unicode 的可变长度字符编码，也是一种前缀码，同时也是应用最为广泛的编码方式。可以用来表示 Unicode 标准中的任何字元，而且对于 ASCII 只需要一个字节，实际上也就是说 UTF-8 是 Unicode 的实现。

UTF-8 最早可以使用一至六个字节为每个字符编码，2003年11月 UTF-8 被 RFC-3629 重新规范，只能使用原来 Unicode 定义的区域（U+0000 ~ U+10FFFF），也就是说最多会占用 4 位字节。

UTF-8 编码有如下的特点：

* 前 128 个 ASCII 字符只需一个字节编码，Unicode 范围由 U+0000 至 U+007F。

* 带有附加符号的拉丁文、希腊文、西里尔字母、亚美尼亚语、希伯来文、阿拉伯文、叙利亚文及它拿字母则需要两个字节编码，Unicode 范围由 U+0080 至 U+07FF。

* 其他基本多文种平面（BMP）中的字元，这包含了大部分常用字，如大部分的汉字，使用三个字节编码，Unicode 范围由 U+0800 至 U+FFFF。

* 其他极少使用的 Unicode 辅助平面的字元使用四字节编码，Unicode 范围由 U+10000 至 U+10FFFF 。

UTF-8 的编码规则很简单，只有二条：

1. 对于单字节的符号，字节的第一位设为 0，后面 7 位为这个符号的 unicode 码。因此对于英语字母， UTF-8 编码和 ASCII 码是相同的。

2. 对于 n 字节的符号（n>1），第一个字节的前 n 位都设为 1，第 n+1 位设为 0，后面字节的前两位一律设为 10。 剩下的没有提及的二进制位，全部为这个符号的 Unicode 码。

下表总结了编码规则，字母 x, y, z 表示可用编码的位。

![codepoint-utf8]{: .pull-center width='650'}

简单来看，对于非 ASCII 字符而言，UTF-8 的编码开头通常是 C~F 。而且，对于这样的规则，UTF-8 解码时十分方便，如果一个字节的第一位是 0，则这个字节单独就是一个字符；如果第一位是 1，则连续有多少个 1，就表示当前字符占用多少个字节。

UTF-8 在处理经常会用到的 ASCII 字符方面非常有效，处理扩展的拉丁字符集方面也不比 UTF-16 差，对于中文字符来说，比 UTF-32 要好。同时，不存在字节顺序的问题，一份以 utf-8 编码的文档在不同的计算机之间是一样的比特流。

举例如下：已知 "严" 的 Unicode 是 4E25（100111000100101），根据上表，可以发现 4E25 处在第三行的范围内（0000 0800-0000 FFFF），因此 "严" 的 UTF-8 编码需要三个字节，即格式是 "1110xxxx 10xxxxxx 10xxxxxx"。然后，从 "严" 的最后一个二进制位开始，依次从后向前填入格式中的 x，多出的位补 0。这样就得到了，"严" 的 UTF-8 编码是 "11100100 10111000 10100101"，转换成十六进制就是 E4B8A5。

关于 UTF-8 的协议，详细介绍可以直接参考 [RFC3629][utf8-rfc3629] 。



## 其它常见概念

一些关于其它相关的概念。

### little-endian VS. big-endian

对于需要使用多个字节存储的编码方式来说，如 Unicode 可以采用 UCS-2/4 格式直接存储，也即 16-bits 或者 32-bits。以汉字 "严" 为例，采用 UCS-2，Unicode 码是 4E25，需要用两个字节存储，存储的时候，4E在前，25在后，就是 big endian 方式；25在前，4E在后，就是 little endian 方式。

在 UCS 编码中有一个叫做 "ZERO WIDTH NO-BREAK SPACE" 的字符，它的编码是 FEFF，而 FFFE 在 UCS 中是不存在的字符，所以不应该出现在实际传输中。UCS 规范建议我们在传输字节流前，先传输字符 "ZERO WIDTH NO-BREAK SPACE"。

这样如果接收者收到 FEFF，就表明这个字节流是大端的；如果收到 FFFE，就表明这个字节流是小端的。同样，如果一个文本文件的头两个字节是 FE FF，就表示该文件采用大端方式；如果头两个字节是 FF FE，就表示该文件采用小端方式。

因此字符 "ZERO WIDTH NO-BREAK SPACE" 又被称作 BOM（Byte Order Mark）。

UTF-8 不需要 BOM 来表明字节顺序，但可以用 BOM 来表明编码方式。字符 EFFF 的 UTF-8 编码是 EF BB BF，所以如果接收者收到以 EF BB BF 开头的字节流，就知道这是 UTF-8 编码了。

虽然 UTF-8 不推荐使用无意义的 BOM，但许多 Windows 程序却在保存 UTF-8 文件时将其存为带 BOM 的格式，如 Windows 的记事本。

### Emoji

这是一组 12x12 像素的表情符号，来自日语词汇 “絵文字”（假名为“えもじ”，读音即emoji），最初是在 1999 年由一个名叫 Shigetaka Kurita（栗田穣崇）的日本人发明的，是为了能在消息对话中，相较于传统的纯文字或者简单的表情，提供出更丰富的感情色彩，让对话不至于被曲解。

emoji 表情随着 IOS 的普及和微信的支持越来越常见。

在 Unicode 中，emoji 就是 U+1F601~U+1F64F 区段的字符，这个编码方式超过了目前常用的 UTF-8 字符集的编码范围（U+0000~U+FFFF）。

下面是 emoji 表情符号中的部分内容。

![emoji-examples]{: .pull-center width="450"}

### 代码页 Code Page

这个出现在 Windows 系统中，是 20 世纪八九十年代用于支持多语言系统，也称为内码表。在超过 ASCII 所表示的字符后，对于不同的地域采用了不同的编码方式，如 cp936 表示中文、cp932 表示日文、cp950 表示繁体、cp1258 表示越南语等。Windows 从底层支持 Unicode 之后，代码页被逐渐取代。



## 实战

讲了这么多，接下来整点实用的，我们在 Linux 平台上通过 vim 和 MySQL 做些实验，当然 Windows 也可以通过 UtralEdit 等工具测试。

### 关于乱码

![mojibake-logo]{: .pull-right width="300"}

在解码过程中，通常使用系统默认，或者编辑器默认设置的编码方式或者系统设置的默认编码方式进行解码，如果最初的编码方式和现在的解码方式不同，那么就有可能会出现乱码。

对于中文的乱码，在 vim 中有个 fencview 的插件可以用来处理乱码，当转为正确的编码之后，还需要通过 set fileencoding 设置为本地的编码方式，然后保存文件。

下面我们通过 MySQL 测试一下，当然也可以用 vim 。假设发现有 "浣犵寽" 的乱码，而且知道现在的编码为 GBK（cp936），可以在 MySQL 中将其转换为十六进制。不过，现在只能通过经验判断其编码方式，使用 utf-8 测试一下。

{% highlight text %}
mysql> select hex(convert('浣犵寽' using gbk)) code;
+--------------+
| code         |
+--------------+
| E4BDA0E78C9C |
+--------------+
1 row in set (0.00 sec)

mysql> select convert(0xE4BDA0E78C9C using utf8) code;
+--------+
| code   |
+--------+
| 你猜   |
+--------+
1 row in set (0.00 sec)
{% endhighlight %}

可选的编码方式还有 ascii、utf16 等，详细参考 [MySQL Character Set Support][mysql-charsets] 。<br><br>


也可以在 vim 中查看，通过 set fileencoding 可以查看当前的编码，通过 set fileencoding=utf8 设置编码为 UTF-8，%!xxd 可以用来将文件转换为十六进制（%!xxd -r 恢复）。

注意，在文件中 0x0A 表示换行符，同时文件末尾也会有 0x0A 。在 vim 中，部分的编码是同义的，如 gbk=cp936、unicode=ucs-2，设置完响应的编码之后，可以再次查看。


### Emoji 的影响

如上介绍的 emoji 相关信息，那么 emoji 字符表情会对我们平时的开发运维带来什么影响呢？

如果使用 UTF-8 来存储，那么实际需要 4Bytes ，而现在常见的 UTF-8 只支持 3Bytes，如 MySQL 。在 MySQL 中，utf8mb4 在 5.5 以后才被支持，那么当把 emoji 存入数据库的时候就会报错。
{% highlight text %}
mysql> set session sql_mode='TRADITIONAL';
Query OK, 0 rows affected (0.01 sec)

mysql> create table test(msg varchar(20)) engine=InnoDB charset=utf8 collate=utf8_general_ci;
Query OK, 0 rows affected (0.03 sec)

mysql> insert into test values(0xF09D8C86);
ERROR 1366 (22007): Incorrect string value: '\xF0\x9D\x8C\x86' for column 'msg' at row 1
{% endhighlight %}
如果不设置 sql_mode 则会显示一条 Warning（可以通过 show warnings 查看） ，此时可以插入数据，但是插入数据为空值。为了支持 4Bytes，在建表的时候需要指定 charset=utf8mb4 collate=utf8mb4_general_ci 。

当然，也可以通过如下方式进行简单测试。
{% highlight text %}
mysql> SELECT HEX('this is a test str')
+--------------------------------------+
| HEX('this is a test str')            |
+--------------------------------------+
| 746869732069732061207465737420737472 |
+--------------------------------------+
1 row in set (0.00 sec)

mysql> SELECT 0x746869732069732061207465737420737472
+----------------------------------------+
| 0x746869732069732061207465737420737472 |
+----------------------------------------+
| this is a test str                     |
+----------------------------------------+
1 row in set (0.00 sec)

mysql> select convert(0xF09D8C86 using utf8) code;
+------+
| code |
+------+
| NULL |
+------+
1 row in set, 1 warning (0.00 sec)

mysql> show warnings;
+---------+------+-----------------------------------------+
| Level   | Code | Message                                 |
+---------+------+-----------------------------------------+
| Warning | 1300 | Invalid utf8 character string: 'F09D8C' |
+---------+------+-----------------------------------------+
1 row in set (0.00 sec)

{% endhighlight %}


### Tips

一些常见的问题。

* 在 vim 中可以通过，set nobomb 和 set bomb 取消或者设置 BOM ，通过 set bomb? 查看。以小端为例，UCS-4 开头为 0000 FEFF ，UCS-2 为 FEFF 。

* 对于 CentOS7 的字体，保存在 /usr/share/fonts 目录下，可以通过 Tweak Tool 设置。

还可以参考一个入门的资料 [The Absolute Minimum Every Software Developer Absolutely, Positively Must Know About Unicode and Character Sets][basic-intro] 。


<!-- sites -->
[gb2312-charsets]:        http://www.knowsky.com/resource/gb2312tbl.htm                      "GB2312 字符集"
[gb18030-charsets]:       http://demo.icu-project.org/icu-bin/convexp?conv=gb18030           "GB18030 字符集"

[unicode-official]:       http://unicode.org/                                                "Unicode 官方网站"
[unicode-v5.2]:           http://www.unicode.org/Public/5.2.0/charts/CodeCharts-noHan.pdf    "Unicode V5.2"
[unicode-other-version]:  http://unifoundry.com/pub/                                         "其它版本的 Unicode"
[utf8-rfc3629]:           http://www.ietf.org/rfc/rfc3629.txt                                "UTF-8 协议介绍"
[mysql-charsets]:         http://dev.mysql.com/doc/refman/5.7/en/charset.html                "MySQL 支持的字符集"

[basic-intro]:            http://www.joelonsoftware.com/articles/Unicode.html                "对字符集的简单介绍"

<!-- pictures -->
[charsets-logo]:          /images/charsets/charsets-logo.jpg                                 "字符编码logo"

[ascii-table]:            /images/charsets/ASCII-table.jpg                                   "ASCII 字符集"
[ascii-table-ext]:        /images/charsets/ASCII-table-ext.png                               "ASCII 字符集扩展"
[ISO-8859-1]:             /images/charsets/ISO-8859-1.jpg                                    "ISO-8859-1 字符集"
[gb2312-example]:         /images/charsets/GB2312-B0A1.gif                                   "GB2312 字符集示例"
[big5-example]:           /images/charsets/BIG5-B0A1.gif                                     "BIG5 字符集示例"
[gb18030-encoding]:       /images/charsets/GB18030-encoding.jpg                              "GB18030 编码方式"

[unicode-picture]:        http://unifoundry.com/pub/unifont-7.0.03/unifont-7.0.03.bmp        "Unicode 展示在一张图上的全局概览"
[unicode-tai-symbols]:    /images/charsets/unicode-tai-xuan-jing-symbols.png                 "Tai xuan jing symbols"
[unicode-mahjong]:        /images/charsets/unicode-mahjong-tiles.png                         "mahjong tiles"
[unicode-domino]:         /images/charsets/unicode-domino-tiles.png                          "domino tiles"
[codepoint-utf8]:         /images/charsets/unicode-codepoint-utf8.png                        "码位和utf-8之间的转换"

[mojibake-logo]:         /images/charsets/mojibake-logo.gif                                  "乱码的logo"

[emoji-examples]:         /images/charsets/emoji-examples.png                                "emoji 字符"

