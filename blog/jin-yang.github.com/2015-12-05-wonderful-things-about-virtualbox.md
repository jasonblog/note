---
title: VirtualBox 的一些高级特性
layout: post
comments: true
language: chinese
category: [misc]
keywords: virtualbox,高级,特性
description: 本文不会介绍如何安装 VirtualBox，也不会讲解如何安装操作系统，只是介绍一些比较好玩的特性，一些很有意思，但是经常会被忽略的特性。
---

本文不会介绍如何安装 VirtualBox，也不会讲解如何安装操作系统，只是介绍一些比较好玩的特性，一些很有意思，但是经常会被忽略的特性。

怎么样，一块看看吧。

<!-- more -->

## 选型

现在，虚拟机用的比较多的有 KVM、XEN、VirtualBox、QEMU、VMware，除了 VMware 外，其它都是开源的。或许你会说当下很火的 Docker，准确来说它所做的是基于 Linux 做的资源隔离+镜像的管理，所以在此就不讨论了。

其中比较特殊的是 QEMU。QEMU 是通过软件模拟不同的平台，相比来说速度较慢；而其它的基本是基于硬件的虚拟化技术，速度较快。当然，这并非意味着 QEMU 就低人一等，也有其存在的价值。在这几种虚拟机中，估计也就 QEMU 可以支持跨平台的模拟。也就是说，你可以在 X86 上运行 ARM 的代码，而其它的虚拟机却做不到。

PS. QEMU 实际可以只作为一个管理软件，后台通过 KVM 加速。

据说 Android 的原生虚拟机用的是 QEMU，因为实在是太慢了，所以最近出了很多像 Genymotion 这种基于 VirtualBox 的模拟器。性能自然不必说，不过猜测应该也就只能用于 APP 的开发。

当然，除了做底层的开发或者内核的调试等，很少会有这种需求，所以这个跨平台可以直接忽略。

鄙人日常办公用的是 CentOS，比较忧伤的是，公司很多软件只有 Mac+Windows，我们总是处于无人闻津的境地。无奈，还是搭个虚拟机吧... ...

相比来说，VirtualBox 的使用配置比较简单，免费，而且可以支持多平台（Windowns、Mac、Linux）。

## GuestAdditions

安装完之后，首先安装的就是 VirtualBox GuestAdditions (客户端增强包)，该工具提供了很多有用的特性，比如共享文件夹、与客户机之间自动切换、共享剪切板等。

关于如何安装可以参考 [How to install VirtualBox Guest Additions][Guest_Additions]，安装完后重启即可。

安装完之后可以在 [Devices] -> [Share Folders] 中配置与主机的共享目录，对于 Windows7 来说，共享目录会在网络 VBOXSVR 目录下。

![If the picture does not exist]({{ site.url }}/images/tools/virtualbox-sharefolder.png){: .pull-center width="550"}

对于一些大文件可以直接通过共享目录使用，而对于一些小文件，可以通过 [Devices] -> [Drag and Drop] 配置为允许相互拖动文件；同样在 Devices 下也可以配置共享剪切板，


## SnapShots

如果费了很大的劲终于配置好了环境，结下来要做的就是创建一个快照，以防万一啊。通过 VirtualBox 创建快照保存虚拟机的状态，可以在任何时候恢复到保存快照时的状态。

![If the picture does not exist]({{ site.url }}/images/tools/virtualbox-snapshots.png){: .pull-center width="550"}

创建快照，可以直接通过快照面板，直接在界面上保存快照即可。

快照对于一些测试会非常有用，可以恢复原来的快照而且所有的软件痕迹将被删除，而不需要重新安装或手动备份恢复虚拟机里的文件。


## Seamless Mode

这个是比较喜欢的一个功能，如下图所示，在 CentOS 中打开的千牛工作台。

![If the picture does not exist]({{ site.url }}/images/tools/virtualbox-seamless.png){: .pull-center width="550"}

通过无缝模式将你虚拟机里的操作系统窗口放在你的主机操作系统的桌面。

通过 "视图" 菜单，单击 "切换到无缝" 即可。


## Singned Kernel Modules

这个是 CentOS 7 版本新添加的特性，在添加内核模块的时候需要内核模块的签名，否则会直接报错，可以执行如下命令调整。

{% highlight text %}
for i in /usr/lib/modules/$(uname -r)/extra/VirtualBox/*ko; do
   sudo /usr/src/kernels/$(uname -r)/scripts/sign-file sha256 private_key.priv  public_key.der "$i";
done
{% endhighlight %}

解析来就可以直接启动了。


## 参考

[How to install VirtualBox Guest Additions][Guest_Additions] 关于如何安装 Guset Additions 的介绍，也可以参考 [本地文档](/reference/linux/How to install VirtualBox Guest Additions.mht) 。

[10 VirtualBox Tricks and Advanced Features You Should Know About][Advanced_Features] 一些关于 VirtualBox 的高级特性介绍，也可以参考 [本地参考](/reference/linux/10 VirtualBox Tricks and Advanced Features You Should Know About.mht)。

对于 Singned Kernel Modules 模式的处理方式，下面 [VBox & VMware in SecureBoot Linux](http://gorka.eguileor.com/category/technology/linux/) 的文章介绍的很好，也可以参考 [本地文档](/reference/linux/VBox VMware in SecureBoot Linux.mht) 。


[Guest_Additions]:   http://www.dedoimedo.com/computers/virtualbox-guest-addons.html    "How to install VirtualBox Guest Additions"
[Advanced_Features]: http://www.howtogeek.com/171228/10-virtualbox-tricks-and-advanced-features-you-should-know-about/  "10 VirtualBox Tricks and Advanced Features You Should Know About"

{% highlight text %}
{% endhighlight %}
