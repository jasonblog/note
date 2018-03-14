---
title: Kernel 系統時鐘
layout: post
comments: true
language: chinese
category: [linux]
keywords: linux
description:
---


<!-- more -->

## CONFIG_HZ VS. USER_HZ

在內核中維護了兩個 HZ 參數，CONFIG_HZ 和 USER_HZ ，其中前者可以在 ```make menuconfig``` 時進行配置，默認 250, 現在有些設置為了 1000，而後者固定為 100 。

#### CONFIG_HZ / HZ

內核中一般使用 HZ，實際兩者等價。該參數決定了內核的時鐘頻率，由硬件中斷產生，每次中斷是一次 tick ，也就是內核的一次時鐘滴答，同時 jiffies++ 。

可以通過提高該參數增加系統的實時性，但同時會導致過多的時間花費在中斷處理中。

#### USER_HZ

在用戶空間使用 clock_t 表示時鐘的滴答數。由於內核的 CONFIG_HZ 是變化的，那麼如果類似於 glibc 或用戶應用程序，在需要獲得 clock_t 時，要麼通過 sysctl 系統調用，要麼通過 /proc 或 /sys 文件系統去獲得。

與其採用上面的方法，不如內核定死用戶空間看到的 HZ 就是 100 ，然後內核在往用戶空間提供 tick 的時候都以這個為單位，這個也就是 USER_HZ 的作用。

### 使用方式

假設你要設計一個內核模塊，該模塊提供時間信息給用戶程序，且選擇用 tick 為單位，那麼應使用 ```jiffies_64_to_clock_t(jiffiex)``` 將 CONFIG_HZ 轉換成 USER_HZ；通過 ```clock_t_to_jiffies(tick)``` 將 USER_HZ 轉換成 CONFIG_HZ 。

也就是說，在用戶程序中，只能看到 USER_HZ ，除非是內核的 bug ，否則以 tick 或 jiffies 為單位的用戶空間接口，實際指的都是 USER_HZ 。

### 查看驗證

{% highlight text %}
----- 關於USER_HZ可以在shell中通過如下命令獲取
$ getconf CLK_TCK
100

----- 獲取CONFIG_HZ
$ grep ^CONFIG_HZ /boot/config-`uname -r`
CONFIG_HZ_1000=y
CONFIG_HZ=1000
{% endhighlight %}

對於 USER_HZ 也可以在 C 語言中通過 ```sysconf(_SC_CLK_TCK)``` 獲得。

<!--
有意思的是，systemtap tutorial有個比較好玩的實驗，也可以確定CONFIG_HZ的大小。

global count_jiffies, count_ms
probe timer.jiffies(100) { count_jiffies ++ }
probe timer.ms(100) { count_ms ++ }
probe timer.ms(543210)
{
    hz=(1000*count_jiffies) / count_ms
    printf ("jiffies:ms ratio %d:%d => CONFIG_HZ=%d\n",
    count_jiffies, count_ms, hz)
    exit ()
}

輸出如下：

jiffies:ms ratio 1358:5420 => CONFIG_HZ=250

實驗等待的時間有點久，需要等待543210ms，9分鐘左右，時間越久，誤差越小，如果等待的時間段一些，會出現誤差。
-->

### 其它

2.6.12 以後引入了 NO_HZ 和 high_res timer ，最新的內核使用 NO_HZ_FULL_ALL；也就是當系統空閒時，不再定時產生時鐘中斷，系統可以進入 default_idle() 節省系統 power 。

通常硬件可以提供多個時鐘源，而且可能精度達到 ns 級，這些時鐘源可以供 high_res timer 使用，從而不受 CONFIG_HZ 的限制。




{% highlight text %}
{% endhighlight %}
