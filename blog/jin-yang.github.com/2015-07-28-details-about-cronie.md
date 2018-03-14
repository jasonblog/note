---
title: 你所不知道的 Linux 定時任務
layout: post
comments: true
language: chinese
category: [linux]
keywords: linux,crontab,cron,定時任務
description: 在 Linux 中，我們經常使用 cron 執行一些定時任務，只需要配置一下時間，它就可以週期的執行一些任務。不知道你是否清楚它的詳細用法？是否發現，腳本單獨運行時是好好的，放到 cron 任務裡卻掛了！！！一個部署了 crond 的服務器，系統資源卻被莫名其妙的被佔滿了，Why？？？
---

在 Linux 中，我們經常使用 cron 執行一些定時任務，只需要配置一下時間，它就可以週期的執行一些任務。

不知道你是否清楚它的詳細用法？是否發現，腳本單獨運行時是好好的，放到 cron 任務裡卻掛了！！！一個部署了 crond 的服務器，系統資源卻被莫名其妙的被佔滿了，Why？？？

<!-- more -->

## 數學掛鐘

在討論 cron 前，首先看一個很有意思的數學掛鐘，這個時鐘通過各種符號計算公式表示 1~12 個數字。SO, JUST FOR FUN。

 ![mathematic-clock]{: .pull-center width="300"}

#### 1. 勒讓德 (Legendre) 常數

其值為 1 ，可以參考 [維基百科](https://zh.wikipedia.org/zh/%E5%8B%92%E8%AE%A9%E5%BE%B7%E5%B8%B8%E6%95%B0 "勒讓德常數")，大致摘抄如下：勒讓德在研究素數的分佈情況時，發現 pi(x) 滿足以下等式：

![legendre-const]{: .pull-center}

其中 B 是一個常數，稱為勒讓德常數，他估計 B 大約為 1.08366，但不管它的值是什麼，只要它存在，就證明了素數定理。該值經過高斯等大佬的努力，最後被數學家 Charles Jean 證明為 1。

#### 2. 無窮遞減等比數列

該數列的求和後值為 2 ，其首項為 1，公比為 1/2，也就是 1 + 1/2 + 1/4 + 1/8 + ... 1/(2^i)，當 i 趨於無窮大時，最終和為 2 。

#### 3. 在 XML、HTML 中用於標識

簡單來說，也就是說若將上述代碼放在瀏覽器裡，顯示的就是 3 。其中，&# 後面接十進制字符，&#x 後面接十六進制字符，十六進制的 33 就對應 ASCII 中的 '3'。

可以簡單測試下，其中後面就是 ```&#x33;``` 對應的值 &#x33; 。

#### 4. 同餘問題

可以參考 [Modular Multiplicative Inverse](https://en.wikipedia.org/wiki/Modular_multiplicative_inverse)，令 x≡2<sup>-1</sup>(mod 7)，2x≡1(mod 7)，則 x=4 。

#### 5. 黃金分割

其中 φ=(√5+1)/2 是黃金分割數，那麼將其帶進表達式就得 5 。

#### 6. 階乘

3 的階乘，3!=3x2x1=6 。

#### 7. 循環小數

用於表示 6.9999... 其中 9 的頭上一橫，表示循環節是 9 。可以按照無窮遞減等比數列查看，該值等於：6.9999... = 6 + 0.9 + 0.09 + 0.009 + 0.0009 + ... = 7 。

#### 8. 二進制

通過二進制表示的 8 ，其中黑色表示 1 ，其它表示 0 ，也即二進制 1000 。

#### 9. 四進值

以四進製表示的 9 ，21(四進制) = 2 * 4 + 1 = 9 。

#### 10. 組合

5 取 2 的組合數，也即 5!/(2!*3!) = 10 。

#### 11. 十六進制

其中 A 是 10，B 是 11，C 是 12 。

#### 12. 立方根

這個很簡單，12<sup>3</sup> = 1728，也就是 1728 的立方根就是 12 。

OK，接下來進入正題。

## 簡介

在 Linux 平臺上如果需要實現定時或者週期執行某一個任務，可以通過編寫 cron 腳本來實現。Linux 默認會在開機時啟動 crond 進程，該進程負責讀取調度任務並執行，用戶只需要將相應的調度腳本寫入 cron 的調度配置文件中。

另外，需要注意的是，cron 採用的是分鐘級的調度，如果要更高精度的，只能用其它方法。

而且，**每次執行時都是併發執行**，而非串行。

### 安裝、啟動

在很多的發行版本中，cron 是默認安裝的，包括了 crond+crontab 兩個主要命令。前者是後臺任務，用於調度執行；後者用來編譯、查看、管理定時任務。

在 CentOS 7 中，該服務是默認安裝的，如果沒有，則可以安裝 cronie 包，然後通過如下命令啟動 crond 服務。

{% highlight text %}
# systemctl start crond
{% endhighlight %}

其它的相關操作與 systemctl 指令相同，如 status、restart、stop 等操作。

### 相關配置文件

與 cron 相關的有如下的幾個文件，其中前幾個是調度相關的文件：

1. /etc/crontab

    全局配置文件；同時每個用戶有自己的 crontab 配置文件，這些文件在 /var/spool/cron 目錄下。

2. /etc/cron.deny /etc/cron.allow

    分別表示不能/能使用 crontab 命令的用戶。如果兩個文件同時存在，那麼 /etc/cron.allow 優先；如果兩個文件都不存在，那麼只有超級用戶可以安排作業。

3. /etc/cron.d/ /etc/{cron.daily,cron.hourly,cron.monthly,cron.weekly}

    保存的配置文件，後面詳解。

4. /var/log/cron

    默認的日誌文件。如果配置使用了 syslog，那麼日誌會發送到 /var/log/messages 文件中。

對於 CentOS 7 來說，有 cron.deny 文件，但是為空，而且沒有 cron.allow 文件，這也就意味著所有的用戶都可以創建 cron 任務。


### 配置定時任務

定時任務包括了兩類：

* 系統級任務 (/etc/crontab)。需要 root 權限，其中第六部分指定了用戶名，也就是說能以任意用戶執行命令；通常用於系統服務或者一些重要的任務。

* 用戶級任務 (/var/spool/cron/)。注意，此時的第六部分為用戶需要執行的命令，而且只能以創建任務的用戶身份執行。

如果用的任務不是以 hourly monthly weekly 方式執行，則可以將相應的 crontab 寫入到 crontab 或 cron.d 目錄中。如，可以在 cron.d 目錄下新建腳本 echo-date.sh。

{% highlight text %}
# .---------------- minute (0 - 59)
# |  .------------- hour (0 - 23)
# |  |  .---------- day of month (1 - 31)
# |  |  |  .------- month (1 - 12) OR jan,feb,mar,apr ...
# |  |  |  |  .---- day of week (0 - 6) (Sunday=0 or 7) OR sun,mon,tue,wed,thu,fri,sat
# |  |  |  |  |
# *  *  *  *  * user-name  command to be executed
{% endhighlight %}

通常來說，我們是通過 crontab 命令來管理定時任務，常用的參數有：

* -e，編輯 crontab 任務，默認使用當前用戶。

* -l，列出用戶所有的定時任務；注意，不會顯示 /etc 目錄下的配置任務。

* -r，刪除所有的定時任務。

* -u，指定用戶名。

最常見的是通過 crontab -e 編輯 crontab 任務，此時會通過環境變量 $EDITOR 定義的值，調用相應的編輯器，例如 export EDITOR="/usr/bin/vim"，通過 vim 進行編輯。此時會在 /tmp 目錄下生成一個臨時文件，當編輯完成後替換掉 /var/spool/cron/ 中對應用戶的文件。

另外，也可以編輯一個臨時文件如 contabs.tmp，編輯後通過 contab contabs.tmp 命令導入新的配置。一般不建議直接修改 /etc/ 下的相關配置文件。

通常來說，需要檢查 /etc/crontab 文件、/etc/cron.*/ 目錄，以及 /var/spool/cron/ 目錄。


## Anacron

其實在官方的源碼中，還包括了一個 anacron 指令，而 CentOS 7 中是包含在 anaconda-core 包中的；所以，如果使用 anacron 命令，必須安裝該包。

### 簡介

像服務器來說，Linux 主機通常是 24 全天全年的處於開機狀態，此時只需要 atd 與 crond 這兩個服務即可；而對於像我們自己的電腦，經常關機，那麼我們就需要 anacron 的幫助了。

anacron 並不能取代 cron，而是以天為單位或者是在啟動後立刻進行 anacron 的動作。它會去偵測停機期間該執行但未執行的 crontab 任務，並將該任務運行一遍後，anacron 就會自動停止。

通過 anacron 命令，我們可以選擇串行執行（默認）、強制執行（不判斷時間戳）、立即執行未執行任務（不延遲等待）等操作。其配置文件是 /etc/anacrontab，每次執行完成會在 /var/spool/anacron/ 目錄下保存運行的時間點。

### 任務配置

{% highlight text %}
SHELL=/bin/sh
PATH=/sbin:/bin:/usr/sbin:/usr/bin
MAILTO=root
# the maximal random delay added to the base delay of the jobs
RANDOM_DELAY=45
# the jobs will be started during the following hours only
START_HOURS_RANGE=3-22

#period in days   delay in minutes   job-identifier   command
1                 5                  cron.daily       nice run-parts /etc/cron.daily
7                 25                 cron.weekly      nice run-parts /etc/cron.weekly
@monthly          45                 cron.monthly     nice run-parts /etc/cron.monthly
{% endhighlight %}

其中開頭定義了一些環境變量等信息，而配置項的含義如下：

1. period in days：指定指令執行的週期，即指定任務在多少天內執行一次，也可以使用宏定義，如 @day、@weekly、@monthly。

2. delay in minutes：當命令已經就緒後，並非立即執行，而是要延遲等待一段時間。

3. job-identifier：每次啟動時都會在 /var/spool/anacron 裡面建立一個以 job-identifier 為文件名的文件，記錄著任務完成的時間。

4. command：要運行的命令，其中 run-parts 用來運行整個目錄的可執行程序。

### 命令詳解

實際上，anacron 也是一個 cron 任務，可以通過 ls /etc/cron*/*anacron 查看。通常是通過 anacron -s 執行，以 anacron -s cron.daily 為例，會有如下的步驟：

1. 由配置文件 /etc/anacrontab 解析到 cron.daily 這項工作名稱的執行週期為一天。

2. 從 /var/spool/anacron/cron.daily 取出最近一次運行 anacron 的時間戳。

3. 把取出的時間戳與當前的時間戳相比較，如果差異超過了一天，那麼就準備進行命令。

4. 若準備執行命令，根據 /etc/anacrontab 的配置，計算延遲的時間。

5. 延遲時間後，開始運行後續命令，也就是 run-parts  /etc/cron.daily 這串命令。

6. 運行完畢後，anacron 程序結束。

另外，需要注意的是，命令執行通常為串行執行。



## 示例

在配置 crontab 任務時，有幾個特殊的符號：A) ```"*"``` 所有的取值範圍內的數字；B) ```"/"``` 每的意思，如 ```"*/5"``` 表示每 5 個單位；C) ```"-"``` 從某個數字到某個數字；D) ```","``` 用來分開幾個離散的數字。

以下舉幾個例子說明問題：
{% highlight text %}
*/10 * * * *     echo "Ten minutes ago." >> /tmp/foo.txt    // 每十分鐘執行一次
0 6 * * *        echo "Good morning." >> /tmp/foo.txt       // 每天早上6點
0 */2 * * *      echo "Have a break now." >> /tmp/foo.txt   // 每兩個小時
45 4 1,10,22 * * echo "Restart server." >> /tmp/foo.txt     // 每月1、10、22日的4:45
0 23-7/2,8 * * * echo "Have a good dream." >> /tmp/foo.txt  // 晚上11點到早上8點之間每兩個小時，早上八點
0 11 4 * 1-3     echo "Just kidding." >> /tmp/foo.txt       // 每月4號和每週的週一到週三的早上11點
45 11 * * 0,6    echo "Have a good lunch." >> /tmp/foo.txt  // 每週六、週日的11點45分
0 9 * * 1-5      echo "Work hard." >> /tmp/foo.txt          // 從週一到週五的9點
2 8-16/3 * * *   echo "Some examples." >> /tmp/foo.txt      // 8:02、11:02、14:02執行

0,30 18-23 * * *    echo "Same." >> /tmp/foo.txt            // 每天18:00到23:00之間每隔30分鐘
0-59/30 18-23 * * * echo "Same." >> /tmp/foo.txt            // 同上
*/30 18-23 * * *    echo "Same." >> /tmp/foo.txt            // 同上
{% endhighlight %}
<!--
at -f test-cron.sh -v 10:25    // -f指定腳本文件，-v指定運行時間
-->

另外，需要注意的幾點：

1. 可以在 crontab 的命令中使用環境變量，如：```*/1 * * * * echo $HOME``` 。

2. 第三個域和第五個域是 "或" 操作。

通常，使用 crontab -e 進行的配置是針對某個用戶的，而編輯 /etc/crontab 是針對系統的任務。


### 特殊用法

除了上述的寫法外，crontab 提供了一些簡單的時間定義方法，如：

{% highlight text %}
@daily         echo "Hi" >> /tmp/foo.txt
{% endhighlight %}

除此之外還有如下類似的類型，可以通過 man 5 crontab 查看。

{% highlight text %}
@reboot    :    Run once after reboot.
@yearly    :    Run once a year, ie.  "0 0 1 1 *".
@annually  :    Run once a year, ie.  "0 0 1 1 *".
@monthly   :    Run once a month, ie. "0 0 1 * *".
@weekly    :    Run once a week, ie.  "0 0 * * 0".
@daily     :    Run once a day, ie.   "0 0 * * *".
@hourly    :    Run once an hour, ie. "0 * * * *".
{% endhighlight %}

<!--
 SHELL=/bin/bash
 PATH=/sbin:/bin:/usr/sbin:/usr/bin
 MAILTO=root      //如果出現錯誤，或者有數據輸出，數據作為郵件發給這個帳號
 HOME=/    //使用者運行的路徑,這裡是根目錄

# run-parts
01 * * * * root run-parts /etc/cron.hourly //每小時執行/etc/cron.hourly內的腳本
02 4 * * * root run-parts /etc/cron.daily //每天執行/etc/cron.daily內的腳本
22 4 * * 0 root run-parts /etc/cron.weekly //每星期執行/etc/cron.weekly內的腳本
42 4 1 * * root run-parts /etc/cron.monthly //每月去執行/etc/cron.monthly內的腳本
大家注意"run-parts"這個參數了，如果去掉這個參數的話，後面就可以寫要運行的某個腳本名，而不是文件夾名了。
-->


### 常用技巧

可以通過如下命令查看所有用戶的 cron 定時任務，注意需要使用 root 權限。

{% highlight bash %}
for user in $(cut -f1 -d: /etc/passwd); do echo "### Crontabs for $user ####"; crontab -u $user -l; done
{% endhighlight %}

需要注意的是，上述腳本只能顯示 user 的 crontabs，如果要查看所有的還需要解析 /etc/crontab、/etc/crontab.d、/etc/cron.daily 等配置文件中的任務。



## '%' 導致的問題

例如寫個了一個 shell 腳本，其中參數中使用了 '%'，那麼在 cron 任務中就可能會執行錯誤，或者與預期的不符。為簡單起見只是將傳入的參數保存在 /tmp/foobar.log 中，腳本文件為 /tmp/foobar.sh，其內容如下：

{% highlight bash %}
#!/bin/bash
echo $1 >> /tmp/foobar.log
{% endhighlight %}

然後我們新建一個 cron 任務，內容如下：

{% highlight text %}
*/1 * * * * /tmp/foobar.sh "`date '+%Y-%m-%d %H:%M:%S'`" > /dev/null 2>&1
{% endhighlight %}

正常來說在 /tmp/foobar.log 中會每隔 1 分鐘打印一條日誌，而實際上卻沒有。查看 /var/log/cron 日誌可以發現，實際執行的命令是 /tmp/foobar.sh `date +，汗 ^_^""

實際上，在 cron 任務中，'%' 是有特殊意義的，在這裡需要轉義，通過 man 5 crontab 查看幫助，可以看到如下內容：

> A "%" character in the command, unless escaped with a backslash (\\), will be changed into newline characters, and all data after the first % will be sent to the command as standard input.

也就是說，如果 % 沒有通過 \ 轉義，那麼就會被替換成換行，上述命令的正確打開姿勢是：
{% highlight text %}
*/1 * * * * /tmp/foobar.sh "`date '+\%Y-\%m-\%d \%H:\%M:\%S'`" > /dev/null 2>&1
{% endhighlight %}


## 輸出字符引發的血案

現象是，登陸一臺公用的跳板機時，當嘗試從個人帳號切換到公用帳號時發現報錯，是由於進程數超過了最大限制 (ulimit -u)，然後通過 ps aux 發現有很多進程，其中比較多的是如下的兩條命令：
{% highlight text %}
/usr/sbin/postdrop -r
/usr/sbin/sendmail -FCronDaemon -i -odi -oem -oi -t -f root
{% endhighlight %}

基本可以確定是郵件的發送服務導致的，那麼是什麼程序調用的呢？通過 pstree 發現，其父進程為 crond 。

然後，通過 du -i 查看，發現 /var 的 inode 數目使用了 100%，通過如下命令查看根目錄下的文件數目，也就是大約每個子目錄中 inode 的數目。
{% highlight text %}
$ for dir in `ls -1Ad /*`; do echo -e "${dir} \t\t `find ${dir} | wc -l`"; done
{% endhighlight %}

然後，可以逐層向下查找，最後可以發現是 /var/spool/postfix/maildrop 目錄下有大量的文件。通過 file 命令查看是 data 類型，實際上該目錄下的文件可以通過 strings 命令查看，其內容為 crond 發送的郵件。

大概定位到了原因，先恢復掉。kill 掉所有 postdrop、sendmail 進程，然後清空 maildrop 目錄下的文件。此時如果直接通過 rm * -f 刪除，會由於文件過多而報錯，可以通過如下兩種方式進行刪除。
{% highlight text %}
# find /tmp -type f -exec rm {} \;
# ls | xargs rm -vf
{% endhighlight %}

OK，環境已經恢復，那麼具體是什麼原因導致的呢？

查看 cronie 的源碼可以發現，crond 會 fork 一個子進程去執行任務，而該進程有會再 fork 一個孫子進程執行真正的命令，代碼的調用邏輯如下。
{% highlight text %}
main()                       # C入口函數
 |-job_runqueue()            # 執行隊列中的命令
   |-do_command()            # 此時會fork一個子進程執行，主進程繼續工作
     |-child_process()       # 再fork一個進程，通過execle執行shell命令
       |-execle()            # 執行真正的shell指令，在孫子進程中執行
{% endhighlight %}
在 child_process() 函數中通過 fork() 子進程前，會創建兩個管道 (stdin+stdout) 用來與子進程通訊，分別表示輸入和輸出。默認情況下，crontab 會將命令執行的輸出，通過郵件發送給用戶。

而在查看 /var/log/maillog 日誌發現，是由於 pickup 管道不存在，導致郵件一直在積壓。

通常來說，我們不需要發送郵件，為了防止上述問題的發生，可以配置運行腳本輸出為 >/dev/null 2>&1，來避免 crontab 運行中有內容輸出。
{% highlight text %}
0 * * * * /path/to/script.sh    > /dev/null
0 * * * * /path/to/script.sh    > /dev/null 2>&1
0 * * * * /path/to/command arg1 > /dev/null 2>&1 || true
{% endhighlight %}

或者直接在 crontab 文件的頂部設置 MAILTO 變量為空，也就是 ```MAILTO=""``` 。


當然，如果有必要，可以將輸出發送到指定郵箱，但是需要首先確保郵件服務是正常的，然後添加如下配置項：
{% highlight text %}
MAILTO="ooops@foobar.com"
0 3 * * * echo "Helloooo!"
{% endhighlight %}

## 參考

源碼可以從 [cronie project][cronie official site] 官方網站下載，相關的幫助可以查看 man 1 crontab (命令行語法相關)、man 5 crontab (配置文件相關)。


<!-- images -->
[mathematic-clock]:    /images/linux/cronie-mathematic-clock.jpg    "數字鐘"
[legendre-const]:      /images/linux/cronie-legendre.png            "勒讓德常數"

<!-- URLs -->
[cronie official site]: https://fedorahosted.org/cronie/

