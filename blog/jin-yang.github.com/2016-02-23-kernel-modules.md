---
title: Linux 内核模块
layout: post
comments: true
language: chinese
category: [linux]
keywords: linux,内核模块,kernel,module
description: 简单介绍下 Linux 中的内核模块编写，包括了内核签名机制的配置。
---

简单介绍下 Linux 中的内核模块编写，包括了内核签名机制的配置。

<!-- more -->

## 简单示例

一个很简单的 helloworld 程序，可以参考 [github LKM helloworld]({{ site.example_repository }}/linux/LKM) 。

如下是 Makefile 文件。

{% highlight makefile %}
ifneq	($(KERNELRELEASE),)
obj-m := hello.o
else
KERNEL_DIR := /lib/modules/$(shell uname -r)/build
PWD := $(shell pwd)
all:
	make -C $(KERNEL_DIR) SUBDIRS=$(PWD) modules
#    rm -r -f .tmp_versions *.mod.c .*.cmd *.o *.symvers
endif

clean:
	rm -rf *.o *.ko *.mod.c *.order *.symvers .*.cmd .tmp_versions
.PHONY:clean
{% endhighlight %}

下面是驱动的测试程序。

{% highlight c %}
/* FILE: hello.c */
#include <linux/init.h>
#include <linux/module.h>

static int __init hello_init(void)
{
    printk(KERN_ALERT "hello module!\n");
    return 0;
}

static void __exit hello_exit(void)
{
    printk(KERN_ALERT "bye module!\n");
}

module_init(hello_init);
module_exit(hello_exit);

MODULE_LICENSE("Dual BSD/GPL");
MODULE_AUTHOR("Andy <justkidding@gmail.com>");
MODULE_DESCRIPTION("A simple Hello World Module");
MODULE_ALIAS("the simplest module");
{% endhighlight %}

然后可以通过如的方式进行测试。

{% highlight text %}
# insmod hello.ko
# lsmod
# dmesg | tail -5
Module                  Size  Used by
hello                  12496  0
# rmmod hello
{% endhighlight %}



## 内核签名机制

在插入到内核模块时，可能会报如下错误。

{% highlight text %}
# insmod hello.ko
insmod: ERROR: could not insert module hello.ko: Required key not available
{% endhighlight %}

原因是内核启用了 [Module signature verification](https://lwn.net/Articles/222162/) ，可以通过如下命令检测内核配置项。

{% highlight text %}
----- 查看内核的配置项
$ grep "CONFIG_MODULE_SIG" /boot/config-`uname -r`

----- 查看当前系统key
# keyctl list %:.system_keyring
{% endhighlight %}

内核启动时，会有类似如下的输出。

{% highlight text %}
Loaded X.509 cert 'CentOS Linux kpatch signing key: xxxx'
Loaded X.509 cert 'CentOS Linux Driver update signing key: xxxx'
Loaded X.509 cert 'CentOS Linux kernel signing key: xxxx'
EFI: Loaded cert 'Lenovo Ltd.: ThinkPad Product CA 2012: xxxx' linked to '.system_keyring'
EFI: Loaded cert 'Lenovo UEFI CA 2014: xxxx' linked to '.system_keyring'
{% endhighlight %}

主要是由于目前 BIOS 支持 EFI，如果支持 UEFI Secure Boot 启动，那么内核所有模块都必须使用 UEFI Secure key 签名；当然，如果 BIOS 支持关闭 UEFI Secure Boot，那么可以在 BIOS 的 boot 项中关闭 UEFI Secure Boot 。

否则只能为自己制作一个。

接下来看看如何使用，主要包括了如下工具：

* openssl，生成X509公私秘钥对。
* sign-file，对内核模块使用X509公私秘钥对签名。
* mokutil，手动注册公钥到系统。
* keyctl，手动取消注册公钥到系统。

下面看看如何设置。

### 配置示例

#### 1. 生成配置文件

{% highlight text %}
# cat << EOF > configuration_file.config
[ req ]
default_bits = 4096
distinguished_name = req_distinguished_name
prompt = no
string_mask = utf8only
x509_extensions = myexts

[ req_distinguished_name ]
O = Organization
CN = Organization signing key
emailAddress = E-mail address

[ myexts ]
basicConstraints=critical,CA:FALSE
keyUsage=digitalSignature
subjectKeyIdentifier=hash
authorityKeyIdentifier=keyid
EOF
{% endhighlight %}

#### 2. 生成秘钥

一般会把公私钥放在 ``` /usr/src/kernels/`uname -r` ``` 文件夹中，不过还是建议方法 HOME 目录下。

{% highlight text %}
$ openssl req -x509 -new -nodes -utf8 -sha256 -days 36500 \
  -batch -config configuration_file.config -outform DER \
  -out public_key.der -keyout private_key.priv
{% endhighlight %}

#### 3. 导入到 mok 列表

上步生成了一对公私钥，接下来将其添加到 mok 列表中，此时会需要输入密码，该密码会在确认 MOK 请求的时候输入。

{% highlight text %}
# mokutil --import public_key.der
{% endhighlight %}

#### 4. 重启系统

{% highlight text %}
# shutdown -r now
{% endhighlight %}

在启动时，shim.efi 会发现新添加的 KEY，并会启动 MokManager.efi 模块，此时需要选择 Eroll Key 选项，然后输入上面的密码。

#### 5. 查看 key ring

接着 key ring 会添加到内核中，其中描述是配置文件中指定的 req_distinguished_name.O 中。

{% highlight text %}
# keyctl list %:.system_keyring | grep "Organization"
# cat /proc/keys
{% endhighlight %}

同样，也可以查看系统的启动日志。

{% highlight text %}
# dmesg | grep 'EFI: Loaded cert'
{% endhighlight %}

#### 6. 添加到模块中

{% highlight text %}
/usr/src/kernels/$(uname -r)/scripts/sign-file sha256 private_key.priv public_key.der hello.ko
{% endhighlight %}

#### 7. 添加 hello.ko

仍然同上，直接插入模块即可。

{% highlight text %}
# insmod hello.ko
{% endhighlight %}

<!--
  7. I ended up signing the kernel modules of VirtualBox with the following for loop:

  for i in /usr/lib/modules/$(uname -r)/extra/VirtualBox/*ko; do
        sudo /usr/src/kernels/$(uname -r)/scripts/sign-file sha256 private_key.priv  public_key.der "$i";
done

        8. VirtualBox will happily run virtual machines. ==> Done!

        I expect VirtualBox to break everytime an update for VirtualBox is released. In that case, you will have to repeat step 7. I am not sure what the effect of a shim update will be.
-->

## PROC 文件系统

最初 proc 文件系统是为提供一种内核及其模块向进程 (process) 发送信息的机制，这就是 proc 名字的由来。通过 proc 可以和内核内部数据结构进行交互，获取对于进程的有用信息，并可在运行时改变设置内核参数。

后来，这个系统的使用有些混乱和失控，于是在新内核中建议使用 sysfs 实现内核信息向用户空间的导出，例如，模块参数。


{% highlight text %}
$ ls /sys/module/hid/parameters               # 查看HID(Human Interface Devices)的参数
{% endhighlight %}

Linux 内核和用户空间的通信可通过 /proc 目录下的文件实现，编译时需要 ```CONFIG_PROC_FS``` 配置，通常来说，可写的配置项一般保存在 /proc/sys 目录下。其它介绍如下：

#### 1. 进程相关的目录

proc 目录下以 PID 作为目录，存储了进程相关信息，可以通过 cat 查看。

{% highlight text %}
$ cat /proc/1/cmdline              # 查看init命令启动参数
{% endhighlight %}

#### 2. 通用系统信息

包括了内存、文件系统、设备驱动、系统总线、电源等管理，如 devices、diskstats、meminfo、modules 等。

#### 3. 系统控制信息

用来检测修改系统的运行参数，存在于 ```/proc/sys``` 目录下，可以使用 cat、echo 来查看或修改系统的运行参数。不过此处修改只是临时，如果要持久化需要修改 ```/etc/sysctl.conf``` 中的配置。

接下来看看内核中的实现。

### 内核实现

相关的 API 实现在 ```fs/proc/generic.c``` 文件中，常见的 ```/proc/meminfo```、```/proc/stat``` 等统计项的源码在 fs/proc 目录下，具体的实现可以查看这里的源码。

很多简单的示例可以参考源码中的 loadavg.c 实现。



## 参考

关于 Signed Kernel Modules 可以参考 Gentoo 中的文档 [Signed kernel module support](https://wiki.gentoo.org/wiki/Signed_kernel_module_support)，或者 [本地保存的文档](/reference/linux/Signed kernel module support.mht) 。

关于 proc 的文档，可参考源码 Documentation/filesystems 目录下的 [seq_file.txt]({{ site.kernel_docs_url }}/Documentation/filesystems/seq_file.txt) 以及 [proc.txt]({{ site.kernel_docs_url }}/Documentation/filesystems/proc.txt) 。


{% highlight text %}
{% endhighlight %}
