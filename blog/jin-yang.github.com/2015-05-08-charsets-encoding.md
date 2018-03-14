---
title: 字符集與編碼雜談
layout: post
comments: true
language: chinese
category: [misc]
keywords: 字符集,編碼
description: 是不是還分不清什麼是字符集，什麼是編碼，兩者到底有什麼區別？ 什麼是 GB2312、GB18030、BIG5？ Unicode 和 UTF-8、UTF-16、UTF-32 到底什麼關係？ BMP、UCS、emoji 又是什麼？ 為什麼會出現亂碼？OK，接下來我們介紹一下他們之間剪不斷理還亂的關係 ... ...
---

是不是還分不清什麼是字符集，什麼是編碼，兩者到底有什麼區別？ 什麼是 GB2312、GB18030、BIG5？ Unicode 和 UTF-8、UTF-16、UTF-32 到底什麼關係？ BMP、UCS、emoji 又是什麼？ 為什麼會出現亂碼？

OK，接下來我們介紹一下他們之間剪不斷理還亂的關係 ... ...

<!-- more -->

## 為什麼會有字符集和編碼

![charsets-logo]{: .pull-right width="350"}

首先說一下什麼是字符集，簡單地說，那就是一堆的符號，一堆我們可以看得見（如 A、B、C）或者看不見（如一些控制字符，像回車、振鈴等）的東西。當然，不同的字符集可能會包含不同的範圍，可能只包含了英語用的字符，也可能包含了一些中文字符 ... ...

實際上，字符集通常與不同地域的語言相關，當然也可能包含了所有的字符，下面會做介紹。

OK，那什麼是編碼。我們知道，在計算機中儲存的信息實際上都是用二進制數表示的 Bytes 流，而我們在屏幕上看到的英文、漢字、標點符號等字符都是二進制數轉換之後的結果。而 A *<font color='red' size='6'>A</font>* **A** 或者 <font face='atrament-web-1' size='6'>A</font> 都表示是同一個字符，只是最後轉換顯示的格式不同。

通俗的說，按照何種規則將字符存儲在計算機中，就是編碼方式。如 'a' 用什麼樣的二進制表示，稱為 "編碼"；反之，將存儲在計算機中的二進制數解析並顯示出來，稱為 "解碼"。當然，如果在解碼過程中，使用了錯誤的解碼規則，可能會導致 'a' 會解析成 'b' 或者直接導致亂碼（mojibake）。<br><br>

而字符集實際就是一堆字符的集合，對於一個字符集來說要正確編碼解碼一個字符需要三個關鍵元素：抽象字庫表（abstract character repertoire）、編碼字符集（coded character set）、字符編碼（character encoding form），當然還有一個不太重要的字節序。

其中抽象字庫表包括了所有字符的數據庫，決定了整個字符集能夠表示的所有字符的範圍，字符就是各種文字和符號的總稱，包括各國家文字、標點符號、圖形符號、數字等。編碼字符集，用來表示抽象字庫表中對應字符的表示方法，通常使用用數子來表示，如 Unicode 中採用碼值（code point）來標示一個字符在字庫中的位置，GB2312 用區位碼標示。字符編碼，用來指定編碼字符集和實際存儲數值之間的轉換關係。

為什麼要整的這麼複雜。其實，原因也是比較明瞭的，抽象字庫表的目的是為了表示所能夠涵蓋字符的範圍，可能值包含了部分字符（如ASCII），當然，也有可能打算包含這個世界上所有的字符（如Unicode）。但實際使用過程中會發現，真正用的上的字符相對整個字庫表來說比例非常低，例如在美國 ASCII 基本就能滿足需求，中文地區的程序幾乎不會需要日語字符。而如果把每個字符都用字庫表中的序號來存儲的話，每個字符就需要 3 個字節（以 Unicode 字庫為例），這樣對於原本用僅佔一個字符的 ASCII 編碼的英語地區國家顯然是一個額外成本，存儲體積是原來的三倍。

為此，就把字符集和對應的編碼方式分開，在保證字符集不變的情況下，可以靈活地改變編碼方式。

有些字符集會直接將字符集中的編號與字符編碼對應，如在 ASCII 中，'A' 在表中排第 65 位，而編碼後 A 的數值也是 65；而有些則不同，如在 unicode 中，'中'的碼值為 3197，而用 UTF-8 表示為 E4B8AD 。<br><br>


簡單的說，實際用到的概念主要是 **字符集** 和 **編碼方式**。其中 **字符集決定了我們所能使用的字符**，例如，當使用 ASCII 字符集的時候，不能表示中文。**編碼方式則決定了這些字符在磁盤上的存儲方式**，如對於 Unicode 字符集，我可以選擇 UTF-8、UTF-16、UTF-32 中的一種方式進行存儲。


常見字符集有，ASCII 字符集、GB2312 字符集、BIG5 字符集、GB18030 字符集、Unicode 字符集等。


## ASCII

ASCII（American Standard Code for Information Interchange, 美國信息交換標準代碼）是最簡單的字符集了。是一套基於拉丁字母的編碼系統，主要用於顯示現代英語，其擴展版本 EASCII 可以勉強顯示其他西歐語言。

![ascii-table]{: .pull-center width='750' }

ASCII 碼一共規定了 128 個字符的編碼，只佔用了一個字節的後面 7 位，最前面的 1 位統一規定為 0。<br><br>

另一個單字節編碼是 ISO-8859-1，也就是 MySQL 上的 latin1 編碼，其編碼範圍是 0x00~0xFF。並向下兼容 ASCII，其中 0x00~0x7F 與 ASCII 一致，0x80~0x9F 之間是控制字符，0xA0~0xFF 之間是文字符號，新加的字符如下。

![ISO-8859-1]{: .pull-center }

此字符集支持部分於歐洲使用的語言，包括阿爾巴尼亞語、丹麥語、荷蘭語、法羅語、德語、格陵蘭語、冰島語、愛爾蘭蓋爾語、意大利語、拉丁語、挪威語、葡萄牙語、裡託羅曼斯語、蘇格蘭蓋爾語、西班牙語等；也可以支持歐洲以外的部分語言，如南非荷蘭語、斯瓦希里語、印尼語及馬來語等。


## 中文編碼

最初的計算機，只採用 ASCII 就可以完成編碼，但是當傳播到天朝之後，如何顯示中文就成了不可避免的一個問題，必須有種方法可以顯示中文的字符。

### GB2312

GB2312 又稱國標碼，是指國家 1980 的一個標準，由國家標準總局發佈，1981.5.1 開始實施，通行於大陸，新加坡等地也使用此編碼。

GB2312 規定 "對任意一個圖形字符都採用兩個字節表示，每個字節均採用七位編碼表示"，稱第一個字節為高字節，也稱為"區"；第二個字節為低字節，也稱為"位"，所以也稱為區位碼。GB2312 是一個簡化字的編碼規範，也包括其他的符號、字母、日文假名等，一共收錄了 7445 個圖形字符，其中包括漢字 6763 個和 682 個其它符號。

在這些編碼中，同時將 ASCII 本來就有的數字、標點、字母都重新編了兩個字節長的編碼，這就是常說的 "全角" 字符，而原來在 127 號以下的那些就叫 "半角" 字符了。

GB2312 將代碼表分為 94 個區，對應第一字節（0xA1-0xFE）；每個區 94 個位（0xA1-0xFE），對應第二字節，兩個字節的值分別為區號值和位號值。01-09 區為符號、數字區；16-87 區為漢字區；10-15區、88-94區是有待進一步標準化的空白區。

漢字區的編碼範圍高字節從 B0-F7，低字節從 A1-FE，佔用的碼位是 72*94=6768，其中有 5 個空位是 D7FA-D7FE，也就是如上所述的，包括了 6763 個漢字。

如，對於漢字 "啊" 的區位碼是 1601，寫成 16 進制是 0x10 0x01。而這和計算機廣泛使用的 ASCII 編碼衝突，為了兼容 00-7f 的 ASCII 編碼，我們在區位碼的高、低字節上分別加上 A0。這樣 "啊" 的編碼就成為 B0A1，我們將加過兩個 A0 的編碼也稱為 GB2312 編碼。

![gb2312-example]{: .pull-center}

GB2312 的詳細編碼可以參考 [GB2312 簡體中文編碼表][gb2312-charsets] 。


### BIG5

Big5 又稱大五碼，主要為香港、臺灣、澳門等地域使用，是一個繁體字編碼，共收錄了 13,060 個漢字。每個漢字由兩個字節構成，第一個字節的範圍從 0x81-0xFE，共 126 種；第二個字節的範圍不連續，分別為 0x40~0x7E 和 0xA1-0xFE，共 157 種。

![big5-example]{: .pull-center}

如圖所示，GB2312 和 BIG5 的字符集對應的編碼是不兼容的。


### GB18030

GB2312 所收錄的漢字已經覆蓋中國大陸 99.75% 的使用頻率，但對於人名、古漢語等方面出現的罕用字，仍不能處理，這導致了後來 GBK 及 GB18030 漢字字符集的出現。

在 1995 年對 GB2312 進行了擴充，擴展為規範 GBK1.0。GBK 中每個漢字仍然包含兩個字節，第一個字節的範圍是 0x81-0xFE，第二個字節的範圍是 0x40-0xFE，也就是 GBK 中有碼位 23940 個，該字符集收錄了 21886 個符號，分為漢字區和圖形符號區，漢字區包括 21003 個字符，同時還包括了繁體（不過與 Big5 不兼容）。

2000 年的 GB18030 取代 GBK1.0 正式成為國家標準，編碼空間約為 160 萬碼位。該標準收錄了 27484 個漢字，同時還收錄了藏文、蒙文、維吾爾文等主要的少數民族文字。從漢字字彙上說，GB18030 在 GB13000.1 的 20902 個漢字的基礎上增加了 CJK 擴展 A 的 6582 個漢字（Unicode 碼 0x3400-0x4db5），一共收錄了 27484 個漢字。

GB18030 的編碼採用單字節、雙字節和 4 字節方案，其中單字節、雙字節和 GBK 是完全兼容的。4 字節編碼的碼位就是收錄了 CJK 擴展 A 的 6582 個漢字。

GB18030 標準採用單字節、雙字節和四字節三種方式對字符編碼。單字節部分使用 0×00 至 0×7F 碼位，對應於 ASCII 碼的相應碼位。雙字節部分，首字節碼位從 0x81 至 0xFE，尾字節碼位分別是 0x40 至 0x7E 和 0x80 至 0xFE。四字節部分採用 0x30 到 0x39 作為對雙字節編碼擴充的後綴，這樣擴充的四字節編碼，其中第一、三個字節編碼碼位均為 0x81 至 0xFE，第二、四個字節編碼碼位均為 0x30 至 0x39。碼位總體結構見下圖。

![gb18030-encoding]{: .pull-center}

從 ASCII、GB2312 到 GBK，這些編碼方法是向下兼容的，即同一個字符在這些方案中總是有相同的編碼，只是後面的標準可以支持更多的字符。在這些編碼中，中英文可以統一處理，區分中文編碼的方法是高字節的最高位不為 0。

關於 GB18030 可以參考 [GB18030][gb18030-charsets] 。

### 其它

微軟的 CP936 通常被視為等同 GBK，事實上比較起來，GBK 定義之字符較 CP936 多出 95 字，15 個非漢字及 80 個漢字。



## Unicode

像天朝一樣，在計算機傳到世界各個國家後，為了適合當地語言和字符，都會設計和實現類似 GB2312/GBK/GB18030/BIG5 的編碼方案。這樣各搞一套，在本地使用沒有問題，一旦出現在網絡中，由於不兼容，互相訪問時就會出現亂碼現象。於是乎就出現了萬能的 Unicode，Unicode 的發展由一個非營利機構 "統一碼聯盟" 負責。

當然，當時意識這一問題的還有一個國際標準化組織（ISO），採用通用字符集（Universal Character Set，UCS），也就是由 ISO 制定的 ISO-10646 標準所定義的標準字符集。在 1991 年前後，兩個項目開始合併雙方的工作成果，併為創立一個單一字符集而協同工作，從 Unicode 2.0 開始，Unicode 採用了與 ISO 10646-1 相同的字庫和字碼；同時 ISO 也承諾，ISO-10646 將不會替超出 U+10FFFF 的 UCS-4 編碼賦值，以使得兩者保持一致。當然，現在兩者仍然在獨立發佈著自己的標準。

Unicode，又叫做萬國碼、國際碼、統一碼、單一碼，是計算機科學領域裡的一項業界標準，對世界上大部分的文字系統進行了整理、編碼，使得電腦可以用更為簡單的方式來呈現和處理不同地域的文字。而且，Unicode 至今仍在不斷增修，每個新版本都加入更多新的字符。目前最新的穩定版本為 2014.6.16 公佈的 7.0.0（8.0 會在 2015.6 公佈），已收入超過十萬個字符。

Unicode 編碼系統就是為表達任意語言、任意字符而設計，使用 4 字節來表達字母、符號，或者表意文字（ideograph），每個數字代表唯一的至少在某種語言中使用的符號。被幾種語言共用的字符通常使用相同的數字來編碼，除非存在一個合理的語源學（etymological）理由不這樣做。不考慮這種情況的話，每個字符對應一個數字，每個數字對應一個字符，即不存在二義性。如 U+0041 總是代表 'A'，即使這種語言沒有 'A' 這個字符。

Unicode 不但包含了一些語言對應的字符，還包括了一堆其它的 ... ...

![unicode-tai-symbols]{: .pull-center width="380"}

額，先天八卦？易有太極，是生兩儀，兩儀生四象，四象生八卦 ... ...

![unicode-mahjong]{: .pull-center width="280"}

好吧，麻將 ！！！

![unicode-domino]{: .pull-center width="450"}

小時候玩過的骨牌 ^_^ 或者是多米諾 ... ...



### 字符集編碼詳解

Unicode 的編碼空間從 U+0000 到 U+10FFFF，共有 1,112,064 個碼位（code point）可用來映射字符，其編碼空間劃分為 17 個平面（plane），每個平面包含 2^16（65,536）個碼位。17 個平面的碼位可表示為從 U+xx0000 到 U+xxFFFF，其中 xx 表示十六進制值從 00 到 10，共計 17 個平面。這也意味著，現在實際上採用的是 3 字節來映射字符，而且可以有映射100多萬的字符，對於目前的需求來說是足夠了。

其中，第一個平面稱為基本多語言平面（Basic Multilingual Plane, BMP），或稱第零平面（Plane 0），其他平面統稱為輔助平面（Supplementary Planes），如 0x10000-0x1FFFF 為 Supplementary Multilingual Plane 等。

注意，在基本多語言平面內，從 U+D800 到 U+DFFF 之間的碼位區段是永久保留不映射到 Unicode 字符的，UTF-16 就利用這段區域來對輔助平面字符的碼位進行編碼的，下面會對其進行詳細的介紹。

如果想看看，當所有 Unicode 字符放在一塊是什麼樣情形，可以參考 [Unicode概覽][unicode-picture] ，圖片有些大，加載會有些慢。<br><br>


UTF（Unicode/UCS Transformation Format） 是對 unicode 字符集的編碼方案，主要包括了三種： UTF-32、UTF-16、UTF-8，其中 UTF-8 使用最為廣泛。

當然，還有 UCS（Universal Character Set）編碼方案，針對的是 ISO 定義的標準，當然，是與 Unicode 兼容的。主要包含了有兩種格式，UCS-2 和 UCS-4，從子面意思很容可以看出，UCS-2 用兩個字節編碼，UCS-4 用 4 個字節（實際上只用了 31 位，最高位必須為0）編碼0。在 UCS-4 中，高兩個字節為 0 的碼位被稱作 BMP。

關於 Unicode 可以參考 [5.2版本][unicode-v5.2] ，或者 [其它版本][unicode-other-version] ，再或者可以參考 [官方網站][unicode-official] ，關於中文的部分可以參考 Code Charts 的 CJK（Chinese Japanese Korean）部分。CJK 就是中日韓的意思，Unicode 為了節省碼位，將中日韓三國語言中的文字統一編碼。


### UTF-32

UTF-32 又稱 UCS-4 是一種將 Unicode 字符編碼的協議，對每個字符都使用 4 字節。這種方法有其優點，最重要的一點就是可以在常數時間內定位字符串裡的第 N 個字符，因為第 N 個字符從第 4*Nth 個字節開始，但是對於只使用 ASCII 字符集而言，存儲空間同樣會增大 4 倍。


### UTF-16

儘管 Unicode 字符集中的字符非常多，但是實際上大多數人不會用到超過前 65535 個以外的字符，因此，就有了另外一種 Unicode 編碼方式，叫做 UTF-16。

UTF-16 是一種變長的 2 或 4 字節編碼模式，對於 BMP 內的字符使用 2 字節編碼，其它的則使用 4 字節組成所謂的代理對來編碼。

在 BMP 平面內，從 U+D800 到 U+DFFF 之間的碼位區段是永久保留不映射到 Unicode 字符，UTF-16 就利用這段區域，對輔助平面的字符碼進行編碼。

其編碼方法是，如果字符編碼小於 0x10000，也即 U+0000~U+D7FF 以及 U+E000~U+FFFF 的碼位，則直接使用兩字節表示。

當字符編碼大於 0x10000 時會有些複雜。由於 UNICODE 編碼範圍最大為 0x10FFFF，從 0x10000 到 0x10FFFF 之間，共有 0xFFFFF 個編碼，也就是需要 20 個 bit 就可以標示這些編碼。此時採用 4bytes 表示，前兩個表示為前導代理(lead surrogates)，後兩者為後尾代理(trail surrogates)。

首先將該碼值減 0x10000 ，前導代理取高位的 10bits 值(0x000~0x3FF)加上 0xD800 ，該值的範圍為 0xD800~0xDBFF； 後尾代理則取低位的 10bits 值，然後加上 0xDC00 得到。


### UTF-8

UTF-8 是一種針對 Unicode 的可變長度字符編碼，也是一種前綴碼，同時也是應用最為廣泛的編碼方式。可以用來表示 Unicode 標準中的任何字元，而且對於 ASCII 只需要一個字節，實際上也就是說 UTF-8 是 Unicode 的實現。

UTF-8 最早可以使用一至六個字節為每個字符編碼，2003年11月 UTF-8 被 RFC-3629 重新規範，只能使用原來 Unicode 定義的區域（U+0000 ~ U+10FFFF），也就是說最多會佔用 4 位字節。

UTF-8 編碼有如下的特點：

* 前 128 個 ASCII 字符只需一個字節編碼，Unicode 範圍由 U+0000 至 U+007F。

* 帶有附加符號的拉丁文、希臘文、西裡爾字母、亞美尼亞語、希伯來文、阿拉伯文、敘利亞文及它拿字母則需要兩個字節編碼，Unicode 範圍由 U+0080 至 U+07FF。

* 其他基本多文種平面（BMP）中的字元，這包含了大部分常用字，如大部分的漢字，使用三個字節編碼，Unicode 範圍由 U+0800 至 U+FFFF。

* 其他極少使用的 Unicode 輔助平面的字元使用四字節編碼，Unicode 範圍由 U+10000 至 U+10FFFF 。

UTF-8 的編碼規則很簡單，只有二條：

1. 對於單字節的符號，字節的第一位設為 0，後面 7 位為這個符號的 unicode 碼。因此對於英語字母， UTF-8 編碼和 ASCII 碼是相同的。

2. 對於 n 字節的符號（n>1），第一個字節的前 n 位都設為 1，第 n+1 位設為 0，後面字節的前兩位一律設為 10。 剩下的沒有提及的二進制位，全部為這個符號的 Unicode 碼。

下表總結了編碼規則，字母 x, y, z 表示可用編碼的位。

![codepoint-utf8]{: .pull-center width='650'}

簡單來看，對於非 ASCII 字符而言，UTF-8 的編碼開頭通常是 C~F 。而且，對於這樣的規則，UTF-8 解碼時十分方便，如果一個字節的第一位是 0，則這個字節單獨就是一個字符；如果第一位是 1，則連續有多少個 1，就表示當前字符佔用多少個字節。

UTF-8 在處理經常會用到的 ASCII 字符方面非常有效，處理擴展的拉丁字符集方面也不比 UTF-16 差，對於中文字符來說，比 UTF-32 要好。同時，不存在字節順序的問題，一份以 utf-8 編碼的文檔在不同的計算機之間是一樣的比特流。

舉例如下：已知 "嚴" 的 Unicode 是 4E25（100111000100101），根據上表，可以發現 4E25 處在第三行的範圍內（0000 0800-0000 FFFF），因此 "嚴" 的 UTF-8 編碼需要三個字節，即格式是 "1110xxxx 10xxxxxx 10xxxxxx"。然後，從 "嚴" 的最後一個二進制位開始，依次從後向前填入格式中的 x，多出的位補 0。這樣就得到了，"嚴" 的 UTF-8 編碼是 "11100100 10111000 10100101"，轉換成十六進制就是 E4B8A5。

關於 UTF-8 的協議，詳細介紹可以直接參考 [RFC3629][utf8-rfc3629] 。



## 其它常見概念

一些關於其它相關的概念。

### little-endian VS. big-endian

對於需要使用多個字節存儲的編碼方式來說，如 Unicode 可以採用 UCS-2/4 格式直接存儲，也即 16-bits 或者 32-bits。以漢字 "嚴" 為例，採用 UCS-2，Unicode 碼是 4E25，需要用兩個字節存儲，存儲的時候，4E在前，25在後，就是 big endian 方式；25在前，4E在後，就是 little endian 方式。

在 UCS 編碼中有一個叫做 "ZERO WIDTH NO-BREAK SPACE" 的字符，它的編碼是 FEFF，而 FFFE 在 UCS 中是不存在的字符，所以不應該出現在實際傳輸中。UCS 規範建議我們在傳輸字節流前，先傳輸字符 "ZERO WIDTH NO-BREAK SPACE"。

這樣如果接收者收到 FEFF，就表明這個字節流是大端的；如果收到 FFFE，就表明這個字節流是小端的。同樣，如果一個文本文件的頭兩個字節是 FE FF，就表示該文件採用大端方式；如果頭兩個字節是 FF FE，就表示該文件採用小端方式。

因此字符 "ZERO WIDTH NO-BREAK SPACE" 又被稱作 BOM（Byte Order Mark）。

UTF-8 不需要 BOM 來表明字節順序，但可以用 BOM 來表明編碼方式。字符 EFFF 的 UTF-8 編碼是 EF BB BF，所以如果接收者收到以 EF BB BF 開頭的字節流，就知道這是 UTF-8 編碼了。

雖然 UTF-8 不推薦使用無意義的 BOM，但許多 Windows 程序卻在保存 UTF-8 文件時將其存為帶 BOM 的格式，如 Windows 的記事本。

### Emoji

這是一組 12x12 像素的表情符號，來自日語詞彙 “絵文字”（假名為“えもじ”，讀音即emoji），最初是在 1999 年由一個名叫 Shigetaka Kurita（慄田穣崇）的日本人發明的，是為了能在消息對話中，相較於傳統的純文字或者簡單的表情，提供出更豐富的感情色彩，讓對話不至於被曲解。

emoji 表情隨著 IOS 的普及和微信的支持越來越常見。

在 Unicode 中，emoji 就是 U+1F601~U+1F64F 區段的字符，這個編碼方式超過了目前常用的 UTF-8 字符集的編碼範圍（U+0000~U+FFFF）。

下面是 emoji 表情符號中的部分內容。

![emoji-examples]{: .pull-center width="450"}

### 代碼頁 Code Page

這個出現在 Windows 系統中，是 20 世紀八九十年代用於支持多語言系統，也稱為內碼錶。在超過 ASCII 所表示的字符後，對於不同的地域採用了不同的編碼方式，如 cp936 表示中文、cp932 表示日文、cp950 表示繁體、cp1258 表示越南語等。Windows 從底層支持 Unicode 之後，代碼頁被逐漸取代。



## 實戰

講了這麼多，接下來整點實用的，我們在 Linux 平臺上通過 vim 和 MySQL 做些實驗，當然 Windows 也可以通過 UtralEdit 等工具測試。

### 關於亂碼

![mojibake-logo]{: .pull-right width="300"}

在解碼過程中，通常使用系統默認，或者編輯器默認設置的編碼方式或者系統設置的默認編碼方式進行解碼，如果最初的編碼方式和現在的解碼方式不同，那麼就有可能會出現亂碼。

對於中文的亂碼，在 vim 中有個 fencview 的插件可以用來處理亂碼，當轉為正確的編碼之後，還需要通過 set fileencoding 設置為本地的編碼方式，然後保存文件。

下面我們通過 MySQL 測試一下，當然也可以用 vim 。假設發現有 "浣犵寽" 的亂碼，而且知道現在的編碼為 GBK（cp936），可以在 MySQL 中將其轉換為十六進制。不過，現在只能通過經驗判斷其編碼方式，使用 utf-8 測試一下。

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

可選的編碼方式還有 ascii、utf16 等，詳細參考 [MySQL Character Set Support][mysql-charsets] 。<br><br>


也可以在 vim 中查看，通過 set fileencoding 可以查看當前的編碼，通過 set fileencoding=utf8 設置編碼為 UTF-8，%!xxd 可以用來將文件轉換為十六進制（%!xxd -r 恢復）。

注意，在文件中 0x0A 表示換行符，同時文件末尾也會有 0x0A 。在 vim 中，部分的編碼是同義的，如 gbk=cp936、unicode=ucs-2，設置完響應的編碼之後，可以再次查看。


### Emoji 的影響

如上介紹的 emoji 相關信息，那麼 emoji 字符表情會對我們平時的開發運維帶來什麼影響呢？

如果使用 UTF-8 來存儲，那麼實際需要 4Bytes ，而現在常見的 UTF-8 只支持 3Bytes，如 MySQL 。在 MySQL 中，utf8mb4 在 5.5 以後才被支持，那麼當把 emoji 存入數據庫的時候就會報錯。
{% highlight text %}
mysql> set session sql_mode='TRADITIONAL';
Query OK, 0 rows affected (0.01 sec)

mysql> create table test(msg varchar(20)) engine=InnoDB charset=utf8 collate=utf8_general_ci;
Query OK, 0 rows affected (0.03 sec)

mysql> insert into test values(0xF09D8C86);
ERROR 1366 (22007): Incorrect string value: '\xF0\x9D\x8C\x86' for column 'msg' at row 1
{% endhighlight %}
如果不設置 sql_mode 則會顯示一條 Warning（可以通過 show warnings 查看） ，此時可以插入數據，但是插入數據為空值。為了支持 4Bytes，在建表的時候需要指定 charset=utf8mb4 collate=utf8mb4_general_ci 。

當然，也可以通過如下方式進行簡單測試。
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

一些常見的問題。

* 在 vim 中可以通過，set nobomb 和 set bomb 取消或者設置 BOM ，通過 set bomb? 查看。以小端為例，UCS-4 開頭為 0000 FEFF ，UCS-2 為 FEFF 。

* 對於 CentOS7 的字體，保存在 /usr/share/fonts 目錄下，可以通過 Tweak Tool 設置。

還可以參考一個入門的資料 [The Absolute Minimum Every Software Developer Absolutely, Positively Must Know About Unicode and Character Sets][basic-intro] 。


<!-- sites -->
[gb2312-charsets]:        http://www.knowsky.com/resource/gb2312tbl.htm                      "GB2312 字符集"
[gb18030-charsets]:       http://demo.icu-project.org/icu-bin/convexp?conv=gb18030           "GB18030 字符集"

[unicode-official]:       http://unicode.org/                                                "Unicode 官方網站"
[unicode-v5.2]:           http://www.unicode.org/Public/5.2.0/charts/CodeCharts-noHan.pdf    "Unicode V5.2"
[unicode-other-version]:  http://unifoundry.com/pub/                                         "其它版本的 Unicode"
[utf8-rfc3629]:           http://www.ietf.org/rfc/rfc3629.txt                                "UTF-8 協議介紹"
[mysql-charsets]:         http://dev.mysql.com/doc/refman/5.7/en/charset.html                "MySQL 支持的字符集"

[basic-intro]:            http://www.joelonsoftware.com/articles/Unicode.html                "對字符集的簡單介紹"

<!-- pictures -->
[charsets-logo]:          /images/charsets/charsets-logo.jpg                                 "字符編碼logo"

[ascii-table]:            /images/charsets/ASCII-table.jpg                                   "ASCII 字符集"
[ascii-table-ext]:        /images/charsets/ASCII-table-ext.png                               "ASCII 字符集擴展"
[ISO-8859-1]:             /images/charsets/ISO-8859-1.jpg                                    "ISO-8859-1 字符集"
[gb2312-example]:         /images/charsets/GB2312-B0A1.gif                                   "GB2312 字符集示例"
[big5-example]:           /images/charsets/BIG5-B0A1.gif                                     "BIG5 字符集示例"
[gb18030-encoding]:       /images/charsets/GB18030-encoding.jpg                              "GB18030 編碼方式"

[unicode-picture]:        http://unifoundry.com/pub/unifont-7.0.03/unifont-7.0.03.bmp        "Unicode 展示在一張圖上的全局概覽"
[unicode-tai-symbols]:    /images/charsets/unicode-tai-xuan-jing-symbols.png                 "Tai xuan jing symbols"
[unicode-mahjong]:        /images/charsets/unicode-mahjong-tiles.png                         "mahjong tiles"
[unicode-domino]:         /images/charsets/unicode-domino-tiles.png                          "domino tiles"
[codepoint-utf8]:         /images/charsets/unicode-codepoint-utf8.png                        "碼位和utf-8之間的轉換"

[mojibake-logo]:         /images/charsets/mojibake-logo.gif                                  "亂碼的logo"

[emoji-examples]:         /images/charsets/emoji-examples.png                                "emoji 字符"

