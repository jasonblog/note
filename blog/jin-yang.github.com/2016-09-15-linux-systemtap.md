---
title: Systemtap
layout: post
comments: true
language: chinese
category: [linux]
keywords: systemtap,监控
description: Systemtap 几乎是目前所知的最强大的内核调试工具，甚至有人说它无所不能，接下来，我们就看看 SystemTAP 。
---

Systemtap 几乎是目前所知的最强大的内核调试工具，甚至有人说它无所不能，接下来，我们就看看 SystemTAP 。

<!-- more -->

## 简介

![Systemtap Logo]({{ site.url }}/images/linux/systemtap-logo.svg "Systemtap Logo"){: .pull-center}

通过 Kprobe 可以动态地收集调试和性能信息，是一种非破坏性的工具，用户可以用它跟踪运行中内核任何函数或执行的指令等。但 Kprobe 并没有提供一种易用的框架，用户需要自己去写模块，然后安装，所以在使用时非常麻烦。

而 systemtap 是利用 Kprobe 提供的 API 来实现动态地监控和跟踪运行中的 Linux 内核的工具，相比于 Kprobe，systemtap 更加简单，提供给用户简单的命令行接口，以及编写内核指令的脚本语言。

### 工作原理

systemtap 的核心思想是定义一个事件 (event)，并给出处理该事件的句柄 (Handler)，当一个特定的事件发生时，内核运行该处理句柄，就像快速调用一个子函数一样，处理完之后恢复到内核原始状态。

* 事件<br>定义了很多种事件，例如进入或退出某个内核函数、定时器时间到、整个 systemtap 会话启动或退出等等。

* 句柄<br>描述了当事件发生时要完成的工作，通常是从事件的上下文提取数据，将它们存入内部变量中，或者打印出来。

如下是 systemtap 的工作原理。

![How Systemtap works]({{ site.url }}/images/linux/systemtap-how-works.png "How Systemtap Works"){: .pull-center; width="90%"}

其原理是，systemtap 通过将脚本语句翻译成 C 语言，编译成内核模块；模块加载之后，将所有探测的事件以钩子的方式挂到内核上，当任何处理器上的某个事件发生时，相应钩子上句柄就会被执行。

最后，当 systemtap 会话结束之后，钩子从内核上取下，然后移除模块；而整个过程只需要用一个命令 stap 就可以完成。



### 安装

可以通过 YUM 进行安装，详细的内容在下面；对于 CentOS 来说，此时会在 ```/usr/share/systemtap/tapset/``` 目录下安装 tapset 库。

#### 0. 判断内核是否支持 systemtap

在 /boot 目录下查看当前版本的配置信息，是否包含如下的编译选项，CentOS 中通常都会包含。

{% highlight text %}
# cat /boot/config-`uname -r` | egrep 'CONFIG_(DEBUG_INFO|KPROBES|DEBUG_FS|RELAY)='
CONFIG_RELAY=y
CONFIG_KPROBES=y
CONFIG_DEBUG_FS=y
CONFIG_DEBUG_INFO=y
{% endhighlight %}

如果没有上述的配置项，则需要重新编译，需要打开调试信息、Kprobe 和 debugfs ，确保 .config 文件中看到上面的四个选项。

#### 1. 安装

使用 systemtap 需要安装几个依赖包：kernel-devel (一般会安装在 /usr/src/kernels/\`uname -r\` 目录下)，kernel-debuginfo (包比较大，下载很慢)；对于 systemtap 直接通过 yum 安装即可。

{% highlight text %}
# yum install kernel-devel kernel-debuginfo kernel-debuginfo-common
# yum install systemtap systemtap-server
{% endhighlight %}

除了上述的包之外，如果需要编译还可以安装 devel，还有测试示例 testsuite 等包。另外，需要注意是，安装时一定要注意 systemtap 版本要与内核的版本一致。


#### 2. 简单测试

如下是一个简单的示例，会输出 hello world。

{% highlight text %}
# stap -v -e 'probe begin { printf("Hello, World!\n"); exit() }'
{% endhighlight %}

### 源码安装

上述的方式只能安装 repository 中的包，通常都比较老，如果需要安装最新的版本可以从源码编译安装。可以从 [SystemTap](ftp://sources.redhat.com/pub/systemtap/releases/) 下载相应的版本，而且需要 [elfutils](https://fedorahosted.org/releases/e/l/elfutils/) 的支持。

{% highlight text %}
./configure --with-elfutils=/local/elfutils-0.166 --with-prefix=/usr
{% endhighlight %}


### 远程使用

如果有很多相同类型的机器（内核版本、机型），那么只需要在一台机器上做上述的完整安装，然后在其它机器安装 systemtap-runtime 即可。

{% highlight text %}
# stap -r `uname -r` -e 'probe vfs.read {exit()}' -m simple    # 生成simple.ko
# staprun simple.ko
{% endhighlight %}

除了上述的标准使用、远程使用，还可以使用 Flight Recorder Mode，简单说就是会在内核中申请一块内存，在需要的时候链接上去。




## UEFI Secure Boot Support

这是 2.5 引入的新特性，对安全的支持，需要添加一个认证方式到 MOK(Machine Owner Keys) 数据库中，详细的可以参考 [Systemtap UEFI Secure Boot Support](https://sourceware.org/systemtap/wiki/SecureBoot)，也可以参考 [本地文档](/reference/linux/systemtap/SecureBoot.mht)。

上述文章没有提及需要安装 systemtap-server 并启动，实际上，在 CentOS 7 中可以通过如下的方式安装，简单记录如下。

{% highlight text %}
# yum install systemtap-server       # 安装服务端
# systemctl start stap-server        # 启动服务
{% endhighlight %}

然后，通过如下的方式执行，详细的内容可以参考上述的示例。

{% highlight text %}
----- 查看当前所有的server
# stap --list-servers=all

----- 生成相应的signing_key.x509
# stap --use-server=192.168.2.108:38557 -e 'probe begin { exit() }'

----- 导入
# mokutil --import signing_key.x509

----- 重启，需要验证输入密码
# shutdown -r now

----- 查看是否生效
# stap --list-servers

----- 测试
# stap -ve 'probe begin { printf("hello\n"); exit() }'
{% endhighlight %}


<!--
CentOS 中会检查 securelevel
/sys/kernel/security/securelevel
-->


## 详细使用

还可以参考 /usr/share/doc/systemtap-client-X.X/examples 中的示例，可以简单使用如下文件。

{% highlight text %}
# cat hello-world.stp
probe begin
{
    print ("hello world\n")
    exit ()
}

# stap hello-world.stp
{% endhighlight %}


探测点描述包含三个部分，function@filename:lineno ，可以使用 * 表示任意字符，例如，指定文件名为 net/socket.c，探测函数的入口和返回。

{% highlight text %}
probe kernel.function("*@net/socket.c") {}
probe kernel.function("*@net/socket.c").return {}
{% endhighlight %}

可以通过如下方式查找匹配的内核函数和变量。

{% highlight text %}
# stap -l 'kernel.function("*nit*")'      // 查看函数
# stap -L 'kernel.function("*nit*")'      // 查找函数以及变量
{% endhighlight %}

### 执行步骤

脚本的执行总共分为了 5 步：

#### 1. 解析脚本

解析 stap 输入脚本，或者 -e 中指定的内容，以及 tapset 库，也可以通过 -I 指定检索路径中的 *.stp 文件。

{% highlight text %}
# stap --vp 50000 -I /tmp -e 'probe syscall.read {printf("hello i am digoal.\n"); exit()}'
{% endhighlight %}

#### 2. 匹配库

解析脚本，对库进行匹配，并选择对应的 stp(探针库)、stpm(宏库) 文件，直到所有的符号都分析匹配完。如果没有使用 -u 选项，则优化脚本，移除未使用的变量、表达式、函数等。

{% highlight text %}
# stap --vp 05000 -I /tmp -e 'probe syscall.read {printf("hello i am digoal.\n"); exit()}'
{% endhighlight %}

#### 3. 转换成C

将以上解析好的 stap 脚本转换成 C 代码。

{% highlight text %}
# stap --vp 00500 -I /tmp -e 'probe syscall.read {printf("hello i am digoal.\n"); exit()}'
{% endhighlight %}

#### 4. 编译成.ko文件

使用上述生成的 C 文件创建 Linux 内核对象文件 (.ko)。

{% highlight text %}
# stap --vp 00050 -I /tmp -e 'probe syscall.read {printf("hello i am digoal.\n"); exit()}'
{% endhighlight %}

#### 5. 加载的内核运行

调用 staprun 将 ko 文件加载到内核中，并与之通讯，直到运行结束或受到中断请求信号；最后，staprun 从内核中卸载这个 ko 模块，并且清除整个 stap 过程产生的临时文件。

{% highlight text %}
# stap --vp 00005 -I /tmp -e 'probe syscall.read {printf("hello i am digoal.\n"); exit()}'
{% endhighlight %}


### 配置选项

常用的配置选项有：

* -p NUM<br>运行至某个步骤停止，一般可用于生成可移动模块。

* \-\-vp ABCDE<br>指定各步骤的输出详细级别。

## 常见错误排查

简单列举如下。

### semantic error

详细报错信息如下，主要原因是缺乏 debuginfo 包：

{% highlight text %}
semantic error: missing x86_64 kernel/module debuginfo [man warning::debuginfo] under '/lib/modules/xxx/build'
{% endhighlight %}

可以参考 [systemtap: a linux trace/probe tool](https://www.sourceware.org/git/?p=systemtap.git;a=blob_plain;f=README;hb=HEAD) ，其中包含了如下内容：

{% highlight text %}
- By default, systemtap looks for the debug info in these locations:
/boot/vmlinux-`uname -r`
/usr/lib/debug/lib/modules/`uname -r`/vmlinux
/lib/modules/`uname -r`/vmlinux
/lib/modules/`uname -r`/build/vmlinux
{% endhighlight %}

也就是说，依据这个默认搜索路径，可以将 debug 的 vmlinux 链接到 /lib/modules 下。

{% highlight text %}
# ln -s /usr/lib/debug/lib/modules/`uname -r`/vmlinux /lib/modules/`uname -r`
{% endhighlight %}



## 参考

可以参考 [systemtap官网](https://sourceware.org/systemtap/) ；也可以参考 [systemtap Documentation](https://sourceware.org/systemtap/documentation.html)，也就是 systemtap 的参考文档；以及相关的示例 [WarStories](https://sourceware.org/systemtap/wiki/WarStories) 。

另外，[SystemTap Beginners Guide](https://sourceware.org/systemtap/SystemTap_Beginners_Guide/index.html) 是一篇不错的入门文档，包括了详细的介绍以及示例。


<!--
[本地文档](/reference/linux/systemtap/Red_Hat_Enterprise_Linux-7-SystemTap_Beginners_Guide-en-US.pdf)

http://blog.csdn.net/zhangskd/article/details/38084337  内核调试工具 — kdump & crash
https://segmentfault.com/a/1190000000680628
http://myaut.github.io/dtrace-stap-book/
-->

{% highlight text %}
{% endhighlight %}
