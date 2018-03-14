---
title: Linux 時間同步
layout: post
comments: true
language: chinese
category: [linux,program,misc]
keywords: linux,時間,ntp,ntpdate
description: 簡單介紹在 Linux 中部分與時間相關的概念，例如時區、閏秒、夏令時、ntp 等。
---

簡單介紹在 Linux 中部分與時間相關的概念，例如時區、閏秒、夏令時、ntp 等。

<!-- more -->

## 簡介

"現在幾點了？" 或者 "離過年還有多久？" 這恐怕連小學生也覺得是再簡單不過的問題了；但是如果問及 "時間是什麼？" 恐怕絕大多數人都會頓覺茫然。

在中學生數學、物理課本中，時間被表述成一條有起點、有單位、有指向、無始無終的直線，這就是符合人們常識的牛頓 "絕對時間"，也是直到本世紀初被普遍接受的科學的時間概念。

進入二十世紀後，物理學、天文學的新成果、新發現向 "絕對時間" 的基本觀念提出了挑戰。愛因斯坦狹義相對論指出，時間不能脫離宇宙及其事件的觀察者而獨立存在，時間是宇宙與其觀察者之間的聯繫的一個方面。愛因斯坦廣義相對論的一個直接推論是，由於引力場的原因處於地球表面不同高度的時鐘走速不一樣，海拔越高鐘速越快，差值約為 1.09×10-16 秒/米（海拔），即每升高100米，時鐘變快百萬億分之一秒。


"時間是什麼？" 這一個問題實質上是探索時間的本質，這只是極少數科學家、哲學家熱心研究的課題，而且遠沒有得出一個令人滿意的結果，看來還需要長期探索下去。

> 關於世界時的介紹
>
> 地球自轉運動是個相當不錯的天然時鐘，以它為基礎可以建立一個很好的時間計量系統。地球自轉的角度可用地方子午線相對於天球上的基本參考點的運動來度量。為了測定地球自轉，人們在天球上選取了兩個基本參考點：春分點和平太陽，以此確定的時間分別稱為恆星時和平太陽時。恆星時雖然與地球自轉的角度相對應，符合以地球自轉運動為基礎的時間計量標準的要求，但不能滿足日常生活和應用的需要。人們習慣上是以太陽在天球上的位置來確定時間的，但因為地球繞太陽公轉運動的軌道是橢圓，所以真太陽週日視運動的速度是不均勻的（即真太陽時是不均勻的）。為了得到以真太陽週日視運動為基礎而又克服其不均勻性的時間計量系統，人們引進了一個假想的參考點─平太陽。它在天赤道上作勻速運動，其速度與真太陽的平均速度相一致。
>
> 平太陽時的基本單位是平太陽日，1平太陽日等於24平太陽小時，86400平太陽秒。以平子夜作為0時開始的格林威治平太陽時，就稱為世界時，簡稱UT。世界時與恆星時有嚴格的轉換關係，人們是通過觀測恆星得到世界時的。後來發現，由於地極移動和地球自轉的不均勻性，最初得到的世界時，記為UT0，也是不均勻的，人們對UT0 加上極移改正得到UT1，如果再加上地球自轉速率季節性變化的經驗改正就得到UT2。
>
> 六十年代以前，世界時作為基本時間計量系統被廣泛應用，因為它與地球自轉的角度有關，所以即使出現了更為均勻的原子時系統，世界時對於日常生活、大地測量、天文導航及其它有關地球的科學仍是必需的。

![ntp rigui logo]({{ site.url }}/images/misc/ntp-rigui-ex.jpg "ntp rigui logo"){: .pull-center width="60%"}


日晷名稱是由“日”和“晷”兩字組成。“日”指“太陽”，“晷”表示“影子”，“日晷”的意思為“太陽的影子”。因此，所謂日晷，就是白天通過測日影定時間的儀器。日晷計時的原理是這樣。在一天中，被太陽照射到的物體投下的影子在不斷地改變著，第一是影子的長短在改變，早晨的影子最長，隨著時間的推移，影子逐漸變短，一過中午它又重新變長；第二是影子的方向在改變，因為我們在北半球，早晨的影子在西方，中午的影子在北方，傍晚的影子在東方。從原理上來說，根據影子的長度或方向都可以計時，但根據影子的方向來計時更方便一些。故通常都是以影子的方位計時。由於日晷必須依賴日照，不能用於陰天和黑夜。因此，單用日晷來計時是不夠的，還需要其他種類的計時器，如水鍾，來與之相配。


<!--
時間計量系統    時間測量誤差（秒/天）   頻率準確度
世界時(UT)  10-3    10-8
曆書時(ET)  10-4    10-9
原子時(TA)  10-14   10-16
-->

## 時區 TimeZone

網上聊天時，有人問你說現在幾點? 你看了看錶回答他說晚上 8 點了，但是這個在歐洲的哥們有點納悶了，因為他那裡還太陽當空呢！

![ntp time zone]({{ site.url }}/images/misc/ntp-time-zone-logo.jpg "ntp time zone"){: .pull-center width="70%"}

實際上，人們習慣於按照太陽的位置對當地的時間進行劃分，通常是太陽在正中時作為 12 點。因為地球是圓的，在環繞太陽旋轉的 24 個小時中，世界各地日出日落的時間是不一樣的，所以我們才有劃分時區的必要。

在真正使用時，為了照顧到行政上的方便，時區並不嚴格按南北直線來劃分，而是按自然條件來劃分。例如，我國差不多跨 5 個時區，但實際上在只用東八時區的標準時即北京時間為準。

![ntp time zone]({{ site.url }}/images/misc/ntp-global-time-zone.jpg "ntp time zone"){: .pull-center width="100%"}

1884 年在華盛頓召開的一次國際經度會議，又稱國際子午線會議，規定將全球劃分為 24 個時區，其中東、西各 12 個時區，而且規定英國 (格林尼治天文臺舊址) 為中時區 (零時區)，每個時區橫跨經度 15 度，時間正好是1小時；最後的東、西第 12 區各跨經度 7.5 度，以東、西經 180 度為界。


<!--
地理課上我們都學過格林威治時間(GMT), 它也就是0時區時間. 但是我們在計算機中經常看到的是UTC. 它是Coordinated Universal Time的簡寫. 雖然可以認為UTC和GMT的值相等(誤差相當之小),但是UTC已經被認定為是國際標準,所以我們都應該遵守標準只使用UTC

那麼假如現在中國當地的時間是晚上8點的話,我們可以有下面兩種表示方式
20:00 CST
12:00 UTC

這裡的CST是Chinese Standard Time,也就是我們通常所說的北京時間了. 因為中國處在UTC+8時區,依次類推那麼也就是12:00 UTC了.
-->

### Linux 下時區設置

在 Linux 中的 glibc 提供很多編譯好的 timezone 文件，存放在 /usr/share/zoneinfo 目錄下，基本涵蓋了大部分的國家和城市。

{% highlight text %}
# ls -F /usr/share/zoneinfo/
Africa/      Chile/   Factory    Iceland      Mexico/   posix/      Universal
America/     CST6CDT  GB         Indian/      Mideast/  posixrules  US/
Antarctica/  Cuba     GB-Eire    Iran         MST       PRC         UTC
Arctic/      EET      GMT        iso3166.tab  MST7MDT   PST8PDT     WET
Asia/        Egypt    GMT0       Israel       Navajo    right/      W-SU
Atlantic/    Eire     GMT-0      Jamaica      NZ        ROC         zone.tab
Australia/   EST      GMT+0      Japan        NZ-CHAT   ROK         Zulu
Brazil/      EST5EDT  Greenwich  Kwajalein    Pacific/  Singapore
Canada/      Etc/     Hongkong   Libya        Poland    Turkey
CET          Europe/  HST        MET          Portugal  UCT
{% endhighlight %}

在這裡面就可以找到自己所在城市的 time zone 文件，文件的內容可以通過 zdump 查看，

{% highlight text %}
# zdump /usr/share/zoneinfo/Asia/Shanghai
Shanghai  Sun Jan  1 07:46:01 2013 Shanghai
{% endhighlight %}

可以通過設置 /etc/localtime 符號鏈接，或者設置 TZ 環境變量設置本地的時區，其中後者優先。

{% highlight text %}
----- 1. 直接添加符號鏈接
# cd /etc
# ln -sf ../usr/share/zoneinfo/Asia/Shanghai localtime

----- 2. 首先根據提示選擇時區，然後設置環境變量
# tzselect
# export TZ='America/Los_Angeles'
{% endhighlight %}

注意，第二種方法需要添加到 profile 文件中。

## 閏秒

首先，看下我們當前對秒的定義。

* 1956 年，秒定義為平均太陽日的 1/86400；
* 1960 至 1967 年之間，定義為 1960 年地球自轉一週時間的 1/86400；
* 1967 年，國際計量大會決定，以銫原子基態的兩個超精細能級間在零磁場下躍遷輻射 9,192,631,770 週期所持續的時間；
* 1977 年，人類認識到了引力時間膨脹會導致在不同高度的原子鐘時間不同，重新定義為 "水平面的銫原子" 基態的兩個超精細能級間在零磁場下躍遷輻射 9,192,631,770 週期所持續的時間；
* 2019 年，國際時間頻率諮詢委員會將討論“秒”的重新定義問題。

隨著人類認知的進步，對秒的定義逐漸發生了變化，從 1967 年開始，時間的計量標準便正式由天文學的宏觀領域過渡到了物理學的微觀領域。

### 常見概念

接著看下與閏秒相關的概念。

#### UT (Universal Time，世界時)

一種以格林尼治子夜起算的平太陽時，由於以地球自轉為基準，觀測精度受限於地球的自轉速度的穩定，地球體積不均勻、潮汐引力以及其他星球的擾動的原因，導致地球轉速不穩定，每日誤差達數毫秒。

#### TAI (International Atomic Time) 國際原子時

1971 年建立，利用某些元素(如銫、氫、銣)的原子能級躍遷頻率有極高穩定性的特性定義時間標準，現為國際計量局 (BIPM) 的時間部門維持，綜合全球約 60 個實驗室中的大約 240 臺各種自由運轉的原子鐘提供的數據進行處理，得出 "國際時間標準" 稱為國際原子時 (TAI)，每日誤差為數納秒。

TAI 時間原點為 UT 1958.01.01 00:00:00，在此之後 TAI 就沿著原子秒的節拍一直走下去，和 UT 誤差也越來越大。

#### UTC (協調世界時)

如上所述，現在有兩種時間計量系統：基於天文測量而得出的 "世界時" 和以物理學發展發現的原子振盪週期固定這一特性而來的 "原子時" 。

UTC 就是用於將世界時 (UT，天文時間) 和國際原子時 (TAI，原子時間) 協調起來另外一套計量系統。1971 年國際計量大會通過決議，使用 UTC 來計量時間，協調的原則就是 UTC 以原子秒長節拍，在時刻上儘量接近於世界時 (UT)。

目前 UTC 是事實上的時間標準，比如所有計算機中的時間就是 UTC 時間，通過時區換算為本地時間，而閏秒實際上就是 UTC 特有的。

### 閏秒

閏秒簡單來說，就是為了讓 UTC 儘量靠近 UT 時間做出的人為調整。

詳細來說，世界時依靠天文觀測因此誤差較大 (每日數毫秒)，而原子時依靠原子的物理特性，相對精度較高 (每日誤差幾個納秒)，UTC 是以原子秒長為基礎，在時刻上儘量接近於世界時的一種時間計量系統。

為了確保 UTC 與 UT 儘量接近，保證誤差小於 0.9 秒，當 UTC 與 UT 之間的誤差超過 0.6 秒時，國際地球自轉服務組織 (IERS) 就會決定在最快到來的閏秒調整日期 (目前基本都是 6.30 或 12.31) 的最後一分鐘，對 UTC 增加或減少一秒，也就是這一分鐘將變為 61 秒或 59 秒， 這個增加或減少的一秒即為閏秒。

#### 閏秒操作

如果增加一秒，稱為正閏秒，會在調整日的 23:59:59 後額外插入一個 23:59:60，然後再到次日的 00:00:00，這樣當日最後一分鐘就有 61 秒。

如果減少一秒，則為負閏秒，會在調整日的 23:59:58 秒後跳過 23:59:59 這一秒，直接到達次日 00:00:00， 這樣當日最後一分鐘就只有 59 秒。

當前為止所有的閏秒調整都是正閏秒，也就是說地球轉的越來越慢了。

### 系統狀態

注意，閏秒的插入是在 UTC 時間，所以不同的時區閏秒寫入時間會有所區別。

{% highlight text %}
# ntpq -crv
associd=0 status=4519 **leap_add_sec**(leap_none), sync_ntp, 1 event, leap_armed(clock_sync),
version="ntpd 4.2.6p5@1.2349-o Mon Feb  6 07:34:43 UTC 2017 (1)",
processor="x86_64", system="Linux/3.10.0-514.10.2.el7.x86_64", leap=00,
stratum=3, precision=-18, rootdelay=70.647, rootdisp=142.974,
refid=59.46.44.253, reftime=dcb86ce1.cc4a9654  **Sat, May  6 2017 23:40:17.798**,
clock=dcb86dd0.e933c60d  Sat, May  6 2016 23:44:16.910, peer=41148, tc=6,
mintc=3, offset=-18.414, frequency=16.806, sys_jitter=83.733,
clk_jitter=49.584, clk_wander=6.979, tai=35, leapsec=201507010000,
expire=201512280000
{% endhighlight %}

由於閏秒是整數加入，因此 UTC 與 TAI 之間會出現若干整數秒差別，從 1958.01.01 00:00:00 這一 TAI 時間開始，到 1970.01.01 00:00:00 (epoch time，UNIX 0 second)，UTC 已經比 TAI 慢了 10 秒，後經過多次閏秒調整，2015.07.01 00:00:00 開始 (閏秒調整後)，UTC 將比 TAI 慢 36 秒。

在內核中，可以通過 ```"inserting leap second"``` 關鍵詞查看。

另外，需要注意，內核日誌顯示的是 23:59:60 這一閏秒，實際驗證時發現不一定會出現 23:59:60 這一時間點，而是和本機的 tzdata 版本有關，如果未升級到包含閏秒信息的最新版本的話，結果會是重複一次 23:59:59 。

可以通過 ```while true; do date; sleep 1; done``` 命令查看。

## Linux 時間設置

首先，需要明確兩個概念，在一臺計算機上我們有兩個時鐘：A) 硬件時間時鐘 (Real Time Clock, RTC)；B) 系統時鐘 (System Clock) 。

硬件時鐘是主板上的一特殊電路，由單獨的電池供電，從而保證關機之後還可以計算時間。系統時鐘就是內核用來計算時間的時鐘，是從 1970-01-01 00:00:00 UTC 到目前為止秒數總和。

在 Linux 下系統時間在開機的時候會和硬件時間同步，之後也就各自獨立運行了；當然，時間久了之後必然就會產生誤差。可以通過如下命令查看系統時間以及硬件時間。

{% highlight text %}
----- 1. 查看系統時間和硬件時間
# date
Fri Jul  6 00:27:13 CST 2007
# hwclock --show
Fri 06 Jul 2007 12:27:17 AM CST  -0.968931 seconds

----- 2. 時間同步
# hwclock --hctosys               # 把硬件時間設置成系統時間
# hwclock --systohc               # 把系統時間設置成硬件時間

----- 3. 時間設置
# hwclock --set --date="mm/dd/yy hh:mm:ss"
# date -s "dd/mm/yyyy hh:mm:ss"
{% endhighlight %}

實際上服務器的時鐘與標準時間隨著時間偏移都會有些偏差，因為現在使用的 "銫/銣原子鐘" 成本過高，為此我們就需要在互聯網上找到一個可以提供準確時間的服務器，然後通過一種協議來同步我們的系統時間。

這個協議就是 NTP 了。

## NTP

網絡時間協議 (Network Time Protocol, NTP) 是通過 [RFC-1305](https://tools.ietf.org/html/rfc1305) 定義的時間同步協議，該協議基於 UDP 協議，使用端口號為 123；該協議用於在時間服務器和客戶端之間同步時間，從而使網絡內所有設備的時鐘保持一致。

對於運行NTP的本地系統，既可以接收來自其他時鐘源的同步，又可以作為時鐘源同步其他的時鐘，並且可以和其他設備互相同步。


### 時鐘層級 Stratum Levels

NTP 時間同步是按照層級分發時間的。

![ntp stratum levels]({{ site.url }}/images/misc/ntp-stratum-levels.png "ntp stratum levels"){: .pull-center width="60%"}

時鐘層級定義了到參考時鐘的距離，如上圖所示。

* stratum-0 也即參考時鐘，有極高的精度，提供可靠的 UTC 時間，通常為銫/銣原子鐘(Atomic Cesium/Rubidium Clocks)、GPS 授時或者無線電波授時 (如 CDMA)，這類的時鐘源不會直接為互聯網提供服務。

* stratum-1 直接（非通過網絡）鏈接到 stratum-0 提供服務，通常通過 RS-232、IRIG-B 鏈接，為互聯網提供標準時間。

* stratum-2+ 通過網絡向上一級請求服務，通常利用 NTP 協議。

通常距離頂層的時鐘源越遠時鐘的精度也就越差。

### 安裝

在 CentOS 中，可以通過如下方式進行安裝。

{% highlight text %}
# yum install ntp ntpdate              # 安裝
# systemctl [enable|start] ntpd        # 設置開機啟動，或者直接啟動
{% endhighlight %}

在 Linux 下有兩種時鐘同步方式，分別為直接同步和平滑同步：

* 通過 ntpdate 直接同步時間。但異常情況時，若機器有個 12 點的定時任務，當前時間認為是 13 點，而實際的標準時間為 11 點，直接使用該命令會造成任務重複執行；因此該命令多用於時鐘同步服務的第一次同步時間時使用。

* 通過 ntpd 可以平滑同步時鐘，通過每次同步較小的時間的偏移量，慢慢進行調整，也因此會耗費較長的時間，但可以保證一個時間不經歷兩次。

[www.pool.ntp.org](http://www.pool.ntp.org/zone/cn) 提供了全球的標準時鍾同步服務，對於中國可以參考其中的建議 ntp 配置文件，NTP 建議我們為了保障時間的準確性，最少找兩個個 NTP Server 。

{% highlight text %}
server 0.asia.pool.ntp.org
server 1.asia.pool.ntp.org
server 2.asia.pool.ntp.org
server 3.asia.pool.ntp.org
{% endhighlight %}

它的一般格式都是 number.country.pool.ntp.org ；當然，也可以使用阿里的公共服務。

![ntp alibaba basic service]({{ site.url }}/images/misc/ntp-alibaba-basic-service.png "ntp alibaba basic service"){: .pull-center width="60%"}

接下來可以使用如下命令。

{% highlight text %}
{% endhighlight %}


### 工作原理

NTP 授時有以下三種工作模式。

#### broadcast/multicast

適用於局域網的環境，時間服務器週期性以廣播的方式，將時間信息傳送給其他網路中的時間服務器，配置簡單 (可做到客戶端免配置)，但是精確度不高，對時間精確度要求不是很高的情況下可以採用。

#### client/server

最主流的 ntp 授時方式，精度較高，對網絡沒有特殊要求；同時可以指定多個 ntp server，因而也可避免網絡路徑失效帶來的時鐘不能同步。

#### symmetric

一臺服務器可以從遠端時間服務器獲取時鐘，如果需要也可提供時間信息給遠端的時間服務器，服務器之間的 peer 關係即為這種工作方式。

接下來，簡單介紹下 Client/Server 模式。

如下圖所示，Device A 和 Device B 通過網絡相連，且都有自己獨立的系統時鐘，需要通過 NTP 實現各自系統時鐘的自動同步。假設在系統時鐘同步前，Device A 的時鐘設定為 10:00:00am，Device B 的時鐘設定為 11:00:00am。

其中 Device B 作為 NTP 時間服務器，即 Device A 將使自己的時鐘與 Device B 的時鐘同步，另外，假設 NTP 報文在 Device A 和 Device B 之間單向傳輸所需要的時間為 1 秒。

![how ntp works]({{ site.url }}/images/misc/ntp-how-ntp-works.png "how ntp works"){: .pull-center }

系統時鐘同步的工作過程如下：

1. Device A 發送一個 NTP 報文給 Device B，該報文帶有它離開 Device A 時的時間戳，該時間戳為 10:00:00am(T1)。

2. 當 NTP 報文到達 Device B 時，Device B 會加上自己的時間戳，該時間戳為 11:00:01am(T2)。

3. 此 NTP 報文離開 Device B 時，Device B 再加上自己的時間戳，該時間戳為 11:00:02am(T3)。

4. 當 Device A 接收到該響應報文時，Device A 的本地時間為 10:00:03am(T4)。

至此，Device A 已經擁有足夠的信息來計算兩個重要的參數：A）報文的往返時延 Delay=(T4-T1)-(T3-T2)=2s；B) Device A 相對 Device B 的時間差 Offset=((T2-T1)+(T3-T4))/2=1h。這樣，Device A 就能夠根據這些信息來設定自己的時鐘，使之與 Device B 的時鐘同步。

### NTP 的報文格式

NTP 有兩種不同類型的報文，一種是時鐘同步報文，另一種是控制報文。控制報文僅用於需要網絡管理的場合，它對於時鐘同步功能來說並不是必需的，在此只介紹時鐘同步報文。

![ntp packets format]({{ site.url }}/images/misc/ntp-packets-format.jpg "ntp packets format"){: .pull-center }

<!--
主要字段的解釋如下：
LI（Leap Indicator）：長度為2比特，值為“11”時表示告警狀態，時鐘未被同步。為其他值時NTP本身不做處理。
VN（Version Number）：長度為3比特，表示NTP的版本號，目前的最新版本為3。
Mode：長度為3比特，表示NTP的工作模式。不同的值所表示的含義分別是：0未定義、1表示主動對等體模式、2表示被動對等體模式、3表示客戶模式、4表示服務器模式、5表示廣播模式或組播模式、6表示此報文為NTP控制報文、7預留給內部使用。
Stratum：系統時鐘的層數，取值範圍為1～16，它定義了時鐘的準確度。層數為1的時鐘準確度最高，準確度從1到16依次遞減，層數為16的時鐘處於未同步狀態，不能作為參考時鐘。
Poll：輪詢時間，即兩個連續NTP報文之間的時間間隔。
Precision：系統時鐘的精度。
Root Delay：本地到主參考時鐘源的往返時間。
Root Dispersion：系統時鐘相對於主參考時鐘的最大誤差。
Reference Identifier：參考時鐘源的標識。
Reference Timestamp：系統時鐘最後一次被設定或更新的時間。
Originate Timestamp：NTP請求報文離開發送端時發送端的本地時間。
Receive Timestamp：NTP請求報文到達接收端時接收端的本地時間。
Transmit Timestamp：應答報文離開應答者時應答者的本地時間。
Authenticator：驗證信息。
-->

NTP 提供了多種工作模式進行時間同步，包括了：客戶端/服務器模式、對等體模式、廣播模式、組播模式。

在不能確定服務器或對等體IP地址、網絡中需要同步的設備很多等情況下，可以通過廣播或組播模式實現時鐘同步；客戶端/服務器和對等體模式中，設備從指定的服務器或對等體獲得時鐘同步，增加了時鐘的可靠性。

### 啟動 NTP 服務

接下來，看看如何在 Linux 中配置並啟動 ntp 服務器。

#### 配置文件

先修改配置文件 /etc/ntp.conf ，只需要加入上面的 NTP Server 和一個 driftfile 就可以了。

{% highlight text %}
# vi /etc/ntp.conf
driftfile /var/lib/ntp/drift
server 0.asia.pool.ntp.org
server 1.asia.pool.ntp.org
server 2.asia.pool.ntp.org
server 3.asia.pool.ntp.org
{% endhighlight %}

fudge 127.127.1.0 stratum 0  stratum  這行是時間服務器的層次。設為0則為頂級，如果要向別的NTP服務器更新時間，請不要把它設為0


<!--
那麼這裡一個很簡單的思路就是第一我們只允許局域網內一部分的用戶連接到我們的服務器. 第二個就是這些client不能修改我們服務器上的時間

權限的設定主要以 restrict 這個參數來設定，主要的語法為：
restrict IP地址 mask 子網掩碼 參數
其中 IP 可以是IP地址，也可以是 default ，default 就是指所有的IP
參數有以下幾個：
ignore　：關閉所有的 NTP 聯機服務
nomodify：客戶端不能更改服務端的時間參數，但是客戶端可以通過服務端進行網絡校時。
notrust ：客戶端除非通過認證，否則該客戶端來源將被視為不信任子網
noquery ：不提供客戶端的時間查詢
注意：如果參數沒有設定，那就表示該 IP (或子網)沒有任何限制！

在/etc/ntp.conf文件中我們可以用restrict關鍵字來配置上面的要求

首先我們對於默認的client拒絕所有的操作
代碼:
restrict default kod nomodify notrap nopeer noquery

然後允許本機地址一切的操作
代碼:
restrict 127.0.0.1

最後我們允許局域網內所有client連接到這臺服務器同步時間.但是拒絕讓他們修改服務器上的時間
代碼:
restrict 192.168.1.0 mask 255.255.255.0 nomodify

把這三條加入到/etc/ntp.conf中就完成了我們的簡單配置. NTP還可以用key來做authentication,這裡就不詳細介紹了
-->




#### 更新時間

如上，在正式啟動 NTP 服務器之前，需要先通過 ntpdate 命令同步系統時間。需要注意的是，此時最好不要啟動服務，否則可能由於時間跳變導致服務異常。

{% highlight text %}
# ntpdate 0.asia.pool.ntp.org
# ntpdate 1.asia.pool.ntp.org
{% endhighlight %}

需要注意的是，通過 ntpdate 更新時間時，不能開啟 NTP 服務，否則會提示端口被佔用。

{% highlight text %}
1 Jan 22:35:08 ntpdate[12481]: no servers can be used, exiting
{% endhighlight %}

當然，可以通過 cron 定期更新時間，

{% highlight text %}
30 8 * * * root /usr/sbin/ntpdate 192.168.0.1; /sbin/hwclock -w
{% endhighlight %}

這樣，每天 8:30 Linux 系統就會自動的進行網絡時間校準，並寫入硬件時鐘。


#### 查看狀態

首先，通過 ntpstat 查看 NTP 服務的整體狀態。

{% highlight text %}
# ntpstat
synchronised to NTP server (202.112.29.82) at stratum 3
   time correct to within 44 ms
   polling server every 256 s
{% endhighlight %}

然後，可以通過 ntpq (NTP query) 來查看當前服務器的狀態，到底和那些服務器做時間同步。

{% highlight text %}
# watch ntpq -pn
Every 2.0s: ntpq -p                                  Sat Jul  7 00:41:45 2007

     remote           refid      st t when poll reach   delay   offset  jitter
===========================================================
+193.60.199.75   193.62.22.98     2 u   52   64  377    8.578   10.203 289.032
*mozart.musicbox 192.5.41.41      2 u   54   64  377   19.301  -60.218 292.411
{% endhighlight %}

<!--ntpdc -np -c peer 127.0.0.1-->

現在我就來解釋一下其中各個字段的含義。

{% highlight text %}
remote: 本地機器所連接的遠程NTP服務器；
 refid: 給遠程服務器提供時間同步的服務器，也就是上級服務器；
    st: 遠程服務器的層級別，級別從1~16，原則上不會與1直接鏈接；
     t: 也就是type，表示該時間服務器的工作模式(local, unicast, multicast or broadcast)；

  when: 多少秒之前與服務器成功做過時間同步；
  poll: 和遠程服務器多少秒進行一次同步，開始較小，後面會逐漸增加，可在配置文件中通過minpoll+maxpoll設置；
 reach: 成功連接服務器的次數，這是八進值；
 delay: 從本地機發送同步要求到服務器的網絡延遲 (round trip time)；
offset: 本地和服務器之間的時間差別，該值越接近於0，說明和服務器的時間越接近；
jitter: 統計值，統計了offset的分佈情況，這個數值的絕對值越小則說明和服務器的時間就越精確；
{% endhighlight %}

對於上述輸出，有幾個問題，第一我們在配置文件中設置與 remote 不同？

因為 NTP 提供的是一個集群，所以每次連接的得到的服務器都有可能是不一樣，也就意味著，配置文件中最好指定 hostname 而非 IP 。

第二問題是，那個最前面的 + 和 * 都是什麼意思呢？如上，提供了多個服務器同步時間，這些標誌就是用來標識服務器的狀態。

{% highlight text %}
  (space) reject，節點網絡不可達，或者 ACL 有限制；
x falsetick 通過算法確定遠程服務器不可用；
. excess 不在前10個同步距離較短的節點所以被排除，如果前10個節點故障，該節點可進一步被選中；
- outlyer 遠程服務器被clustering algorithm認為是不合格的NTP Server；
+ candidate 將作為輔助的服務器，作為組合算法的一個候選者，當上述的服務器不可用時，該服務器會接管；
* sys.peer 服務器被確認為我們的主NTP Server，系統時間將由這臺機器所提供；
o pps.peer 該節點為選中的時鐘源，系統時鐘以其為參考；
{% endhighlight %}

<!--對於上述的 o ，實際上，系統時鐘無論是間接地通過PPS參考時鐘驅動器或直接通過內核接口同步，原則上都是是從秒脈衝信號（PPS）進行同步的。-->

也即是說，輸出內容中第一個域標記符號是 * 即可認為系統通過該時間源同步時間。

瞭解這些之後我們就可以實時監測我們系統的時間同步狀況了；注意，部分虛擬機使用的是容器，無獨立 kernel，使用的就是宿主機的時間源，所以這時就需要到對應宿主機器排查。

{% highlight text %}
# systemctl start ntpd
$ ps aux | grep ntp
{% endhighlight %}



#### 其它

記錄下常見的問題。

##### 1. 配置文件中的driftfile是什麼?

每個 system clock 的頻率都有誤差，NTP 會監測我們時鐘的誤差值並予以調整，但是 NTP 不會立即調整時間，這樣會導致時鐘跳變，所以採用的是漸進方式，進而導致這是一個冗長的過程。

為此，它會把記錄下來的誤差先寫入 driftfile，這樣即使重啟之前的計算結果也就不會丟失了。

另外，記錄的單為是 PPM (Part Per Million)，是指百萬分之一秒，也就是微秒，

##### 2. 如何同步硬件時鐘?

NTP 一般只會同步 system clock，如果我們也要同步 RTC (hwclock) 的話那麼只需要把下面的選項打開就可以了。

{% highlight text %}
# vi /etc/sysconfig/ntpd
SYNC_HWCLOCK=yes
{% endhighlight %}

<!--
2、fudge 127.127.1.0 stratum 10 如果是LINUX做為NTP服務器，stratum(層級)的值不能太大，如果要向上級NTP更新可以設成2

ntpd -gq


 frequency error -506 PPM exceeds tolerance 500 PPM


-g
    Normally, ntpd exits if the offset exceeds the sanity limit, which is 1000 s by default. If the sanity limit is set to zero, no sanity checking is performed and any offset is acceptable. This option overrides the limit and allows the time to be set to any value without restriction; however, this can happen only once. After that, ntpd will exit if the limit is exceeded.
-q
Exit the ntpd just after the first time the clock is set. This behavior mimics that of the ntpdate program, which is to be retired. The -g and -x options can be used with this option.



-->


## 夏令時

夏令時 (Daylight Saving Time, DST) 是一種為節約能源而人為規定地方時間的制度，在這一制度實行期間所採用的統一時間稱為 "夏令時間"。

一般在天亮早的夏季人為將時間調快一小時，可以使人早起早睡，減少照明量，以充分利用光照資源，從而節約照明用電。各個採納夏時制的國傢俱體規定不同，目前全世界有近 110 個國家每年要實行夏令時。

北京夏令時是我國在 1986~1991 年間實施的一種夏季時間制度，北京夏令時比標準的北京時間早一個小時。


## 參考

另外，有個 adjtimex 命令，可以用來調整系統和硬件時間；Linux 中通過 tzdata 保存了與時區相關的信息，包括夏令時、閏秒等信息，不過對於新發布的閏秒則需要最近的更新。

### NTP

[standard NTP query program](https://www.eecis.udel.edu/~mills/ntp/html/ntpq.html) ntpq 的實現官網，而且包括了很多相關的資料 [The Network Time Protocol (NTP) Distribution](https://www.eecis.udel.edu/~mills/ntp/html/index.html) 。

<!-- https://www.eecis.udel.edu/~mills/ntp/html/warp.html -->

關於時間的簡單介紹，包括了時區、NTP 等 [Managing Accurate Date and Time](http://www.tldp.org/HOWTO/TimePrecision-HOWTO/index.html)，或者 [本地文檔](/reference/misc/ntp/HOWTO/TimePrecision-HOWTO/index.html) ；其它的一些關於 GPS 相關的信息可以參考 [GPS時間同步原理及其應用](/reference/misc/ntp/GPS_NTP.doc) ，一篇十分不錯的文章。

[Stratum Levels Defined](/reference/misc/ntp/sync_an02-StratumLevelDefined.pdf) 關於時鐘層級的定義；另外一篇關於 ntpq 的介紹可以參考 [網絡時間的那些事及 ntpq 詳解](https://linux.cn/article-4664-1.html?pr) ，或者參考 [本地文檔](/reference/misc/ntp/ntpq_details.mht)<!--；還有一篇是關於 [ntp反射式攻擊原理](/reference/misc/ntp/ntp_attack.mht)--> 。

實際可以通過 GPS 和 Raspberry Pi 搭建 stratum-1 服務器，可以參考 [Building a Stratum 1 NTP Server with a Raspberry Pi](https://xmission.com/blog/2014/05/28/building-a-stratum-1-ntp-server-with-a-raspberry-pi) ，也可參考 [本地文檔](/reference/misc/ntp/Building a Stratum 1 NTP Server with a Raspberry Pi.mht)；或者另一篇 [The Raspberry Pi as a Stratum-1 NTP Server](http://www.satsignal.eu/ntp/Raspberry-Pi-NTP.html)，也可查看 [本地文檔](/reference/misc/ntp/Building a Raspberry-Pi Stratum-1 NTP Server.mht) 。

<!--
另外兩篇包括了 常見服務器時鐘不準的解決方案 、 [NTP alibaba](/reference/misc/ntp/ntp.tar.gz) 。


-->

{% highlight text %}
{% endhighlight %}
