---
title: Linux 監控
layout: post
comments: true
language: chinese
category: [linux]
keywords: linux,monitor,監控
description: Linux 中的系統監控，包括了常見的計算、內存、網絡等監控指標，以及相應的工具。
---

Linux 中的系統監控，包括了常見的計算、內存、網絡等監控指標，以及相應的工具。

<!-- more -->

很多的性能監控都是通過 proc 文件系統中的文件獲得，相應的內核實現保存在 fs/proc/ 目錄下。對於日誌的監控，除了 tail -f logfile 之外，還可以使用 tailf 。

如下是記錄的些工具，不過部分還沒有仔細研究過：

* nmon<br>需要下載。

* ftools<br>可以通過  hg clone https://code.google.com/p/linux-ftools/ 下載源碼 (hg 在 mercurial 包中)，當然需要翻牆才可以，也可以查看 [本地壓縮包](/reference/linux/monitor/linux-ftools.tar.bz2)，據說是一個很牛掰的工具。

* slabtop<br>實時顯示 /proc/slabinfo 中的內容。

* Netdata<br>一個很牛掰的高度優化的監控工具，通過 bootstrap框架 做展示，可以參考 [官方網站](https://github.com/firehol/netdata) 。


<!--
首先通過 lscpu 查看 CPU 的基本配置，然後通過 mpstat -P ALL 1 查看多核的性能，查看是否負載均衡。如果不均衡可能是由於：設置了 NUMA、網卡綁定不均衡。<br><br>
BIOS設置  sudo ./idracadm7  -r 10.212.199.160 -u root -p calvin get bios.memsettings.nodeinterleave
/home/tops/bin/python   /usr/alisys/dragoon/libexec/hwqc/hwqc.py bios          查看BIOS設置
上面錯誤顯示，目前BIOS層存在三個問題：
1. BIOS層的NUMA設置。OS層的NUMA已經設置好了，但是BIOS層沒有進行修改，這個之前進行過測試，如果出現內存跨CPU訪問，會對數據庫的性能有較大影響。
2. BIOS層的功耗設置。OS層的功耗目前還是節能模式，在訪問IO、內存等處理時，CPU會有一些延遲處理，對性能有較大影響。
3. BIOS層的Turbo Boost設置。Turbo Boost在BIOS層沒有關閉。

https://linux.cn/article-6924-1.html   Linux 性能分析的前 60 秒
-->

常用工具介紹。

| 工具     | 簡單介紹                                                    |
| :----:   | :--------                                                   |
| top      | 查看進程活動狀態以及一些系統狀況。                          |
| mpstat   | 查看多處理器狀況                                            |
| vmstat   | 查看系統狀態、硬件和系統信息等。                            |
| iostat   | 查看CPU 負載，硬盤狀況                                      |
| pidstat  | 監控全部或指定進程，包括CPU、內存、IO、任務切換、線程等     |
| netstat  | 查看網絡狀況                                                |
| iptraf   | 實時網絡狀況監測                                            |
| tcpdump  | 抓取網絡數據包，詳細分析                                    |
| mpstat   | 查看多處理器狀況                                            |
| tcptrace | 數據包分析工具                                              |
| netperf  | 網絡帶寬工具                                                |
| sar      | 綜合工具，查看系統狀況，可以記錄歷史狀態                    |
| dstat    | 綜合工具，綜合了 vmstat, iostat, ifstat, netstat 等多個信息 |

在 CentOS 中，sysstat 包中包括了 sar、iostat、mpstat、pidstat 等。

## 經典

一些比較經典的參考文檔，其實很多的性能監控會和調優綁定到一塊。

* [18 Command Line Tools to Monitor Linux Performance](http://www.tecmint.com/command-line-tools-to-monitor-linux-performance/) 也可以參考 [中文版](http://os.51cto.com/art/201402/429890.htm) 。

* [Linux Performance Analysis and Tools.pdf](/reference/linux/monitor/Linux_Performance_Analysis_and_Tools.pdf) ，非常非常經典的調試圖；也可以查看作者 [Brendan D. Gregg](http://www.brendangregg.com/) 的 blog 。

* 另外一些常見的經典文檔， [Linux System and Performance Monitoring](/reference/linux/monitor/linuxcon2010-linux-monitoring.pdf)、[Linux Performance and Tuning Guidelines](/reference/linux/monitor/redp4285.pdf)。


{% highlight text %}
{% endhighlight %}
