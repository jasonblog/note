---
title: PGP 簡介
layout: post
comments: true
language: chinese
category: [linux,misc]
keywords: linux,pgp
description: PGP 是一個基於公鑰加密體系的加密軟件，功能強大，有很快的速度，而且源碼是免費的。這裡簡單介紹下 PGP 相關內容。
---

GnuPGP 是一個基於公鑰加密體系的加密軟件，功能強大，有很快的速度，而且源碼是免費的。

這裡簡單介紹下 PGP 相關內容。

<!-- more -->

## 簡介

Pretty Good Privacy, PGP 加密由一系列散列、數據壓縮、對稱密鑰加密，以及公鑰加密的算法組合而成，遵守 OpenPGP 數據加密標準 [RFC 4880](http://tools.ietf.org/rfc/rfc4880.txt) ，每個步驟支持幾種算法，可以選擇一個使用。

<!-- https://tools.ietf.org/html/rfc3156 -->

注意 PGP 是一個商用軟件，對應的開源軟件為 GnuPGP ，兩者都遵守 OpenPGP 標準。

![pgp diagram]({{ site.url }}/images/security/pgp-diagram.png "pgp diagram"){: .pull-center }


通常，使用如上的加密和解密過程大致為。

1. 用戶產生需要加密的信息。
2. OpenPGP 發送方產生一串隨機數，作為對稱加密密鑰，這一隨機數只對該信息或該會話有效。
3. 使用接受者的公鑰加密上述的隨機數 (密鑰)，放置到需要發送消息的開頭。
4. 通過上述產生的隨機數加密需要加密的信息，通常會先對信息進行壓縮。
5. 接收方則會先通過私鑰解密隨機數，然後解密信息。

當然，也可以先產生一個數字簽名，並將數字簽名添加到信息中，然後同樣會通過隨機串加密。在 OpenPGP 中也可以對信息進行壓縮，通常壓縮是在獲得數字簽名後且在壓縮之前。

> The Public-Key Cryptography Standards(PKCS) 是由美國 RSA 數據安全公司及其合作伙伴制定的一組公鑰密碼學標準，包括證書申請、證書更新、證書作廢表發佈、擴展證書內容以及數字簽名、數字信封的格式等方面的一系列相關協議。

<!--
    PGP的實際操作由五種服務組成：鑑別、機密性、電子郵件的兼容性、壓縮、分段和重裝。

    1.1 鑑別

    步驟如下：
    （1）發送者創建報文；
    （2）發送者使用SHA-1生成報文的160bit散列代碼（郵件文摘）；
    （3）發送者使用自己的私有密鑰，採用RSA算法對散列代碼進行加密，串接在報文的前面；
    （4）接收者使用發送者的公開密鑰，採用RSA解密和恢復散列代碼；
    （5）接收者為報文生成新的散列代碼，並與被解密的散列代碼相比較。如果兩者匹配，則報文作為已鑑別的報文而接收。
    另外，簽名是可以分離的。例如法律合同，需要多方簽名，每個人的簽名是獨立的，因而可以僅應用到文檔上。否則，簽名將只能遞歸使用，第二個簽名對文檔的第一個簽名進行簽名，依此類推。

    1.2 機密性
    在PGP中，每個常規密鑰只使用一次，即對每個報文生成新的128bit的隨機數。為了保護密鑰，使用接收者的公開密鑰對它進行加密。描述如下：
    （1）發送者生成報文和用作該報文會話密鑰的128bit隨機數；
    （2）發送者採用CAST-128加密算法，使用會話密鑰對報文進行加密。也可使用IDEA或3DES；
    （3）發送者採用RSA算法，使用接收者的公開密鑰對會話密鑰進行加密，並附加到報文前面；
    （4）接收者採用RSA算法，使用自己的私有密鑰解密和恢復會話密鑰；
    （5）接收者使用會話密鑰解密報文。
    除了使用RSA算法加密外，PGP還提供了DiffieHellman的變體EIGamal算法。

    3 公開密鑰管理

    3.1 公開密鑰管理機制
    一個成熟的加密體系必然要有一個成熟的密鑰管理機制配磁。公鑰體制的提出就是為了解決傳統加密體系的密鑰分配過程不安全、不方便的缺點。例如網絡黑客們常用的手段之一就是“監聽”，通過網絡傳送的密鑰很容易被截獲。對PGP來說，公鑰本來就是要公開，就沒有防監聽的問題。但公鑰的發佈仍然可能存在安全性問題，例如公鑰被篡改（public key tampering），使得使用公鑰與公鑰持有人的公鑰不一致。這在公鑰密碼體系中是很嚴重的安全問題。因此必須幫助用戶確信使用的公鑰是與他通信的對方的公鑰。

    以用戶A和用戶B通信為例，現假設用戶A想給用戶B發信。首先用戶A就必須獲取用戶B的公鑰，用戶A從BBS上下載或通過其它途徑得到B的公鑰，並用它加密信件發給B。不幸的是，用戶A和B都不知道，攻擊者C潛入BBS或網絡中，偵聽或截取到用戶B的公鑰，然後在自己的PGP系統中以用戶B的名字生成密鑰對中的公鑰，替換了用戶B的公鑰，並放在BBS上或直接以用戶B的身份把更換後的用戶B的“公鑰”發給用戶A。那A用來發信的公鑰是已經更改過的，實際上是C偽裝B生成的另一個公鑰（A得到的B的公鑰實際上是C的公鑰/密鑰對，用戶名為B）。這樣一來B收到A的來信後就不能用自己的私鑰解密了。更可惡的是，用戶C還可偽造用戶B的簽名給A或其他人發信，因為A手中的B的公鑰是仿造的，用戶A會以為真是用戶B的來信。於是C就可以用他手中的私鑰來解密A給B的信，還可以用B真正的公鑰來轉發A給B的信，甚至還可以改動A給B的信。

    3.2 防止篡改公鑰的方法

    （1）直接從B手中得到其公鑰，這種方法有侷限性。

    （2）通過電話認證密鑰：在電話上以radix-64的形式口述密鑰或密鑰指紋。密鑰指紋（keys fingerprint）就是PGP生成密鑰的160bit的SHA-1摘要（16個8位十六進制）。

    （3）從雙方信任的D那裡獲得B的公鑰。如果A和B有一個共同的朋友D，而D知道他手中的B的公鑰是正確的。D簽名的B的公鑰上載到BBS上讓用戶去拿，A想要獲得B的公鑰就必須先獲取D的公鑰來解密BBS或網上經過D簽名的B的公鑰，這樣就等於加了雙重保險，一般沒有可能去篡改而不被用戶發現，即使是BBS管理員。這就是從公共渠道傳遞公鑰的安全手段。有可能A拿到的D或其他簽名的朋友的公鑰也是假的，但這就要求攻擊者C必須對三人甚至很多人都很熟悉，這樣的可能性不大，而且必需經過長時間的策劃。

    只通過一個簽名證力度可能是小了一點，於是PGP把用不同私鑰簽名的公鑰收集在一起，發送到公共場合，希望大部分從至少認識其中一個，從而間接認證了用戶（A）的公鑰。同樣用戶（D）簽了朋友（A）的公鑰後應該寄回給他（A）（朋友），這樣可以讓他（A）通過該用戶（D）被該用戶（D）的其他朋友所認證。與現實中人的交往一樣。PGP會自動根據用戶拿到的公鑰分析出哪些是朋友介紹來的簽名的公鑰，把它們賦以不同的信任級別，供用戶參考決定對它們的信任程度。也可指定某人有幾層轉介公鑰的能力，這種能力隨著認證的傳遞而遞減的。

    （4）由一個普通信任的機構擔當第三方，即“認證機構”。這樣的“認證機構”適合由非個人控制的組織或政府機構充當，來註冊和管理用戶的密鑰對。現在已經有等級認證制定的機構存在，如廣東省電子商務電子認證中心（WWW.cnca.net）就是一個這樣的認證機構。對於那些非常分散的用戶，PGP更贊成使用私人方式的密鑰轉介。

    3.3 信任的使用

    PGP確實為公開密鑰附加侂任和開發信任信息提供了一種方便的方法使用信任。

    公開密鑰環的每個實體都是一個公開的密鑰證書。與每個這親的實體相聯繫的是密鑰合法性字段，用來指示PGP信任“這是這個用戶合法的公開密鑰”的程度；信任程度越高，這個用戶ID與這個密鑰的綁定越緊密。這個字段由PGP計算。與每個實體相聯繫的還有用戶收集的多個簽名。反過來，每個簽名都帶有簽名信任字段，用來指示該PGP用戶信任簽名者對這個公開密鑰證明的程度。密鑰合法性字段是從這個實體的一組簽名信任字節中推導出來的。最後，每個實體定義了與特定的擁有者相聯繫的公開密鑰，包括擁有者信任字段，用來指示這個公開密鑰對其他公開密鑰證書進行簽名的信任程度（這個信任程度是由該用戶指定的）。可以把簽名信任字段看成是來自於其他實體的擁有者信任字段的副本。

    例如正在處理用戶A的公開密鑰環，操作描述如下：

    （1）當A在公開密鑰環中插入了新的公開密鑰時，PGP為與這個公開密鑰擁有者相關聯的信任標誌賦值，插入KUa，則賦值=1終極信任；否則，需說明這個擁有者是未知的、不可任信的、少量信任的和完全可信的等，賦以相應的權重值1/x、1/y等。

    （2）當新的公開密鑰輸入後，可以在它上面附加一個或多個簽名，以後還可以增加更多的簽名。在實體中插入簽名時，PGP在公開密鑰環中搜索，查看這個簽名的作者是否屬於已知的公開密鑰擁有者。如果是，為這個簽名的SIGTRUST字段賦以該擁的者的OWNERTRUST值。否則，賦以不認識的用戶值。

    （3）密鑰合法性字段的值是在這個實體的簽名信任字段的基礎上計算的。如果至少一個簽名具有終極信任的值，那麼密鑰合法性字段的設置為完全；否則，PGP計算信任值的權重和。對於總是可信任的簽名賦以1/x的權重，對於通常可信任的簽名賦以權重1/y，其中x和y都是用戶可配置的參數。當介紹者的密鑰/用戶ID綁定的權重總達到1時，綁定被認為是值得信任的，密鑰合法性被設置為完全。因此，在沒有終極信任的情況下，需要至少x個簽名總是可信的，或者至少y個簽名是可信的，或者上述兩種情況的某種組合。如圖4所示。

    總之，PGP採用了RSA和傳統加密的雜合算法，用於數字簽名的郵件文摘算法、加密前壓縮等，可以用來加密文件，還可以代替Uuencode生成RADIX 64格式（就是MIME的BASE 64格式）的編碼文件。PGP創造性地把RSA公鑰體系的方便和傳統加密體系的高速度結合起來，並且在數字簽名和密鑰認證管理機制上有巧妙的設計。這是目前最難破譯的密碼體系之一。

    用戶通過PGP的軟件加密程序，可以在不安全的通信鏈路上創建安全的消息和通信。PGP協議已經成為公鑰加密技術和全球範圍內消息安全性的事實標準。因為所有人都能看到它的源代碼，從而查找出故障和安全性漏局
-->

### 信任模型

PGP 的好處在於它的信任模型 \-\- Web Trust Model，一種非常貼近生活中人與人的信任模型。在信息世界裡面，人與人之間的信任紐帶有兩種方式被建立。

#### 集中式

如 CA 結構，大家都信任 CA ，於是 Peter 是不是 Peter ，是根據 CA 是否認識這個 Peter ，我們現在通常所說的的數字證書認證，就是這種集中認證的模式。

#### 分佈式

假設沒有CA，只信任自己和朋友，每個人都是在一個朋友圈子中，朋友圈子以外的人，則需要一箇中間人作為介紹人，這就是現實的生活。

我有一個朋友叫 Andy，且我有另一個朋友叫 Bob，如果 Bob 想認識 Andy，於是他想通過我來認識 Andy，方法很簡單，鑑於我認識他們兩者，通過我為 Bob 的公鑰簽名，於是 Andy 可以直接向 Bob 證明他是我的朋友。

PGP 採用的後者是這種方式，每個人都以自己為中心，然後是朋友圈，然後是朋友的朋友。


## GnuPGP

公鑰算法的關鍵點在於如何傳播公鑰，如果有惡意用戶傳遞了一把虛假公鑰，此時，如果別人不知就裡，用這把公鑰加密信息，持有該虛假鑰匙的侵入者就可以解密而讀到信息。而如果侵入者再將解密的訊息加以修改，並以真正的公開鑰匙加密，然後傳送出去，這種進攻無法被發現。

PGP 的解決方法是對公鑰進行簽名，也即生成指紋 (fingerprint)，只有公鑰會有指紋。每個公鑰均綁定唯一的用戶身份 (包括用戶名、註釋、郵箱)，當然一個人的公鑰可以由別人來簽名，**簽名 (指紋) 是用來測試該公鑰是否是由其聲明的用戶發出的** 。

至於有多信任這些簽名，完全取決於 GPG 用戶，當你信任給這把鑰匙簽名的人時，你認為這把鑰匙是可信的，並確信這把鑰匙確實屬於擁有相應用戶身份的人。只有當你信任簽名者的公開鑰匙時，你才能信任這個簽名。要想絕對確信一把鑰匙是正確和真實的，你就得在給予絕對信任之前，通過可靠渠道比較鑰匙的 "指紋"。

### 使用

在 ```~/.gnupg``` 目錄下，保存了一些必須的文件，其中密鑰以加密形式存儲在磁盤上，包括了兩個文件，一個存儲公鑰 (```pubring.gpg```)、一個存儲私鑰 (```secring.gpg```)。

GPG 會通過 username、comment、email 生成一個 UID(hash value)，在使用時可以用其中的一種方式。

#### 1. 生成密鑰對 (公鑰+私鑰)

生成時需要選擇 A) 加密算法、密鑰長度 (長度越長解密時間越長，不過也會更安全)、密鑰的有效時間；B) 區分該密鑰對的用戶名、e-mail、註釋 (這些信息是可以修改的)；C) 一個保護該私鑰的驗證密碼。

{% highlight text %}
$ gpg --gen-key
Please select what kind of key you want:                  # 選擇加密以及指紋算法
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
You need a Passphrase to protect your secret key.         # 輸入密碼

gpg: key FCC7A473 marked as ultimately trusted            # 用戶的hash值，可以用來替代userid
public and secret key created and signed.

gpg: checking the trustdb
gpg: 3 marginal(s) needed, 1 complete(s) needed, PGP trust model
gpg: depth: 0  valid:   1  signed:   0  trust: 0-, 0q, 0n, 0m, 0f, 1u
pub   2048R/FCC7A473 2016-07-06
      Key fingerprint = ACB2 B707 B2D8 A548 55A8  5E24 A5CC BA5C FCC7 A473   # 可以用來電話驗證^_^
uid                  foobar (just for test) <foobar@ooops.com>               # 三元素組成userid
sub   2048R/3B44DA00 2016-07-06
{% endhighlight %}

生成的公鑰 (```pubring.gpg```)、私鑰 (```secring.gpg```) 保存在 ```~/.gnupg/``` 目錄下。

<!-- 生成密碼時需要花費一些時間獲取隨機數據，通常選擇 2048~4096bit 的 RSA 加密方式。最後需要輸入一個密碼，不知道是作什麼用的 -->

#### 2. 撤銷公鑰

當密鑰對生成之後，應該立即做一個公鑰回收證書，當忘記了私鑰的口令或者私鑰丟失或被盜竊，可以發佈這個證書來聲明以前的公鑰不再有效。

{% highlight text %}
$ gpg --output revoke.asc --gen-revoke "user-name/e-mail/user-id"           # 生成回收證書，以備不時之需
$ gpg --import revoke.asc                              # 導入回收證書
$ gpg --keyserver keys.gnugp.net --send "8D253E8A"     # 發送回收證書到服務器，聲明原GPG Key作廢
{% endhighlight %}

當然如果要這麼做，一定要先有私鑰，否則任何人都能取消你的公鑰。

#### 3. 傳播公鑰

導出公鑰之後，就可以通過一些可靠的方式傳播公鑰。可以將公鑰通過 e-mail 發送給好友，或者上傳到指定的服務器(如果不指定使用默認的 [keys.gnupg.net](http://keys.gnupg.net) ，其它的還有 [keyserver.ubuntu.com](http://keyserver.ubuntu.com/) 。

{% highlight text %}
$ gpg --keyserver keys.gnugp.net --send-keys "8D253E8A"                     # 上傳到服務器
$ gpg --fingerprint                                                         # 打印公鑰指紋
$ gpg --keyserver keys.gnugp.net --search-keys "user-name/e-mail/user-all"  # 在服務器上查找
{% endhighlight %}

由於公鑰服務器沒有檢查機制，任何人都可以用你的名義上傳公鑰，所以沒有辦法保證服務器上的公鑰的可靠性。通常，可以在網站上公佈一個公鑰指紋，讓其他人核對下載到的公鑰是否為真。

#### 4. 查看密鑰

{% highlight text %}
$ gpg --list-keys                       # 輸出現有公鑰，包括UID、公私鑰的HASH值，等同於gpg -k
$ gpg --list-sigs                       # 同時顯示簽名，實際也就是上述的HASH值
$ gpg --fingerprint                     # 輸出公鑰的指紋
$ gpg --list-secret-keys                # 列出私鑰，PS. 列出私鑰的指紋和簽名根本就沒用，等同於gpg -K
{% endhighlight %}

通過 ```gpg -k``` 列出所有公鑰，其中第一行表示保存的文件名 (```pubring.gpg```)；第二行為公鑰的特徵 (加密方式:如 ```2048R``` 表示 ```2048-bit RSA```，```user-id``` 的 ```hash``` 值，生成時間)；第三行表示用戶 ID ；第四行表示私鑰特徵。

#### 5. 導出公鑰

將指定用戶的公鑰以 ASCII 的格式導出。

{% highlight text %}
$ gpg --armor --export "user-name/e-mail/user-id"                           # 導出ASCII公鑰到終端
$ gpg --armor --output public-key.gpg --export "user-name/e-mail/user-id"   # 導出ASCII公鑰到文件，或者-o
$ gpg --armor --export-secret-keys                                          # 導出私鑰
$ gpg --refresh-keys                                                        # 從服務器定期更新公鑰
{% endhighlight %}

其中 ```--armor``` 表示將內容轉換成可見的 ASCII 碼輸出，否則是二進制不方便閱讀。

#### 6. 導入公鑰

除了生成自己的密鑰，還需要別人的公鑰，用於生成加密信息或者驗證數字簽名等。為此，就需要將他人的公鑰或者你的其他密鑰輸入系統，這時可以使用 import 參數。

{% highlight text %}
$ gpg --import public-key.gpg                                                       # 導入ASCII公鑰到文件
$ gpg --keyserver hkp://subkeys.pgp.net --search-keys "user-name/e-mail/user-all"   # 從服務器查看然後選擇
$ gpg --keyserver subkeys.gpg.net --recv-keys 8D253E8A                              # 直接導入
{% endhighlight %}

由於任何人都可以導入公鑰，我們無法保證服務器上的公鑰是否可靠，下載後還需要用其他機制驗證，如數字簽字。

#### 7. 刪除密鑰

我們可以通過如下命令刪除密鑰。

{% highlight text %}
$ gpg --delete-keys "user-name/e-mail/user-id"                    # 從公鑰鑰匙環裡刪除密鑰
$ gpg --delete-secret-keys "user-name/e-mail/user-id"             # 從私鑰鑰匙環裡刪除密鑰
$ gpg --delete-secret-and-public-key "user-name/e-mail/user-id"   # 同時刪除公鑰私鑰
{% endhighlight %}

#### 8. 修改密鑰

用此命令你可以修改鑰匙的失效日期，加進一個指紋，對鑰匙簽名等等。 儘管顯得太清楚而不用提，這裡還是要說，要做以上事情你得用你的通行句。 敲入通行句後，你會見到命令行。

{% highlight text %}
$ gpg --edit-key "user-name/e-mail/user-id"
{% endhighlight %}

#### 9. 加密/解密

在安裝和按照需要設置好所有事以後，我們就可以開始加密和解密了。

{% highlight text %}
$ gpg --recipient "8D253E8A" --output demo.en.txt --encrypt demo.txt   # 用指定用戶公鑰加密

$ gpg --output demo.de.txt --decrypt demo.en.txt                       # 解密，可以不指定用戶名
$ gpg demo.en.txt                                                      # 解密，最簡單方式
{% endhighlight %}

```--recipient``` 參數指定接收者的公鑰，```--output``` 指定加密後的文件名，```--encrypt``` 指定源文件。

#### 10. 簽名

有時只需要對文件簽名，表示這個文件確實是我本人發出的。

{% highlight text %}
$ gpg --sign demo.txt                   # 生成二進制簽名demo.txt.gpg，同時包含文本內容
$ gpg --clearsign demo.txt              # 生成ASCII的簽名文件demo.txt.asc，同時包含文本內容
$ gpg --detach-sign demo.txt            # 只生成二進制簽名demo.txt.sig
$ gpg --armor --detach-sign demo.txt    # 只生成ASCII簽名demo.txt.asc
$ gpg --verify demo.txt.asc demo.txt    # 驗證解密的文件與簽名是否相符

$ gpg --armor --detach-sign -u UID demo.txt    # 可以指定用戶
{% endhighlight %}


#### 11. 簽名+加密

此時簽名和加密同時保存在同一個文件中。

{% highlight text %}
$ gpg --local-user "user-name/e-mail/user-id" --recipient "8D253E8A" --armor --sign --encrypt demo.txt
$ gpg demo.txt.asc                      # 解密
{% endhighlight %}


### 公鑰的管理

正如前言所提到的，這個系統有一個最大的薄弱點，那就是公鑰的真實性問題。可以通過直接向公鑰的提供者當面或者電話驗證指紋，從而確定公鑰的真實性問題。

當確定了公鑰的真實性之後可以對公鑰簽名，表示你絕對確信這把鑰匙是真實的，有了這個保證，用戶就可以開始放心用這把鑰匙加密了。

要對一把鑰匙簽名，用 ```gpg --edit-key UID``` ，然後用 sign 命令。

GPG 會根據現有的簽名和對 "主人的信任度" 來決定鑰匙的真實性，包括了四個值。

{% highlight text %}
1 = 我不知道
2 = 我不信任（這把鑰匙）
3 = 我勉強信任
4 = 我完全信任
{% endhighlight %}

對於用戶不信任的簽名，就會將廢棄這個簽名不用。注意這些信任信息不是存在儲存鑰匙的文件裡，而是存在另一個文件裡。

<!--

<br><h2>編輯密鑰</h2><p>
另外，一些縮寫的對應方式大致為
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
通過 subkey 可以更方便管理密鑰，以及在某些情況下可以保護好密鑰。例如電腦丟了，



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
$ gpg --gen-key                          // 此時會要求輸入主密碼，以後會用的到
pub   2048R/43E315AF 2015-05-30
      Key fingerprint = 70B5 4B54 B31A 32E8 2E0B  F4A0 67DC A662 43E3 15AF
uid                  foobar (test) &lt;foobar@oops.com&gt;
sub   2048R/2065CC7D 2015-05-30

$ gpg --edit-key 43E315AF                // 使用公鑰ID，添加新的UID
gpg> adduid                              // 輸入新的UID，最後確認時需要上述的密碼
... ...                                  // 此時包含了兩個UID
gpg> uid 2
gpg> primary                             // 將新的設置為主
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
上述產生的密碼中，43E315AF 含有SC標誌，是主密碼；2065CC7D 含有E標誌，是用於加密的從密碼； AD5CC9E2 含有S標誌，用於Sign的從密碼。<br><br>

然後可以生成的撤銷證書，可以加密後保存在單獨的地方，最好網絡不可達的，如一個加密的U盤，當然也可以打印出來，因為需要輸入的內容很少。<br><br>

接下來，就是比較關鍵的部分了。將主健(master key)從電腦上刪除，僅留下子鍵(subkeys)，然後將主健保存在加密U盤即可。
<pre style="font-size:0.8em; face:arial;">
$ rsync -avp $HOME/.gnupg /media/encrypted-usb               // 將數據同步到加密的磁盤，或者同下
$ umask 077; tar -cf gnupg-backup-new.tar -C $HOME .gnupg

$ gpg --export-secret-subkeys 43E315AF &gt; subkeys          // 將subkeys導出
$ gpg --delete-secret-key 43E315AF                           // 刪除主健，此時私鑰已經刪除，公鑰還在
$ gpg --import subkeys
</pre>
注意，此時主健保存在了加密後的U盤上，所以不要丟掉U盤。可以通過如下查看刪除主鑰後的差別。
<pre style="font-size:0.8em; face:arial;">
$ gpg -K 43E315AF
sec#  2048R/43E315AF 2015-05-30
... ...
$ gpg --home=gnupg -K 43E315AF
sec  2048R/43E315AF 2015-05-30
</pre>
刪除主健後顯示 # 字符，此時可以將新的健上傳到服務器。<br><br>

此時，仍然可以作簽名。
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


## 參考

關於如何使用 subkey [Creating a new GPG key with subkeys](https://www.void.gr/kargig/blog/2013/12/02/creating-a-new-gpg-key-with-subkeys/) 一個不錯的介紹，可以參考 [本地文檔](/reference/security/Creating_a_new_GPG_key_with_subkeys.maff)；另一篇參考的是最佳實踐，可以參考 [OpenPGP Best Practices](https://help.riseup.net/en/security/message-security/openpgp/best-practices) ，或者 [本地文檔](/reference/security/OpenPGP_Best_Practices.maff) 。

一個官方的中文文檔 [GnuPG 袖珍 HOWTO](https://www.gnupg.org/howtos/zh/index.html) ，或者英文版 [The GNU Privacy Handbook](https://www.gnupg.org/gph/en/manual.html) 。

<!--
GnuPG 的官方為
https://www.gnupg.org/index.html

Speed Hashing，包括了加密揭祕速度
http://blog.codinghorror.com/speed-hashing/

關於SHA1～SHA3的介紹
http://www.answers.com/article/1163617/the-evolution-of-the-nist-secure-hash-algorithm-from-sha-1-to-sha-3

http://www.ruanyifeng.com/blog/2013/07/gpg.html
-->


{% highlight text %}
{% endhighlight %}
