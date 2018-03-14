---
title: Systemtap
layout: post
comments: true
language: chinese
category: [linux]
keywords: systemtap,監控
description: Systemtap 幾乎是目前所知的最強大的內核調試工具，甚至有人說它無所不能，接下來，我們就看看 SystemTAP 。
---

Systemtap 幾乎是目前所知的最強大的內核調試工具，甚至有人說它無所不能，接下來，我們就看看 SystemTAP 。

<!-- more -->

## 簡介

![Systemtap Logo]({{ site.url }}/images/linux/systemtap-logo.svg "Systemtap Logo"){: .pull-center}

通過 Kprobe 可以動態地收集調試和性能信息，是一種非破壞性的工具，用戶可以用它跟蹤運行中內核任何函數或執行的指令等。但 Kprobe 並沒有提供一種易用的框架，用戶需要自己去寫模塊，然後安裝，所以在使用時非常麻煩。

而 systemtap 是利用 Kprobe 提供的 API 來實現動態地監控和跟蹤運行中的 Linux 內核的工具，相比於 Kprobe，systemtap 更加簡單，提供給用戶簡單的命令行接口，以及編寫內核指令的腳本語言。

### 工作原理

systemtap 的核心思想是定義一個事件 (event)，並給出處理該事件的句柄 (Handler)，當一個特定的事件發生時，內核運行該處理句柄，就像快速調用一個子函數一樣，處理完之後恢復到內核原始狀態。

* 事件<br>定義了很多種事件，例如進入或退出某個內核函數、定時器時間到、整個 systemtap 會話啟動或退出等等。

* 句柄<br>描述了當事件發生時要完成的工作，通常是從事件的上下文提取數據，將它們存入內部變量中，或者打印出來。

如下是 systemtap 的工作原理。

![How Systemtap works]({{ site.url }}/images/linux/systemtap-how-works.png "How Systemtap Works"){: .pull-center; width="90%"}

其原理是，systemtap 通過將腳本語句翻譯成 C 語言，編譯成內核模塊；模塊加載之後，將所有探測的事件以鉤子的方式掛到內核上，當任何處理器上的某個事件發生時，相應鉤子上句柄就會被執行。

最後，當 systemtap 會話結束之後，鉤子從內核上取下，然後移除模塊；而整個過程只需要用一個命令 stap 就可以完成。



### 安裝

可以通過 YUM 進行安裝，詳細的內容在下面；對於 CentOS 來說，此時會在 ```/usr/share/systemtap/tapset/``` 目錄下安裝 tapset 庫。

#### 0. 判斷內核是否支持 systemtap

在 /boot 目錄下查看當前版本的配置信息，是否包含如下的編譯選項，CentOS 中通常都會包含。

{% highlight text %}
# cat /boot/config-`uname -r` | egrep 'CONFIG_(DEBUG_INFO|KPROBES|DEBUG_FS|RELAY)='
CONFIG_RELAY=y
CONFIG_KPROBES=y
CONFIG_DEBUG_FS=y
CONFIG_DEBUG_INFO=y
{% endhighlight %}

如果沒有上述的配置項，則需要重新編譯，需要打開調試信息、Kprobe 和 debugfs ，確保 .config 文件中看到上面的四個選項。

#### 1. 安裝

使用 systemtap 需要安裝幾個依賴包：kernel-devel (一般會安裝在 /usr/src/kernels/\`uname -r\` 目錄下)，kernel-debuginfo (包比較大，下載很慢)；對於 systemtap 直接通過 yum 安裝即可。

{% highlight text %}
# yum install kernel-devel kernel-debuginfo kernel-debuginfo-common
# yum install systemtap systemtap-server
{% endhighlight %}

除了上述的包之外，如果需要編譯還可以安裝 devel，還有測試示例 testsuite 等包。另外，需要注意是，安裝時一定要注意 systemtap 版本要與內核的版本一致。


#### 2. 簡單測試

如下是一個簡單的示例，會輸出 hello world。

{% highlight text %}
# stap -v -e 'probe begin { printf("Hello, World!\n"); exit() }'
{% endhighlight %}

### 源碼安裝

上述的方式只能安裝 repository 中的包，通常都比較老，如果需要安裝最新的版本可以從源碼編譯安裝。可以從 [SystemTap](ftp://sources.redhat.com/pub/systemtap/releases/) 下載相應的版本，而且需要 [elfutils](https://fedorahosted.org/releases/e/l/elfutils/) 的支持。

{% highlight text %}
./configure --with-elfutils=/local/elfutils-0.166 --with-prefix=/usr
{% endhighlight %}


### 遠程使用

如果有很多相同類型的機器（內核版本、機型），那麼只需要在一臺機器上做上述的完整安裝，然後在其它機器安裝 systemtap-runtime 即可。

{% highlight text %}
# stap -r `uname -r` -e 'probe vfs.read {exit()}' -m simple    # 生成simple.ko
# staprun simple.ko
{% endhighlight %}

除了上述的標準使用、遠程使用，還可以使用 Flight Recorder Mode，簡單說就是會在內核中申請一塊內存，在需要的時候鏈接上去。




## UEFI Secure Boot Support

這是 2.5 引入的新特性，對安全的支持，需要添加一個認證方式到 MOK(Machine Owner Keys) 數據庫中，詳細的可以參考 [Systemtap UEFI Secure Boot Support](https://sourceware.org/systemtap/wiki/SecureBoot)，也可以參考 [本地文檔](/reference/linux/systemtap/SecureBoot.mht)。

上述文章沒有提及需要安裝 systemtap-server 並啟動，實際上，在 CentOS 7 中可以通過如下的方式安裝，簡單記錄如下。

{% highlight text %}
# yum install systemtap-server       # 安裝服務端
# systemctl start stap-server        # 啟動服務
{% endhighlight %}

然後，通過如下的方式執行，詳細的內容可以參考上述的示例。

{% highlight text %}
----- 查看當前所有的server
# stap --list-servers=all

----- 生成相應的signing_key.x509
# stap --use-server=192.168.2.108:38557 -e 'probe begin { exit() }'

----- 導入
# mokutil --import signing_key.x509

----- 重啟，需要驗證輸入密碼
# shutdown -r now

----- 查看是否生效
# stap --list-servers

----- 測試
# stap -ve 'probe begin { printf("hello\n"); exit() }'
{% endhighlight %}


<!--
CentOS 中會檢查 securelevel
/sys/kernel/security/securelevel
-->


## 詳細使用

還可以參考 /usr/share/doc/systemtap-client-X.X/examples 中的示例，可以簡單使用如下文件。

{% highlight text %}
# cat hello-world.stp
probe begin
{
    print ("hello world\n")
    exit ()
}

# stap hello-world.stp
{% endhighlight %}


探測點描述包含三個部分，function@filename:lineno ，可以使用 * 表示任意字符，例如，指定文件名為 net/socket.c，探測函數的入口和返回。

{% highlight text %}
probe kernel.function("*@net/socket.c") {}
probe kernel.function("*@net/socket.c").return {}
{% endhighlight %}

可以通過如下方式查找匹配的內核函數和變量。

{% highlight text %}
# stap -l 'kernel.function("*nit*")'      // 查看函數
# stap -L 'kernel.function("*nit*")'      // 查找函數以及變量
{% endhighlight %}

### 執行步驟

腳本的執行總共分為了 5 步：

#### 1. 解析腳本

解析 stap 輸入腳本，或者 -e 中指定的內容，以及 tapset 庫，也可以通過 -I 指定檢索路徑中的 *.stp 文件。

{% highlight text %}
# stap --vp 50000 -I /tmp -e 'probe syscall.read {printf("hello i am digoal.\n"); exit()}'
{% endhighlight %}

#### 2. 匹配庫

解析腳本，對庫進行匹配，並選擇對應的 stp(探針庫)、stpm(宏庫) 文件，直到所有的符號都分析匹配完。如果沒有使用 -u 選項，則優化腳本，移除未使用的變量、表達式、函數等。

{% highlight text %}
# stap --vp 05000 -I /tmp -e 'probe syscall.read {printf("hello i am digoal.\n"); exit()}'
{% endhighlight %}

#### 3. 轉換成C

將以上解析好的 stap 腳本轉換成 C 代碼。

{% highlight text %}
# stap --vp 00500 -I /tmp -e 'probe syscall.read {printf("hello i am digoal.\n"); exit()}'
{% endhighlight %}

#### 4. 編譯成.ko文件

使用上述生成的 C 文件創建 Linux 內核對象文件 (.ko)。

{% highlight text %}
# stap --vp 00050 -I /tmp -e 'probe syscall.read {printf("hello i am digoal.\n"); exit()}'
{% endhighlight %}

#### 5. 加載的內核運行

調用 staprun 將 ko 文件加載到內核中，並與之通訊，直到運行結束或受到中斷請求信號；最後，staprun 從內核中卸載這個 ko 模塊，並且清除整個 stap 過程產生的臨時文件。

{% highlight text %}
# stap --vp 00005 -I /tmp -e 'probe syscall.read {printf("hello i am digoal.\n"); exit()}'
{% endhighlight %}


### 配置選項

常用的配置選項有：

* -p NUM<br>運行至某個步驟停止，一般可用於生成可移動模塊。

* \-\-vp ABCDE<br>指定各步驟的輸出詳細級別。

## 常見錯誤排查

簡單列舉如下。

### semantic error

詳細報錯信息如下，主要原因是缺乏 debuginfo 包：

{% highlight text %}
semantic error: missing x86_64 kernel/module debuginfo [man warning::debuginfo] under '/lib/modules/xxx/build'
{% endhighlight %}

可以參考 [systemtap: a linux trace/probe tool](https://www.sourceware.org/git/?p=systemtap.git;a=blob_plain;f=README;hb=HEAD) ，其中包含了如下內容：

{% highlight text %}
- By default, systemtap looks for the debug info in these locations:
/boot/vmlinux-`uname -r`
/usr/lib/debug/lib/modules/`uname -r`/vmlinux
/lib/modules/`uname -r`/vmlinux
/lib/modules/`uname -r`/build/vmlinux
{% endhighlight %}

也就是說，依據這個默認搜索路徑，可以將 debug 的 vmlinux 鏈接到 /lib/modules 下。

{% highlight text %}
# ln -s /usr/lib/debug/lib/modules/`uname -r`/vmlinux /lib/modules/`uname -r`
{% endhighlight %}



## 參考

可以參考 [systemtap官網](https://sourceware.org/systemtap/) ；也可以參考 [systemtap Documentation](https://sourceware.org/systemtap/documentation.html)，也就是 systemtap 的參考文檔；以及相關的示例 [WarStories](https://sourceware.org/systemtap/wiki/WarStories) 。

另外，[SystemTap Beginners Guide](https://sourceware.org/systemtap/SystemTap_Beginners_Guide/index.html) 是一篇不錯的入門文檔，包括了詳細的介紹以及示例。


<!--
[本地文檔](/reference/linux/systemtap/Red_Hat_Enterprise_Linux-7-SystemTap_Beginners_Guide-en-US.pdf)

http://blog.csdn.net/zhangskd/article/details/38084337  內核調試工具 — kdump & crash
https://segmentfault.com/a/1190000000680628
http://myaut.github.io/dtrace-stap-book/
-->

{% highlight text %}
{% endhighlight %}
