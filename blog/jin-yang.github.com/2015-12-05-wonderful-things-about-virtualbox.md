---
title: VirtualBox 的一些高級特性
layout: post
comments: true
language: chinese
category: [misc]
keywords: virtualbox,高級,特性
description: 本文不會介紹如何安裝 VirtualBox，也不會講解如何安裝操作系統，只是介紹一些比較好玩的特性，一些很有意思，但是經常會被忽略的特性。
---

本文不會介紹如何安裝 VirtualBox，也不會講解如何安裝操作系統，只是介紹一些比較好玩的特性，一些很有意思，但是經常會被忽略的特性。

怎麼樣，一塊看看吧。

<!-- more -->

## 選型

現在，虛擬機用的比較多的有 KVM、XEN、VirtualBox、QEMU、VMware，除了 VMware 外，其它都是開源的。或許你會說當下很火的 Docker，準確來說它所做的是基於 Linux 做的資源隔離+鏡像的管理，所以在此就不討論了。

其中比較特殊的是 QEMU。QEMU 是通過軟件模擬不同的平臺，相比來說速度較慢；而其它的基本是基於硬件的虛擬化技術，速度較快。當然，這並非意味著 QEMU 就低人一等，也有其存在的價值。在這幾種虛擬機中，估計也就 QEMU 可以支持跨平臺的模擬。也就是說，你可以在 X86 上運行 ARM 的代碼，而其它的虛擬機卻做不到。

PS. QEMU 實際可以只作為一個管理軟件，後臺通過 KVM 加速。

據說 Android 的原生虛擬機用的是 QEMU，因為實在是太慢了，所以最近出了很多像 Genymotion 這種基於 VirtualBox 的模擬器。性能自然不必說，不過猜測應該也就只能用於 APP 的開發。

當然，除了做底層的開發或者內核的調試等，很少會有這種需求，所以這個跨平臺可以直接忽略。

鄙人日常辦公用的是 CentOS，比較憂傷的是，公司很多軟件只有 Mac+Windows，我們總是處於無人聞津的境地。無奈，還是搭個虛擬機吧... ...

相比來說，VirtualBox 的使用配置比較簡單，免費，而且可以支持多平臺（Windowns、Mac、Linux）。

## GuestAdditions

安裝完之後，首先安裝的就是 VirtualBox GuestAdditions (客戶端增強包)，該工具提供了很多有用的特性，比如共享文件夾、與客戶機之間自動切換、共享剪切板等。

關於如何安裝可以參考 [How to install VirtualBox Guest Additions][Guest_Additions]，安裝完後重啟即可。

安裝完之後可以在 [Devices] -> [Share Folders] 中配置與主機的共享目錄，對於 Windows7 來說，共享目錄會在網絡 VBOXSVR 目錄下。

![If the picture does not exist]({{ site.url }}/images/tools/virtualbox-sharefolder.png){: .pull-center width="550"}

對於一些大文件可以直接通過共享目錄使用，而對於一些小文件，可以通過 [Devices] -> [Drag and Drop] 配置為允許相互拖動文件；同樣在 Devices 下也可以配置共享剪切板，


## SnapShots

如果費了很大的勁終於配置好了環境，結下來要做的就是創建一個快照，以防萬一啊。通過 VirtualBox 創建快照保存虛擬機的狀態，可以在任何時候恢復到保存快照時的狀態。

![If the picture does not exist]({{ site.url }}/images/tools/virtualbox-snapshots.png){: .pull-center width="550"}

創建快照，可以直接通過快照面板，直接在界面上保存快照即可。

快照對於一些測試會非常有用，可以恢復原來的快照而且所有的軟件痕跡將被刪除，而不需要重新安裝或手動備份恢復虛擬機裡的文件。


## Seamless Mode

這個是比較喜歡的一個功能，如下圖所示，在 CentOS 中打開的千牛工作臺。

![If the picture does not exist]({{ site.url }}/images/tools/virtualbox-seamless.png){: .pull-center width="550"}

通過無縫模式將你虛擬機裡的操作系統窗口放在你的主機操作系統的桌面。

通過 "視圖" 菜單，單擊 "切換到無縫" 即可。


## Singned Kernel Modules

這個是 CentOS 7 版本新添加的特性，在添加內核模塊的時候需要內核模塊的簽名，否則會直接報錯，可以執行如下命令調整。

{% highlight text %}
for i in /usr/lib/modules/$(uname -r)/extra/VirtualBox/*ko; do
   sudo /usr/src/kernels/$(uname -r)/scripts/sign-file sha256 private_key.priv  public_key.der "$i";
done
{% endhighlight %}

解析來就可以直接啟動了。


## 參考

[How to install VirtualBox Guest Additions][Guest_Additions] 關於如何安裝 Guset Additions 的介紹，也可以參考 [本地文檔](/reference/linux/How to install VirtualBox Guest Additions.mht) 。

[10 VirtualBox Tricks and Advanced Features You Should Know About][Advanced_Features] 一些關於 VirtualBox 的高級特性介紹，也可以參考 [本地參考](/reference/linux/10 VirtualBox Tricks and Advanced Features You Should Know About.mht)。

對於 Singned Kernel Modules 模式的處理方式，下面 [VBox & VMware in SecureBoot Linux](http://gorka.eguileor.com/category/technology/linux/) 的文章介紹的很好，也可以參考 [本地文檔](/reference/linux/VBox VMware in SecureBoot Linux.mht) 。


[Guest_Additions]:   http://www.dedoimedo.com/computers/virtualbox-guest-addons.html    "How to install VirtualBox Guest Additions"
[Advanced_Features]: http://www.howtogeek.com/171228/10-virtualbox-tricks-and-advanced-features-you-should-know-about/  "10 VirtualBox Tricks and Advanced Features You Should Know About"

{% highlight text %}
{% endhighlight %}
